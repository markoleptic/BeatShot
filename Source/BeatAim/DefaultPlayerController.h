// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DefaultPlayerController.generated.h"

class UDefaultGameInstance;
class UCrosshair;
class UPlayerHUD;
class USettingsMenuWidget;
class UMainMenuWidget;
class UPauseMenu;
class UCountdown;
/**
 * 
 */
UCLASS()
class BEATAIM_API ADefaultPlayerController : public APlayerController
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

public:

	void setPlayerEnabledState(bool bPlayerEnabled);

	// Make BlueprintCallable for testing
	UFUNCTION(BlueprintCallable)
		void ShowMainMenu();
	UFUNCTION(BlueprintCallable)
		void HideMainMenu();
	UFUNCTION(BlueprintCallable)
		void ShowPauseMenu();
	UFUNCTION(BlueprintCallable)
		void HidePauseMenu();
	UFUNCTION(BlueprintCallable)
		void ShowCrosshair();
	UFUNCTION(BlueprintCallable)
		void HideCrosshair();
	UFUNCTION(BlueprintCallable)
		void ShowPlayerHUD();
	UFUNCTION(BlueprintCallable)
		void HidePlayerHUD();
	UFUNCTION(BlueprintCallable)
		void ShowCountdown();
	UFUNCTION(BlueprintCallable)
		void HideCountdown();
	UFUNCTION(BlueprintCallable)
		bool IsPlayerHUDActive();

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMainMenuWidget> MainMenuClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCrosshair> CrosshairClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPlayerHUD> PlayerHUDClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPauseMenu> PauseMenuClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCountdown> CountdownClass;

	UPROPERTY()
	UMainMenuWidget* MainMenu;

	UPROPERTY()
	UCrosshair* Crosshair;

	UPROPERTY()
	UPlayerHUD* PlayerHUD;

	UPROPERTY()
	UPauseMenu* PauseMenu;

	UPROPERTY()
	UCountdown* Countdown;

private:

	UDefaultGameInstance* GI;

	bool PlayerHUDActive;
};
