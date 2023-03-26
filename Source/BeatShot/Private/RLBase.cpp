// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "RLBase.h"

URLBase::URLBase()
{
	EpisodeRewards = nc::NdArray<float>(1);
	QTable = nc::NdArray<float>();
}

nc::NdArray<float> URLBase::ScaleQTable(const int32 SpawnCounterRows, const int32 SpawnCounterColumns)
{
	NumColsPerScaledCol = SpawnCounterRows / ColScale;
	NumRowsPerScaledRow = SpawnCounterColumns / RowScale;
	UE_LOG(LogTemp, Display, TEXT("NumColsPerScaledCol: %d NumRowsPerScaledRow: %d"), NumColsPerScaledCol, NumRowsPerScaledRow);
	return nc::zeros<float>(nc::Shape(ColScale * RowScale, ColScale * RowScale));
}

int32 URLBase::GetQTableIndexFromSpawnCounterIndex(const int32 SpawnCounterIndex) const
{
	const float ScaledAreaSize = static_cast<float>(NumColsPerScaledCol * NumRowsPerScaledRow);
	const int32 NewIndex = floorf(static_cast<float>(SpawnCounterIndex) / ScaledAreaSize);
	UE_LOG(LogTemp, Display, TEXT("SpawnCounterIndex: %d QTableIndex: %d"), SpawnCounterIndex, NewIndex);
	return NewIndex;
}

int32 URLBase::GetSpawnCounterIndexFromQTableIndex(const int32 QTableIndex) const
{
	const int32 ScaledAreaSize = NumColsPerScaledCol * NumRowsPerScaledRow;
	const int32 Max = (QTableIndex * ScaledAreaSize + ScaledAreaSize) - 1;
	const int32 Min = (QTableIndex * ScaledAreaSize) - 1;
	const int32 RandNum = FMath::RandRange(Min, Max);
	UE_LOG(LogTemp, Display, TEXT("QTableIndex: %d Min %d Max %d Result: RandNum %d"), QTableIndex, Min, Max, RandNum);
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
	UE_LOG(LogTemp, Display, TEXT("OutSize: %d"), Out.size());
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

int32 URLBase::GetRandomAction() const
{
	const int32 MaxValue = NumColsPerScaledCol * RowScale * NumRowsPerScaledRow * ColScale;
	const int32 RandomValue = FMath::RandRange(0, MaxValue - 1);
	return RandomValue;
}

int32 URLBase::GetMaxActionIndex(const int32 QTableIndex) const
{
	return QTable.argmax(nc::Axis::COL)(0,QTableIndex);
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

	UE_LOG(LogTemp, Display, TEXT("New QTable Value for SpawnCounterIndex [%d, %d] QTableIndex [%d, %d]: %f"), In.StateIndex, In.ActionIndex, InCopy.StateIndex, InCopy.ActionIndex, NewValue);
	
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
