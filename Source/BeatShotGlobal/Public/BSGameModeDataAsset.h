// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GlobalStructs.h"
#include "Engine/DataAsset.h"
#include "BSGameModeDataAsset.generated.h"

UCLASS(Const, Abstract, Blueprintable, BlueprintType)
class BEATSHOTGLOBAL_API UBSGameModeDataAsset : public UDataAsset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FBSConfig GameModeConfig;
};
