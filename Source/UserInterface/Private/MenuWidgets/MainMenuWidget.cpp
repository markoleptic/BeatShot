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

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	ScoresWidget->OnURLChangedResult.AddUObject(this, &UMainMenuWidget::OnURLChangedResult_ScoresWidget);
	LoginWidget->OnLoginButtonClicked.AddDynamic(this, &UMainMenuWidget::OnButtonClicked_Login);
	LoginWidget->OnExitAnimationCompletedDelegate.AddDynamic(this, &ThisClass::OnLoginWidgetExitAnimationCompleted);
	Button_Feedback->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	LoginWidget->Button_RetrySteamLogin->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
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

void UMainMenuWidget::OnPlayerSettingsChanged_User(const FPlayerSettings_User& UserSettings)
{
	UE_LOG(LogTemp, Display, TEXT("PlayerSettingsChangedUser in MainMenu"));
}

void UMainMenuWidget::LoginScoresWidgetWithSteam(const FString SteamAuthTicket)
{
	CurrentLoginMethod = ELoginMethod::Steam;
	ScoresWidget->LoginUserBrowser(FString(SteamAuthTicket));
}

void UMainMenuWidget::TryFallbackLogin()
{
	CurrentLoginMethod = ELoginMethod::Legacy;
	const FPlayerSettings_User PlayerSettings = LoadPlayerSettings().User;
	if (IsRefreshTokenValid(PlayerSettings.RefreshCookie) && !PlayerSettings.UserID.IsEmpty())
	{
		ScoresWidget->LoadProfile(PlayerSettings.UserID);
	}
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
		if (bFadeInOverlayTextOnButtonPress)
		{
			ScoresWidget->FadeOutLoadingIconAndShowText();
			bFadeInOverlayTextOnButtonPress = false;
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
		LoginWidget->ShowSteamLoginScreen();
	}

	// Feedback button
	else if (Button == Button_Feedback)
	{
		FeedbackWidget->ShowFeedbackWidget();
	}

	else if (Button == LoginWidget->Button_RetrySteamLogin)
	{
		CurrentLoginMethod = ELoginMethod::Steam;
		TextBlock_SignInState->SetText(FText());
		if (OnSteamLoginRequest.IsBound()) OnSteamLoginRequest.Execute();
	}
}

void UMainMenuWidget::OnURLChangedResult_ScoresWidget(const bool bSuccess)
{
	UpdateLoginState(bSuccess);
}

void UMainMenuWidget::UpdateLoginState(const bool bSuccessfulLogin, const FString OptionalFailureMessage)
{
	if (!bSuccessfulLogin)
	{
		if (OptionalFailureMessage.IsEmpty())
		{
			TextBlock_SignInState->SetText(FText::FromString("Not Signed In"));
		}
		else
		{
			TextBlock_SignInState->SetText(FText::FromString(OptionalFailureMessage));
		}
		TextBlock_Username->SetText(FText());
		Button_Login_Register->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		
		bFadeInOverlayTextOnButtonPress = true;
		CurrentLoginMethod = ELoginMethod::None;
		return;
	}

	
	if (CurrentLoginMethod == ELoginMethod::Steam)
	{
		TextBlock_SignInState->SetText(FText::FromString("Signed in through Steam as"));
		Button_Login_Register->SetVisibility(ESlateVisibility::Collapsed);
	}
	else if (CurrentLoginMethod == ELoginMethod::Legacy)
	{
		TextBlock_SignInState->SetText(FText::FromString("Signed in as"));
		Button_Login_Register->ChangeButtonText(FText::FromString("Login Through Steam"));
	}
	else
	{
		TextBlock_SignInState->SetText(FText::FromString("Unhandled Login Method"));
	}

	TextBlock_Username->SetText(FText::FromString(LoadPlayerSettings().User.DisplayName));
	
	bFadeInScoreBrowserOnButtonPress = true;
	CurrentLoginMethod = ELoginMethod::None;
}

void UMainMenuWidget::OnLoginWidgetExitAnimationCompleted()
{
	Button_Login_Register->SetInActive();
}

// ReSharper disable once CppPassValueParameterByConstReference
void UMainMenuWidget::OnButtonClicked_Login(const FLoginPayload LoginPayload)
{
	CurrentLoginMethod = ELoginMethod::Legacy;
	OnLoginResponse.BindLambda([this, LoginPayload] (const FLoginResponse& Response, const FString& ResponseMsg, int ResponseCode)
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

			// Callback function for this will be OnURLChangedResult_ScoresWidget
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
