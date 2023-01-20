// Fill out your copyright notice in the Description page of Project Settings.


#include "SubMenuWidgets/ScoreBrowserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Overlay.h"
#include "OverlayWidgets/LoginWidget.h"
#include "WidgetComponents/WebBrowserWidget.h"

void UScoreBrowserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BindToAnimationFinished(FadeOutOverlay, FadeOutDelegate);
	FadeOutDelegate.BindUFunction(BrowserOverlay, FName("RemoveFromParent"));
	MID_LoadingIcon = LoadingIcon->GetDynamicMaterial();
	LoginWidget->OnLoginButtonClicked.AddDynamic(this, &UScoreBrowserWidget::LoginUserHttp);
	BrowserWidget->OnURLLoaded.AddDynamic(this, &UScoreBrowserWidget::OnURLLoaded);
}

void UScoreBrowserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	TotalDeltaTime += InDeltaTime;
	if (BrowserOverlay)
	{
		MID_LoadingIcon->SetScalarParameterValue(FName("Time"), TotalDeltaTime);
	}
}

void UScoreBrowserWidget::InitializeScoringOverlay()
{
	/* If user has logged in before, check Refresh Token */
	if (const FPlayerSettings PlayerSettings = LoadPlayerSettings(); PlayerSettings.HasLoggedInHttp)
	{
		if (!IsRefreshTokenValid(PlayerSettings))
		{
			LoginWidget->ShowLoginScreen("Login_LoginErrorText");
			OnLoginStateChange.Broadcast(ELoginState::InvalidHttp, false);
		}
		else
		{
			/* If logged in and has valid refresh token */
			OnAccessTokenResponseDelegate.BindUFunction(this, "OnAccessTokenResponse");
			RequestAccessToken(PlayerSettings.LoginCookie, OnAccessTokenResponseDelegate);
		}
	}
	/* Show Register screen if new user */
	else
	{
		LoginWidget->ShowRegisterScreen();
		OnLoginStateChange.Broadcast(ELoginState::NewUser, false);
	}
}

void UScoreBrowserWidget::InitializePostGameScoringOverlay(const ELoginState& LoginState)
{
	switch (LoginState)
	{
	/** Didn't save scores because of zero score or unsaved game mode */
	case ELoginState::None:
		{
			SetOverlayText("SBW_DidNotSaveScores");
			return;
		}
	/** User doesn't have an account*/
	case ELoginState::NewUser:
		{
			SetOverlayText("SBW_NoAccount");
			return;
		}
	/** Successful Http POST scores request*/
	case ELoginState::LoggedInHttp:
		{
			break;
		}
	/** Invalid Refresh Token or Http POST scores request failed */
	default:
		{
			SetOverlayText("SBW_SavedScoresLocallyOnly");
			return;
		}
	}
	
	int64 MinTimeDifference = FDateTime::MinValue().ToUnixTimestamp();
	FPlayerScore MinDateScore = FPlayerScore();
	/** Could probably just use the last value in PlayerScoreArray, but just to be sure: */
	for (const FPlayerScore& Score : LoadPlayerScores())
	{
		FDateTime ParsedTime;
		FDateTime::ParseIso8601(*Score.Time, ParsedTime);
		if (ParsedTime.ToUnixTimestamp() > MinTimeDifference)
		{
			MinTimeDifference = ParsedTime.ToUnixTimestamp();
			MinDateScore = Score;
		}
	}
	if (MinDateScore.CustomGameModeName.IsEmpty())
	{
		BrowserWidget->LoadDefaultGameModesURL(LoadPlayerSettings().Username);
	}
	else
	{
		BrowserWidget->LoadCustomGameModesURL(LoadPlayerSettings().Username);
	}
}

void UScoreBrowserWidget::SetOverlayText(const FString& Key)
{
	OverlayText->SetText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets",
	                                            Key));
	FadeInText();
}

void UScoreBrowserWidget::FadeOutLoadingOverlay()
{
	PlayAnimationForward(FadeOutOverlay);
}

void UScoreBrowserWidget::FadeInText()
{
	PlayAnimationForward(FadeInOverlayText);
}

void UScoreBrowserWidget::LoginUserHttp(const FLoginPayload LoginPayload, const bool bIsPopup)
{
	bSignedInThroughPopup = bIsPopup;
	OnLoginResponse.BindUFunction(this, "OnHttpLoginResponse");
	LoginInfo = LoginPayload;
	LoginUser(LoginPayload, OnLoginResponse);
}

void UScoreBrowserWidget::OnHttpLoginResponse(const FPlayerSettings& PlayerSettings, FString ResponseMsg, const int32 ResponseCode)
{
	if (ResponseCode != 200)
	{
		if (ResponseCode == 401)
		{
			OnLoginStateChange.Broadcast(ELoginState::InvalidCredentials, bSignedInThroughPopup);
			if (!bSignedInThroughPopup)
			{
				LoginWidget->ShowLoginScreen("Login_InvalidCredentialsText");
			}
		}
		else if (ResponseCode == 408 || ResponseCode == 504)
		{
			OnLoginStateChange.Broadcast(ELoginState::TimeOut, bSignedInThroughPopup);
			if (!bSignedInThroughPopup)
			{
				LoginWidget->ShowLoginScreen("Login_TimeOutErrorText");
			}
		}
		else
		{
			OnLoginStateChange.Broadcast(ELoginState::InvalidHttp, bSignedInThroughPopup);
			if (!bSignedInThroughPopup)
			{
				LoginWidget->ShowLoginScreen("Login_LoginErrorText");
			}
		}
	}
	else
	{
		FPlayerSettings PlayerSettingsToSave = LoadPlayerSettings();
		PlayerSettingsToSave.HasLoggedInHttp = PlayerSettings.HasLoggedInHttp;
		PlayerSettingsToSave.Username = PlayerSettings.Username;
		PlayerSettingsToSave.LoginCookie = PlayerSettings.LoginCookie;
		SavePlayerSettings(PlayerSettingsToSave);
		LoginInfo.Username = PlayerSettings.Username;
		LoginUserBrowser(LoginInfo);
	}
	LoginInfo = FLoginPayload();
	if (OnLoginResponse.IsBound())
	{
		OnLoginResponse.Unbind();
	}
}

void UScoreBrowserWidget::LoginUserBrowser(const FLoginPayload LoginPayload)
{
	BrowserWidget->HandleUserLogin(LoginPayload);
}

void UScoreBrowserWidget::OnURLLoaded(const bool bLoadedSuccessfully)
{
	/* Limited set of instructions if this widget is a child of PostGameMenuWidget */
	if (bIsPostGameScoringOverlay)
	{
		if (bLoadedSuccessfully)
		{
			FadeOutLoadingOverlay();
		}
		else
		{
			SetOverlayText("SBW_SavedScoresButNotLoggedIn");
		}
		return;
	}

	/* Unsuccessful URL load in browser */
	if (!bLoadedSuccessfully)
	{
		LoginWidget->ShowLoginScreen("Login_BrowserLoginErrorText");
		OnLoginStateChange.Broadcast(ELoginState::InvalidBrowser, bSignedInThroughPopup);
		return;
	}

	/** Don't not show a success message if they were logging in from the Popup in MainMenu so the user does not receive two success messages */
	if (!bSignedInThroughPopup)
	{
		LoginWidget->OkayButton->OnClicked.AddDynamic(this, &UScoreBrowserWidget::FadeOutLoadingOverlay);
		LoginWidget->OnLoginSuccess();
	}
	else
	{
		LoginWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	OnLoginStateChange.Broadcast(ELoginState::LoggedInHttpAndBrowser, bSignedInThroughPopup);
}

void UScoreBrowserWidget::OnAccessTokenResponse(const FString& AccessToken)
{
	if (!AccessToken.IsEmpty())
	{
		BrowserWidget->LoadProfileURL(ISaveLoadInterface::LoadPlayerSettings().Username);
	}
	else
	{
		SetOverlayText("SBW_InvalidHttpResponse");
		OnLoginStateChange.Broadcast(ELoginState::InvalidHttp, bSignedInThroughPopup);
	}
	if (OnAccessTokenResponseDelegate.IsBound())
	{
		OnAccessTokenResponseDelegate.Unbind();
	}
}
