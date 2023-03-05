// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

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
class UCountdownWidget;

DECLARE_DELEGATE(FOnScreenFadeToBlackFinish);

UCLASS()
class BEATSHOT_API ADefaultPlayerController : public APlayerController, public ISaveLoadInterface, public IHttpRequestInterface
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

public:
	/** Sets the enabled state of the pawn */
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

	void ShowInteractInfo();
	void HideInteractInfo();
	
	UFUNCTION()
	void OnPostScoresResponseReceived(const ELoginState& LoginState);
	
	UFUNCTION(BlueprintCallable)
	bool IsPostGameMenuActive() const { return PostGameMenuActive; }
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HandlePause();

	/** Whether or not the countdown widget is active, called by the gun */
	UPROPERTY()
	bool CountdownActive;

	/** Delegate that executes when the ScreenFadeWidget completes its animation*/
	FOnScreenFadeToBlackFinish OnScreenFadeToBlackFinish;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMainMenuWidget> MainMenuClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCrossHairWidget> CrossHairClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPlayerHUD> PlayerHUDClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPauseMenuWidget> PauseMenuClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCountdownWidget> CountdownClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPostGameMenuWidget> PostGameMenuWidgetClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UFPSCounterWidget> FPSCounterClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UScreenFadeWidget> ScreenFadeClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> InteractInfoWidgetClass;

private:
	UFUNCTION()
	void OnFadeScreenFromBlackFinish();
	UFUNCTION()
	void OnPlayerSettingsChanged(const FPlayerSettings& PlayerSettings);
	
	UPROPERTY()
	UMainMenuWidget* MainMenu;
	UPROPERTY()
	UCrossHairWidget* CrossHair;
	UPROPERTY()
	UPlayerHUD* PlayerHUD;
	UPROPERTY()
	UPauseMenuWidget* PauseMenu;
	UPROPERTY()
	UCountdownWidget* Countdown;
	UPROPERTY()
	UPostGameMenuWidget* PostGameMenuWidget;
	UPROPERTY()
	UFPSCounterWidget* FPSCounter;
	UPROPERTY()
	UScreenFadeWidget* ScreenFadeWidget;
	UPROPERTY()
	UUserWidget* InteractInfoWidget;

	bool PlayerHUDActive;
	bool PostGameMenuActive;
	const int32 ZOrderFadeScreen = 20;
	const int32 ZOrderFPSCounter = 19;
};
