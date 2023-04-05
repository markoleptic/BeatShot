// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GlobalStructs.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameCustomGameMode.generated.h"

UCLASS()
class GLOBAL_API USaveGameCustomGameMode : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<FBSConfig> CustomGameModes;
};
