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
		bool HasLoggedInBrowser;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Login")
		FString LoginCookie;

	FPlayerSettings()
	{
		Sensitivity = 0.3f;
		GlobalVolume = 50.f;
		MenuVolume = 50.f;
		MusicVolume = 10.f;
		HasLoggedInHttp = false;
		HasLoggedInBrowser = false;
		Username = "";
		LoginCookie = "";
	}

	void ResetStruct()
	{
		Sensitivity = 0.3f;
		GlobalVolume = 50.f;
		MenuVolume = 50.f;
		MusicVolume = 10.f;
		HasLoggedInHttp = false;
		HasLoggedInBrowser = false;
		Username = "";
		LoginCookie = "";
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
