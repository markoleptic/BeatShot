// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSConstants.h"
#include "InputCoreTypes.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "PlayerSettings_User.generated.h"

/** User settings. */
USTRUCT(BlueprintType)
struct BEATSHOTGLOBAL_API FPlayerSettings_User
{
	GENERATED_BODY()

	/** Sensitivity of Character. */
	UPROPERTY(BlueprintReadOnly)
	float Sensitivity;

	UPROPERTY(BlueprintReadOnly)
	FString UserID;

	UPROPERTY(BlueprintReadOnly)
	FString DisplayName;

	UPROPERTY(BlueprintReadOnly)
	FString RefreshCookie;

	UPROPERTY(BlueprintReadOnly)
	bool bNightModeUnlocked;

	UPROPERTY(BlueprintReadOnly)
	bool bHasRanBenchmark;

	UPROPERTY(BlueprintReadOnly)
	TMap<FName, FKey> Keybindings;

	FPlayerSettings_User()
	{
		Sensitivity = Constants::DefaultSensitivity;
		UserID = FString();
		RefreshCookie = FString();
		bNightModeUnlocked = false;
		bHasRanBenchmark = false;
		Keybindings = TMap<FName, FKey>();
	}

	/** @return an array of Keybindings for use with UEnhancedInputUserSettings. Empties the Keybindings map */
	TArray<FMapPlayerKeyArgs> GetLegacyKeybindings()
	{
		TArray<FMapPlayerKeyArgs> Out;
		for (const TPair<FName, FKey>& Keybinding : Keybindings)
		{
			FGameplayTagContainer Failure;
			FMapPlayerKeyArgs Args;
			Args.NewKey = Keybinding.Value;

			FString StringKey = Keybinding.Key.ToString();
			if (StringKey.Len() > 2 && StringKey.EndsWith("_2"))
			{
				StringKey = StringKey.LeftChop(2);
				FString Last;
				Last.AppendChar(StringKey[StringKey.Len() - 1]);
				if (!Last.IsNumeric())
				{
					Args.MappingName = FName(StringKey);
					Args.Slot = EPlayerMappableKeySlot::Second;
				}
			}
			else
			{
				Args.MappingName = Keybinding.Key;
				Args.Slot = EPlayerMappableKeySlot::First;
			}
			Out.Add(Args);
		}
		Keybindings.Empty();
		return Out;
	}
};
