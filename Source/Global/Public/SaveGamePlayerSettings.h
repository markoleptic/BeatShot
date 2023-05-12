// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GlobalStructs.h"
#include "GameFramework/SaveGame.h"
#include "SaveGamePlayerSettings.generated.h"

UCLASS()
class GLOBAL_API USaveGamePlayerSettings : public USaveGame
{     
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FPlayerSettings PlayerSettings;
};
