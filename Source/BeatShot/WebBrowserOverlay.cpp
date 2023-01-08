// Fill out your copyright notice in the Description page of Project Settings.


#include "WebBrowserOverlay.h"

#include "LoginWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Overlay.h"
#include "GameModeActorBase.h"
#include "Kismet/GameplayStatics.h"

void UWebBrowserOverlay::NativeConstruct()
{
	Super::NativeConstruct();
	BindToAnimationFinished(FadeOutOverlay, FadeOutDelegate);
	FadeOutDelegate.BindUFunction(BrowserOverlay, FName("RemoveFromParent"));
	MID_LoadingIcon = LoadingIcon->GetDynamicMaterial();
	LoginWidget->OnLoginButtonClicked.AddDynamic(this, &UWebBrowserOverlay::LoginUserHttp);
	BrowserWidget->OnURLLoaded.AddDynamic(this, &UWebBrowserOverlay::OnURLLoaded);
}

void UWebBrowserOverlay::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	TotalDeltaTime += InDeltaTime;
	if (BrowserOverlay)
	{
		MID_LoadingIcon->SetScalarParameterValue(FName("Time"), TotalDeltaTime);
	}
}

void UWebBrowserOverlay::InitializeScoringOverlay()
{
	UDefaultGameInstance* GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	/* If user has logged in before, check Refresh Token */
	if (const FPlayerSettings PlayerSettings = GI->LoadPlayerSettings(); PlayerSettings.HasLoggedInHttp)
	{
		if (!GI->IsRefreshTokenValid())
		{
			LoginWidget->ShowLoginScreen("LoginErrorText");
			OnLoginStateChange.Broadcast(ELoginState::InvalidHttp, false);
		}
		else
		{
			/* If logged in and has valid refresh token */
			GI->OnRefreshTokenResponse.AddDynamic(this, &UWebBrowserOverlay::OnAccessTokenResponse);
			GI->RequestAccessToken();
		}
	}
	/* Show Register screen if new user */
	else
	{
		LoginWidget->ShowRegisterScreen();
		OnLoginStateChange.Broadcast(ELoginState::NewUser, false);
	}
}

void UWebBrowserOverlay::InitializePostGameScoringOverlay(const ELoginState& LoginState)
{
	switch (LoginState)
	{
	/** User doesn't have an account*/
	case ELoginState::NewUser:
		{
			SetOverlayText("NoAccount");
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
			SetOverlayText("SavedScoresLocallyOnly");
			return;
		}
	}

	const UDefaultGameInstance* GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	int64 MinTimeDifference = FDateTime::MinValue().ToUnixTimestamp();
	FPlayerScore MinDateScore = FPlayerScore();
	/** Could probably just use the last value in PlayerScoreArray, but just to be sure: */
	for (const FPlayerScore& Score : GI->LoadPlayerScores())
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
		BrowserWidget->LoadDefaultGameModesURL(GI->LoadPlayerSettings().Username);
	}
	else
	{
		BrowserWidget->LoadCustomGameModesURL(GI->LoadPlayerSettings().Username);
	}
}

void UWebBrowserOverlay::SetOverlayText(const FString& Key)
{
	OverlayText->SetText(FText::FromStringTable("/Game/StringTables/ST_WebBrowserOverlay.ST_WebBrowserOverlay",
	                                            Key));
	FadeInText();
}

void UWebBrowserOverlay::FadeOutLoadingOverlay()
{
	PlayAnimationForward(FadeOutOverlay);
}

void UWebBrowserOverlay::FadeInText()
{
	PlayAnimationForward(FadeInOverlayText);
}

void UWebBrowserOverlay::LoginUserHttp(const FLoginPayload LoginPayload, const bool bIsPopup)
{
	Reset();
	bSignedInThroughPopup = bIsPopup;
	UDefaultGameInstance* GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GI->OnLoginResponse.AddDynamic(this, &UWebBrowserOverlay::OnHttpLoginResponse);
	LoginInfo = LoginPayload;
	GI->LoginUser(LoginPayload);
}

void UWebBrowserOverlay::OnHttpLoginResponse(FString ResponseMsg, const int32 ResponseCode)
{
	if (ResponseCode != 200)
	{
		if (ResponseCode == 401)
		{
			OnLoginStateChange.Broadcast(ELoginState::InvalidCredentials, bSignedInThroughPopup);
			if (!bSignedInThroughPopup)
			{
				LoginWidget->ShowLoginScreen("InvalidCredentialsText");
			}
		}
		else if (ResponseCode == 408 || ResponseCode == 504)
		{
			OnLoginStateChange.Broadcast(ELoginState::TimeOut, bSignedInThroughPopup);
			if (!bSignedInThroughPopup)
			{
				LoginWidget->ShowLoginScreen("TimeOutErrorText");
			}
		}
		else
		{
			OnLoginStateChange.Broadcast(ELoginState::InvalidHttp, bSignedInThroughPopup);
			if (!bSignedInThroughPopup)
			{
				LoginWidget->ShowLoginScreen("LoginErrorText");
			}
		}
	}
	else
	{
		LoginInfo.Username = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->
		                     LoadPlayerSettings().Username;
		LoginUserBrowser(LoginInfo);
	}
	LoginInfo = FLoginPayload();
	Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->OnLoginResponse.RemoveDynamic(
		this, &UWebBrowserOverlay::OnHttpLoginResponse);
}

void UWebBrowserOverlay::LoginUserBrowser(const FLoginPayload LoginPayload)
{
	BrowserWidget->HandleUserLogin(LoginPayload);
}

void UWebBrowserOverlay::OnURLLoaded(const bool bLoadedSuccessfully)
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
			SetOverlayText("SavedScoresButNotLoggedIn");
		}
		return;
	}

	/* Unsuccessful URL load in browser */
	if (!bLoadedSuccessfully)
	{
		LoginWidget->ShowLoginScreen("BrowserLoginErrorText");
		OnLoginStateChange.Broadcast(ELoginState::InvalidBrowser, bSignedInThroughPopup);
		return;
	}

	/** Don't not show a success message if they were logging in from the Popup in MainMenu so the user does not receive two success messages */
	if (!bSignedInThroughPopup)
	{
		LoginWidget->OkayButton->OnClicked.AddDynamic(this, &UWebBrowserOverlay::FadeOutLoadingOverlay);
		LoginWidget->OnLoginSuccess();
	}
	else
	{
		LoginWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	OnLoginStateChange.Broadcast(ELoginState::LoggedInHttpAndBrowser, bSignedInThroughPopup);
}

void UWebBrowserOverlay::Reset()
{
	bSignedInThroughPopup = false;
}

void UWebBrowserOverlay::OnAccessTokenResponse(const bool Success)
{
	if (Success)
	{
		BrowserWidget->LoadProfileURL(
			Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->LoadPlayerSettings().Username);
	}
	else
	{
		SetOverlayText("InvalidHttpResponse");
		OnLoginStateChange.Broadcast(ELoginState::InvalidHttp, bSignedInThroughPopup);
	}
}
