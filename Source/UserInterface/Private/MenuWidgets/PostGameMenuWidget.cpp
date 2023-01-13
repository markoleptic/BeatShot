// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuWidgets/PostGameMenuWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "OverlayWidgets/QuitMenuWidget.h"
#include "SubMenuWidgets/AASettingsWidget.h"
#include "SubMenuWidgets/SettingsMenuWidget.h"
#include "SubMenuWidgets/ScoreBrowserWidget.h"
#include "WidgetComponents/SlideRightButton.h"

void UPostGameMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	MenuWidgets.Add(ScoresButton, Scores);
	MenuWidgets.Add(GameModesButton, GameModes);
	MenuWidgets.Add(SettingsButton, Settings);
	MenuWidgets.Add(FAQButton, FAQ);
	
	ScoresButton->Button->OnClicked.AddDynamic(this, &UPostGameMenuWidget::OnScoresButtonClicked);
	PlayAgainButton->Button->OnClicked.AddDynamic(this, &UPostGameMenuWidget::Restart);
	GameModesButton->Button->OnClicked.AddDynamic(this, &UPostGameMenuWidget::OnGameModesButtonClicked);
	SettingsButton->Button->OnClicked.AddDynamic(this, &UPostGameMenuWidget::OnSettingsButtonClicked);
	FAQButton->Button->OnClicked.AddDynamic(this, &UPostGameMenuWidget::OnFAQButtonClicked);
	QuitButton->Button->OnClicked.AddDynamic(this, &UPostGameMenuWidget::OnQuitButtonClicked);

	QuitMenuWidget->OnExitQuitMenu.BindUFunction(this, "SlideQuitMenuButtonsLeft");
	SettingsMenuWidget->AASettingsWidget->OnRestartButtonClicked.BindDynamic(this, &UPostGameMenuWidget::Restart);
	ScoresWidget->OnLoginStateChange.AddDynamic(this, &UPostGameMenuWidget::OnLoginStateChange);
	FadeInWidgetDelegate.BindDynamic(this, &UPostGameMenuWidget::SetScoresWidgetVisibility);
	BindToAnimationFinished(FadeInWidget, FadeInWidgetDelegate);
	GameModesWidget->OnStartGameMode.BindLambda([&]
	{
		Restart();
	});
	PlayFadeInWidget();
}

void UPostGameMenuWidget::Restart()
{
	if (!StartGameModePostGameMenu.ExecuteIfBound())
	{
		UE_LOG(LogTemp, Display, TEXT("StartGameModePostGameMenu not bound."));
	}
}

void UPostGameMenuWidget::SetScoresWidgetVisibility()
{
	ScoresWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	SlideButtons(ScoresButton);
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

void UPostGameMenuWidget::OnGameModesButtonClicked()
{
	SlideButtons(GameModesButton);
}

void UPostGameMenuWidget::OnSettingsButtonClicked()
{
	SlideButtons(SettingsButton);
}

void UPostGameMenuWidget::OnFAQButtonClicked()
{
	SlideButtons(FAQButton);
}

void UPostGameMenuWidget::OnQuitButtonClicked()
{
	SlideButtons(QuitButton);
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
	PlayAgainButton->SlideButton(false);
	QuitButton->SlideButton(false);
}

void UPostGameMenuWidget::OnScoresButtonClicked()
{
	SlideButtons(ScoresButton);
}
