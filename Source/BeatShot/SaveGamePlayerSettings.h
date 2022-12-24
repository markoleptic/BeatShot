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
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		float Sensitivity;

	// GlobalVolume, which also affects Menu and Music volume
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		float GlobalVolume;

	// Volume of the Main Menu Music
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		float MenuVolume;

	// Volume of the AudioAnalyzer Tracker
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		float MusicVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Login")
		FString Username;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Login")
		bool HasLoggedInHttp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Login")
		FString LoginCookie;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair")
		int32 LineWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair")
		int32 LineLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair")
		int32 InnerOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair")
		FLinearColor CrosshairColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair")
		float OutlineOpacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair")
		int32 OutlineWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gunplay")
		bool bShouldRecoil;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gunplay")
		bool bAutomaticFire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
		bool bShowBulletDecals;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
		bool bNightModeSelected;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		bool bShowFPSCounter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		int32 FrameRateLimitMenu;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		int32 FrameRateLimitGame;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		bool bShowStreakCombatText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
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
		CrosshairColor = FLinearColor(63.f / 255.f, 199.f / 255.f, 235.f / 255.f, 1.f);
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
		CrosshairColor = FLinearColor(63.f / 255.f, 199.f / 255.f, 235.f / 255.f, 1.f);
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

	void ResetSettings()
	{
		GlobalVolume = 50.f;
		MenuVolume = 50.f;
		MusicVolume = 10.f;
		FrameRateLimitMenu = 144;
		FrameRateLimitGame = 0;
		bShowStreakCombatText = true;
		CombatTextFrequency = 5;
	}
};
UCLASS()
class BEATSHOT_API USaveGamePlayerSettings : public USaveGame
{
	GENERATED_BODY()

public:
	USaveGamePlayerSettings();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FPlayerSettings PlayerSettings;
};
