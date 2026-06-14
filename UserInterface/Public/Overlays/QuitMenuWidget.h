// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "QuitMenuWidget.generated.h"

struct FGameModeTransitionState;
class UTextBlock;
class UVerticalBox;
class UOverlay;
class UBSButton;
class UWidgetAnimation;
class UScreenFadeWidget;

/** Provides a quit menu to other widgets. */
UCLASS()
class USERINTERFACE_API UQuitMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetIsPostGameModeMenuChild(bool IsPostGameModeMenuChild);

	/** Fades in the MenuOverlay and fades in the background blur. */
	void PlayInitialFadeInMenu();

	/** Fades in the RestartOverlay and fades in the background blur. */
	void PlayFadeInRestartMenu();

	/** Executed if the user exits the quit menu using a cancel button. */
	TDelegate<void()> OnExitQuitMenu;

	/** Executed when the user has elected to restart, quit to the main menu, or quit to desktop. */
	TDelegate<void(const FGameModeTransitionState& TransitionState)> OnGameModeStateChanged;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UOverlay* Overlay_QuitMenuSwitcher;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UOverlay* Overlay_Menu;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UOverlay* Overlay_SaveMenu;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UOverlay* Overlay_Restart;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_QuitMainMenu;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_QuitDesktop;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_QuitBack;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_QuitAndSave;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_QuitWithoutSave;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_SaveBack;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_RestartAndSave;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_RestartWithoutSave;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_RestartBack;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_SaveMenuTitle;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutMenu;
	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutSaveMenu;
	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutRestartMenu;
	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutBackgroundBlur;

	/** Delegate used to bind RemoveFromParent to FadeOutBackgroundBlur. */
	FWidgetAnimationDynamicEvent FadeOutWidgetDelegate;

private:
	void OnButtonClicked_BSButton(const UBSButton* Button);

	/** Fades in the MenuOverlay. */
	void PlayFadeInMenu() { PlayAnimationReverse(FadeOutMenu); }

	/** Fades out the MenuOverlay. */
	void PlayFadeOutMenu() { PlayAnimationForward(FadeOutMenu); }

	/** Fades in the SaveMenuOverlay. */
	void PlayFadeInSaveMenu() { PlayAnimationReverse(FadeOutSaveMenu); }

	/** Fades out the SaveMenuOverlay. */
	void PlayFadeOutSaveMenu() { PlayAnimationForward(FadeOutSaveMenu); }

	/** Fades out the RestartOverlay. */
	void PlayFadeOutRestartMenu() { PlayAnimationForward(FadeOutRestartMenu); }

	/** Called when either QuitAndSaveButton or Button_QuitWithoutSave is clicked. */
	void Quit();

	/** Called from Quit() if bGotoMainMenu is true. */
	void OnQuitToMainMenu();

	/** Called from Quit() if bGotoMainMenu is false. */
	void OnQuitToDesktop();

	/** Called when either Button_RestartAndSave or Button_RestartWithoutSave is clicked. */
	void OnRestart();

	/** Fades out the background blur. The widget will be removed from parent when the animation is completed. */
	void InitializeExit();

	void SetGoToMainMenu(bool GoToMainMenu);

	void SetShouldSaveScores(bool ShouldSaveScores);

	/** Whether this instance of QuitMenu belongs to The PostGameMenuWidget or not. */
	bool bIsPostGameMenuChild;

	/** Whether to save scores, used as argument when calling EndGameMode() from DefaultGameMode. */
	bool bShouldSaveScores;

	/** Whether to go to the MainMenuWidget vs exiting to desktop. Used in Quit(). */
	bool bGotoMainMenu;
};
