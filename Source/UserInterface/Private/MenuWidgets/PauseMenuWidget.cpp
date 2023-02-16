// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "MenuWidgets/PauseMenuWidget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/VerticalBox.h"
#include "OverlayWidgets/QuitMenuWidget.h"
#include "SubMenuWidgets/AASettingsWidget.h"
#include "SubMenuWidgets/SettingsMenuWidget.h"
#include "WidgetComponents/SlideRightButton.h"

void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ResumeButton->Button->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnResumeButtonClicked);
	SettingsButton->Button->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnSettingsButtonClicked);
	FAQButton->Button->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnFAQButtonClicked);
	
	RestartCurrentModeButton->Button->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnRestartCurrentModeClicked);
	QuitButton->Button->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnQuitButtonClicked);
	
	PauseMenuWidgets.Add(ResumeButton, PauseScreen);
	PauseMenuWidgets.Add(RestartCurrentModeButton, PauseScreen);
	PauseMenuWidgets.Add(SettingsButton, SettingsMenu);
	PauseMenuWidgets.Add(FAQButton, FAQ);
	PauseMenuWidgets.Add(QuitButton, PauseScreen);

	QuitMenuWidget->OnExitQuitMenu.BindUFunction(this, "SlideQuitMenuButtonsLeft");
	SettingsMenuWidget->OnRestartButtonClicked.BindUFunction(this, "OnRestartCurrentModeClicked");
	FadeInWidget();
}

void UPauseMenuWidget::OnResumeButtonClicked()
{
	if (!ResumeGame.ExecuteIfBound())
	{
		UE_LOG(LogTemp, Display, TEXT("ResumeGame not bound."));
	}
}

void UPauseMenuWidget::OnRestartCurrentModeClicked()
{
	SlideButtons(RestartCurrentModeButton);
	QuitMenuWidget->SetVisibility(ESlateVisibility::Visible);
	QuitMenuWidget->PlayFadeInRestartMenu();
}

void UPauseMenuWidget::OnQuitButtonClicked()
{
	SlideButtons(QuitButton);
	QuitMenuWidget->SetVisibility(ESlateVisibility::Visible);
	QuitMenuWidget->PlayInitialFadeInMenu();
}

void UPauseMenuWidget::SlideQuitMenuButtonsLeft()
{
	RestartCurrentModeButton->SlideButton(false);
	QuitButton->SlideButton(false);
}

void UPauseMenuWidget::SlideButtons(const USlideRightButton* ActiveButton)
{
	for (TTuple<USlideRightButton*, UVerticalBox*>& Elem : PauseMenuWidgets)
	{
		if (Elem.Key != ActiveButton)
		{
			Elem.Key->SlideButton(false);
			continue;
		}
		Elem.Key->SlideButton(true);
		PauseMenuSwitcher->SetActiveWidget(Elem.Value);
	}
}
