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
	Browser->LoadURL(UserProfileURL + CustomModesString);
	OnURLChanged(FString());
}

void UWebBrowserWidget::LoadDefaultGameModesURL(const FString& Username)
{
	UserProfileURL = ProfileURL + Username;
	UE_LOG(LogTemp, Display, TEXT("URL: %s"), *UserProfileURL.Append(DefaultModesString));
	Browser->LoadURL(UserProfileURL + DefaultModesString);
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

void UWebBrowserWidget::HandleUserLogin(const FLoginPayload LoginPayload)
{
	if (!FillLoginForm(LoginPayload))
	{
		return;
	}
	if (!LoginPayload.Username.IsEmpty())
	{
		UserProfileURL = ProfileURL + LoginPayload.Username;
	}
	GetWorld()->GetTimerManager().SetTimer(CheckCheckboxDelay, this, &UWebBrowserWidget::CheckPersistCheckbox, 0.1f, false);
}

bool UWebBrowserWidget::FillLoginForm(const FLoginPayload LoginPayload) const
{
	if ((LoginPayload.Username.IsEmpty() && LoginPayload.Email.IsEmpty()) || LoginPayload.Password.IsEmpty())
	{
		return false;
	}
	Browser->ExecuteJavascript(InitialInputEventScript);
	if (!LoginPayload.Username.IsEmpty())
	{
		Browser->ExecuteJavascript(SetElementUsernameScript + LoginPayload.Username + "');" + DispatchUsernameChangeEventScript);
	}
	else
	{
		Browser->ExecuteJavascript(SetElementEmailScript + LoginPayload.Email + "');" + DispatchEmailChangeEventScript);
	}
	Browser->ExecuteJavascript(SetElementPasswordScript + LoginPayload.Password + "');" + DispatchPasswordChangeEventScript);
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
	if (const FPlayerSettings PlayerSettings = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->
		LoadPlayerSettings(); PlayerSettings.HasLoggedInHttp)
	{
		UserProfileURL = ProfileURL + PlayerSettings.Username;
	}
	Browser->ExecuteJavascript(ClickLoginScript);
	OnURLChanged(FString());
}

void UWebBrowserWidget::OnURLChanged(const FString& LastURL)
{
	URLCheckAttempts++;
	UE_LOG(LogTemp, Display, TEXT("Last URL: %s"), *LastURL);
	if (URLCheckAttempts > 15)
	{
		GetWorld()->GetTimerManager().ClearTimer(URLCheckDelay);
		OnURLLoaded.Broadcast(false);
		UserProfileURL="";
		URLCheckAttempts = 0;
		return;
	}
	if (LastURL.IsEmpty() && !GetWorld()->GetTimerManager().IsTimerActive(URLCheckDelay))
	{
		/** TODO: Come up with a fix to deal with timers while the game is paused (inside PostGameMenuWidget) */
		GetWorld()->GetTimerManager().SetTimer(URLCheckDelay, this, &UWebBrowserWidget::OnURLChangedCallback, 0.3f, true);
		return;
	}
	if (UKismetStringLibrary::StartsWith(LastURL, UserProfileURL, ESearchCase::IgnoreCase))
	{
		GetWorld()->GetTimerManager().ClearTimer(URLCheckDelay);
		UserProfileURL="";
		URLCheckAttempts = 0;
		OnURLLoaded.Broadcast(true);
	}
}

void UWebBrowserWidget::OnURLChangedCallback()
{
	OnURLChanged(Browser->GetUrl());
}
