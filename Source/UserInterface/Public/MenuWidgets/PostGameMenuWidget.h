// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SubMenuWidgets/GameModesWidget.h"
#include "SubMenuWidgets/ScoreBrowserWidget.h"
#include "PostGameMenuWidget.generated.h"

class USlideRightButton;
class UWidgetSwitcher;
class UVerticalBox;
class UGameModesWidget;
class USettingsMenuWidget;
class UScoreBrowserWidget;
class UFAQWidget;
class UQuitMenuWidget;

/** Widget displayed after a game mode has been completed */
UCLASS()
class USERINTERFACE_API UPostGameMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

#pragma region MenuWidgets

	/** A map to store buttons and the widgets they associate with */
	UPROPERTY()
	TMap<USlideRightButton*, UVerticalBox*> MenuWidgets;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_Scores;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_GameModes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_Settings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_FAQ;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UScoreBrowserWidget* ScoresWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UGameModesWidget* GameModesWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UQuitMenuWidget* QuitMenuWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USettingsMenuWidget* SettingsMenuWidget;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UFAQWidget* FAQWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* SlideRightButton_Scores;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* SlideRightButton_PlayAgain;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* SlideRightButton_GameModes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* SlideRightButton_Settings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* SlideRightButton_FAQ;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* SlideRightButton_Quit;

#pragma	endregion

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeInWidget;
	UFUNCTION()
	void PlayFadeInWidget() { PlayAnimationForward(FadeInWidget); }

	bool bSavedScores;
	/** Delegate used to bind CollapseWidget to FadeOutBackgroundBlur */
	FWidgetAnimationDynamicEvent FadeInWidgetDelegate;

	UFUNCTION()
	virtual void Restart();
	UFUNCTION()
	void SetScoresWidgetVisibility();
	UFUNCTION()
	void SlideButtons(const USlideRightButton* ActiveButton);
	UFUNCTION()
	void SlideQuitMenuButtonsLeft();
	UFUNCTION()
	void OnButtonClicked_Scores();
	UFUNCTION()
	void OnButtonClicked_GameModes();
	UFUNCTION()
	void OnButtonClicked_Settings();
	UFUNCTION()
	void OnButtonClicked_FAQ();
	UFUNCTION()
	void OnButtonClicked_Quit();
	UFUNCTION()
	void OnLoginStateChange(const ELoginState& LoginState, bool bIsPopup);
};
