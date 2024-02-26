// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SaveGameCustomGameMode.h"

USaveGameCustomGameMode::USaveGameCustomGameMode() : Version(-1)
{
}

void USaveGameCustomGameMode::Serialize(FStructuredArchive::FRecord Record)
{
	Super::Serialize(Record);
	FArchive& UnderlyingArchive = Record.GetUnderlyingArchive();
	if (UnderlyingArchive.IsLoading())
	{
		UnderlyingArchive << Version;
		UE_LOG(LogTemp, Warning, TEXT("Version: %d"), Version);
	}
}

TArray<FBSConfig> USaveGameCustomGameMode::GetCustomGameModes() const
{
	return CustomGameModes;
}

FBSConfig USaveGameCustomGameMode::FindCustomGameMode(const FString& GameModeName) const
{
	for (const FBSConfig& Mode : CustomGameModes)
	{
		if (Mode.DefiningConfig.CustomGameModeName.Equals(GameModeName))
		{
			return Mode;
		}
	}
	return FBSConfig();
}

void USaveGameCustomGameMode::SaveCustomGameMode(const FBSConfig& InCustomGameMode)
{
	const int32 Index = CustomGameModes.Find(InCustomGameMode);
	if (CustomGameModes.IsValidIndex(Index))
	{
		CustomGameModes[Index] = InCustomGameMode;
	}
	else
	{
		CustomGameModes.Add(InCustomGameMode);
	}
}

int32 USaveGameCustomGameMode::RemoveCustomGameMode(const FBSConfig& InCustomGameMode)
{
	const int32 NumRemoved = CustomGameModes.Remove(InCustomGameMode);
	CustomGameModes.Shrink();
	return NumRemoved;
}

int32 USaveGameCustomGameMode::RemoveAll()
{
	const int32 NumRemoved = CustomGameModes.Num();
	CustomGameModes.Empty();
	return NumRemoved - CustomGameModes.Num();
}

bool USaveGameCustomGameMode::IsCustomGameMode(const FString& GameModeName) const
{
	for (const FBSConfig& GameMode : CustomGameModes)
	{
		if (GameMode.DefiningConfig.CustomGameModeName.Equals(GameModeName, ESearchCase::IgnoreCase))
		{
			return true;
		}
	}
	return false;
}
