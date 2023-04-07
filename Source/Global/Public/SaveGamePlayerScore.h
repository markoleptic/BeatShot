// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GlobalStructs.h"
#include "GameFramework/SaveGame.h"
#include "SaveGamePlayerScore.generated.h"

UCLASS()
class GLOBAL_API USaveGamePlayerScore : public USaveGame
{
	GENERATED_BODY()

public:
	/* Array containing all saved score instances */
	UPROPERTY()
	TArray<FPlayerScore> PlayerScoreArray;

	/* Map containing common score info for each unique defining config */
	UPROPERTY()
	TMap<FBS_DefiningConfig, FCommonScoreInfo> CommonScoreInfo;
};
