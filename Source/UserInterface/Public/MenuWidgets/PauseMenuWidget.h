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
	UVerticalBox* PauseScreen;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* SettingsMenu;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* FAQ;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USettingsMenuWidget* SettingsMenuWidget;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UFAQWidget* FAQWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* ResumeButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* RestartCurrentModeButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* SettingsButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* FAQButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* QuitButton;

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
	void FadeInWidget() { PlayAnimationForward(FadeInPauseMenu); }

	UFUNCTION()
	virtual void OnResumeButtonClicked();
	UFUNCTION()
	void OnRestartCurrentModeClicked();
	UFUNCTION()
	void OnSettingsButtonClicked() { SlideButtons(SettingsButton); }

	UFUNCTION()
	void OnFAQButtonClicked() { SlideButtons(FAQButton); }

	UFUNCTION()
	void OnQuitButtonClicked();
	UFUNCTION()
	void SlideQuitMenuButtonsLeft();
};
