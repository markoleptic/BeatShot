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
		Button_QuitMainMenu->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeOutMenu);
		Button_QuitMainMenu->OnClicked.AddDynamic(this, &UQuitMenuWidget::SetGotoMainMenuTrue);
		Button_QuitMainMenu->OnClicked.AddDynamic(this, &UQuitMenuWidget::Quit);

		Button_QuitDesktop->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeOutMenu);
		Button_QuitDesktop->OnClicked.AddDynamic(this, &UQuitMenuWidget::SetGotoMainMenuFalse);
		Button_QuitDesktop->OnClicked.AddDynamic(this, &UQuitMenuWidget::Quit);

		Button_QuitBack->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeOutMenu);
		Button_QuitBack->OnClicked.AddDynamic(this, &UQuitMenuWidget::InitializeExit);
	}
	else
	{
		Button_QuitMainMenu->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeOutMenu);
		Button_QuitMainMenu->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeInSaveMenu);
		Button_QuitMainMenu->OnClicked.AddDynamic(this, &UQuitMenuWidget::SetGotoMainMenuTrue);
		Button_QuitMainMenu->OnClicked.AddDynamic(this, &UQuitMenuWidget::SetSaveMenuTitleMainMenu);

		Button_QuitDesktop->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeOutMenu);
		Button_QuitDesktop->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeInSaveMenu);
		Button_QuitDesktop->OnClicked.AddDynamic(this, &UQuitMenuWidget::SetGotoMainMenuFalse);
		Button_QuitDesktop->OnClicked.AddDynamic(this, &UQuitMenuWidget::SetSaveMenuTitleDesktop);

		Button_QuitBack->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeOutMenu);
		Button_QuitBack->OnClicked.AddDynamic(this, &UQuitMenuWidget::InitializeExit);

		Button_QuitAndSave->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeOutSaveMenu);
		Button_QuitAndSave->OnClicked.AddDynamic(this, &UQuitMenuWidget::SetShouldSaveScoresTrue);
		Button_QuitAndSave->OnClicked.AddDynamic(this, &UQuitMenuWidget::Quit);

		Button_QuitWithoutSave->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeOutSaveMenu);
		Button_QuitWithoutSave->OnClicked.AddDynamic(this, &UQuitMenuWidget::SetShouldSaveScoresFalse);
		Button_QuitWithoutSave->OnClicked.AddDynamic(this, &UQuitMenuWidget::Quit);

		Button_SaveBack->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeOutSaveMenu);
		Button_SaveBack->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeInMenu);

		Button_RestartAndSave->OnClicked.AddDynamic(this, &UQuitMenuWidget::SetShouldSaveScoresTrue);
		Button_RestartAndSave->OnClicked.AddDynamic(this, &UQuitMenuWidget::OnRestart);
		Button_RestartWithoutSave->OnClicked.AddDynamic(this, &UQuitMenuWidget::SetShouldSaveScoresFalse);
		Button_RestartWithoutSave->OnClicked.AddDynamic(this, &UQuitMenuWidget::OnRestart);

		Button_RestartBack->OnClicked.AddDynamic(this, &UQuitMenuWidget::PlayFadeOutRestartMenu);
		Button_RestartBack->OnClicked.AddDynamic(this, &UQuitMenuWidget::InitializeExit);
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
