// Fill out your copyright notice in the Description page of Project Settings.


#include "WebBrowserOverlay.h"

#include "DefaultGameInstance.h"
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
	//OverlayText->SetText(FText::FromStringTable("WebBrowserOverlayStrings", "BaseGameMode"));
}

void UWebBrowserOverlay::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UWebBrowserOverlay::InitializeScoringOverlay()
{
	const UDefaultGameInstance* GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	const FPlayerSettings PlayerSettings = GI->LoadPlayerSettings();
	if (!PlayerSettings.HasLoggedInHttp)
	{
		LoginWidget->ShowRegisterScreen(false);
		LoginWidget->OnLoginButtonClicked.AddDynamic(this, &UWebBrowserOverlay::LoginUser);
		OnLoginAttempted.Broadcast(false, false);
		return;
	}
	BrowserWidget->OnURLLoaded.AddDynamic(this, &UWebBrowserOverlay::OnURLLoaded);
	BrowserWidget->LoadProfileURL(PlayerSettings.Username);
}

void UWebBrowserOverlay::SetOverlayText(const FString& Key) const
{
	OverlayText->SetText(FText::FromStringTable("WebBrowserOverlayStrings", Key));
}

void UWebBrowserOverlay::FadeOut()
{
	PlayAnimationForward(FadeOutOverlay);
}

void UWebBrowserOverlay::OnURLLoaded(const bool bLoadedSuccessfully)
{
	if (!bLoadedSuccessfully)
	{
		LoginWidget->ShowLoginScreen(false);
		OnLoginAttempted.Broadcast(false, true);
		return;
	}
	if (bIsLoggingIn)
	{
		const UDefaultGameInstance* GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		FPlayerSettings PlayerSettings = GI->LoadPlayerSettings();
		PlayerSettings.HasLoggedInBrowser = true;
		PlayerSettings.HasLoggedInHttp = true;
		GI->SavePlayerSettings(PlayerSettings);
		LoginWidget->OkayButton->OnClicked.AddDynamic(this, &UWebBrowserOverlay::FadeOut);
		LoginWidget->OnLoginSuccess();
	}
	OnLoginAttempted.Broadcast(true, true);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UWebBrowserOverlay::OnURLLoadedFromPopup(const bool bLoadedSuccessfully)
{
	if (!bLoadedSuccessfully)
	{
		LoginWidget->ShowLoginScreen(false);
		OnLoginAttempted.Broadcast(false, true);
		return;
	}
	if (bIsLoggingIn)
	{
		const UDefaultGameInstance* GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		FPlayerSettings PlayerSettings = GI->LoadPlayerSettings();
		PlayerSettings.HasLoggedInBrowser = true;
		PlayerSettings.HasLoggedInHttp = true;
		GI->SavePlayerSettings(PlayerSettings);
	}
	LoginWidget->SetVisibility(ESlateVisibility::Collapsed);
	OnLoginAttempted.Broadcast(true, true);
}

void UWebBrowserOverlay::LoginUser(const FString Username, const FString Email, const FString Password)
{
	bIsLoggingIn = true;
	BrowserWidget->OnURLLoaded.AddDynamic(this, &UWebBrowserOverlay::OnURLLoaded);
	BrowserWidget->HandleUserLogin(Username, Email, Password);
}

void UWebBrowserOverlay::LoginUserFromPopup(const FString Username, const FString Email, const FString Password)
{
	bIsLoggingIn = true;
	BrowserWidget->OnURLLoaded.AddDynamic(this, &UWebBrowserOverlay::OnURLLoadedFromPopup);
	BrowserWidget->HandleUserLogin(Username, Email, Password);
}
