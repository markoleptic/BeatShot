// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Target/ReinforcementLearningComponent.h"
#include "Target/TargetManager.h"


UReinforcementLearningComponent::UReinforcementLearningComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	CompositeCurveTable_HyperParameters = nullptr;
	ReinforcementLearningMode = EReinforcementLearningMode::None;
	HyperParameterMode = EReinforcementLearningHyperParameterMode::None;
	QTable = nc::NdArray<float>();
	TrainingSamples = nc::NdArray<int32>();
	Alpha = 0;
	Gamma = 0;
	Epsilon = 0;
	SpawnAreasHeight = 0;
	SpawnAreasWidth = 0;
	SpawnAreasSize = 0;
	ScaledSize = 0;
	HeightScaleFactor = 0;
	WidthScaleFactor = 0;
	TotalTrainingSamples = 0;

	IntegerFormatting.MaximumFractionalDigits = 0;
	IntegerFormatting.MaximumIntegralDigits = 2;
	IntegerFormatting.MinimumIntegralDigits = 2;

	FloatFormatting.MinimumFractionalDigits = 2;
	FloatFormatting.MaximumFractionalDigits = 2;
	FloatFormatting.MaximumIntegralDigits = 1;
	FloatFormatting.MinimumIntegralDigits = 1;
}

void UReinforcementLearningComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UReinforcementLearningComponent::DestroyComponent(bool bPromoteChildren)
{
	QTableIndices.Empty();
	TargetPairs.Empty();
	ActiveTargetPairs.Empty();
	Super::DestroyComponent(bPromoteChildren);
}

void UReinforcementLearningComponent::Init(const FRLAgentParams& AgentParams)
{
	Alpha = AgentParams.AIConfig.Alpha;
	Gamma = AgentParams.AIConfig.Gamma;
	Epsilon = AgentParams.AIConfig.Epsilon;
	ReinforcementLearningMode = AgentParams.AIConfig.ReinforcementLearningMode;
	HyperParameterMode = AgentParams.AIConfig.HyperParameterMode;
	SpawnAreasHeight = AgentParams.SpawnAreasHeight;
	SpawnAreasWidth =  AgentParams.SpawnAreasWidth;
	SpawnAreasSize = SpawnAreasHeight * SpawnAreasWidth;
	TotalTrainingSamples = AgentParams.TotalTrainingSamples;
	
	if (SpawnAreasHeight % 5 != 0 || SpawnAreasWidth % 5 != 0 )
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnArea size is not compatible with QTable size"));
	}

	WidthScaleFactor = SpawnAreasWidth / ScaledWidth;
	HeightScaleFactor = SpawnAreasHeight / ScaledHeight;
	ScaledSize = ScaledWidth * ScaledHeight;

	// Each row in QTable has size equal to ScaledSize, and so does each column
	QTable = nc::zeros<float>(nc::Shape(ScaledSize, ScaledSize));
	TrainingSamples = nc::zeros<int32>(nc::Shape(ScaledSize, ScaledSize));
	
	// Initialize struct array containing mappings for each QTable index to multiple SpawnArea indices
	for (int i = 0; i < ScaledSize; i++)
	{
		QTableIndices.Add(FQTableIndex(i));
	}

	// Init QTableIndices array
	for (int i = 0; i < SpawnAreasSize; i++)
	{
		const int32 Index = QTableIndices.Find(GetIndex_FromSpawnArea_ToQTable(i));
		if (QTableIndices.IsValidIndex(Index))
		{
			QTableIndices[Index].SpawnAreasIndices.AddUnique(i);
		}
	}

	// Use existing QTable if possible
	if (AgentParams.QTable.Num() == QTable.size())
	{
		QTable = GetNdArrayFromTArray<float>(AgentParams.QTable);
	}
	if (AgentParams.TrainingSamples.Num() == TrainingSamples.size())
	{
		TrainingSamples = GetNdArrayFromTArray<int32>(AgentParams.TrainingSamples);
	}
	
	UE_LOG(LogTemp, Display, TEXT("In QTable Size: %d  Actual QTable Size: %d"), AgentParams.QTable.Num(), QTable.size());
	UE_LOG(LogTemp, Display, TEXT("SpawnAreasRows: %d SpawnAreasColumns: %d"), SpawnAreasHeight, SpawnAreasWidth);
	UE_LOG(LogTemp, Display, TEXT("QTableRows: %d QTableColumns: %d"), ScaledHeight, ScaledWidth);
	UE_LOG(LogTemp, Display, TEXT("QTable Training Samples: %lld"), TotalTrainingSamples);
}

// Main QTable functions

void UReinforcementLearningComponent::AddToActiveTargetPairs(const int32 PreviousIndex, const int32 CurrentIndex)
{
	if (PreviousIndex == CurrentIndex)
	{
		return;
	}
	if (ActiveTargetPairs.FindByPredicate([&PreviousIndex, &CurrentIndex] (const FTargetPair& TargetPair)
	{
		return TargetPair.Current == CurrentIndex && TargetPair.Previous == PreviousIndex;
	}))
	{
		return;
	}
	ActiveTargetPairs.Emplace(PreviousIndex, CurrentIndex);
}

void UReinforcementLearningComponent::SetActiveTargetPairReward(const int32 SpawnAreaIndex, const bool bHit)
{
	FTargetPair FoundPair = FindTargetPairByCurrentIndex(SpawnAreaIndex);
	if (FoundPair.Previous == -1 || FoundPair.Current == -1)
	{
		UE_LOG(LogTargetManager, Warning, TEXT("Location not found in ActiveTargetPairs %d"), SpawnAreaIndex);
		ActiveTargetPairs.Remove(FoundPair);
		return;
	}

	/* Update reward */
	const float Reward = bHit ? -1.f : 1.f;
	FoundPair.SetReward(Reward);
	ActiveTargetPairs.Remove(FoundPair);
	TargetPairs.Enqueue(FoundPair);
	ClearCachedTargetPairs();
}

void UReinforcementLearningComponent::ClearCachedTargetPairs()
{
	if (!OnSpawnAreaValidityRequest.IsBound())
	{
		return;
	}
	
	while (!TargetPairs.IsEmpty())
	{
		FTargetPair TargetPair;
		if (!TargetPairs.Peek(TargetPair))
		{
			UE_LOG(LogTargetManager, Warning, TEXT("No targets in OpenLocations or No targets in TargetPairs"));
			break;
		}
		const int32 StateIndex = TargetPair.Previous;
		const int32 State2Index = TargetPair.Current;
		const int32 ActionIndex = State2Index;
		const int32 Action2Index = GetIndex_MaxAction(State2Index);
		
		/* Don't update RLAgent if this is not a valid Spawn Area index */
		const bool bIsValidIndex = OnSpawnAreaValidityRequest.Execute(Action2Index);
		if (bIsValidIndex)
		{
			UpdateQTable(FAlgoInput(StateIndex, State2Index, ActionIndex, Action2Index, TargetPair.GetReward()));
		}
		else
		{
			UE_LOG(LogTargetManager, Warning, TEXT("Invalid Spawn Area from RLAgent: TargetPair.Previous %d TargetPair.Current %d"), TargetPair.Previous, TargetPair.Current);
		}
		TargetPairs.Pop();
	}
}

int32 UReinforcementLearningComponent::ChooseNextActionIndex(const TArray<int32>& SpawnAreaIndices) const
{
	// Only Exploration and ActiveAgent Reinforcement Learning Modes should choose spawn locations
	if (SpawnAreaIndices.IsEmpty() || ReinforcementLearningMode == EReinforcementLearningMode::None ||
		ReinforcementLearningMode == EReinforcementLearningMode::Training)
	{
		return INDEX_NONE;
	}
	
	if (FMath::FRandRange(0, 1.f) > Epsilon)
	{
		const int32 BestActionIndex = ChooseBestActionIndex(SpawnAreaIndices);
		if (BestActionIndex == INDEX_NONE)
		{
			UE_LOG(LogTemp, Display, TEXT("No acceptable index range found, falling back to choosing random action"));
			return ChooseRandomActionIndex(SpawnAreaIndices);
		}
		return BestActionIndex;
	}
	return ChooseRandomActionIndex(SpawnAreaIndices);
}

int32 UReinforcementLearningComponent::ChooseRandomActionIndex(const TArray<int32>& SpawnAreaIndices)
{
	return SpawnAreaIndices[FMath::RandRange(0, SpawnAreaIndices.Num() - 1)];
}

int32 UReinforcementLearningComponent::ChooseBestActionIndex(const TArray<int32>& SpawnAreaIndices) const
{
	/* Basically argmax but in descending order */
	auto MaxIndicesReverseSorted = flip(GetQTable().max(nc::Axis::ROW).argsort(nc::Axis::COL));
	auto MaxesReverseSorted = GetQTable().max(nc::Axis::ROW).sort();
	
	//PrintMaxAverageIndices(MaxIndicesReverseSorted, MaxesReverseSorted);
	
	for (int j = 0; j < static_cast<int>(MaxIndicesReverseSorted.numCols()); j++)
	{
		/* Get the index from MaxesReverseSort */
		const int32 ChosenIndex = MaxIndicesReverseSorted(0, j);

		/* Get the SpawnArea indices that the chosen index represents */
		TArray<int32> UnfilteredSpawnAreaIndices = GetSpawnAreaIndexRange(ChosenIndex);

		/* Remove any indices that aren't inside the current SpawnAreasIndices */
		const TArray<int32> FilteredSpawnAreaIndices = UnfilteredSpawnAreaIndices.FilterByPredicate([&SpawnAreaIndices] (const int32& Value)
		{
			if (SpawnAreaIndices.Contains(Value))
			{
				return true;
			}
			return false;
		});

		//UE_LOG(LogTemp, Display, TEXT("FilteredSpawnIndices Size: %d"), FilteredSpawnAreaIndices.Num());

		/* Return a random point inside the filtered spawn indices if not empty */
		if (!FilteredSpawnAreaIndices.IsEmpty())
		{
			const int32 RandomFilteredIndex = FMath::RandRange(0, FilteredSpawnAreaIndices.Num() - 1);
			//UE_LOG(LogTemp, Display, TEXT("RandomFilteredIndex: %d"), FilteredSpawnAreaIndices[RandomFilteredIndex]);
			return FilteredSpawnAreaIndices[RandomFilteredIndex];
		}
	}
	return INDEX_NONE;
}

void UReinforcementLearningComponent::UpdateQTable(const FAlgoInput& In)
{
	FAlgoInput InCopy = In;
	InCopy.StateIndex = GetIndex_FromSpawnArea_ToQTable(In.StateIndex);
	InCopy.ActionIndex = GetIndex_FromSpawnArea_ToQTable(In.ActionIndex);
	InCopy.StateIndex_2 = GetIndex_FromSpawnArea_ToQTable(In.StateIndex_2);
	InCopy.ActionIndex_2 = GetIndex_FromSpawnArea_ToQTable(In.ActionIndex_2);
	
	const float Predict = QTable(InCopy.StateIndex, InCopy.ActionIndex);
	const float Target = InCopy.Reward + Gamma * QTable(InCopy.StateIndex_2, InCopy.ActionIndex_2);
	const float NewValue = QTable(InCopy.StateIndex, InCopy.ActionIndex) + Alpha * (Target - Predict);

	//UE_LOG(LogTemp, Display, TEXT("New QTable Value for SpawnAreaIndex [%d, %d] QTableIndex [%d, %d]: %f"), In.StateIndex, In.ActionIndex, InCopy.StateIndex, InCopy.ActionIndex, NewValue);
	
	QTable(InCopy.StateIndex, InCopy.ActionIndex) = NewValue;
	TrainingSamples(InCopy.StateIndex, InCopy.ActionIndex) += 1;
	TotalTrainingSamples += 1;
	UpdateQTableWidget();
}

// Getters and utility functions

TArray<float> UReinforcementLearningComponent::GetTArray_FromNdArray_QTableAvg() const
{
	nc::NdArray<float> QTableMean = nc::mean(GetQTable(), nc::Axis::ROW).astype<float>();
	const nc::NdArray<float> Reshaped = QTableMean.reshape(ScaledHeight,ScaledWidth);
	const nc::NdArray<float> Flipped = nc::flipud<float>(Reshaped);
	return GetTArrayFromNdArray<float>(Flipped);
}

int32 UReinforcementLearningComponent::GetIndex_MaxAction(const int32 SpawnAreaIndex) const
{
	const int32 Index = GetQTable().argmax(nc::Axis::COL)(0, GetIndex_FromSpawnArea_ToQTable(SpawnAreaIndex));
	//UE_LOG(LogTemp, Display, TEXT("MaxActionIndex for SpawnAreaIndex %d: %d"), SpawnAreaIndex, Index);
	if (Index == INDEX_NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetIndex_MaxAction return INDEX_NONE for QTableIndex of %d"), GetIndex_FromSpawnArea_ToQTable(SpawnAreaIndex));
	}
	return Index;
}

int32 UReinforcementLearningComponent::GetIndex_FromSpawnArea_ToQTable(const int32 SpawnAreaIndex) const
{
	/* First find the Row and Column number that corresponds to the SpawnArea index */
	const int32 SpawnAreaRowNum = SpawnAreaIndex / SpawnAreasWidth;
	const int32 SpawnAreaColNum = SpawnAreaIndex % SpawnAreasWidth;

	/* Scale down the SpawnArea row and column numbers */
	const int32 QTableRow = SpawnAreaRowNum / HeightScaleFactor;
	const int32 QTableCol = SpawnAreaColNum / WidthScaleFactor /*% ScaledWidth*/;
	const int32 QTableIndex = QTableRow * ScaledHeight + QTableCol;

	//UE_LOG(LogTemp, Display, TEXT(" %d|   %d %d  %d %d   |%d"), SpawnAreaIndex, SpawnAreaRowNum, SpawnAreaColNum, QTableRow, QTableCol, QTableIndex);
	
	return QTableIndex;
}

TArray<int32> UReinforcementLearningComponent::GetSpawnAreaIndexRange(const int32 QTableIndex) const
{
	return QTableIndices[QTableIndex].SpawnAreasIndices;
}

FTargetPair UReinforcementLearningComponent::FindTargetPairByCurrentIndex(const int32 InCurrentIndex)
{
	const FTargetPair* Found = ActiveTargetPairs.FindByPredicate([&InCurrentIndex] (const FTargetPair& TargetPair)
	{
		return TargetPair.Current == InCurrentIndex;
	});
	return Found ? *Found : FTargetPair();
}

// Debug functions

void UReinforcementLearningComponent::UpdateQTableWidget() const
{
	OnQTableUpdate.Broadcast(GetTArray_FromNdArray_QTableAvg());
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

void UReinforcementLearningComponent::PrintMaxAverageIndices(const nc::NdArray<unsigned>& MaxIndicesReverseSorted, const nc::NdArray<float>& MaxesReverseSorted) const
{
	FString Row = " ";
	FString Row2;
	FString Row3 = " ";
	FString Row4;
	
	/* Averages instead of maxes */
	auto AveragesReverseSorted = flip(mean(GetQTable(), nc::Axis::ROW).sort());
	auto AverageIndicesReverseSorted = flip(mean(GetQTable(), nc::Axis::ROW).argsort(nc::Axis::COL));
	
	for (int j = 0; j < static_cast<int>(MaxIndicesReverseSorted.numCols()); j++)
	{
		const float MaxIndex = MaxIndicesReverseSorted(0, j);
		const float AverageIndex = AverageIndicesReverseSorted(0, j);
		const float MaxValue = roundf(MaxesReverseSorted(0, j) * 100.0) / 100.0;
		const float AverageValue = roundf(AveragesReverseSorted(0, j) * 100.0) / 100.0;

		Row += (FText::AsNumber(MaxIndex, &IntegerFormatting).ToString() + "     ");
		
		if (MaxValue >= 0.f)
		{
			Row2 += ("  " + FText::AsNumber(MaxValue, &FloatFormatting).ToString() + " ");
		}
		else
		{
			Row2 += (" " + FText::AsNumber(MaxValue, &FloatFormatting).ToString() + " ");
		}

		Row3 += (FText::AsNumber(AverageIndex, &IntegerFormatting).ToString() + "     ");

		if (AverageValue >= 0.f)
		{
			Row4 += ("  " + FText::AsNumber(AverageValue, &FloatFormatting).ToString() + " ");
		}
		else
		{
			Row4 += (" " + FText::AsNumber(AverageValue, &FloatFormatting).ToString() + " ");
		}
	}
	UE_LOG(LogTemp, Display, TEXT("Max Indices & Max Values:"));
	UE_LOG(LogTemp, Display, TEXT("%s"), *Row);
	UE_LOG(LogTemp, Display, TEXT("%s"), *Row2);
	UE_LOG(LogTemp, Display, TEXT("Average Indices & Average Values"));
	UE_LOG(LogTemp, Display, TEXT("%s"), *Row3);
	UE_LOG(LogTemp, Display, TEXT("%s"), *Row4);
}

