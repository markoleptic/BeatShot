// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "OverlayWidgets/QuitMenuWidget.h"
#include "OverlayWidgets/ScreenFadeWidget.h"
#include "Components/Button.h"

void UQuitMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (bIsPostGameMenuChild)
	{
		bShouldSaveScores = false;
		QuitMainMenuButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeOutMenu);
		QuitMainMenuButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::SetGotoMainMenuTrue);
		QuitMainMenuButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::Quit);

		QuitDesktopButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeOutMenu);
		QuitDesktopButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::SetGotoMainMenuFalse);
		QuitDesktopButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::Quit);

		QuitBackButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeOutMenu);
		QuitBackButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::InitializeExit);
	}
	else
	{
		QuitMainMenuButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeOutMenu);
		QuitMainMenuButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeInSaveMenu);
		QuitMainMenuButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::SetGotoMainMenuTrue);
		QuitMainMenuButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::SetSaveMenuTitleMainMenu);

		QuitDesktopButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeOutMenu);
		QuitDesktopButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeInSaveMenu);
		QuitDesktopButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::SetGotoMainMenuFalse);
		QuitDesktopButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::SetSaveMenuTitleDesktop);

		QuitBackButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeOutMenu);
		QuitBackButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::InitializeExit);

		QuitAndSaveButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeOutSaveMenu);
		QuitAndSaveButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::SetShouldSaveScoresTrue);
		QuitAndSaveButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::Quit);

		QuitWithoutSaveButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeOutSaveMenu);
		QuitWithoutSaveButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::SetShouldSaveScoresFalse);
		QuitWithoutSaveButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::Quit);

		SaveBackButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeOutSaveMenu);
		SaveBackButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeInMenu);

		RestartAndSaveButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::SetShouldSaveScoresTrue);
		RestartAndSaveButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::OnRestart);
		RestartWithoutSaveButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::SetShouldSaveScoresFalse);
		RestartWithoutSaveButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::OnRestart);

		RestartBackButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeOutRestartMenu);
		RestartBackButton->OnClicked.AddDynamic(this, &UQuitMenuWidget::InitializeExit);
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
	OnGameModeStateChanged.Broadcast(TransitionState);
}

void UQuitMenuWidget::OnQuitToDesktop()
{
	FGameModeTransitionState TransitionState;
	TransitionState.TransitionState = ETransitionState::QuitToDesktop;
	TransitionState.bSaveCurrentScores = bShouldSaveScores;
	OnGameModeStateChanged.Broadcast(TransitionState);
}

void UQuitMenuWidget::OnRestart()
{
	FGameModeTransitionState TransitionState;
	TransitionState.TransitionState = ETransitionState::Restart;
	TransitionState.bSaveCurrentScores = bShouldSaveScores;
	OnGameModeStateChanged.Broadcast(TransitionState);
}

void UQuitMenuWidget::CollapseWidget()
{
	SetVisibility(ESlateVisibility::Collapsed);
	UnbindFromAnimationFinished(FadeOutBackgroundBlur, FadeOutWidgetDelegate);
}

void UQuitMenuWidget::InitializeExit()
{
	PlayAnimationForward(FadeOutBackgroundBlur);
	FadeOutWidgetDelegate.BindDynamic(this, & UQuitMenuWidget::CollapseWidget);
	BindToAnimationFinished(FadeOutBackgroundBlur, FadeOutWidgetDelegate);
	if (!OnExitQuitMenu.ExecuteIfBound())
	{
		UE_LOG(LogTemp, Display, TEXT("OnExitQuitMenu not bound."));
	}
}
