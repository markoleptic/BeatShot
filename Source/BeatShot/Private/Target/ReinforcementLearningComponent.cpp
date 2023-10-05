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

	// Check NaNs
	QTable = nc::nan_to_num<float>(QTable, 0.f);
	
	UE_LOG(LogTemp, Display, TEXT("In QTable Size: %d  Actual QTable Size: %d"), AgentParams.QTable.Num(), QTable.size());
	UE_LOG(LogTemp, Display, TEXT("SpawnAreasRows: %d SpawnAreasColumns: %d"), SpawnAreasHeight, SpawnAreasWidth);
	UE_LOG(LogTemp, Display, TEXT("QTableRows: %d QTableColumns: %d"), ScaledHeight, ScaledWidth);
	UE_LOG(LogTemp, Display, TEXT("QTable Training Samples: %lld"), TotalTrainingSamples);
}

// Main QTable functions

void UReinforcementLearningComponent::AddToActiveTargetPairs(const int32 SpawnAreaIndex_First, const int32 SpawnAreaIndex_Second)
{
	if (SpawnAreaIndex_First == SpawnAreaIndex_Second)
	{
		return;
	}
	if (ActiveTargetPairs.FindByPredicate([&SpawnAreaIndex_First, &SpawnAreaIndex_Second] (const FTargetPair& TargetPair)
	{
		return TargetPair.Second == SpawnAreaIndex_Second && TargetPair.First == SpawnAreaIndex_First;
	}))
	{
		return;
	}
	ActiveTargetPairs.Emplace(SpawnAreaIndex_First, SpawnAreaIndex_Second);
}

void UReinforcementLearningComponent::SetActiveTargetPairReward(const int32 SpawnAreaIndex, const bool bHit)
{
	FTargetPair FoundPair = FindTargetPairByCurrentIndex(SpawnAreaIndex);
	if (FoundPair.First == -1 || FoundPair.Second == -1)
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
	while (!TargetPairs.IsEmpty())
	{
		FTargetPair TargetPair;
		if (!TargetPairs.Peek(TargetPair))
		{
			break;
		}

		FQTableUpdateParams UpdateParams = FQTableUpdateParams(TargetPair);
		UpdateQTable(UpdateParams);
		
		TargetPairs.Pop();
	}
}

int32 UReinforcementLearningComponent::ChooseNextActionIndex(const TArray<int32>& SpawnAreaIndices, const int32 PreviousSpawnAreaIndex) const
{
	// Only Exploration and ActiveAgent Reinforcement Learning Modes should choose spawn locations
	if (SpawnAreaIndices.IsEmpty() || ReinforcementLearningMode == EReinforcementLearningMode::None ||
		ReinforcementLearningMode == EReinforcementLearningMode::Training)
	{
		return INDEX_NONE;
	}
	
	if (FMath::FRandRange(0, 1.f) > Epsilon)
	{
		const int32 BestActionIndex = ChooseBestActionIndex(SpawnAreaIndices, PreviousSpawnAreaIndex);
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

int32 UReinforcementLearningComponent::ChooseBestActionIndex(const TArray<int32>& SpawnAreaIndices, const int32 PreviousSpawnAreaIndex) const
{
	//PrintMaxAverageIndices();
	TArray<int32> MaxIndices;
	if (PreviousSpawnAreaIndex == INDEX_NONE)
	{
		MaxIndices = GetIndices_MaximizeFirst();
		UE_LOG(LogTemp, Display, TEXT("GetIndices_MaximizeFirst size: %d"), MaxIndices.Num());
	}
	else
	{
		MaxIndices = GetIndices_MaximizeSecond(GetIndex_FromSpawnArea_ToQTable(PreviousSpawnAreaIndex));
		UE_LOG(LogTemp, Display, TEXT("GetIndices_MaximizeSecond size: %d"), MaxIndices.Num());
	}
	
	for (const int32 Index : MaxIndices)
	{
		/* Get the SpawnArea indices that the chosen index represents */
		TArray<int32> UnfilteredSpawnAreaIndices = GetSpawnAreaIndexRange(Index);

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

void UReinforcementLearningComponent::UpdateQTable(FQTableUpdateParams& UpdateParams)
{
	// Convert SpawnArea indices to QTable indices
	UpdateParams.StateIndex = GetIndex_FromSpawnArea_ToQTable(UpdateParams.TargetPair.First);
	UpdateParams.ActionIndex = GetIndex_FromSpawnArea_ToQTable(UpdateParams.TargetPair.Second);
	UpdateParams.StateIndex_2 = UpdateParams.ActionIndex;

	// Choose a random max value
	const TArray<int32> MaxIndex_2_Candidates = GetIndices_MaximizeSecond(UpdateParams.StateIndex_2);
	UpdateParams.ActionIndex_2 = FMath::RandRange(0, MaxIndex_2_Candidates.Num() - 1);

	// Q value for starting at State 1 and taking Action 1 (State 1, Action 1)
	const float Predict = QTable(UpdateParams.StateIndex, UpdateParams.ActionIndex);

	// Q value for starting at State 2 and taking the Max Action (State 2, Action 2)
	const float Target = UpdateParams.TargetPair.GetReward() + Gamma * QTable(UpdateParams.StateIndex_2, UpdateParams.ActionIndex_2);

	// Q Table update function
	const float NewValue = QTable(UpdateParams.StateIndex, UpdateParams.ActionIndex) + Alpha * (Target - Predict);

	UE_LOG(LogTemp, Display, TEXT("Before Update: %f"), QTable(UpdateParams.StateIndex, UpdateParams.ActionIndex));
	
	// Assign new value to Q Table entry at (State 1, Action 1)
	QTable(UpdateParams.StateIndex, UpdateParams.ActionIndex) = NewValue;

	// Increment training samples and TotalTrainingSamples
	TrainingSamples(UpdateParams.StateIndex, UpdateParams.ActionIndex) += 1;
	TotalTrainingSamples += 1;
	
	UE_LOG(LogTemp, Display, TEXT("New QTable Value for TargetPair: [%d, %d] QTableIndex [%d, %d]: %f"),
		UpdateParams.TargetPair.First, UpdateParams.TargetPair.Second,UpdateParams.StateIndex, UpdateParams.ActionIndex, NewValue);
	
	UpdateQTableWidget();
}

// Getters and utility functions

TArray<int32> UReinforcementLearningComponent::GetIndices_MaximizeFirst() const
{
	TArray<int32> MaxIndices;
	
	// Compute the sum of values for each row
	const nc::NdArray<float> RowSum = nc::sum(GetQTable(), nc::Axis::COL);

	// Get the indices that correspond to sums
	const nc::NdArray<unsigned> SortedIndices = nc::argsort(RowSum);

	// Reverse to get descending order
	const nc::NdArray<unsigned> ReverseSortedIndices = nc::flip(SortedIndices);

	// Compute MaxValue
	const float MaxValue = RowSum[ReverseSortedIndices[0]];

	// Find any values equal to the MaxValue, and add them to MaxIndices
	for (size_t i = 0; i < ReverseSortedIndices.size(); ++i)
	{
		if (RowSum[ReverseSortedIndices[i]] == MaxValue)
		{
			MaxIndices.Add(static_cast<int32>(ReverseSortedIndices[i]));
		}
		else
		{
			break;  // Stop when values are no longer the maximum
		}
	}

	return MaxIndices;
}

TArray<int32> UReinforcementLearningComponent::GetIndices_MaximizeSecond(const int32 InPreviousIndex) const
{
	TArray<int32> MaxIndices;
	
	// Extract the row (all the columns) corresponding to the previous index
	const nc::NdArray<float> PreviousRow = GetQTable()(InPreviousIndex, nc::Slice());

	// Get the indices that correspond to sums
	const nc::NdArray<unsigned> ArgSortedIndices = nc::argsort(PreviousRow);

	// Reverse to get descending order
	nc::NdArray<unsigned> ReverseSortedIndices = nc::flip(ArgSortedIndices);
	
	// Find the maximum value within the previous row
	const float MaxValue = PreviousRow[ReverseSortedIndices[0]];

	// Find any values equal to the MaxValue, and add them to MaxIndices
	for (size_t i = 0; i < ReverseSortedIndices.size(); ++i)
	{
		if (PreviousRow[ReverseSortedIndices[i]] == MaxValue)
		{
			MaxIndices.Add(static_cast<int32>(ReverseSortedIndices[i]));
		}
		else
		{
			break;  // Stop when values are no longer the maximum
		}
	}

	return MaxIndices;
}

TArray<float> UReinforcementLearningComponent::GetTArray_FromNdArray_QTableAvg() const
{
	nc::NdArray<float> QTableMean = nc::mean(GetQTable(), nc::Axis::ROW).astype<float>();
	const nc::NdArray<float> Reshaped = QTableMean.reshape(ScaledHeight,ScaledWidth);
	const nc::NdArray<float> Flipped = nc::flipud<float>(Reshaped);
	return GetTArrayFromNdArray<float>(Flipped);
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
		return TargetPair.Second == InCurrentIndex;
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

void UReinforcementLearningComponent::PrintMaxAverageIndices() const
{
	FString Row = " ";
	FString Row2;
	FString Row3 = " ";
	FString Row4;
	
	/* Averages instead of maxes */
	auto AvgValues = mean(GetQTable(), nc::Axis::ROW).sort();
	auto AvgIndices = AvgValues.argsort(nc::Axis::COL);
	
	auto MaxValues = nc::max(GetQTable(), nc::Axis::ROW).sort();
	auto MaxIndices = MaxValues.argsort(nc::Axis::COL);
	
	for (int j = 0; j < static_cast<int>(MaxIndices.numCols()); j++)
	{
		const float MaxIndex = MaxIndices(0, j);
		const float AverageIndex = AvgIndices(0, j);
		const float MaxValue = MaxValues(0, j);
		const float AverageValue = AvgValues(0, j);

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