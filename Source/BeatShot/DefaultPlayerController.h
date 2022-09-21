// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DefaultPlayerController.generated.h"

class UPostGameMenuWidget;
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
class BEATSHOT_API ADefaultPlayerController : public APlayerController
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
		void ShowPostGameMenu();
	UFUNCTION(BlueprintCallable)
		void HidePostGameMenu();
	UFUNCTION(BlueprintCallable)
		void HideCountdown();
	UFUNCTION(BlueprintCallable)
		bool IsPlayerHUDActive();
	UFUNCTION(BlueprintCallable)
		bool IsPostGameMenuActive();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Countdown")
	bool CountdownActive;

	UPROPERTY()
	UPlayerHUD* PlayerHUD;

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

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPostGameMenuWidget> PostGameMenuWidgetClass;

	UPROPERTY()
	UPostGameMenuWidget* PostGameMenuWidget;

	UPROPERTY()
	UMainMenuWidget* MainMenu;

	UPROPERTY()
	UCrosshair* Crosshair;

	UPROPERTY()
	UPauseMenu* PauseMenu;

	UPROPERTY()
	UCountdown* Countdown;

private:

	UDefaultGameInstance* GI;

	bool PlayerHUDActive;

	bool PostGameMenuActive;
};
