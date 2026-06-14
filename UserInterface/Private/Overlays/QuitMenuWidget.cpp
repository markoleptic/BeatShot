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

void UQuitMenuWidget::PlayInitialFadeInMenu()
{
	PlayAnimationReverse(FadeOutMenu);
	PlayAnimationReverse(FadeOutBackgroundBlur);
}

void UQuitMenuWidget::PlayFadeInRestartMenu()
{
	PlayAnimationReverse(FadeOutRestartMenu);
	PlayAnimationReverse(FadeOutBackgroundBlur);
}

void UQuitMenuWidget::OnButtonClicked_BSButton(const UBSButton* Button)
{
	if (Button == Button_QuitBack)
	{
		PlayFadeOutMenu();
		OnExitQuitMenu.ExecuteIfBound();
		InitializeExit();
	}
	else if (Button == Button_QuitMainMenu)
	{
		PlayFadeOutMenu();
		SetGoToMainMenu(true);
		if (bIsPostGameMenuChild)
		{
			Quit();
		}
		else
		{
			PlayFadeInSaveMenu();
			TextBlock_SaveMenuTitle->SetText(IBSWidgetInterface::GetWidgetTextFromKey("QM_QuitMainMenuButtonText"));
		}
	}
	else if (Button == Button_QuitDesktop)
	{
		PlayFadeOutMenu();
		SetGoToMainMenu(false);
		if (bIsPostGameMenuChild)
		{
			Quit();
		}
		else
		{
			PlayFadeInSaveMenu();
			TextBlock_SaveMenuTitle->SetText(IBSWidgetInterface::GetWidgetTextFromKey("QM_QuitDesktopButtonText"));
		}
	}
	else if (Button == Button_QuitAndSave)
	{
		PlayFadeOutSaveMenu();
		SetShouldSaveScores(true);
		Quit();
	}
	else if (Button == Button_QuitWithoutSave)
	{
		PlayFadeOutSaveMenu();
		SetShouldSaveScores(false);
		Quit();
	}
	else if (Button == Button_SaveBack)
	{
		PlayFadeOutSaveMenu();
		PlayFadeInMenu();
	}
	else if (Button == Button_RestartAndSave)
	{
		PlayFadeOutRestartMenu();
		SetShouldSaveScores(true);
		OnRestart();
		InitializeExit();
	}
	else if (Button == Button_RestartWithoutSave)
	{
		PlayFadeOutRestartMenu();
		SetShouldSaveScores(false);
		OnRestart();
		InitializeExit();
	}
	else if (Button == Button_RestartBack)
	{
		PlayFadeOutRestartMenu();
		OnExitQuitMenu.ExecuteIfBound();
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

void UQuitMenuWidget::InitializeExit()
{
	FadeOutWidgetDelegate.BindDynamic(this, &UQuitMenuWidget::RemoveFromParent);
	BindToAnimationFinished(FadeOutBackgroundBlur, FadeOutWidgetDelegate);
	PlayAnimationForward(FadeOutBackgroundBlur);
}

void UQuitMenuWidget::SetGoToMainMenu(const bool GoToMainMenu)
{
	bGotoMainMenu = GoToMainMenu;
}

void UQuitMenuWidget::SetShouldSaveScores(const bool ShouldSaveScores)
{
	bShouldSaveScores = ShouldSaveScores;
}
