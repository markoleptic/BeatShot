// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GlobalStructs.h"
#include "Engine/DataAsset.h"
#include "BSGameModeDataAsset.generated.h"

UCLASS(Blueprintable, BlueprintType)
class BEATSHOTGLOBAL_API UBSGameModeDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UBSGameModeDataAsset()
	{
		for (const EBaseGameMode& GameMode : TEnumRange<EBaseGameMode>())
		{
			for (const EGameModeDifficulty& Difficulty : TEnumRange<EGameModeDifficulty>())
			{
				if (Difficulty == EGameModeDifficulty::None)
				{
					continue;
				}
				FBSConfig TempConfig = FBSConfig::MakePresetConfig(GameMode, Difficulty);
				DefaultGameModes.Add(TempConfig.DefiningConfig, TempConfig);
			}
		}
	}

	/** Returns all FBSConfig structs representing the default/preset game modes */
	TArray<FBSConfig> GetDefaultGameModes() const
	{
		TArray<FBSConfig> ReturnArray;
		for (const TTuple<FBS_DefiningConfig, FBSConfig>& KeyValue : DefaultGameModes)
		{
			ReturnArray.Add(KeyValue.Value);
		}
		return ReturnArray;
	}

	TMap<FBS_DefiningConfig, FBSConfig> GetDefaultGameModesMap() const
	{
		return DefaultGameModes;
	}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ShowOnlyInnerProperties))
	TMap<FBS_DefiningConfig, FBSConfig> DefaultGameModes;
};

