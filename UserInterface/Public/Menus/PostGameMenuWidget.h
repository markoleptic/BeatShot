// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Utilities/GameModeTransitionState.h"
#include "PostGameMenuWidget.generated.h"

class USaveGamePlayerScore;
class UAudioSelectWidget;
class UMenuStyle;
class UWidgetSwitcher;
class UVerticalBox;
class UGameModeMenuWidget;
class USettingsMenuWidget;
class UScoreViewerWidget;
class UFAQWidget;
class UQuitMenuWidget;
class UBSButton;
class UMenuButton;

/** Widget displayed after a game mode has been completed. */
UCLASS()
class USERINTERFACE_API UPostGameMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;

public:
	/** Executes when the user is exiting the GameModesWidget, broadcast to GameInstance to handle transition. */
	TDelegate<void(const FGameModeTransitionState& TransitionState)> OnGameModeStateChanged;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USettingsMenuWidget* SettingsMenuWidget;

	void SetSaveGamePlayerScore(USaveGamePlayerScore* InSaveGamePlayerScore);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UScoreViewerWidget* ScoreViewerWidget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UGameModeMenuWidget* GameModesWidget;

	UPROPERTY(EditDefaultsOnly, Category = "PostGameMenuWidget")
	TSubclassOf<UQuitMenuWidget> QuitMenuWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "PostGameMenuWidget")
	TSubclassOf<UMenuStyle> MenuStyleClass;
	UPROPERTY(EditDefaultsOnly, Category = "PostGameMenuWidget")
	TSubclassOf<UAudioSelectWidget> AudioSelectClass;

	UPROPERTY()
	const UMenuStyle* MenuStyle;

	void SetStyles();

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_Scores;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_GameModes;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_Settings;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_FAQ;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UFAQWidget* FAQWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_Scores;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_PlayAgain;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_GameModes;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_Settings;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_FAQ;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_Quit;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeInWidget;

	UFUNCTION()
	void PlayFadeInWidget() { PlayAnimationForward(FadeInWidget); }

	void ShowAudioFormatSelect();

	void OnButtonClicked_BSButton(const UBSButton* Button);

	TWeakObjectPtr<UMenuButton> LastMenuButton;

	UPROPERTY()
	TObjectPtr<USaveGamePlayerScore> SaveGamePlayerScore;
};
