// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "RLBase.h"

URLBase::URLBase()
{
	EpisodeRewards = nc::NdArray<float>(1);
	QTable = nc::NdArray<float>();
}

void URLBase::Init(const FRLAgentParams& AgentParams)
{
	Alpha = AgentParams.InAlpha;
	Gamma = AgentParams.InGamma;
	Epsilon = AgentParams.InEpsilon;
	NumSpawnCounterRows = AgentParams.Rows;
	NumSpawnCounterColumns =  AgentParams.Columns;
	
	if (AgentParams.Columns % 5 == 0)
	{
		ColScale = 5;
	}
	else
	{
		ColScale = 3;
	}
	if (AgentParams.Rows % 5 == 0)
	{
		RowScale = 5;
	}
	else
	{
		RowScale = 3;
	}

	NumColsPerScaledCol = NumSpawnCounterColumns / ColScale;
	NumRowsPerScaledRow = NumSpawnCounterRows / RowScale;
	Size = ColScale * RowScale;
	
	QTableWrapper.CustomGameModeName = AgentParams.CustomGameModeName;
	QTableWrapper.GameModeActorName = AgentParams.GameModeActorName;
	QTableWrapper.ColSize = ColScale * RowScale;
	QTableWrapper.RowSize = ColScale * RowScale;

	UE_LOG(LogTemp, Display, TEXT("SpawnCounterRows: %d SpawnCounterColumns: %d"), NumSpawnCounterRows, NumSpawnCounterColumns);
	QTable = nc::zeros<float>(nc::Shape(Size, Size));
	
	/* Initialize struct array containing mappings for each QTable index to multiple SpawnCounter indices */
	for (int i  = 0; i < Size; i++)
	{
		QTableIndices.Add(FQTableIndex(i));
	}
	for (int i  = 0; i < NumSpawnCounterColumns * NumSpawnCounterRows; i++)
	{
		if (const int32 Found = QTableIndices.Find(GetQTableIndexFromSpawnCounterIndex(i)); Found != INDEX_NONE)
		{
			QTableIndices[Found].SpawnCounterIndices.AddUnique(i);
		}
	}
	
	if (const TArray<FQTableWrapper> QTables = LoadQTables(); !QTables.IsEmpty())
	{
		const int32 Index = QTables.Find(FQTableWrapper(AgentParams.GameModeActorName, AgentParams.CustomGameModeName));
		if (Index != INDEX_NONE)
		{
			const FQTableWrapper FoundQTable = QTables[Index];
			if (FoundQTable.QTable.Num() == QTable.size())
			{
				UE_LOG(LogTemp, Display, TEXT("QTable Match"));
				QTable = GetQTableFromTArray(FoundQTable);
				QTableWrapper = QTables[Index];
			}
		}
	}
	UE_LOG(LogTemp, Display, TEXT("QTable Size: %d"), QTable.size());
}

void URLBase::UpdateQTable(const FAlgoInput& In)
{
	FAlgoInput InCopy = In;
	InCopy.StateIndex = GetQTableIndexFromSpawnCounterIndex(In.StateIndex);
	InCopy.ActionIndex = GetQTableIndexFromSpawnCounterIndex(In.ActionIndex);
	InCopy.StateIndex_2 = GetQTableIndexFromSpawnCounterIndex(In.StateIndex_2);
	InCopy.ActionIndex_2 = GetQTableIndexFromSpawnCounterIndex(In.ActionIndex_2);
	
	const float Predict = QTable(InCopy.StateIndex, InCopy.ActionIndex);
	const float Target = InCopy.Reward + Gamma * QTable(InCopy.StateIndex_2, InCopy.ActionIndex_2);
	const float NewValue = QTable(InCopy.StateIndex, InCopy.ActionIndex) + Alpha * (Target - Predict);

	//UE_LOG(LogTemp, Display, TEXT("New QTable Value for SpawnCounterIndex [%d, %d] QTableIndex [%d, %d]: %f"), In.StateIndex, In.ActionIndex, InCopy.StateIndex, InCopy.ActionIndex, NewValue);
	
	QTable(InCopy.StateIndex, InCopy.ActionIndex) = NewValue;
	UpdateQTableWidget();
}

void URLBase::UpdateEpisodeRewards(const float RewardReceived)
{
	nc::NdArray<float> Array = nc::NdArray<float>(1);
	Array(0,0) = RewardReceived;
	EpisodeRewards = append(EpisodeRewards, Array);
}

int32 URLBase::GetMaxActionIndex(const int32 SpawnCounterIndex) const
{
	const int32 Index = QTable.argmax(nc::Axis::COL)(0,GetQTableIndexFromSpawnCounterIndex(SpawnCounterIndex));
	if (Index == INDEX_NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetMaxActionIndex return INDEX_NONE for QTableIndex of %d"), GetQTableIndexFromSpawnCounterIndex(SpawnCounterIndex));
	}
	return Index;
}

int32 URLBase::ChooseNextActionIndex(const TArray<int32>& SpawnCounterIndices) const
{
	if (SpawnCounterIndices.IsEmpty())
	{
		return INDEX_NONE;
	}
	if (FMath::FRandRange(0, 1.f) > Epsilon)
	{
		const int32 BestActionIndex = ChooseBestActionIndex(SpawnCounterIndices);
		if (BestActionIndex == INDEX_NONE)
		{
			UE_LOG(LogTemp, Display, TEXT("No acceptable index range found, falling back to choosing random action"));
			return ChooseRandomActionIndex(SpawnCounterIndices);
		}
		return BestActionIndex;
	}
	return ChooseRandomActionIndex(SpawnCounterIndices);
}

void URLBase::PrintRewards() const
{
	FString Row;
	nc::NdArray<float> QTableCopy = GetQTable();
	for(int j = 0; j < static_cast<int>(QTableCopy.numCols()); j++)
	{
		Row.Empty();
		for(int i = 0; i < static_cast<int>(QTableCopy.numRows()); i++)
		{
			const float Value = round(QTableCopy(i,j) * 100.0) / 100.0;
			if (Value >= 0.f)
			{
				Row.Append("+" + FString::SanitizeFloat(Value, 2) + " ");
			}
			else
			{
				Row.Append(FString::SanitizeFloat(Value, 2) + " ");
			}
		}
		UE_LOG(LogTemp, Display, TEXT("%s"), *Row);
	}

	int i = 0;
	nc::NdArray<double> FlippedMean = flipud(mean(GetQTable(), nc::Axis::ROW).reshape(RowScale,ColScale));
	Row.Empty();
	for (const double It : FlippedMean)
	{
		const float Value = roundf(static_cast<float>(It) * 100.0) / 100.0;;
		if (It >= 0.f)
		{
			Row.Append("+" + FString::SanitizeFloat(Value, 2) + " ");
		}
		else
		{
			Row.Append(FString::SanitizeFloat(Value, 2) + " ");
		}
		i++;
		if (i % RowScale == 0)
		{
			UE_LOG(LogTemp, Display, TEXT("%s"), *Row);
			Row.Empty();
		}
	}
}

void URLBase::SaveQTable()
{
	QTableWrapper.QTable = GetTArrayFromQTable(QTable);
	TArray<FQTableWrapper> QTables = LoadQTables();
	if (const int32 Index = QTables.Find(QTableWrapper); Index != INDEX_NONE)
	{
		QTables[Index] = QTableWrapper;
		SaveQTables(QTables);
		return;
	}
	QTables.Add(QTableWrapper);
	SaveQTables(QTables);
	PrintRewards();
}

TArray<float> URLBase::GetTArrayQTable() const
{
	return GetTArrayFromQTable(flipud(mean(GetQTable(), nc::Axis::ROW).reshape(RowScale,ColScale)));
}

nc::NdArray<float> URLBase::GetQTable() const
{
	return QTable;
}

int32 URLBase::ChooseRandomActionIndex(const TArray<int32>& SpawnCounterIndices) const
{
	return SpawnCounterIndices[FMath::RandRange(0, SpawnCounterIndices.Num() - 1)];
}

int32 URLBase::ChooseBestActionIndex(const TArray<int32>& SpawnCounterIndices) const
{
	/* Basically argmax but in descending order */
	auto MaxesReverseSort = flip(QTable.max(nc::Axis::COL).argsort(nc::Axis::COL));
	
	for (int j = 0; j < static_cast<int>(MaxesReverseSort.numCols()); j++)
	{
		const int32 ChosenIndex = MaxesReverseSort(0, j);
		TArray<int32> UnfilteredSpawnCounterIndices = GetSpawnCounterIndexRange(ChosenIndex);
		const TArray<int32> FilteredSpawnCounterIndices = UnfilteredSpawnCounterIndices.FilterByPredicate([&SpawnCounterIndices] (const int32& Value)
		{
			if (SpawnCounterIndices.Contains(Value))
			{
				return true;
			}
			return false;
		});
		
		if (!FilteredSpawnCounterIndices.IsEmpty())
		{
			return FilteredSpawnCounterIndices[FMath::RandRange(0, FilteredSpawnCounterIndices.Num() - 1)];
		}
	}
	return INDEX_NONE;
}

int32 URLBase::GetQTableIndexFromSpawnCounterIndex(const int32 SpawnCounterIndex) const
{
	/* First find the Row and Column number that corresponds to the SpawnCounter index */
	const int32 SpawnCounterRowNum = SpawnCounterIndex / NumSpawnCounterColumns;
	const int32 SpawnCounterColNum = SpawnCounterIndex % NumSpawnCounterColumns;

	/* Scale down the SpawnCounter row and column numbers */
	const int32 QTableRow = SpawnCounterRowNum / NumRowsPerScaledRow;
	const int32 QTableCol = SpawnCounterColNum / NumColsPerScaledCol;
	const int32 QTableIndex = QTableRow * ColScale + QTableCol;
	//UE_LOG(LogTemp, Display, TEXT("QTableRow: %d QTableCol: %d QTableIndex %d"), QTableRow, QTableCol, QTableIndex);
	return QTableIndex;
}

TArray<int32> URLBase::GetSpawnCounterIndexRange(const int32 QTableIndex) const
{
	return QTableIndices[QTableIndex].SpawnCounterIndices;
}

nc::NdArray<float> URLBase::GetQTableFromTArray(const FQTableWrapper& InWrapper) const
{
	nc::NdArray<float> Out = nc::zeros<float>(InWrapper.RowSize, InWrapper.ColSize);
	for (int j = 0; j < InWrapper.ColSize; j++)
	{
		for (int i = 0; i < InWrapper.RowSize; i++)
		{
			Out(i, j) = InWrapper.QTable[(InWrapper.RowSize * j) + i];
		}
	}
	UE_LOG(LogTemp, Display, TEXT("OutSize: %d"), Out.numCols());
	return Out;
}

TArray<float> URLBase::GetTArrayFromQTable(const nc::NdArray<float>& InQTable)
{
	TArray<float> Out;
	Out.Init(0.f, InQTable.size());
	for(int j = 0; j < static_cast<int>(InQTable.numCols()); j++)
	{
		for(int i = 0; i < static_cast<int>(InQTable.numRows()); i++)
		{
			Out[(static_cast<int>(InQTable.numRows()) * j) + i] = InQTable(i, j);
		}
	}
	UE_LOG(LogTemp, Display, TEXT("OutSize: %d"), Out.Num());
	return Out;
}

TArray<float> URLBase::GetTArrayFromQTable(const nc::NdArray<double>& InQTable)
{
	TArray<float> Out;
	Out.Init(0.f, InQTable.size());
	for(int j = 0; j < static_cast<int>(InQTable.numCols()); j++)
	{
		for(int i = 0; i < static_cast<int>(InQTable.numRows()); i++)
		{
			Out[(static_cast<int>(InQTable.numRows()) * j) + i] = InQTable(i, j);
		}
	}
	UE_LOG(LogTemp, Display, TEXT("OutSize: %d"), Out.Num());
	return Out;
}

void URLBase::UpdateQTableWidget() const
{
	const nc::NdArray<double> FlippedMean = flipud(mean(GetQTable(), nc::Axis::ROW).reshape(RowScale,ColScale));
	OnQTableUpdate.Broadcast(GetTArrayFromQTable(FlippedMean));
}
