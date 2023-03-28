// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "RLBase.h"

URLBase::URLBase()
{
	EpisodeRewards = nc::NdArray<float>(1);
	QTable = nc::NdArray<float>();
}

nc::NdArray<float> URLBase::ScaleQTable(const int32 SpawnCounterRows, const int32 SpawnCounterColumns)
{
	NumColsPerScaledCol = SpawnCounterColumns / ColScale;
	NumRowsPerScaledRow = SpawnCounterRows / RowScale;
	UE_LOG(LogTemp, Display, TEXT("SpawnCounterColumns: %d SpawnCounterRows: %d"), SpawnCounterColumns, SpawnCounterRows);
	UE_LOG(LogTemp, Display, TEXT("NumColsPerScaledCol: %d NumRowsPerScaledRow: %d"), NumColsPerScaledCol, NumRowsPerScaledRow);
	return nc::zeros<float>(nc::Shape(ColScale * RowScale, ColScale * RowScale));
}

int32 URLBase::GetQTableIndexFromSpawnCounterIndex(const int32 SpawnCounterIndex) const
{
	const float ScaledAreaSize = static_cast<float>(NumColsPerScaledCol * NumRowsPerScaledRow);
	const int32 NewIndex = floorf(static_cast<float>(SpawnCounterIndex) / ScaledAreaSize);
	//UE_LOG(LogTemp, Display, TEXT("SpawnCounterIndex: %d QTableIndex: %d"), SpawnCounterIndex, NewIndex);
	return NewIndex;
}

int32 URLBase::GetSpawnCounterIndexFromQTableIndex(const int32 QTableIndex) const
{
	/* TODO: Probably a better method that doesn't involve random numbers */
	const int32 ScaledAreaSize = NumColsPerScaledCol * NumRowsPerScaledRow;
	int32 Max = QTableIndex * ScaledAreaSize + ScaledAreaSize;
	const int32 Min = QTableIndex * ScaledAreaSize;
	/* Don't include Max max value */
	Max = Max - 1;
	const int32 RandNum = FMath::RandRange(Min, Max);
	//UE_LOG(LogTemp, Display, TEXT("GetSCIFromQTI QTI: %d Min %d Max %d Result: RandNum %d"), QTableIndex, Min, Max, RandNum);
	return RandNum;
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

TArray<float> URLBase::GetTArrayFromQTable(nc::NdArray<float> InQTable)
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

void URLBase::Init(const FRLAgentParams& AgentParams)
{
	Alpha = AgentParams.InAlpha;
	Gamma = AgentParams.InGamma;
	Epsilon = AgentParams.InEpsilon;

	/* TODO: Adapt to all target spawner sizes */
	/* TODO: Instead of points, use the square subareas so its an even number */
	/* TODO: Maybe pass through currently open points so it never chooses an invalid one, or at least some of the time */

	QTable = ScaleQTable(AgentParams.Rows, AgentParams.Columns);
	QTableWrapper.CustomGameModeName = AgentParams.CustomGameModeName;
	QTableWrapper.GameModeActorName = AgentParams.GameModeActorName;
	QTableWrapper.ColSize = ColScale * RowScale;
	QTableWrapper.RowSize = ColScale * RowScale;
	
	if (const TArray<FQTableWrapper> QTables = LoadQTables(); !QTables.IsEmpty())
	{
		UE_LOG(LogTemp, Display, TEXT("QTables IsNotEmpty"));
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

int32 URLBase::GetNextActionIndex(const int32 SpawnCounterIndex) const
{
	const int32 QTableIndex = GetQTableIndexFromSpawnCounterIndex(SpawnCounterIndex);
	if (FMath::FRandRange(0, 1.f) > Epsilon)
	{
		return GetSpawnCounterIndexFromQTableIndex(GetMaxActionIndex(QTableIndex));
	}
	return GetRandomAction();
}

int32 URLBase::ChooseNextActionIndex(const TArray<int32> SpawnCounterIndices) const
{
	if (FMath::FRandRange(0, 1.f) > Epsilon)
	{
		/*TArray<int32> QTablesIndices;
		for (const int Index : SpawnCounterIndices)
		{
			QTablesIndices.AddUnique(GetQTableIndexFromSpawnCounterIndex(Index));
		}
		for (const int32 Index : QTablesIndices)
		{
			UE_LOG(LogTemp, Display, TEXT("QTablesIndices: %d"), Index);
		}*/
		
		FIntPoint IndexRange = ChooseBestActionIndices(SpawnCounterIndices);

		if (IndexRange == FIntPoint::ZeroValue)
		{
			UE_LOG(LogTemp, Display, TEXT("No acceptable index range found"));

			/* TODO: Bound GetRandomAction */
			//return GetRandomAction();
			return INDEX_NONE;
		}
		
		TArray<int32> ValidChosenPoints = SpawnCounterIndices.FilterByPredicate([&IndexRange] (const int32& Value)
		{
			if (Value >= IndexRange.X && Value <= IndexRange.Y)
			{
				return true;
			}
			return false;
		});
		
		if (!ValidChosenPoints.IsEmpty())
		{
			UE_LOG(LogTemp, Display, TEXT("Acceptable Index Range: %d %d"), ValidChosenPoints[0], ValidChosenPoints.Last());
			return ValidChosenPoints[FMath::RandRange(0, ValidChosenPoints.Num() - 1)];
		}
		UE_LOG(LogTemp, Display, TEXT("No acceptable index range found"));

		/* TODO: Bound GetRandomAction */
		//return GetRandomAction();
		return INDEX_NONE;
	}
	
	/* TODO: Bound GetRandomAction */
	return INDEX_NONE;
	// return GetRandomAction();
}

int32 URLBase::GetRandomAction() const
{
	const int32 MaxValue = NumColsPerScaledCol * RowScale * NumRowsPerScaledRow * ColScale;
	const int32 RandomValue = FMath::RandRange(0, MaxValue - 1);
	return RandomValue;
}

int32 URLBase::GetMaxActionIndex(const int32 QTableIndex) const
{
	const int32 Index = QTable.argmax(nc::Axis::COL)(0,QTableIndex);
	if (Index == INDEX_NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetMaxActionIndex return INDEX_NONE for QTableIndex of %d"), QTableIndex);
	}
	return Index;
}

FIntPoint URLBase::ChooseBestActionIndices(const TArray<int32> SpawnCounterIndices) const
{
	//float MaxValue = -INFINITY;
	//int32 MaxIndex = INDEX_NONE;
	const int32 ScaledAreaSize = NumColsPerScaledCol * NumRowsPerScaledRow;
	auto MaxIndices = QTable.argmax(nc::Axis::COL);
	
	/* Basically argmax but in descending order */
	auto MaxesReverseSort = flip(QTable.max(nc::Axis::COL).argsort(nc::Axis::COL));
	
	for (int j = 0; j < static_cast<int>(MaxesReverseSort.numCols()); j++)
	{
		const int32 ChosenIndex = MaxesReverseSort(0, j);
		int32 Max = ChosenIndex * ScaledAreaSize + ScaledAreaSize;
		const int32 Min = ChosenIndex * ScaledAreaSize;
		Max = Max - 1;
		if (SpawnCounterIndices.ContainsByPredicate([&Min, &Max] (const int32 Value)
		{
			if (Value >= Min || Value <= Max)
			{
				return true;
			}
			return false;
		}))
		{
			return FIntPoint(Min, Max);
		}
		//UE_LOG(LogTemp, Display, TEXT("Sorted Max indices %u"), MaxesReverseSort(0, j));
	}
	return FIntPoint::ZeroValue;

	/*for (const int Index : SpawnCounterIndices)
	{
		if (MaxIndices(0, Index) > MaxValue)
		{
			MaxValue = MaxIndices(0, Index);
			MaxIndex = Index;
		}
		UE_LOG(LogTemp, Display, TEXT("QTable Index: %d Value at max index: %f"), Index, MaxValue);
	}*/
	//return GetSpawnCounterIndexFromQTableIndex(MaxIndex);
}

void URLBase::UpdateQTable(const FAlgoInput In)
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
}

void URLBase::UpdateEpisodeRewards(const float RewardReceived)
{
	nc::NdArray<float> Array = nc::NdArray<float>(1);
	Array(0,0) = RewardReceived;
	EpisodeRewards = nc::append(EpisodeRewards, Array);
}

void URLBase::PrintRewards()
{
	FString Row;
	for(int j = 0; j < static_cast<int>(QTable.numCols()); j++)
	{
		Row.Empty();
		for(int i = 0; i < static_cast<int>(QTable.numRows()); i++)
		{
			float Value = round(QTable(i,j) * 100.0) / 100.0;
			Row.Append(FString::SanitizeFloat(Value, 2) + " ");
		}
		UE_LOG(LogTemp, Display, TEXT("%s"), *Row);
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
}

/*for(int j = 0; j < static_cast<int>(MaxIndices.numCols()); j++)
{
	UE_LOG(LogTemp, Display, TEXT("Max indices / values %u %f"), MaxIndices(0,j), Maxes(0, MaxIndices(0,j)));
}
for(int j = 0; j < static_cast<int>(Maxes.numCols()); j++)
{
	UE_LOG(LogTemp, Display, TEXT("Maxes %f"), Maxes(0, j));
}
for(int j = 0; j < static_cast<int>(MaxesReverseSort.numCols()); j++)
{
	UE_LOG(LogTemp, Display, TEXT("Sorted Max indices %u"), MaxesReverseSort(0, j));
}*/