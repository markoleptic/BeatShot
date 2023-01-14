// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuWidgets/MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"
#include "OverlayWidgets/LoginWidget.h"
#include "SubMenuWidgets/ScoreBrowserWidget.h"
#include "WidgetComponents/SlideRightButton.h"
#include "WidgetComponents/WebBrowserWidget.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	MainMenuWidgets.Add(PatchNotesButton, PatchNotes);
	MainMenuWidgets.Add(GameModesButton, GameModes);
	MainMenuWidgets.Add(ScoresButton, Scores);
	MainMenuWidgets.Add(SettingsButton, SettingsMenu);
	MainMenuWidgets.Add(FAQButton, FAQ);

	ScoresWidget->OnLoginStateChange.AddDynamic(this, &UMainMenuWidget::OnLoginStateChange);
	LoginWidget->OnLoginButtonClicked.AddDynamic(this, &UMainMenuWidget::OnLoginButtonClicked);
	LoginWidget->OkayButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnScoringButtonClicked);
	PatchNotesButton->Button->OnClicked.AddDynamic(this, &UMainMenuWidget::OnPatchNotesButtonClicked);
	GameModesButton->Button->OnClicked.AddDynamic(this, &UMainMenuWidget::OnGameModesButtonClicked);
	ScoresButton->Button->OnClicked.AddDynamic(this, &UMainMenuWidget::OnScoringButtonClicked);
	SettingsButton->Button->OnClicked.AddDynamic(this, &UMainMenuWidget::OnSettingsButtonClicked);
	FAQButton->Button->OnClicked.AddDynamic(this, &UMainMenuWidget::OnFAQButtonClicked);
	QuitButton->Button->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitButtonClicked);
	GitHubIssueButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnGitHubButtonClicked);
	ScoresWidget->InitializeScoringOverlay();
	WebBrowserOverlayPatchNotes->BrowserWidget->LoadPatchNotesURL();
	WebBrowserOverlayPatchNotes->FadeOutLoadingOverlay();
	OnPatchNotesButtonClicked();
}

void UMainMenuWidget::SlideButtons(const USlideRightButton* ActiveButton)
{
	for (TTuple<USlideRightButton*, UVerticalBox*>& Elem : MainMenuWidgets)
	{
		if (Elem.Key != ActiveButton)
		{
			Elem.Key->SlideButton(false);
			continue;
		}
		Elem.Key->SlideButton(true);
		MainMenuSwitcher->SetActiveWidget(Elem.Value);
	}
}

void UMainMenuWidget::OnPatchNotesButtonClicked()
{
	SlideButtons(PatchNotesButton);
}

void UMainMenuWidget::OnGameModesButtonClicked()
{
	SlideButtons(GameModesButton);
}

void UMainMenuWidget::OnScoringButtonClicked()
{
	SlideButtons(ScoresButton);
	if (bShowWebBrowserScoring) ScoresWidget->FadeOutLoadingOverlay();
}

void UMainMenuWidget::OnSettingsButtonClicked()
{
	SlideButtons(SettingsButton);
}

void UMainMenuWidget::OnFAQButtonClicked()
{
	SlideButtons(FAQButton);
}

void UMainMenuWidget::OnLoginButtonClicked(const FLoginPayload LoginPayload, const bool bIsPopup)
{
	ScoresWidget->LoginUserHttp(LoginPayload, bIsPopup);
}

void UMainMenuWidget::OnGitHubButtonClicked()
{
	UKismetSystemLibrary::LaunchURL(GitHubURL);
}

void UMainMenuWidget::OnLoginStateChange(const ELoginState& LoginState, const bool bIsPopup)
{
	switch (LoginState)
	{
	case ELoginState::NewUser:
		{
			LoginWidget->ShowRegisterScreen();
			UpdateLoginState(false);
			return;
		}
	case ELoginState::InvalidHttp:
		{
			LoginWidget->ShowLoginScreen("LoginErrorText");
			UpdateLoginState(false);
			return;
		}
	case ELoginState::InvalidBrowser:
		{
			LoginWidget->ShowLoginScreen("BrowserLoginErrorText");
			UpdateLoginState(false);
			return;
		}
	case ELoginState::LoggedInHttpAndBrowser:
		{
			bShowWebBrowserScoring = true;
			if (bIsPopup)
			{
				LoginWidget->OnLoginSuccess();
			}
			UpdateLoginState(true);
			break;
		}
	case ELoginState::InvalidCredentials:
		{
			LoginWidget->ShowLoginScreen("InvalidCredentialsText");
			UpdateLoginState(false);
			break;
		}
	case ELoginState::TimeOut:
		{
			LoginWidget->ShowLoginScreen("TimeOutErrorText");
			UpdateLoginState(false);
			break;
		}
	default:
		{
			break;
		}
	}
}

void UMainMenuWidget::UpdateLoginState(const bool bSuccessfulLogin)
{
	if (!bSuccessfulLogin)
	{
		SignInStateText->SetText(FText::FromString("Not Signed In"));
		UsernameText->SetText(FText());
		return;
	}
	SignInStateText->SetText(FText::FromString("Signed in as"));
	UsernameText->SetText(FText::FromString(LoadPlayerSettings().Username));
}

void UMainMenuWidget::OnQuitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0),
	                               EQuitPreference::Quit, false);
}
