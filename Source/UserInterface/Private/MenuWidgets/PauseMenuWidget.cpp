// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "MenuWidgets/PauseMenuWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Components/VerticalBox.h"
#include "OverlayWidgets/QuitMenuWidget.h"
#include "SubMenuWidgets/SettingsMenuWidget.h"
#include "WidgetComponents/BSButton.h"
#include "WidgetComponents/MenuButton.h"

void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MenuButton_Resume->SetDefaults(Box_PauseScreen, MenuButton_Settings);
	MenuButton_Settings->SetDefaults(Box_SettingsMenu, MenuButton_FAQ);
	MenuButton_FAQ->SetDefaults(Box_FAQ, MenuButton_RestartCurrentMode);
	MenuButton_RestartCurrentMode->SetDefaults(Box_PauseScreen, MenuButton_Quit);
	MenuButton_Quit->SetDefaults(Box_PauseScreen, MenuButton_Resume);
	
	MenuButton_Resume->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	MenuButton_Settings->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	MenuButton_FAQ->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	MenuButton_RestartCurrentMode->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	MenuButton_Quit->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	
	QuitMenuWidget->OnExitQuitMenu.BindUFunction(this, "SetQuitMenuButtonsInActive");
	SettingsMenuWidget->OnRestartButtonClicked.BindUFunction(this, "OnButtonClicked_RestartCurrentMode");
	FadeInWidget();
}

void UPauseMenuWidget::SetQuitMenuButtonsInActive()
{
	MenuButton_RestartCurrentMode->SetInActive();
	MenuButton_Quit->SetInActive();
}

void UPauseMenuWidget::OnButtonClicked_BSButton(const UBSButton* Button)
{
	if (Button == MenuButton_Resume)
	{
		if (!ResumeGame.ExecuteIfBound())
		{
			UE_LOG(LogTemp, Display, TEXT("ResumeGame not bound."));
		}
	}
	else if (Button == MenuButton_Quit)
	{
		QuitMenuWidget->SetVisibility(ESlateVisibility::Visible);
		QuitMenuWidget->PlayInitialFadeInMenu();
	}
	else if (Button == MenuButton_RestartCurrentMode)
	{
		QuitMenuWidget->SetVisibility(ESlateVisibility::Visible);
		QuitMenuWidget->PlayFadeInRestartMenu();
	}
	PauseMenuSwitcher->SetActiveWidget(Cast<UMenuButton>(Button)->GetBox());
}

void UPauseMenuWidget::OnButtonClicked_RestartCurrentMode()
{
	QuitMenuWidget->SetVisibility(ESlateVisibility::Visible);
	QuitMenuWidget->PlayFadeInRestartMenu();
	MenuButton_RestartCurrentMode->SetActive();
	PauseMenuSwitcher->SetActiveWidget(MenuButton_RestartCurrentMode);
}
