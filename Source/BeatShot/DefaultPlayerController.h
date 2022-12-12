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
class UCrosshair;
class UPlayerHUD;
class USettingsMenuWidget;
class UMainMenuWidget;
class UPauseMenu;
class UCountdown;
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScreenFadeToBlackFinish);
UCLASS()
class BEATSHOT_API ADefaultPlayerController : public APlayerController
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

public:
	void SetPlayerEnabledState(bool bPlayerEnabled);
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
	void ShowPostGameMenu(bool bSavedScores);
	UFUNCTION(BlueprintCallable)
	void HidePostGameMenu();
	UFUNCTION(BlueprintCallable)
	void ShowPopupMessage();
	UFUNCTION(BlueprintCallable)
	void HidePopupMessage();
	UFUNCTION(BlueprintCallable)
	void ShowFPSCounter();
	UFUNCTION(BlueprintCallable)
	void HideFPSCounter();
	UFUNCTION(BlueprintCallable)
	void ShowLogin(bool bHasSignedIn);
	UFUNCTION(BlueprintCallable)
	void HideLogin();
	UFUNCTION(BlueprintCallable)
	void FadeScreenToBlack();
	UFUNCTION(BlueprintCallable)
	void FadeScreenFromBlack();
	UFUNCTION(BlueprintCallable)
	void FadeInLoadingScreen();
	UFUNCTION(BlueprintCallable)
	void FadeOutLoadingScreen(float LastTime);
	UFUNCTION(BlueprintCallable)
	bool IsPlayerHUDActive() const;
	UFUNCTION(BlueprintCallable)
	bool IsPostGameMenuActive() const;
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HandlePause();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HandlePostGameMenuPause(bool bShouldPause);
	UFUNCTION(BlueprintCallable)
	UPopupMessageWidget* CreatePopupMessageWidget(bool bDestroyOnClick, int32 ButtonIndex = -1);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Countdown")
	bool CountdownActive;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Countdown")
	UPlayerHUD* PlayerHUD;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Countdown")
	UCountdown* Countdown;
	UPROPERTY(BlueprintAssignable, Category = "Screen Fade")
	FOnScreenFadeToBlackFinish OnScreenFadeToBlackFinish;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ULoadingScreenWidget> LoadingScreenClass;

private:
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
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPopupMessageWidget> PopupMessageClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UFPSCounterWidget> FPSCounterClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ULoginWidget> LoginClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UScreenFadeWidget> ScreenFadeClass;
	
	UPROPERTY()
	UPopupMessageWidget* PopupMessageWidget;
	UPROPERTY()
	UPostGameMenuWidget* PostGameMenuWidget;
	UPROPERTY()
	UMainMenuWidget* MainMenu;
	UPROPERTY()
	UCrosshair* Crosshair;
	UPROPERTY()
	UPauseMenu* PauseMenu;
	UPROPERTY()
	UFPSCounterWidget* FPSCounter;
	UPROPERTY()
	ULoginWidget* LoginWidget;
	UPROPERTY()
	UDefaultGameInstance* GI;
	UPROPERTY()
	UScreenFadeWidget* ScreenFadeWidget;
	UPROPERTY()
	ULoadingScreenWidget* LoadingScreenWidget;

	bool PlayerHUDActive;
	bool PostGameMenuActive;
	
	const int32 ZOrderFadeScreen = 20;
	const int32 ZOrderFPSCounter = 19;
	
	UFUNCTION()
	void OnFadeScreenToBlackFinish();
	UFUNCTION()
	void OnFadeScreenFromBlackFinish();
	UFUNCTION()
	void OnPlayerSettingsChange(FPlayerSettings PlayerSettings);
	UFUNCTION()
	void OnPostPlayerScoresResponse(FString Message, int32 ResponseCode);
	UFUNCTION()
	void OnURLLoaded(const bool bLoadedSuccessfully);
	UFUNCTION()
	void OnLoadingScreenFadeOutFinish();
	
};
