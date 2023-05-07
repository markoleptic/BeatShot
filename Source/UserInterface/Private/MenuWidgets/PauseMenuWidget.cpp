// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "MenuWidgets/PauseMenuWidget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/VerticalBox.h"
#include "OverlayWidgets/QuitMenuWidget.h"
#include "SubMenuWidgets/SettingsMenuWidget.h"
#include "WidgetComponents/SlideRightButton.h"

void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SlideRightButton_Resume->Button->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnButtonClicked_Resume);
	SlideRightButton_Settings->Button->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnSettingsButtonClicked);
	SlideRightButton_FAQ->Button->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnFAQButtonClicked);

	SlideRightButton_RestartCurrentMode->Button->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnButtonClicked_RestartCurrentMode);
	SlideRightButton_Quit->Button->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnButtonClicked_Quit);

	PauseMenuWidgets.Add(SlideRightButton_Resume, Box_PauseScreen);
	PauseMenuWidgets.Add(SlideRightButton_RestartCurrentMode, Box_PauseScreen);
	PauseMenuWidgets.Add(SlideRightButton_Settings, Box_SettingsMenu);
	PauseMenuWidgets.Add(SlideRightButton_FAQ, Box_FAQ);
	PauseMenuWidgets.Add(SlideRightButton_Quit, Box_PauseScreen);

	QuitMenuWidget->OnExitQuitMenu.BindUFunction(this, "SlideQuitMenuButtonsLeft");
	SettingsMenuWidget->OnRestartButtonClicked.BindUFunction(this, "OnButtonClicked_RestartCurrentMode");
	FadeInWidget();
}

void UPauseMenuWidget::OnButtonClicked_Resume()
{
	if (!ResumeGame.ExecuteIfBound())
	{
		UE_LOG(LogTemp, Display, TEXT("ResumeGame not bound."));
	}
}

void UPauseMenuWidget::OnButtonClicked_RestartCurrentMode()
{
	SlideButtons(SlideRightButton_RestartCurrentMode);
	QuitMenuWidget->SetVisibility(ESlateVisibility::Visible);
	QuitMenuWidget->PlayFadeInRestartMenu();
}

void UPauseMenuWidget::OnButtonClicked_Quit()
{
	SlideButtons(SlideRightButton_Quit);
	QuitMenuWidget->SetVisibility(ESlateVisibility::Visible);
	QuitMenuWidget->PlayInitialFadeInMenu();
}

void UPauseMenuWidget::SlideQuitMenuButtonsLeft()
{
	SlideRightButton_RestartCurrentMode->SlideButton(false);
	SlideRightButton_Quit->SlideButton(false);
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
