// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseMenuWidget.h"

#include "DefaultPlayerController.h"
#include "QuitMenuWidget.h"
#include "Components/Button.h"
#include "SlideRightButton.h"
#include "Components/WidgetSwitcher.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"

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
	
	FadeInWidget();
}

void UPauseMenuWidget::OnResumeButtonClicked()
{
	ADefaultPlayerController* Controller = Cast<ADefaultPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	Controller->HandlePause();
	Controller->HidePauseMenu();
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
	QuitMenuWidget->PlayFadeInMenu();
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
