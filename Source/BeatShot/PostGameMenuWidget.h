// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WebBrowserOverlay.h"
#include "PostGameMenuWidget.generated.h"

class USlideRightButton;
class UWidgetSwitcher;
class UVerticalBox;
class UWebBrowserOverlay;
class UGameModesWidget;
class USettingsMenuWidget;
class UFAQWidget;
class UQuitMenuWidget;

/**
 * 
 */
UCLASS()
class BEATSHOT_API UPostGameMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

#pragma region MenuWidgets

	/** A map to store buttons and the widgets they associate with */
	UPROPERTY()
	TMap<USlideRightButton*, UVerticalBox*> MenuWidgets;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* MenuSwitcher;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Scores;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* GameModes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Settings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* FAQ;
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UWebBrowserOverlay* ScoresWidget;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UGameModesWidget* GameModesWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USettingsMenuWidget* SettingsMenuWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UFAQWidget* FAQWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UQuitMenuWidget* QuitMenuWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* ScoresButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* PlayAgainButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* GameModesButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* SettingsButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* FAQButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* QuitButton;

#pragma	endregion

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeInWidget;
	UFUNCTION()
	void PlayFadeInWidget() { PlayAnimationForward(FadeInWidget); }
	UPROPERTY(BlueprintReadOnly)
	bool bSavedScores;
	/** Delegate used to bind CollapseWidget to FadeOutBackgroundBlur */
	FWidgetAnimationDynamicEvent FadeInWidgetDelegate;

	UFUNCTION()
	void Restart();
	UFUNCTION()
	void HandleRestart();
	UFUNCTION()
	void SetScoresWidgetVisibility();
	UFUNCTION()
	void SlideButtons(const USlideRightButton* ActiveButton);
	UFUNCTION()
	void SlideQuitMenuButtonsLeft();
	UFUNCTION()
	void OnScoresButtonClicked();
	UFUNCTION()
	void OnPlayAgainButtonClicked();
	UFUNCTION()
	void OnGameModesButtonClicked();
	UFUNCTION()
	void OnSettingsButtonClicked();
	UFUNCTION()
	void OnFAQButtonClicked();
	UFUNCTION()
	void OnQuitButtonClicked();
	UFUNCTION()
	void OnLoginStateChange(bool bLoggedInHttp, bool bLoggedInBrowser, bool bIsPopup);

};
