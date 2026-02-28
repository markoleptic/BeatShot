// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Menus/MainMenuWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "Menus/GameModeMenuWidget.h"
#include "Menus/ScoreViewerWidget.h"
#include "Overlays/FeedbackWidget.h"
#include "Styles/MenuStyle.h"
#include "Utilities/BSWidgetInterface.h"
#include "Utilities/Buttons/MenuButton.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetStyles();

	MenuButton_PatchNotes->SetDefaults(Box_PatchNotes, MenuButton_GameModes);
	MenuButton_GameModes->SetDefaults(Box_GameModes, MenuButton_Scores);
	MenuButton_Scores->SetDefaults(Box_Scores, MenuButton_Settings);
	MenuButton_Settings->SetDefaults(Box_Settings, MenuButton_FAQ);
	MenuButton_FAQ->SetDefaults(Box_FAQ, Button_Feedback);
	Button_Feedback->SetDefaults(nullptr, MenuButton_Quit);
	MenuButton_Quit->SetDefaults(Box_PatchNotes, MenuButton_PatchNotes);

	MenuButton_PatchNotes->OnBSButtonPressed.AddUObject(this, &ThisClass::OnMenuButtonClicked_BSButton);
	MenuButton_GameModes->OnBSButtonPressed.AddUObject(this, &ThisClass::OnMenuButtonClicked_BSButton);
	MenuButton_Scores->OnBSButtonPressed.AddUObject(this, &ThisClass::OnMenuButtonClicked_BSButton);
	MenuButton_Settings->OnBSButtonPressed.AddUObject(this, &ThisClass::OnMenuButtonClicked_BSButton);
	MenuButton_FAQ->OnBSButtonPressed.AddUObject(this, &ThisClass::OnMenuButtonClicked_BSButton);
	Button_Feedback->OnBSButtonPressed.AddUObject(this, &ThisClass::OnMenuButtonClicked_BSButton);
	MenuButton_Quit->OnBSButtonPressed.AddUObject(this, &ThisClass::OnMenuButtonClicked_BSButton);

	MenuButton_PatchNotes->SetActive();
	MainMenuSwitcher->SetActiveWidget(MenuButton_PatchNotes->GetAssociatedWidget());
	LastMenuButton = MakeWeakObjectPtr(MenuButton_PatchNotes);

	ScoreViewerWidget->LoadScores(LoadSaveGamePlayerScore(), false);
}

void UMainMenuWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	SetStyles();
}

void UMainMenuWidget::SetStyles()
{
	MenuStyle = IBSWidgetInterface::GetStyleCDO<UMenuStyle>(MenuStyleClass);
}

UGameModeMenuWidget* UMainMenuWidget::GetGameModesWidget() const
{
	return GameModesWidget;
}

USettingsMenuWidget* UMainMenuWidget::GetSettingsMenuWidget() const
{
	return SettingsMenuWidget;
}

void UMainMenuWidget::OnPlayerSettingsChanged(const FPlayerSettings_User& UserSettings)
{
}

void UMainMenuWidget::OnMenuButtonClicked_BSButton(const UBSButton* Button)
{
	const UMenuButton* MenuButton = Cast<UMenuButton>(Button);
	if (!MenuButton)
	{
		return;
	}

	// Always stop the game mode preview if game modes widget is not visible
	if (MenuButton != MenuButton_GameModes)
	{
		GameModesWidget->StopGameModePreview();
	}

	// Feedback button
	if (Button == Button_Feedback)
	{
		auto* FeedbackWidget = CreateWidget<UFeedbackWidget>(this, FeedbackWidgetClass);
		FeedbackWidget->OnExitFeedback.BindLambda([this]
		{
			Button_Feedback->SetInActive();
			if (auto* LastMenuButtonLock = LastMenuButton.Get())
			{
				if (const auto AssociatedWidget = LastMenuButtonLock->GetAssociatedWidget())
				{
					LastMenuButtonLock->SetActive();
					MainMenuSwitcher->SetActiveWidget(AssociatedWidget);
				}
			}
		});
		FeedbackWidget->AddToViewport();
		FeedbackWidget->ShowFeedbackWidget();
	}
	else if (const auto AssociatedWidget = MenuButton->GetAssociatedWidget())
	{
		MainMenuSwitcher->SetActiveWidget(AssociatedWidget);
		LastMenuButton = MakeWeakObjectPtr(const_cast<UMenuButton*>(MenuButton));
	}
	else if (Button == MenuButton_Quit)
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0),
		                               EQuitPreference::Quit, false);
	}
}

void UMainMenuWidget::UpdateLoginState(const FString& SteamPersonaName)
{
	if (SteamPersonaName.IsEmpty())
	{
		TextBlock_Username->SetText(IBSWidgetInterface::GetWidgetTextFromKey("Login_NotSignedIn"));
	}
	else
	{
		TextBlock_Username->SetText(FText::FromString(SteamPersonaName));
	}
}
