// Fill out your copyright notice in the Description page of Project Settings.


#include "PostGameMenuWidget.h"

#include "AASettings.h"
#include "DefaultGameMode.h"
#include "DefaultPlayerController.h"
#include "SettingsMenuWidget.h"
#include "SlideRightButton.h"
#include "WebBrowserOverlay.h"
#include "QuitMenuWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"

void UPostGameMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	MenuWidgets.Add(ScoresButton, Scores);
	MenuWidgets.Add(GameModesButton, GameModes);
	MenuWidgets.Add(SettingsButton, Settings);
	MenuWidgets.Add(FAQButton, FAQ);
	
	ScoresButton->Button->OnClicked.AddDynamic(this, &UPostGameMenuWidget::OnScoresButtonClicked);
	PlayAgainButton->Button->OnClicked.AddDynamic(this, &UPostGameMenuWidget::OnPlayAgainButtonClicked);
	GameModesButton->Button->OnClicked.AddDynamic(this, &UPostGameMenuWidget::OnGameModesButtonClicked);
	SettingsButton->Button->OnClicked.AddDynamic(this, &UPostGameMenuWidget::OnSettingsButtonClicked);
	FAQButton->Button->OnClicked.AddDynamic(this, &UPostGameMenuWidget::OnFAQButtonClicked);
	QuitButton->Button->OnClicked.AddDynamic(this, &UPostGameMenuWidget::OnQuitButtonClicked);

	QuitMenuWidget->OnExitQuitMenu.BindUFunction(this, "SlideQuitMenuButtonsLeft");
	SettingsMenuWidget->AASettingsWidget->OnRestartButtonClicked.BindDynamic(this, &UPostGameMenuWidget::Restart);
	ScoresWidget->OnLoginStateChange.AddDynamic(this, &UPostGameMenuWidget::OnLoginStateChange);
	FadeInWidgetDelegate.BindDynamic(this, &UPostGameMenuWidget::SetScoresWidgetVisibility);
	BindToAnimationFinished(FadeInWidget, FadeInWidgetDelegate);
	
	PlayFadeInWidget();
}

void UPostGameMenuWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	ScoresWidget->BrowserWidget->ParentTickOverride(InDeltaTime);
}

void UPostGameMenuWidget::Restart()
{
	Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->EndGameMode(false, false);
	ADefaultPlayerController* Controller = Cast<ADefaultPlayerController>(
	UGameplayStatics::GetPlayerController(GetWorld(), 0));
	Controller->OnScreenFadeToBlackFinish.AddDynamic(this, &UPostGameMenuWidget::HandleRestart);
	Controller->FadeScreenToBlack();
}

void UPostGameMenuWidget::HandleRestart()
{
	Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->InitializeGameMode();
	ADefaultPlayerController* Controller = Cast<ADefaultPlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));
	Controller->HidePostGameMenu();
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

void UPostGameMenuWidget::OnPlayAgainButtonClicked()
{
	Restart();
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

void UPostGameMenuWidget::OnLoginStateChange(bool bLoggedInHttp, bool bLoggedInBrowser, bool bIsPopup)
{
	if (bLoggedInHttp && bLoggedInBrowser)
	{
		ScoresWidget->FadeOut();
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
