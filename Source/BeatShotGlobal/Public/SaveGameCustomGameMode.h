// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSGameModeDataAsset.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameCustomGameMode.generated.h"

UCLASS()
class BEATSHOTGLOBAL_API USaveGameCustomGameMode : public USaveGame
{
	GENERATED_BODY()

public:
	/** Returns a copy of CustomGameModes */
	TArray<FBSConfig> GetCustomGameModes() const;

	/** Returns a copy of the CustomGameMode matching the CustomGameModeName, or default struct if not found */
	FBSConfig FindCustomGameMode(const FString& GameModeName) const;

	/** Adds a new entry to CustomGameModes or overwrites one if found with matching FBS_DefiningConfig */
	void SaveCustomGameMode(const FBSConfig& InCustomGameMode);

	/** Removes all instances InCustomGameModeToRemove from the CustomGameModes array, returning the number of removed elements */
	int32 RemoveCustomGameMode(const FBSConfig& InCustomGameMode);

	/** Empties the CustomGameModes array, returning the number of removed elements */
	int32 RemoveAll();

	/** Returns true if there is a CustomGameMode matching the GameModeName */
	bool IsCustomGameMode(const FString& GameModeName) const;

private:
	UPROPERTY()
	TArray<FBSConfig> CustomGameModes;
};
