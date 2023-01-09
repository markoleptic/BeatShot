// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ScreenFadeWidget.h"
#include "FPSCounterWidget.h"
#include "PopupMessageWidget.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "SaveGamePlayerSettings.h"
#include "GameFramework/PlayerController.h"
#include "DefaultPlayerController.generated.h"

class ULoadingScreenWidget;
class ULoginWidget;
class UPostGameMenuWidget;
class UDefaultGameInstance;
class UCrossHairWidget;
class UPlayerHUD;
class USettingsMenuWidget;
class UMainMenuWidget;
class UPauseMenuWidget;
class UCountdown;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScreenFadeToBlackFinish);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPopupMessageButtonOneClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPopupMessageButtonTwoClicked);

UCLASS()
class BEATSHOT_API ADefaultPlayerController : public APlayerController
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

public:
	void SetPlayerEnabledState(const bool bPlayerEnabled);
	UFUNCTION(BlueprintCallable)
	void ShowMainMenu();
	UFUNCTION(BlueprintCallable)
	void HideMainMenu();
	UFUNCTION(BlueprintCallable)
	void ShowPauseMenu();
	UFUNCTION(BlueprintCallable)
	void HidePauseMenu();
	void ShowCrossHair();
	void HideCrossHair();
	void ShowPlayerHUD();
	void HidePlayerHUD();
	void ShowCountdown();
	void HideCountdown();
	void ShowPostGameMenu(const bool bSavedScores);
	void HidePostGameMenu();
	void HidePopupMessage();
	void ShowFPSCounter();
	void HideFPSCounter();
	void FadeScreenToBlack();
	void FadeScreenFromBlack();
	bool IsPlayerHUDActive() const;
	UFUNCTION(BlueprintCallable)
	bool IsPostGameMenuActive() const;
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HandlePause();
	UFUNCTION(BlueprintCallable)
	UPopupMessageWidget* CreatePopupMessageWidget(const bool bDestroyOnClick, const int32 ButtonIndex = -1);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Countdown")
	bool CountdownActive;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Countdown")
	UPlayerHUD* PlayerHUD;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Countdown")
	UCountdown* Countdown;
	UPROPERTY(BlueprintAssignable, Category = "Screen Fade")
	FOnScreenFadeToBlackFinish OnScreenFadeToBlackFinish;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMainMenuWidget> MainMenuClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCrossHairWidget> CrossHairClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPlayerHUD> PlayerHUDClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPauseMenuWidget> PauseMenuClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCountdown> CountdownClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPostGameMenuWidget> PostGameMenuWidgetClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPopupMessageWidget> PopupMessageClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UFPSCounterWidget> FPSCounterClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UScreenFadeWidget> ScreenFadeClass;
	
	UPROPERTY()
	UPopupMessageWidget* PopupMessageWidget;
	UPROPERTY()
	UPostGameMenuWidget* PostGameMenuWidget;
	UPROPERTY()
	UMainMenuWidget* MainMenu;
	UPROPERTY()
	UCrossHairWidget* CrossHair;
	UPROPERTY()
	UPauseMenuWidget* PauseMenu;
	UPROPERTY()
	UFPSCounterWidget* FPSCounter;
	UPROPERTY()
	UDefaultGameInstance* GI;
	UPROPERTY()
	UScreenFadeWidget* ScreenFadeWidget;

	bool PlayerHUDActive;
	bool PostGameMenuActive;
	
	const int32 ZOrderFadeScreen = 20;
	const int32 ZOrderFPSCounter = 19;

public:

	/** Called by PopupMessageWidget when the FadeOut animation is complete */
	UFUNCTION()
	void OnFadeOutPopupMessageFinish();
	
private:
	UFUNCTION()
	void OnFadeScreenToBlackFinish();
	UFUNCTION()
	void OnFadeScreenFromBlackFinish();
	UFUNCTION()
	void OnPlayerSettingsChange(const FPlayerSettings& PlayerSettings);
	UFUNCTION()
	void OnPostPlayerScoresResponse(const ELoginState& LoginState);
};
