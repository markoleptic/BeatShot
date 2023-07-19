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

void UMainMenuWidget::OnPlayerSettingsChanged_User(const FPlayerSettings_User& UserSettings)
{
	UE_LOG(LogTemp, Display, TEXT("PlayerSettingsChangedUser in MainMenu"));
}

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	ScoresWidget->OnURLChangedResult.AddUObject(this, &UMainMenuWidget::OnURLChangedResult_ScoresBrowser);
	LoginWidget->OnLoginButtonClicked.AddDynamic(this, &UMainMenuWidget::OnButtonClicked_Login);
	LoginWidget->OnExitAnimationCompletedDelegate.AddDynamic(this, &ThisClass::OnLoginWidgetExitAnimationCompleted);
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
	
	WebBrowserOverlayPatchNotes->InitScoreBrowser(EScoreBrowserType::PatchNotes);
	ScoresWidget->InitScoreBrowser(EScoreBrowserType::MainMenuScores);
	MenuButton_PatchNotes->SetActive();
	MainMenuSwitcher->SetActiveWidget(MenuButton_PatchNotes->GetBox());
}

void UMainMenuWidget::OnMenuButtonClicked_BSButton(const UBSButton* Button)
{
	const UMenuButton* MenuButton = Cast<UMenuButton>(Button);

	if (!MenuButton)
	{
		return;
	}

	
	// Manually fade out browser overlay to avoid abrupt change
	if (MenuButton == MenuButton_Scores)
	{
		if (bFadeInScoreBrowserOnButtonPress)
		{
			ScoresWidget->FadeOutLoadingOverlay();
			bFadeInScoreBrowserOnButtonPress = false;
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
	// Login/Register button
	if (Button == Button_Login_Register)
	{
		LoginWidget->ShowLoginScreen(FString());
	}

	// Feedback button
	if (Button == Button_Feedback)
	{
		FeedbackWidget->ShowFeedbackWidget();
	}
}

void UMainMenuWidget::OnURLChangedResult_ScoresBrowser(const bool bSuccess)
{
	UpdateLoginState(bSuccess);
	if (!bSuccess)
	{
		bFadeInScoreBrowserOnButtonPress = false;
		LoginWidget->ShowLoginScreen("Login_BrowserLoginErrorText");
	}
	else
	{
		bFadeInScoreBrowserOnButtonPress = true;
	}
}

void UMainMenuWidget::UpdateLoginState(const bool bSuccessfulLogin, const FString OptionalMessage)
{
	if (!bSuccessfulLogin)
	{
		if (!OptionalMessage.IsEmpty())
		{
			TextBlock_SignInState->SetText(FText::FromString(OptionalMessage));
		}
		else
		{
			TextBlock_SignInState->SetText(FText::FromString("Not Signed In"));
		}
		TextBlock_Username->SetText(FText());
		Button_Login_Register->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		return;
	}
	if (!OptionalMessage.IsEmpty())
	{
		TextBlock_SignInState->SetText(FText::FromString(OptionalMessage));
	}
	else
	{
		TextBlock_SignInState->SetText(FText::FromString("Signed in as"));
	}
	TextBlock_Username->SetText(FText::FromString(LoadPlayerSettings().User.DisplayName));
	Button_Login_Register->SetVisibility(ESlateVisibility::Collapsed);
}

void UMainMenuWidget::OnLoginWidgetExitAnimationCompleted()
{
	Button_Login_Register->SetInActive();
}

// ReSharper disable once CppPassValueParameterByConstReference
void UMainMenuWidget::OnButtonClicked_Login(const FLoginPayload LoginPayload)
{
	OnLoginResponse.BindLambda([&] (const FLoginResponse& Response, const FString& ResponseMsg, int ResponseCode)
	{
		if (ResponseCode != 200)
		{
			if (ResponseCode == 401)
			{
				LoginWidget->ShowLoginScreen("Login_InvalidCredentialsText");
				
			}
			else if (ResponseCode == 408 || ResponseCode == 504)
			{
				LoginWidget->ShowLoginScreen("Login_TimeOutErrorText");
			}
			else
			{
				LoginWidget->ShowLoginScreen("Login_LoginErrorText");
			}
			UpdateLoginState(false);
		}
		else
		{
			FPlayerSettings_User PlayerSettingsToSave = LoadPlayerSettings().User;
			PlayerSettingsToSave.UserID = Response.UserID;
			PlayerSettingsToSave.DisplayName = Response.DisplayName;
			PlayerSettingsToSave.RefreshCookie = Response.RefreshToken;
			PlayerSettingsToSave.bHasLoggedInBefore = true;
			SavePlayerSettings(PlayerSettingsToSave);
			TextBlock_SignInState->SetText(FText::FromString("Logging in to web browser..."));

			// Callback function for this will be OnURLChangedResult_ScoresBrowser
			ScoresWidget->LoginUserBrowser(LoginPayload, PlayerSettingsToSave.UserID);
		}
		if (OnLoginResponse.IsBound())
		{
			OnLoginResponse.Unbind();
		}
	});
	TextBlock_SignInState->SetText(FText::FromString("Sending HTTP login request..."));
	LoginUser(LoginPayload, OnLoginResponse);
}
