// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnumStringMap.h"
#include "GameModeAndDifficultyStringMap.generated.h"


enum class EBaseGameMode : uint8;

UCLASS()
class BEATSHOTGLOBAL_API UGameModeAndDifficultyStringMap : public UEnumStringMap
{
	GENERATED_BODY()

public:
	UGameModeAndDifficultyStringMap();

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<EBaseGameMode, FText> GameModeDescriptions;
};
