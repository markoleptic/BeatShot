// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


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
	MainMenuWidgets.Add(SlideRightButton_PatchNotes, Box_PatchNotes);
	MainMenuWidgets.Add(SlideRightButton_GameModes, Box_GameModes);
	MainMenuWidgets.Add(SlideRightButton_Scores, Box_Scores);
	MainMenuWidgets.Add(SlideRightButton_Settings, Box_Settings);
	MainMenuWidgets.Add(SlideRightButton_FAQ, Box_FAQ);

	ScoresWidget->OnLoginStateChange.AddDynamic(this, &UMainMenuWidget::OnLoginStateChange);
	LoginWidget->OnLoginButtonClicked.AddDynamic(this, &UMainMenuWidget::OnButtonClicked_Login);
	LoginWidget->OkayButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnButtonClicked_Scoring);
	SlideRightButton_PatchNotes->Button->OnClicked.AddDynamic(this, &UMainMenuWidget::OnButtonClicked_PatchNotes);
	SlideRightButton_GameModes->Button->OnClicked.AddDynamic(this, &UMainMenuWidget::OnButtonClicked_GameModes);
	SlideRightButton_Scores->Button->OnClicked.AddDynamic(this, &UMainMenuWidget::OnButtonClicked_Scoring);
	SlideRightButton_Settings->Button->OnClicked.AddDynamic(this, &UMainMenuWidget::OnButtonClicked_Settings);
	SlideRightButton_FAQ->Button->OnClicked.AddDynamic(this, &UMainMenuWidget::OnButtonClicked_FAQButton);
	SlideRightButton_Quit->Button->OnClicked.AddDynamic(this, &UMainMenuWidget::OnButtonClicked_Quit);
	Button_GitHubIssue->OnClicked.AddDynamic(this, &UMainMenuWidget::OnButtonClicked_GitHub);
	ScoresWidget->InitializeScoringOverlay();
	WebBrowserOverlayPatchNotes->BrowserWidget->LoadPatchNotesURL();
	WebBrowserOverlayPatchNotes->FadeOutLoadingOverlay();
	OnButtonClicked_PatchNotes();
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

void UMainMenuWidget::OnButtonClicked_PatchNotes()
{
	SlideButtons(SlideRightButton_PatchNotes);
}

void UMainMenuWidget::OnButtonClicked_GameModes()
{
	SlideButtons(SlideRightButton_GameModes);
}

void UMainMenuWidget::OnButtonClicked_Scoring()
{
	SlideButtons(SlideRightButton_Scores);
	if (bShowWebBrowserScoring) ScoresWidget->FadeOutLoadingOverlay();
}

void UMainMenuWidget::OnButtonClicked_Settings()
{
	SlideButtons(SlideRightButton_Settings);
}

void UMainMenuWidget::OnButtonClicked_FAQButton()
{
	SlideButtons(SlideRightButton_FAQ);
}

void UMainMenuWidget::OnButtonClicked_Login(const FLoginPayload LoginPayload, const bool bIsPopup)
{
	ScoresWidget->LoginUserHttp(LoginPayload, bIsPopup);
}

void UMainMenuWidget::OnButtonClicked_GitHub()
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
			LoginWidget->ShowLoginScreen("Login_LoginErrorText");
			UpdateLoginState(false);
			return;
		}
	case ELoginState::InvalidBrowser:
		{
			LoginWidget->ShowLoginScreen("Login_BrowserLoginErrorText");
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
			LoginWidget->ShowLoginScreen("Login_InvalidCredentialsText");
			UpdateLoginState(false);
			break;
		}
	case ELoginState::TimeOut:
		{
			LoginWidget->ShowLoginScreen("Login_TimeOutErrorText");
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
		TextBlock_SignInState->SetText(FText::FromString("Not Signed In"));
		TextBlock_Username->SetText(FText());
		return;
	}
	TextBlock_SignInState->SetText(FText::FromString("Signed in as"));
	TextBlock_Username->SetText(FText::FromString(LoadPlayerSettings().User.Username));
}

void UMainMenuWidget::OnButtonClicked_Quit()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0), EQuitPreference::Quit, false);
}
