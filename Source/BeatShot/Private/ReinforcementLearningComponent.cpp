// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "ReinforcementLearningComponent.h"
#include "Kismet/KismetMathLibrary.h"


UReinforcementLearningComponent::UReinforcementLearningComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	EpisodeRewards = nc::NdArray<float>(1);
	QTable = nc::NdArray<float>();
}

void UReinforcementLearningComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UReinforcementLearningComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UReinforcementLearningComponent::Init(const FRLAgentParams& AgentParams)
{
	Alpha = AgentParams.InAlpha;
	Gamma = AgentParams.InGamma;
	Epsilon = AgentParams.InEpsilon;
	SpawnCounterHeight = AgentParams.SpawnCounterHeight;
	SpawnCounterWidth =  AgentParams.SpawnCounterWidth;
	SpawnCounterSize = SpawnCounterHeight * SpawnCounterWidth;
	
	if (SpawnCounterHeight % 5 != 0 || SpawnCounterWidth % 5 != 0 )
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnCounter is not compatible with QTable size"));
	}

	WidthScaleFactor = SpawnCounterWidth / ScaledWidth;
	HeightScaleFactor = SpawnCounterHeight / ScaledHeight;
	ScaledSize = ScaledWidth * ScaledHeight;

	/* Each row in QTable has size equal to ScaledSize, and so does each column */
	QTable = nc::zeros<float>(nc::Shape(ScaledSize, ScaledSize));
	
	/* Initialize struct array containing mappings for each QTable index to multiple SpawnCounter indices */
	for (int i = 0; i < ScaledSize; i++)
	{
		QTableIndices.Add(FQTableIndex(i));
	}
	
	for (int i  = 0; i < SpawnCounterSize; i++)
	{
		if (const int32 Found = QTableIndices.Find(GetQTableIndexFromSpawnCounterIndex(i)); Found != INDEX_NONE)
		{
			QTableIndices[Found].SpawnCounterIndices.AddUnique(i);
		}
	}

	if (AgentParams.InQTable.Num() == QTable.size())
	{
		QTable = GetQTableFromTArray(AgentParams.InQTable);
	}
	UE_LOG(LogTemp, Display, TEXT("InQTable.Num() %d"), AgentParams.InQTable.Num());
	UE_LOG(LogTemp, Display, TEXT("QTable.size() %u"), QTable.size());
	UE_LOG(LogTemp, Display, TEXT("SpawnCounterRows: %d SpawnCounterColumns: %d"), SpawnCounterHeight, SpawnCounterWidth);
	UE_LOG(LogTemp, Display, TEXT("QTableRows: %d QTableColumns: %d"), ScaledHeight, ScaledWidth);
	UE_LOG(LogTemp, Display, TEXT("QTable Size: %d"), QTable.size());
}

void UReinforcementLearningComponent::UpdateQTable(const FAlgoInput& In)
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

void UReinforcementLearningComponent::UpdateEpisodeRewards(const float RewardReceived)
{
	nc::NdArray<float> Array = nc::NdArray<float>(1);
	Array(0,0) = RewardReceived;
	EpisodeRewards = append(EpisodeRewards, Array);
}

int32 UReinforcementLearningComponent::GetMaxActionIndex(const int32 SpawnCounterIndex) const
{
	const int32 Index = GetQTable().argmax(nc::Axis::COL)(0, GetQTableIndexFromSpawnCounterIndex(SpawnCounterIndex));
	//UE_LOG(LogTemp, Display, TEXT("MaxActionIndex for SpawnCounterIndex %d: %d"), SpawnCounterIndex, Index);
	if (Index == INDEX_NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetMaxActionIndex return INDEX_NONE for QTableIndex of %d"), GetQTableIndexFromSpawnCounterIndex(SpawnCounterIndex));
	}
	return Index;
}

int32 UReinforcementLearningComponent::ChooseNextActionIndex(const TArray<int32>& SpawnCounterIndices) const
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

void UReinforcementLearningComponent::PrintRewards() const
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
	nc::NdArray<double> FlippedMean = flipud(mean(GetQTable(), nc::Axis::ROW).reshape(ScaledHeight,ScaledWidth));
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
		if (i % ScaledHeight == 0)
		{
			UE_LOG(LogTemp, Display, TEXT("%s"), *Row);
			Row.Empty();
		}
	}
}

TArray<float> UReinforcementLearningComponent::GetAveragedTArrayFromQTable() const
{
	return GetTArrayFromQTable(flipud(mean(GetQTable(), nc::Axis::ROW).reshape(ScaledHeight,ScaledWidth)));
}

TArray<float> UReinforcementLearningComponent::GetSaveReadyQTable() const
{
	return GetTArrayFromQTable(GetQTable());
}

nc::NdArray<float> UReinforcementLearningComponent::GetQTable() const
{
	return QTable;
}

int32 UReinforcementLearningComponent::ChooseRandomActionIndex(const TArray<int32>& SpawnCounterIndices) const
{
	return SpawnCounterIndices[FMath::RandRange(0, SpawnCounterIndices.Num() - 1)];
}

int32 UReinforcementLearningComponent::ChooseBestActionIndex(const TArray<int32>& SpawnCounterIndices) const
{
		FString Row;
	FString Row2;
	FString Row3;
	FString Row4;
	
	/* Basically argmax but in descending order */
	auto MaxIndicesReverseSorted = flip(GetQTable().max(nc::Axis::ROW).argsort(nc::Axis::COL));
	auto MaxesReverseSorted = GetQTable().max(nc::Axis::ROW).sort();

	/* Averages instead of maxes */
	auto AveragesReverseSorted = flip(mean(GetQTable(), nc::Axis::ROW).sort());
	auto AverageIndicesReverseSorted = flip(mean(GetQTable(), nc::Axis::ROW).argsort(nc::Axis::COL));
	
	for (int j = 0; j < static_cast<int>(MaxIndicesReverseSorted.numCols()); j++)
	{
		const float MaxIndex = MaxIndicesReverseSorted(0, j);
		const float AverageIndex = AverageIndicesReverseSorted(0, j);
		const float MaxValue = roundf(MaxesReverseSorted(0, j) * 100.0) / 100.0;
		const float AverageValue = roundf(AveragesReverseSorted(0, j) * 100.0) / 100.0;
		
		Row.Append(FString::SanitizeFloat(MaxIndex, 0) + " ");
		Row2.Append(FString::SanitizeFloat(MaxValue, 2) + " ");
		Row3.Append(FString::SanitizeFloat(AverageIndex, 0) + " ");
		Row4.Append(FString::SanitizeFloat(AverageValue, 2) + " ");
	}
	UE_LOG(LogTemp, Display, TEXT("%s"), *Row);
	UE_LOG(LogTemp, Display, TEXT("%s"), *Row2);
	UE_LOG(LogTemp, Display, TEXT("%s"), *Row3);
	UE_LOG(LogTemp, Display, TEXT("%s"), *Row4);
	
	for (int j = 0; j < static_cast<int>(MaxIndicesReverseSorted.numCols()); j++)
	{
		/* Get the index from MaxesReverseSort */
		const int32 ChosenIndex = MaxIndicesReverseSorted(0, j);

		/* Get the SpawnCounter indices that the chosen index represents */
		TArray<int32> UnfilteredSpawnCounterIndices = GetSpawnCounterIndexRange(ChosenIndex);

		/* Remove any indices that aren't inside the current SpawnCounterIndices */
		const TArray<int32> FilteredSpawnCounterIndices = UnfilteredSpawnCounterIndices.FilterByPredicate([&SpawnCounterIndices] (const int32& Value)
		{
			if (SpawnCounterIndices.Contains(Value))
			{
				return true;
			}
			return false;
		});

		UE_LOG(LogTemp, Display, TEXT("FilteredSpawnIndices Size: %d"), FilteredSpawnCounterIndices.Num());

		/* Return a random point inside the filtered spawn indices if not empty */
		if (!FilteredSpawnCounterIndices.IsEmpty())
		{
			const int32 RandomFilteredIndex = FMath::RandRange(0, FilteredSpawnCounterIndices.Num() - 1);
			UE_LOG(LogTemp, Display, TEXT("RandomFilteredIndex: %d"), FilteredSpawnCounterIndices[RandomFilteredIndex]);
			return FilteredSpawnCounterIndices[RandomFilteredIndex];
		}
	}
	return INDEX_NONE;
}

int32 UReinforcementLearningComponent::GetQTableIndexFromSpawnCounterIndex(const int32 SpawnCounterIndex) const
{
	/* First find the Row and Column number that corresponds to the SpawnCounter index */
	const int32 SpawnCounterRowNum = SpawnCounterIndex / SpawnCounterWidth;
	const int32 SpawnCounterColNum = SpawnCounterIndex % SpawnCounterWidth;

	/* Scale down the SpawnCounter row and column numbers */
	const int32 QTableRow = SpawnCounterRowNum / HeightScaleFactor;
	const int32 QTableCol = SpawnCounterColNum / WidthScaleFactor /*% ScaledWidth*/;
	const int32 QTableIndex = QTableRow * ScaledHeight + QTableCol;

	//UE_LOG(LogTemp, Display, TEXT(" %d|   %d %d  %d %d   |%d"), SpawnCounterIndex, SpawnCounterRowNum, SpawnCounterColNum, QTableRow, QTableCol, QTableIndex);
	
	return QTableIndex;
}

TArray<int32> UReinforcementLearningComponent::GetSpawnCounterIndexRange(const int32 QTableIndex) const
{
	return QTableIndices[QTableIndex].SpawnCounterIndices;
}

nc::NdArray<float> UReinforcementLearningComponent::GetQTableFromTArray(const TArray<float>& InTArray) const
{
	const int32 RowSize = UKismetMathLibrary::Sqrt(InTArray.Num());
	const int32 ColSize = RowSize;
	
	nc::NdArray<float> Out = nc::zeros<float>(RowSize, ColSize);
	
	for (int j = 0; j < ColSize; j++)
	{
		for (int i = 0; i < RowSize; i++)
		{
			Out(i, j) = InTArray[RowSize * j + i];
		}
	}
	return Out;
}

TArray<float> UReinforcementLearningComponent::GetTArrayFromQTable(const nc::NdArray<float>& InQTable)
{
	const int32 RowSize = InQTable.numRows();
	const int32 ColSize = InQTable.numCols();
	
	TArray<float> Out;
	Out.Init(0.f, InQTable.size());
	
	for(int j = 0; j < ColSize; j++)
	{
		for(int i = 0; i < RowSize; i++)
		{
			Out[RowSize * j + i] = InQTable(i, j);
		}
	}
	return Out;
}

TArray<float> UReinforcementLearningComponent::GetTArrayFromQTable(const nc::NdArray<double>& InQTable)
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
	return Out;
}

void UReinforcementLearningComponent::UpdateQTableWidget() const
{
	OnQTableUpdate.Broadcast(GetAveragedTArrayFromQTable());
}

