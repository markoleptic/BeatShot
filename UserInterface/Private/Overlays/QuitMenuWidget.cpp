// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Overlays/QuitMenuWidget.h"
#include "Utilities/BSWidgetInterface.h"
#include "Utilities/GameModeTransitionState.h"
#include "Utilities/Buttons/BSButton.h"

void UQuitMenuWidget::SetIsPostGameModeMenuChild(const bool IsPostGameModeMenuChild)
{
	bIsPostGameMenuChild = IsPostGameModeMenuChild;
	if (bIsPostGameMenuChild)
	{
		bShouldSaveScores = false;
		Button_QuitMainMenu->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_BSButton);
		Button_QuitDesktop->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_BSButton);
		Button_QuitBack->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_BSButton);
	}
	else
	{
		Button_QuitMainMenu->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_BSButton);
		Button_QuitDesktop->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_BSButton);
		Button_QuitBack->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_BSButton);
		Button_QuitAndSave->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_BSButton);
		Button_QuitWithoutSave->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_BSButton);
		Button_SaveBack->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_BSButton);
		Button_RestartAndSave->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_BSButton);
		Button_RestartWithoutSave->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_BSButton);
		Button_RestartBack->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_BSButton);
	}
}

void UQuitMenuWidget::OnButtonClicked_BSButton(const UBSButton* Button)
{
	if (bIsPostGameMenuChild)
	{
		if (Button == Button_QuitMainMenu)
		{
			PlayFadeOutMenu();
			SetGotoMainMenuTrue();
			Quit();
		}
		else if (Button == Button_QuitDesktop)
		{
			PlayFadeOutMenu();
			SetGotoMainMenuFalse();
			Quit();
		}
		else if (Button == Button_QuitBack)
		{
			PlayFadeOutMenu();
			InitializeExit();
		}
		return;
	}

	if (Button == Button_QuitMainMenu)
	{
		PlayFadeOutMenu();
		PlayFadeInSaveMenu();
		SetGotoMainMenuTrue();
		SetSaveMenuTitleMainMenu();
	}
	else if (Button == Button_QuitDesktop)
	{
		PlayFadeOutMenu();
		PlayFadeInSaveMenu();
		SetGotoMainMenuFalse();
		SetSaveMenuTitleDesktop();
	}
	else if (Button == Button_QuitBack)
	{
		PlayFadeOutMenu();
		InitializeExit();
	}
	else if (Button == Button_QuitAndSave)
	{
		PlayFadeOutSaveMenu();
		SetShouldSaveScoresTrue();
		Quit();
	}
	else if (Button == Button_QuitWithoutSave)
	{
		PlayFadeOutSaveMenu();
		SetShouldSaveScoresFalse();
		Quit();
	}
	else if (Button == Button_SaveBack)
	{
		PlayFadeOutSaveMenu();
		PlayFadeInMenu();
	}
	else if (Button == Button_RestartAndSave)
	{
		SetShouldSaveScoresTrue();
		OnRestart();
	}
	else if (Button == Button_RestartWithoutSave)
	{
		SetShouldSaveScoresFalse();
		OnRestart();
	}
	else if (Button == Button_RestartBack)
	{
		PlayFadeOutRestartMenu();
		InitializeExit();
	}
}

void UQuitMenuWidget::Quit()
{
	if (bGotoMainMenu)
	{
		OnQuitToMainMenu();
		return;
	}
	OnQuitToDesktop();
}

void UQuitMenuWidget::OnQuitToMainMenu()
{
	FGameModeTransitionState TransitionState;
	TransitionState.TransitionState = ETransitionState::QuitToMainMenu;
	TransitionState.bSaveCurrentScores = bShouldSaveScores;
	OnGameModeStateChanged.ExecuteIfBound(TransitionState);
}

void UQuitMenuWidget::OnQuitToDesktop()
{
	if (bShouldSaveScores)
	{
		PlayFadeInSaveInProgress();
	}
	FGameModeTransitionState TransitionState;
	TransitionState.TransitionState = ETransitionState::QuitToDesktop;
	TransitionState.bSaveCurrentScores = bShouldSaveScores;
	OnGameModeStateChanged.ExecuteIfBound(TransitionState);
}

void UQuitMenuWidget::OnRestart()
{
	FGameModeTransitionState TransitionState;
	TransitionState.TransitionState = ETransitionState::Restart;
	TransitionState.bSaveCurrentScores = bShouldSaveScores;
	OnGameModeStateChanged.ExecuteIfBound(TransitionState);
}

void UQuitMenuWidget::SetSaveMenuTitleMainMenu()
{
	TextBlock_SaveMenuTitle->SetText(IBSWidgetInterface::GetWidgetTextFromKey("QM_QuitMainMenuButtonText"));
}

void UQuitMenuWidget::SetSaveMenuTitleDesktop()
{
	TextBlock_SaveMenuTitle->SetText(IBSWidgetInterface::GetWidgetTextFromKey("QM_QuitDesktopButtonText"));
}

void UQuitMenuWidget::InitializeExit()
{
	FadeOutWidgetDelegate.BindDynamic(this, &UQuitMenuWidget::RemoveFromParent);
	BindToAnimationFinished(FadeOutBackgroundBlur, FadeOutWidgetDelegate);
	PlayAnimationForward(FadeOutBackgroundBlur);
	OnExitQuitMenu.ExecuteIfBound();
}
