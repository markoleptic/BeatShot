// Fill out your copyright notice in the Description page of Project Settings.


#include "WebBrowserOverlay.h"

#include "LoginWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Overlay.h"
#include "Kismet/GameplayStatics.h"

void UWebBrowserOverlay::NativeConstruct()
{
	Super::NativeConstruct();
	BindToAnimationFinished(FadeOutOverlay, FadeOutDelegate);
	FadeOutDelegate.BindUFunction(BrowserOverlay, FName("RemoveFromParent"));
}

void UWebBrowserOverlay::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
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
			LoginWidget->OnLoginButtonClicked.AddDynamic(this, &UWebBrowserOverlay::LoginUserHttp);
			OnLoginStateChange.Broadcast(true, false, false);
		}
		else
		{
			/* Default behavior if logged in and has valid refresh token */
			GI->OnRefreshTokenResponse.AddDynamic(this, &UWebBrowserOverlay::OnAccessTokenResponse);
			GI->RequestAccessToken();
			BrowserWidget->OnURLLoaded.AddDynamic(this, &UWebBrowserOverlay::OnURLLoaded);
			BrowserWidget->LoadProfileURL(PlayerSettings.Username);
		}
	}
	/* Show Register screen if new user */
	else
	{
		LoginWidget->ShowRegisterScreen();
		LoginWidget->OnLoginButtonClicked.AddDynamic(this, &UWebBrowserOverlay::LoginUserHttp);
		OnLoginStateChange.Broadcast(false, false, false);
	}
}

void UWebBrowserOverlay::InitializePostGameScoringOverlay(const FString& ResponseMessage, const int32 ResponseCode)
{
	UE_LOG(LogTemp, Display, TEXT("From InitPostGameScoringOverlay: %s"), *ResponseMessage);
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
	const FPlayerSettings PlayerSettings = GI->LoadPlayerSettings();

	BrowserWidget->OnURLLoaded.AddDynamic(this, &UWebBrowserOverlay::OnURLLoaded);
	if (GI->GameModeActorStruct.CustomGameModeName.IsEmpty())
	{
		BrowserWidget->LoadDefaultGameModesURL(PlayerSettings.Username);
	}
	else
	{
		BrowserWidget->LoadCustomGameModesURL(PlayerSettings.Username);
	}
}

void UWebBrowserOverlay::SetOverlayText(const FString& Key)
{
	OverlayText->SetText(FText::FromStringTable("/Game/UI/StringTables/ST_WebBrowserOverlay.ST_WebBrowserOverlay",
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
	GI->OnLoginResponse.AddDynamic(this, &UWebBrowserOverlay::OnHttpLoginResponse);
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

	UDefaultGameInstance* GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GI->OnLoginResponse.RemoveDynamic(this, &UWebBrowserOverlay::OnHttpLoginResponse);

	UE_LOG(LogTemp, Display, TEXT("Http Login Resposne: %d"), ResponseCode);

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
	BrowserWidget->OnURLLoaded.AddDynamic(this, &UWebBrowserOverlay::OnURLLoaded);
	BrowserWidget->HandleUserLogin(LoginPayload);
}

void UWebBrowserOverlay::OnURLLoaded(const bool bLoadedSuccessfully)
{
	UE_LOG(LogTemp, Display, TEXT("OnURLLoaded called"));
	if (bIsPostGameScoringOverlay)
	{
		UE_LOG(LogTemp, Display, TEXT("bIsPostGameScoringOverlay"));
		if (bLoadedSuccessfully)
		{
			FadeOut();
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("Unsuccessful URL login"));
			SetOverlayText("SavedScoresButNotLoggedIn");
		}
		return;
	}

	URLLoadSuccess = bLoadedSuccessfully;

	if (bHttpResponse)
	{
		UE_LOG(LogTemp, Display, TEXT("bHttpResponse True"));
	}
	/* Set timer to keep calling this function until we get an Http response */
	if (!bHttpResponse)
	{
		if (!GetWorld()->GetTimerManager().IsTimerActive(HttpTimerHandle))
		{
			GetWorld()->GetTimerManager().SetTimer(HttpTimerHandle, this, &UWebBrowserOverlay::HttpDelay, 0.2f, true,
			                                       0.5f);
		}
		UE_LOG(LogTemp, Display, TEXT("bHttpResponse False"));
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("1"));
	/* Clear timer when we get an Http response */
	if (GetWorld()->GetTimerManager().IsTimerActive(HttpTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(HttpTimerHandle);
	}

	UE_LOG(LogTemp, Display, TEXT("2"));
	/* Unsuccessful Http Response */
	if (!bHttpResponseSuccess)
	{
		UE_LOG(LogTemp, Display, TEXT("Failed Http response"));
		SetOverlayText("InvalidHttpResponse");
		OnLoginStateChange.Broadcast(false, false, bSignedInThroughPopup);
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("3"));
	/* Unsuccessful URL load in browser */
	if (!bLoadedSuccessfully)
	{
		LoginWidget->ShowLoginScreen();
		OnLoginStateChange.Broadcast(true, false, bSignedInThroughPopup);
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("4"));
	/* Successful URL load in browser for user that has logged in before */
	if (!bIsLoggingIn)
	{
		UE_LOG(LogTemp, Display, TEXT("Successful URL load in browser for user that has logged in before"));
		OnLoginStateChange.Broadcast(true, true, bSignedInThroughPopup);
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("5"));
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
	UE_LOG(LogTemp, Display, TEXT("Http Delay called"));
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
