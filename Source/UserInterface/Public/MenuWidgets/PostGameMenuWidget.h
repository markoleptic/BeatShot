// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PostGameMenuWidget.generated.h"

class UMenuStyle;
class UWidgetSwitcher;
class UVerticalBox;
class UGameModesWidget;
class USettingsMenuWidget;
class UScoreBrowserWidget;
class UFAQWidget;
class UQuitMenuWidget;
class UBSButton;
class UMenuButton;

/** Widget displayed after a game mode has been completed */
UCLASS()
class USERINTERFACE_API UPostGameMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	
	UPROPERTY(EditDefaultsOnly, Category="PostGameMenuWidget")
	TSubclassOf<UMenuStyle> MenuStyleClass;
	
	UPROPERTY()
	const UMenuStyle* MenuStyle;

	void SetStyles();

#pragma region MenuWidgets
	
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
	UMenuButton* MenuButton_Scores;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_PlayAgain;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_GameModes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_Settings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_FAQ;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_Quit;

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
	void OnButtonClicked_BSButton(const UBSButton* Button);
	UFUNCTION()
	void SetQuitMenuButtonsInActive();
};
