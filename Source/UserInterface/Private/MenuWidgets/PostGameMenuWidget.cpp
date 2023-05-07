// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "MenuWidgets/PostGameMenuWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "OverlayWidgets/QuitMenuWidget.h"
#include "SubMenuWidgets/SettingsMenuWidget.h"
#include "SubMenuWidgets/ScoreBrowserWidget.h"
#include "WidgetComponents/SlideRightButton.h"

void UPostGameMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	MenuWidgets.Add(SlideRightButton_Scores, Box_Scores);
	MenuWidgets.Add(SlideRightButton_GameModes, Box_GameModes);
	MenuWidgets.Add(SlideRightButton_Settings, Box_Settings);
	MenuWidgets.Add(SlideRightButton_FAQ, Box_FAQ);

	SlideRightButton_Scores->Button->OnClicked.AddDynamic(this, &UPostGameMenuWidget::OnButtonClicked_Scores);
	SlideRightButton_PlayAgain->Button->OnClicked.AddDynamic(this, &UPostGameMenuWidget::Restart);
	SlideRightButton_GameModes->Button->OnClicked.AddDynamic(this, &UPostGameMenuWidget::OnButtonClicked_GameModes);
	SlideRightButton_Settings->Button->OnClicked.AddDynamic(this, &UPostGameMenuWidget::OnButtonClicked_Settings);
	SlideRightButton_FAQ->Button->OnClicked.AddDynamic(this, &UPostGameMenuWidget::OnButtonClicked_FAQ);
	SlideRightButton_Quit->Button->OnClicked.AddDynamic(this, &UPostGameMenuWidget::OnButtonClicked_Quit);

	QuitMenuWidget->OnExitQuitMenu.BindUFunction(this, "SlideQuitMenuButtonsLeft");
	SettingsMenuWidget->OnRestartButtonClicked.BindUFunction(this, "Restart");
	ScoresWidget->OnLoginStateChange.AddDynamic(this, &UPostGameMenuWidget::OnLoginStateChange);
	FadeInWidgetDelegate.BindDynamic(this, &UPostGameMenuWidget::SetScoresWidgetVisibility);
	BindToAnimationFinished(FadeInWidget, FadeInWidgetDelegate);
	PlayFadeInWidget();
}

void UPostGameMenuWidget::Restart()
{
	FGameModeTransitionState GameModeTransitionState;
	GameModeTransitionState.TransitionState = ETransitionState::Restart;
	GameModeTransitionState.bSaveCurrentScores = false;
	GameModesWidget->OnGameModeStateChanged.Broadcast(GameModeTransitionState);
}

void UPostGameMenuWidget::SetScoresWidgetVisibility()
{
	ScoresWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	SlideButtons(SlideRightButton_Scores);
}

void UPostGameMenuWidget::SlideButtons(const USlideRightButton* ActiveButton)
{
	for (TTuple<USlideRightButton*, UVerticalBox*>& Elem : MenuWidgets)
	{
		if (Elem.Key != ActiveButton)
		{
			Elem.Key->SlideButton(false);
			continue;
		}
		Elem.Key->SlideButton(true);
		MenuSwitcher->SetActiveWidget(Elem.Value);
	}
}

void UPostGameMenuWidget::OnButtonClicked_GameModes()
{
	SlideButtons(SlideRightButton_GameModes);
}

void UPostGameMenuWidget::OnButtonClicked_Settings()
{
	SlideButtons(SlideRightButton_Settings);
}

void UPostGameMenuWidget::OnButtonClicked_FAQ()
{
	SlideButtons(SlideRightButton_FAQ);
}

void UPostGameMenuWidget::OnButtonClicked_Quit()
{
	SlideButtons(SlideRightButton_Quit);
	QuitMenuWidget->SetVisibility(ESlateVisibility::Visible);
	QuitMenuWidget->PlayInitialFadeInMenu();
}

void UPostGameMenuWidget::OnLoginStateChange(const ELoginState& LoginState, bool bIsPopup)
{
	if (LoginState == ELoginState::LoggedInHttpAndBrowser)
	{
		ScoresWidget->FadeOutLoadingOverlay();
	}
}

void UPostGameMenuWidget::SlideQuitMenuButtonsLeft()
{
	SlideRightButton_PlayAgain->SlideButton(false);
	SlideRightButton_Quit->SlideButton(false);
}

void UPostGameMenuWidget::OnButtonClicked_Scores()
{
	SlideButtons(SlideRightButton_Scores);
}
