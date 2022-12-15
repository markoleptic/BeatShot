// Fill out your copyright notice in the Description page of Project Settings.


#include "WebBrowserWidget.h"

#include "SaveGamePlayerSettings.h"
#include "DefaultGameInstance.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"

void UWebBrowserWidget::LoadCustomGameModesURL(const FString& Username)
{
	UserProfileURL = ProfileURL + Username;
	Browser->LoadURL(ProfileURL + Username + CustomModesString);
	OnURLChanged(FString());
}

void UWebBrowserWidget::LoadDefaultGameModesURL(const FString& Username)
{
	UserProfileURL = ProfileURL + Username;
	Browser->LoadURL(ProfileURL + Username + DefaultModesString);
	OnURLChanged(FString());
}

void UWebBrowserWidget::LoadPatchNotesURL() const
{
	Browser->LoadURL(PatchNotesURL);
}

void UWebBrowserWidget::LoadProfileURL(const FString& Username)
{
	UserProfileURL = ProfileURL + Username;
	Browser->LoadURL(UserProfileURL);
	OnURLChanged(FString());
}

void UWebBrowserWidget::HandleUserLogin(const FString& Username, const FString& Email, const FString& Password)
{
	if (!FillLoginForm(Username, Email, Password))
	{
		return;
	}
	if (!Username.IsEmpty())
	{
		UserProfileURL = ProfileURL + Username;
	}
	GetWorld()->GetTimerManager().SetTimer(CheckCheckboxDelay, this, &UWebBrowserWidget::CheckPersistCheckbox, 0.1f, false);
}

bool UWebBrowserWidget::FillLoginForm(const FString& Username, const FString& Email, const FString& Password) const
{
	if ((Username.IsEmpty() && Email.IsEmpty()) || Password.IsEmpty())
	{
		return false;
	}
	Browser->ExecuteJavascript(InitialInputEventScript);
	if (!Username.IsEmpty())
	{
		Browser->ExecuteJavascript(SetElementUsernameScript + Username + "');" + DispatchUsernameChangeEventScript);
	}
	else
	{
		Browser->ExecuteJavascript(SetElementEmailScript + Email + "');" + DispatchEmailChangeEventScript);
	}
	Browser->ExecuteJavascript(SetElementPasswordScript + Password + "');" + DispatchPasswordChangeEventScript);
	return true;
}

void UWebBrowserWidget::CheckPersistCheckbox()
{
	GetWorld()->GetTimerManager().ClearTimer(CheckCheckboxDelay);
	Browser->ExecuteJavascript(CheckPersistScript);
	GetWorld()->GetTimerManager().SetTimer(ClickLoginDelay, this, &UWebBrowserWidget::ClickLogin, 0.1f, false);
}

void UWebBrowserWidget::ClickLogin()
{
	GetWorld()->GetTimerManager().ClearTimer(ClickLoginDelay);
	const FPlayerSettings PlayerSettings = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->LoadPlayerSettings();
	if (PlayerSettings.HasLoggedInHttp)
	{
		UserProfileURL = ProfileURL + PlayerSettings.Username;
	}
	Browser->ExecuteJavascript(ClickLoginScript);
	OnURLChanged(FString());
}

void UWebBrowserWidget::OnURLChanged(const FString& LastURL)
{
	URLCheckAttempts++;
	if (URLCheckAttempts > 15)
	{
		GetWorld()->GetTimerManager().ClearTimer(URLCheckDelay);
		OnURLLoaded.Broadcast(false);
		UserProfileURL="";
		URLCheckAttempts = 0;
		return;
	}
	if (LastURL.IsEmpty())
	{
		GetWorld()->GetTimerManager().SetTimer(URLCheckDelay, this, &UWebBrowserWidget::OnURLChangedCallback, 0.3f, true, 0.5f);
	}
	else if (UKismetStringLibrary::StartsWith(LastURL, UserProfileURL, ESearchCase::IgnoreCase))
	{
		GetWorld()->GetTimerManager().ClearTimer(URLCheckDelay);
		UserProfileURL="";
		FString Right, Left;
		UKismetStringLibrary::Split(LastURL, "profile/", Left, Right, ESearchCase::CaseSensitive, ESearchDir::FromStart);
		UDefaultGameInstance* GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		FPlayerSettings PlayerSettings = GI->LoadPlayerSettings();
		Right.Split("/", &Left, &Right);
		PlayerSettings.Username = Right;
		PlayerSettings.HasLoggedInHttp = true;
		UE_LOG(LogTemp, Display, TEXT("Right: %s"), *Right);
		GI->SavePlayerSettings(PlayerSettings);
		URLCheckAttempts = 0;
		OnURLLoaded.Broadcast(true);
	}
}

void UWebBrowserWidget::OnURLChangedCallback()
{
	OnURLChanged(Browser->GetUrl());
}
