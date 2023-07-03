// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "MenuWidgets/MainMenuWidget.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"
#include "OverlayWidgets/LoginWidget.h"
#include "OverlayWidgets/FeedbackWidget.h"
#include "SubMenuWidgets/ScoreBrowserWidget.h"
#include "WidgetComponents/MenuButton.h"
#include "WidgetComponents/WebBrowserWidget.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	ScoresWidget->OnLoginStateChange.AddDynamic(this, &UMainMenuWidget::OnLoginStateChange);
	LoginWidget->OnLoginButtonClicked.AddDynamic(this, &UMainMenuWidget::OnButtonClicked_Login);
	LoginWidget->OkayButton->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	Button_Feedback->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	Button_Login_Register->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);

	MenuButton_PatchNotes->SetDefaults(Box_PatchNotes, MenuButton_GameModes);
	MenuButton_GameModes->SetDefaults(Box_GameModes, MenuButton_Scores);
	MenuButton_Scores->SetDefaults(Box_Scores, MenuButton_Settings);
	MenuButton_Settings->SetDefaults(Box_Settings, MenuButton_FAQ);
	MenuButton_FAQ->SetDefaults(Box_FAQ, MenuButton_Quit);
	MenuButton_Quit->SetDefaults(Box_PatchNotes, MenuButton_PatchNotes);
	
	MenuButton_PatchNotes->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnMenuButtonClicked_BSButton);
	MenuButton_GameModes->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnMenuButtonClicked_BSButton);
	MenuButton_Scores->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnMenuButtonClicked_BSButton);
	MenuButton_Settings->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnMenuButtonClicked_BSButton);
	MenuButton_FAQ->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnMenuButtonClicked_BSButton);
	MenuButton_Quit->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnMenuButtonClicked_BSButton);
	
	ScoresWidget->InitializeScoringOverlay();
	WebBrowserOverlayPatchNotes->BrowserWidget->LoadPatchNotesURL();
	WebBrowserOverlayPatchNotes->FadeOutLoadingOverlay();

	MenuButton_PatchNotes->SetActive();
	MainMenuSwitcher->SetActiveWidget(MenuButton_PatchNotes->GetBox());
}

void UMainMenuWidget::OnButtonClicked_Login(const FLoginPayload LoginPayload, const bool bIsPopup)
{
	ScoresWidget->LoginUserHttp(LoginPayload, bIsPopup);
}

void UMainMenuWidget::OnMenuButtonClicked_BSButton(const UBSButton* Button)
{
	const UMenuButton* MenuButton = Cast<UMenuButton>(Button);

	if (!MenuButton)
	{
		return;
	}

	// Scores Menu button
	if (Button == MenuButton_Scores)
	{
		if (bShowWebBrowserScoring)
		{
			ScoresWidget->FadeOutLoadingOverlay();
		}
	}
	
	// Menu button
	if (MenuButton->GetBox())
	{
		MainMenuSwitcher->SetActiveWidget(MenuButton->GetBox());
	}
	
	// Quit button
	if (Button == MenuButton_Quit)
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0), EQuitPreference::Quit, false);
	}
}

void UMainMenuWidget::OnButtonClicked_BSButton(const UBSButton* Button)
{
	// Successful login, show scores menu
	if (LoginWidget && Button == LoginWidget->OkayButton)
	{
		// Manually set scores button active and set score box as active
		MenuButton_Scores->SetActive();
		MainMenuSwitcher->SetActiveWidget(MenuButton_Scores->GetBox());
		if (bShowWebBrowserScoring)
		{
			ScoresWidget->FadeOutLoadingOverlay();
		}
		return;
	}

	// Login/Register button
	if (Button == Button_Login_Register)
	{
		LoginWidget->ShowRegisterScreen();
	}

	// Feedback button
	if (Button == Button_Feedback)
	{
		FeedbackWidget->ShowFeedbackWidget();
	}
}

void UMainMenuWidget::OnLoginStateChange(const ELoginState& LoginState, const bool bIsPopup)
{
	FPlayerSettings_User PlayerSettings = LoadPlayerSettings().User;

	switch (LoginState)
	{
	case ELoginState::NewUser:
		{
			if (!PlayerSettings.bHasSeenRegisterPopup)
			{
				LoginWidget->ShowRegisterScreen();
				PlayerSettings.bHasSeenRegisterPopup = true;
				SavePlayerSettings(PlayerSettings);
			}
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
			UE_LOG(LogTemp, Display, TEXT("Login success"));
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
		Button_Login_Register->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		return;
	}
	TextBlock_SignInState->SetText(FText::FromString("Signed in as"));
	TextBlock_Username->SetText(FText::FromString(LoadPlayerSettings().User.Username));
	Button_Login_Register->SetVisibility(ESlateVisibility::Collapsed);
}
