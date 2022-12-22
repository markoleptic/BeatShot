// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Delegates/DelegateCombinations.h"
#include "Delegates/DelegateCombinations.h"
#include "QuitMenuWidget.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnExitQuitMenu);
/**
 * 
 */
class UTextBlock;
class UVerticalBox;
class UOverlay;
class UButton;
class UWidgetAnimation;

UCLASS()
class BEATSHOT_API UQuitMenuWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:

	/** Fades in the MenuOverlay, and also fades in the background blur */
	UFUNCTION()
	void PlayInitialFadeInMenu() { PlayAnimationReverse(FadeOutMenu); PlayAnimationReverse(FadeOutBackgroundBlur); }
	/** Fades in the RestartOverlay */
	UFUNCTION()
	void PlayFadeInRestartMenu() { PlayAnimationReverse(FadeOutRestartMenu); PlayAnimationReverse(FadeOutBackgroundBlur); }
	/** Delegate used to let PauseMenuWidget know to slide the MenuButtons to the left after exiting the menus */
	UPROPERTY()
	FOnExitQuitMenu OnExitQuitMenu;

protected:

#pragma region MenuWidgets
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UOverlay* QuitMenuSwitcher;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UOverlay* MenuOverlay;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UOverlay* SaveMenuOverlay;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UOverlay* RestartOverlay;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UButton* QuitMainMenuButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UButton* QuitDesktopButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UButton* QuitBackButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UButton* QuitAndSaveButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UButton* QuitWithoutSaveButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UButton* SaveBackButton;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UButton* RestartAndSaveButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UButton* RestartWithoutSaveButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UButton* RestartBackButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* SaveMenuTitle;

#pragma endregion

#pragma region Animations
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutMenu;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutSaveMenu;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutRestartMenu;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutBackgroundBlur;

	/** Fades in the MenuOverlay */
	UFUNCTION()
	void PlayFadeInMenu() { PlayAnimationReverse(FadeOutMenu); }
	/** Fades out the MenuOverlay */
	UFUNCTION()
	void PlayFadeOutMenu() { PlayAnimationForward(FadeOutMenu); }
	/** Fades in the SaveMenuOverlay */
	UFUNCTION()
	void PlayFadeInSaveMenu() { PlayAnimationReverse(FadeOutSaveMenu); }
	/** Fades out the SaveMenuOverlay */
	UFUNCTION()
	void PlayFadeOutSaveMenu() { PlayAnimationForward(FadeOutSaveMenu); }
	/** Fades out the RestartOverlay */
	UFUNCTION()
	void PlayFadeOutRestartMenu() { PlayAnimationForward(FadeOutRestartMenu); }
	/** Delegate used to bind CollapseWidget to FadeOutBackgroundBlur */
	FWidgetAnimationDynamicEvent FadeOutWidgetDelegate;

#pragma endregion

private:
	
	/** Called when either QuitAndSaveButton or QuitWithoutSaveButton is clicked */
	UFUNCTION()
	void Quit();
	/** Called from Quit() if bGotoMainMenu is true */
	UFUNCTION()
	void QuitToMainMenu();
	/** Handler function to hide the Pause Menu and open MainMenuLevel */
	UFUNCTION()
	void HandleQuitToMainMenu();
	/** Called from Quit() if bGotoMainMenu is false */
	UFUNCTION()
	void QuitToDesktop();
	/** Called when either RestartAndSaveButton or RestartWithoutSaveButton is clicked */
	UFUNCTION()
	void Restart();
	/** Handler function called when FadeScreenToBlack is finished which calls InitializeGameMode() from DefaultGameMode */
	UFUNCTION()
	void HandleRestart();
	/** Plays FadeOutBackgroundBlur, binds FadeOutWidgetDelegate to InitializeExit and executes OnExitQuitMenu */	
	UFUNCTION()
	void InitializeExit();
	/** Function that is bound to FadeOutBackgroundBlur to set the visibility of the widget to collapsed */
	UFUNCTION()
	void CollapseWidget();
	UFUNCTION()
	void SetGotoMainMenuTrue() { bGotoMainMenu = true; }
	UFUNCTION()
	void SetGotoMainMenuFalse() { bGotoMainMenu = false; }
	UFUNCTION()
	void SetShouldSaveScoresTrue() { bShouldSaveScores = true; }
	UFUNCTION()
	void SetShouldSaveScoresFalse() { bShouldSaveScores = false; }
	UFUNCTION()
	void SetSaveMenuTitleMainMenu() { SaveMenuTitle->SetText(FText::FromString("Quit to Main Menu")); }
	UFUNCTION()
	void SetSaveMenuTitleDesktop() { SaveMenuTitle->SetText(FText::FromString("Quit to Desktop")); }
	/** Whether or not to save scores, used as argument when calling EndGameMode() from DefaultGameMode */
	bool bShouldSaveScores;
	/** Whether or not to go to the MainMenu vs exiting to desktop. Used in Quit() */
	bool bGotoMainMenu;
};
