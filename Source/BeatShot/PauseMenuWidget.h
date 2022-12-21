// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

class UQuitMenuWidget;
/**
 * 
 */
class UVerticalBox;
class UWidgetSwitcher;
class USlideRightButton;
class USettingsMenuWidget;
class UFAQWidget;
class UTextBlock;

UCLASS()

class BEATSHOT_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()
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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USettingsMenuWidget* SettingsMenuWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UFAQWidget* FAQWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UQuitMenuWidget* QuitMenuWidget;

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

#pragma	endregion

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* PauseMenuSwitcher;
	
	/** Animation to fade in the widget */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeInPauseMenu;


private:
	UFUNCTION()
	void SlideButtons(const USlideRightButton* ActiveButton);
	UFUNCTION()
	void FadeInWidget() { PlayAnimationForward(FadeInPauseMenu); }
	UFUNCTION()
	void OnResumeButtonClicked();
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
