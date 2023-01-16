// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HttpRequestInterface.h"
#include "SaveLoadInterface.h"
#include "DefaultPlayerController.generated.h"

class ULoginWidget;
class UFPSCounterWidget;
class UScreenFadeWidget;
class UPostGameMenuWidget;
class UCrossHairWidget;
class UPlayerHUD;
class UMainMenuWidget;
class UPauseMenuWidget;
class UPopupMessageWidget;
class UCountdown;

DECLARE_DELEGATE(FOnScreenFadeToBlackFinish);

UCLASS()
class BEATSHOT_API ADefaultPlayerController : public APlayerController, public ISaveLoadInterface, public IHttpRequestInterface
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
	void ShowPostGameMenu();
	void HidePostGameMenu();
	void ShowFPSCounter();
	void HideFPSCounter();
	void FadeScreenToBlack();
	void FadeScreenFromBlack();
	bool IsPlayerHUDActive() const;
	UFUNCTION(BlueprintCallable)
	bool IsPostGameMenuActive() const;
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HandlePause();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Countdown")
	bool CountdownActive;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Countdown")
	UPlayerHUD* PlayerHUD;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Countdown")
	UCountdown* Countdown;

	FOnScreenFadeToBlackFinish OnScreenFadeToBlackFinish;
	
	bool IsCrossHairActive() const { if(CrossHair) {return true;} return false; };

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
	TSubclassOf<UFPSCounterWidget> FPSCounterClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UScreenFadeWidget> ScreenFadeClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPopupMessageWidget> PopupMessageClass;
	
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
	UScreenFadeWidget* ScreenFadeWidget;
	UPROPERTY()
	UPopupMessageWidget* PopupMessageWidget;

	bool PlayerHUDActive;
	bool PostGameMenuActive;
	
	const int32 ZOrderFadeScreen = 20;
	const int32 ZOrderFPSCounter = 19;
	
	UFUNCTION()
	void OnFadeScreenFromBlackFinish();
	UFUNCTION()
	void OnPlayerSettingsChanged(const FPlayerSettings& PlayerSettings);

public:
	
	UFUNCTION()
	void OnPostPlayerScoresResponse(const bool bDidPostScores, const ELoginState& LoginState);
};
