// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OverlayWidgets/QuitMenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

DECLARE_DELEGATE(FResumeGame);

class UQuitMenuWidget;
class UVerticalBox;
class UWidgetSwitcher;
class USlideRightButton;
class USettingsMenuWidget;
class UFAQWidget;
class UTextBlock;

/** Widget displayed if the Escape key is pressed, or the SteamOverlay is activated */
UCLASS()
class USERINTERFACE_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Delegate that gets bound to lambda function in DefaultPlayerController */
	FResumeGame ResumeGame;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UQuitMenuWidget* QuitMenuWidget;

protected:
	virtual void NativeConstruct() override;

#pragma region MenuWidgets

	/** A map to store buttons and the widgets they associate with */
	UPROPERTY()
	TMap<USlideRightButton*, UVerticalBox*> PauseMenuWidgets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_PauseScreen;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_SettingsMenu;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_FAQ;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USettingsMenuWidget* SettingsMenuWidget;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UFAQWidget* FAQWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* SlideRightButton_Resume;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* SlideRightButton_RestartCurrentMode;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* SlideRightButton_Settings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* SlideRightButton_FAQ;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* SlideRightButton_Quit;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* PauseMenuSwitcher;
	/** Animation to fade in the widget */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeInPauseMenu;

#pragma	endregion

private:
	UFUNCTION()
	void SlideButtons(const USlideRightButton* ActiveButton);
	UFUNCTION()
	void OnSettingsButtonClicked() { SlideButtons(SlideRightButton_Settings); }
	UFUNCTION()
	void OnFAQButtonClicked() { SlideButtons(SlideRightButton_FAQ); }
	UFUNCTION()
	void FadeInWidget() { PlayAnimationForward(FadeInPauseMenu); }
	UFUNCTION()
	void SlideQuitMenuButtonsLeft();
	
	UFUNCTION()
	void OnButtonClicked_Resume();
	UFUNCTION()
	void OnButtonClicked_RestartCurrentMode();
	UFUNCTION()
	void OnButtonClicked_Quit();
};
