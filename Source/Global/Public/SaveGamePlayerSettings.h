// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveGamePlayerSettings.generated.h"

/* Simple login payload */
USTRUCT(BlueprintType)
struct FLoginPayload
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Login")
	FString Username;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Login")
	FString Email;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Login")
	FString Password;

	FLoginPayload()
	{
		Username = "";
		Email = "";
		Password = "";
	}

	FLoginPayload(const FString& InUsername, const FString& InEmail, const FString& InPassword)
	{
		Username = InUsername;
		Email = InEmail;
		Password = InPassword;
	}
};

/* Used to load and save player settings */
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

	UPROPERTY(BlueprintReadWrite)
	bool bShouldRecoil;

	UPROPERTY(BlueprintReadWrite)
	bool bAutomaticFire;

	UPROPERTY(BlueprintReadWrite)
	bool bShowBulletDecals;

	UPROPERTY(BlueprintReadOnly)
	bool bNightModeUnlocked;

	UPROPERTY(BlueprintReadWrite)
	bool bNightModeSelected;

	UPROPERTY(BlueprintReadWrite)
	bool bShowLightVisualizers;

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

	UPROPERTY(BlueprintReadOnly)
	FString LastSelectedInputAudioDevice;

	UPROPERTY(BlueprintReadOnly)
	FString LastSelectedOutputAudioDevice;

	UPROPERTY(BlueprintReadOnly)
	FLinearColor PeakTargetColor;

	UPROPERTY(BlueprintReadOnly)
	FLinearColor FadeTargetColor;

	UPROPERTY(BlueprintReadOnly)
	bool bUseSeparateOutlineColor;

	UPROPERTY(BlueprintReadOnly)
	FLinearColor TargetOutlineColor;

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
		bNightModeUnlocked = false;
		bNightModeSelected = false;
		bShowLightVisualizers = false;
		bShowFPSCounter = false;
		FrameRateLimitMenu = 144;
		FrameRateLimitGame = 0;
		bShowStreakCombatText = true;
		CombatTextFrequency = 5;
		LastSelectedInputAudioDevice = "";
		LastSelectedOutputAudioDevice = "";
		PeakTargetColor = FLinearColor::Green;
		FadeTargetColor = FLinearColor::Red;
		bUseSeparateOutlineColor = false;
		TargetOutlineColor = FLinearColor::White;
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
		bNightModeUnlocked = false;
		bNightModeSelected = false;
		bShowLightVisualizers = false;
		bShowFPSCounter = false;
		FrameRateLimitMenu = 144;
		FrameRateLimitGame = 0;
		bShowStreakCombatText = true;
		CombatTextFrequency = 5;
		LastSelectedInputAudioDevice = "";
		LastSelectedOutputAudioDevice = "";
		PeakTargetColor = FLinearColor::Green;
		FadeTargetColor = FLinearColor::Red;
		bUseSeparateOutlineColor = false;
		TargetOutlineColor = FLinearColor::White;
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
		PeakTargetColor = FLinearColor::Green;
		FadeTargetColor = FLinearColor::Red;
		bUseSeparateOutlineColor = false;
		TargetOutlineColor = FLinearColor::White;
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

/* Used by AASettings widget to relay Audio Analyzer settings to DefaultGameInstance */
USTRUCT(BlueprintType)
struct FAASettingsStruct
{
	GENERATED_BODY()

	// Number of channels to break Tracker Sound frequencies into
	UPROPERTY(BlueprintReadOnly)
	int NumBandChannels;

	// Array to store Threshold values for each active band channel
	UPROPERTY(BlueprintReadOnly)
	TArray<float> BandLimitsThreshold;

	// Array to store band frequency channels
	UPROPERTY(BlueprintReadOnly)
	TArray<FVector2D> BandLimits;

	// Time window to take frequency sample
	UPROPERTY(BlueprintReadOnly)
	float TimeWindow;

	// History size of frequency sample
	UPROPERTY(BlueprintReadOnly)
	int HistorySize;

	// Max number of band channels allowed
	int32 MaxNumBandChannels = 32;

	FAASettingsStruct()
	{
		BandLimits = {
			FVector2d(0,44),
			FVector2d(45,88),
			FVector2d(89,177),
			FVector2d(178,355),
			FVector2d(356,710),
			FVector2d(711,1420),
			FVector2d(1421,2840),
			FVector2d(2841,5680),
			FVector2d(5681,11360),
			FVector2d(11361,22720),
		};
		BandLimitsThreshold = {2.1,2.1,2.1,2.1,2.1,2.1,2.1,2.1,2.1,2.1};
		NumBandChannels = 10;
		TimeWindow = 0.02f;
		HistorySize = 30.f;
		MaxNumBandChannels = 32;
	}

	void ResetStruct()
	{
		BandLimits = {
			FVector2d(0,44),
			FVector2d(45,88),
			FVector2d(89,177),
			FVector2d(178,355),
			FVector2d(356,710),
			FVector2d(711,1420),
			FVector2d(1421,2840),
			FVector2d(2841,5680),
			FVector2d(5681,11360),
			FVector2d(11361,22720),
		};
		BandLimitsThreshold = {2.1,2.1,2.1,2.1,2.1,2.1,2.1,2.1,2.1,2.1};
		NumBandChannels = 10;
		TimeWindow = 0.02f;
		HistorySize = 30.f;
		MaxNumBandChannels = 32;
	}
};

UENUM()
enum class ELoginState : uint8
{
	None UMETA(DisplayName, "None"),
	NewUser UMETA(DisplayName, "NewUser"),
	LoggedInHttp UMETA(DisplayName, "LoggedInHttp"),
	LoggedInHttpAndBrowser UMETA(DisplayName, "LoggedInHttpAndBrowser"),
	InvalidHttp UMETA(DisplayName, "InvalidHttp"),
	InvalidBrowser UMETA(DisplayName, "InvalidBrowser"),
	InvalidCredentials UMETA(DisplayName, "InvalidCredentials"),
	TimeOut UMETA(DisplayName, "TimeOut"),
};

ENUM_RANGE_BY_FIRST_AND_LAST(ELoginState, ELoginState::None, ELoginState::TimeOut);

UCLASS()
class GLOBAL_API USaveGamePlayerSettings : public USaveGame
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly)
	FPlayerSettings PlayerSettings;

	UPROPERTY(BlueprintReadOnly)
	FAASettingsStruct AASettings;
};
