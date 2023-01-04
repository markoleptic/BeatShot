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
	UDefaultGameInstance* GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	BindToAnimationFinished(FadeOutOverlay, FadeOutDelegate);
	FadeOutDelegate.BindUFunction(BrowserOverlay, FName("RemoveFromParent"));

	MID_LoadingIcon = LoadingIcon->GetDynamicMaterial();

	GI->OnRefreshTokenResponse.AddDynamic(this, &UWebBrowserOverlay::OnAccessTokenResponse);
	GI->OnLoginResponse.AddDynamic(this, &UWebBrowserOverlay::OnHttpLoginResponse);
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
			LoginWidget->ShowLoginScreen();
			OnLoginStateChange.Broadcast(true, false, false);
		}
		else
		{
			/* Default behavior if logged in and has valid refresh token */
			GI->RequestAccessToken();
			BrowserWidget->LoadProfileURL(PlayerSettings.Username);
		}
	}
	/* Show Register screen if new user */
	else
	{
		LoginWidget->ShowRegisterScreen();
		OnLoginStateChange.Broadcast(false, false, false);
	}
}

void UWebBrowserOverlay::InitializePostGameScoringOverlay(const FString& ResponseMessage, const int32 ResponseCode)
{
	switch (ResponseCode)
	{
	/** User doesn't have an account*/
	case 900:
		{
			SetOverlayText(ResponseMessage);
			return;
		}
	/** Invalid Refresh Token */
	case 901:
		{
			SetOverlayText(ResponseMessage);
			return;
		}
	/** Successful Http POST scores request*/
	case 200:
		{
			break;
		}
	/** Http POST scores request failed */
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
		UE_LOG(LogTemp, Display, TEXT("DateTime: %s"), *Score.Time);
		if (ParsedTime.ToUnixTimestamp() > MinTimeDifference)
		{
			MinTimeDifference = ParsedTime.ToUnixTimestamp();
			MinDateScore = Score;
		}
	}
	BrowserWidget->OnURLLoaded.AddDynamic(this, &UWebBrowserOverlay::OnURLLoaded);
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

void UWebBrowserOverlay::FadeOut()
{
	PlayAnimationForward(FadeOutOverlay);
}

void UWebBrowserOverlay::FadeInText()
{
	PlayAnimationForward(FadeInOverlayText);
}

/* Bind to Game Instance's Http login response to call OnHttpLoginResponse, call LoginUserBrowser */
void UWebBrowserOverlay::LoginUserHttp(const FLoginPayload LoginPayload, const bool bIsPopup)
{
	Reset();
	bSignedInThroughPopup = bIsPopup;
	UDefaultGameInstance* GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GI->LoginUser(LoginPayload);
	LoginUserBrowser(LoginPayload);
}

void UWebBrowserOverlay::OnHttpLoginResponse(FString ResponseMsg, const int32 ResponseCode)
{
	/* Clear timer when we get an Http response */
	if (GetWorld()->GetTimerManager().IsTimerActive(HttpTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(HttpTimerHandle);
	}
	bHttpResponse = true;
	if (ResponseCode != 200)
	{
		bHttpResponseSuccess = false;
	}
	else
	{
		bHttpResponseSuccess = true;
	}
}

void UWebBrowserOverlay::LoginUserBrowser(const FLoginPayload LoginPayload)
{
	bIsLoggingIn = true;
	BrowserWidget->HandleUserLogin(LoginPayload);
}

void UWebBrowserOverlay::OnURLLoaded(const bool bLoadedSuccessfully)
{
	URLLoadSuccess = bLoadedSuccessfully;

	/* Limited set of instructions if this widget is a child of PostGameMenuWidget */
	if (bIsPostGameScoringOverlay)
	{
		if (bLoadedSuccessfully)
		{
			FadeOut();
		}
		else
		{
			SetOverlayText("SavedScoresButNotLoggedIn");
		}
		return;
	}

	/* Set timer to keep calling this function until we get an Http response */
	if (!bHttpResponse)
	{
		if (!GetWorld()->GetTimerManager().IsTimerActive(HttpTimerHandle))
		{
			GetWorld()->GetTimerManager().SetTimer(HttpTimerHandle, this, &UWebBrowserOverlay::HttpDelay, 0.2f, true,
			                                       0.5f);
		}
		return;
	}
	
	/* Clear timer when we get an Http response */
	if (GetWorld()->GetTimerManager().IsTimerActive(HttpTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(HttpTimerHandle);
	}
	
	/* Unsuccessful Http Response */
	if (!bHttpResponseSuccess)
	{
		SetOverlayText("InvalidHttpResponse");
		OnLoginStateChange.Broadcast(false, false, bSignedInThroughPopup);
		return;
	}
	
	/* Unsuccessful URL load in browser */
	if (!bLoadedSuccessfully)
	{
		LoginWidget->ShowLoginScreen();
		OnLoginStateChange.Broadcast(true, false, bSignedInThroughPopup);
		return;
	}
	
	/* Successful URL load in browser for user that has logged in before */
	if (!bIsLoggingIn)
	{
		OnLoginStateChange.Broadcast(true, true, bSignedInThroughPopup);
		return;
	}
	
	/** Don't not show a success message if they were logging in from the Popup in MainMenu so the user does not receive two success messages */
	if (!bSignedInThroughPopup)
	{
		LoginWidget->OkayButton->OnClicked.AddDynamic(this, &UWebBrowserOverlay::FadeOut);
		LoginWidget->OnLoginSuccess();
	}
	else
	{
		LoginWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	OnLoginStateChange.Broadcast(true, true, bSignedInThroughPopup);
}

void UWebBrowserOverlay::HttpDelay()
{
	OnURLLoaded(URLLoadSuccess);
}

void UWebBrowserOverlay::Reset()
{
	bHttpResponse = false;
	bHttpResponseSuccess = false;
	bIsLoggingIn = false;
	URLLoadSuccess = false;
	bSignedInThroughPopup = false;
}

void UWebBrowserOverlay::OnAccessTokenResponse(const bool Success)
{
	bHttpResponse = true;
	if (Success)
	{
		bHttpResponseSuccess = true;
	}
	else
	{
		bHttpResponseSuccess = false;
	}
	UDefaultGameInstance* GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GI->OnRefreshTokenResponse.RemoveDynamic(this, &UWebBrowserOverlay::OnAccessTokenResponse);
}
