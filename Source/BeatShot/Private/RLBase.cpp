// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "RLBase.h"

URLBase::URLBase()
{
	EpisodeRewards = nc::NdArray<float>(1);
	QTable = nc::NdArray<float>();
}

void URLBase::Init(const EGameModeActorName GameModeActorName, const FString CustomGameModeName, const int32 Size, const float InAlpha, const float InGamma, const float InEpsilon)
{
	ColSize = Size;
	RowSize = Size;
	Alpha = InAlpha;
	Gamma = InGamma;
	Epsilon = InEpsilon;

	QTableWrapper.CustomGameModeName = CustomGameModeName;
	QTableWrapper.GameModeActorName = GameModeActorName;
	
	if (const TArray<FQTableWrapper> QTables = LoadQTables(); !QTables.IsEmpty())
	{
		if (const int32 Index = QTables.Find(FQTableWrapper(GameModeActorName, CustomGameModeName)); Index != INDEX_NONE)
		{
			if (QTables[Index].QTable.numCols() == ColSize && QTables[Index].QTable.numRows() == RowSize)
			{
				QTableWrapper = QTables[Index];
				QTable = QTableWrapper.QTable;
				return;
			}
		}
	}
	QTable = nc::zeros<float>(nc::Shape(Size, Size));
}

int32 URLBase::GetNextActionIndex(const int32 Index) const
{
	if (const float ActionToTake = FMath::FRandRange(0, 1.f); ActionToTake > Epsilon)
	{
		UE_LOG(LogTemp, Display, TEXT("ActionToTake %f"), ActionToTake);
		return GetMaxActionIndex(Index);
	}
	return FMath::RandRange(0, RowSize - 1);
}

int32 URLBase::GetMaxActionIndex(const int32 Index) const
{
	return QTable.argmax(nc::Axis::COL)(0,Index);
}

void URLBase::UpdateQTable(const FAlgoInput& In)
{
	const float Predict = QTable(In.StateIndex, In.ActionIndex);
	const float Target = In.Reward + Gamma * QTable(In.StateIndex_2, In.ActionIndex_2);
	const float NewValue = QTable(In.StateIndex, In.ActionIndex) + Alpha * (Target - Predict);
	UE_LOG(LogTemp, Display, TEXT("New QTable Value for (%d, %d): %f"), In.StateIndex, In.ActionIndex, NewValue);
	QTable(In.StateIndex, In.ActionIndex) = NewValue;
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
		for(int i = 0; i < static_cast<int>(QTable.numRows()); i++)
		{
			Row.Append(FString::SanitizeFloat(QTable(i,j)));
		}
	}
	UE_LOG(LogTemp, Display, TEXT("%s"), *Row);
}

void URLBase::SaveQTable()
{
	QTableWrapper.QTable = QTable;
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
