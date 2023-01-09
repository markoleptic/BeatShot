// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveGamePlayerSettings.generated.h"

// Used to load and save player settings
USTRUCT(BlueprintType)
struct FPlayerSettings
{
	GENERATED_USTRUCT_BODY()

	// Sensitivity of DefaultCharacter
	UPROPERTY(BlueprintReadOnly)
	float Sensitivity;

	// GlobalVolume, which also affects Menu and Music volume
	UPROPERTY(BlueprintReadOnly)
	float GlobalVolume;

	// Volume of the Main Menu Music
	UPROPERTY(BlueprintReadOnly)
	float MenuVolume;

	// Volume of the AudioAnalyzer Tracker
	UPROPERTY(BlueprintReadOnly)
	float MusicVolume;

	UPROPERTY(BlueprintReadOnly)
	FString Username;

	UPROPERTY(BlueprintReadOnly)
	bool HasLoggedInHttp;

	UPROPERTY(BlueprintReadOnly)
	FString LoginCookie;

	UPROPERTY(BlueprintReadOnly)
	int32 LineWidth;

	UPROPERTY(BlueprintReadOnly)
	int32 LineLength;

	UPROPERTY(BlueprintReadOnly)
	int32 InnerOffset;

	UPROPERTY(BlueprintReadOnly)
	FLinearColor CrossHairColor;

	UPROPERTY(BlueprintReadOnly)
	float OutlineOpacity;

	UPROPERTY(BlueprintReadOnly)
	int32 OutlineWidth;

	UPROPERTY(BlueprintReadOnly)
	bool bShouldRecoil;

	UPROPERTY(BlueprintReadOnly)
	bool bAutomaticFire;

	UPROPERTY(BlueprintReadOnly)
	bool bShowBulletDecals;

	UPROPERTY(BlueprintReadOnly)
	bool bNightModeSelected;

	UPROPERTY(BlueprintReadOnly)
	bool bShowFPSCounter;

	UPROPERTY(BlueprintReadOnly)
	int32 FrameRateLimitMenu;

	UPROPERTY(BlueprintReadOnly)
	int32 FrameRateLimitGame;

	UPROPERTY(BlueprintReadOnly)
	bool bShowStreakCombatText;

	UPROPERTY(BlueprintReadOnly)
	int32 CombatTextFrequency;

	FPlayerSettings()
	{
		Sensitivity = 0.3f;
		GlobalVolume = 50.f;
		MenuVolume = 50.f;
		MusicVolume = 10.f;
		HasLoggedInHttp = false;
		Username = "";
		LoginCookie = "";
		LineWidth = 4;
		LineLength = 10;
		InnerOffset = 6;
		CrossHairColor = FLinearColor(63.f / 255.f, 199.f / 255.f, 235.f / 255.f, 1.f);
		OutlineOpacity = 1.f;
		OutlineWidth = 20;
		bShouldRecoil = true;
		bAutomaticFire = true;
		bShowBulletDecals = true;
		bNightModeSelected = false;
		bShowFPSCounter = false;
		FrameRateLimitMenu = 144;
		FrameRateLimitGame = 0;
		bShowStreakCombatText = true;
		CombatTextFrequency = 5;
	}

	void ResetStruct()
	{
		Sensitivity = 0.3f;
		GlobalVolume = 50.f;
		MenuVolume = 50.f;
		MusicVolume = 10.f;
		HasLoggedInHttp = false;
		Username = "";
		LoginCookie = "";
		LineWidth = 4;
		LineLength = 10;
		InnerOffset = 6;
		CrossHairColor = FLinearColor(63.f / 255.f, 199.f / 255.f, 235.f / 255.f, 1.f);
		OutlineOpacity = 1.f;
		OutlineWidth = 20;
		bShouldRecoil = true;
		bAutomaticFire = true;
		bShowBulletDecals = true;
		bNightModeSelected = false;
		bShowFPSCounter = false;
		FrameRateLimitMenu = 144;
		FrameRateLimitGame = 0;
		bShowStreakCombatText = true;
		CombatTextFrequency = 5;
	}

	void ResetVideoAndSoundSettings()
	{
		GlobalVolume = 50.f;
		MenuVolume = 50.f;
		MusicVolume = 10.f;
		FrameRateLimitMenu = 144;
		FrameRateLimitGame = 0;
		bShowStreakCombatText = true;
		CombatTextFrequency = 5;
	}

	void ResetCrossHair()
	{
		LineWidth = 4;
		LineLength = 10;
		InnerOffset = 6;
		CrossHairColor = FLinearColor(63.f / 255.f, 199.f / 255.f, 235.f / 255.f, 1.f);
		OutlineOpacity = 1.f;
		OutlineWidth = 20;
	}
};

UCLASS()
class BEATSHOT_API USaveGamePlayerSettings : public USaveGame
{
	GENERATED_BODY()

public:
	USaveGamePlayerSettings();

	UPROPERTY(BlueprintReadOnly)
	FPlayerSettings PlayerSettings;
};
