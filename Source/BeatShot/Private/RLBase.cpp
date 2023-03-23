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
			QTableWrapper = QTables[Index];
			QTable = QTableWrapper.QTable;
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

void URLBase::BeginDestroy()
{
	SaveQTable();
	UObject::BeginDestroy();
}


// nc::NdArray<float> Table = nc::zeros<float>(nc::Shape(2, 10));
// Table(0,0) = 0;
// Table(0,1) = 0;
// Table(0,2) = 3;
// Table(0,3) = 0;
// Table(0,4) = 0;
// Table(0,5) = 0;
// Table(0,6) = 0;
// Table(0,7) = 0;
// Table(0,8) = 0;
// Table(0,9) = 15;
// Table(1,0) = 0;
// Table(1,1) = 0;
// Table(1,2) = 0;
// Table(1,3) = 0;
// Table(1,4) = 0;
// Table(1,5) = 0;
// Table(1,6) = 4;
// Table(1,7) = 0;
// Table(1,8) = 8;
// Table(1,9) = 0;
	
// auto Max = Table.max(nc::Axis::ROW);
// for (auto Hey : Max)
// {
// 	UE_LOG(LogTemp, Display, TEXT("maxROW: %f"), Hey);
// }
// auto Max2 = Table.max(nc::Axis::COL);
// for (auto Hey : Max2)
// {
// 	UE_LOG(LogTemp, Display, TEXT("MaxCOL: %f"), Hey);
// }
// auto MaxRow = Table.argmax(nc::Axis::ROW);
// for (auto Hey : MaxRow)
// {
// 	UE_LOG(LogTemp, Display, TEXT("argmaxROW: %u"), Hey);
// }
// auto MaxCol = Table.argmax(nc::Axis::COL);
// for (auto Hey : MaxCol)
// {
// 	UE_LOG(LogTemp, Display, TEXT("argmaxCOL: %u"), Hey);
// }
// return 0;