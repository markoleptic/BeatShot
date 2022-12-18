// Fill out your copyright notice in the Description page of Project Settings.


#include "WebBrowserWidget.h"

#include "SaveGamePlayerSettings.h"
#include "DefaultGameInstance.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"

void UWebBrowserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	bTimerActive = false;
	OnTimerElapsed.BindDynamic(this, &UWebBrowserWidget::OnURLChanged);
}

void UWebBrowserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

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
	Browser->LoadURL(ProfileURL + Username);
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
	GetWorld()->GetTimerManager().SetTimer(CheckCheckboxDelay, this, &UWebBrowserWidget::CheckPersistCheckbox, 0.1f,
	                                       false);
}

void UWebBrowserWidget::ParentTickOverride(float DeltaTime)
{
	if (bTimerActive)
	{
		if (URLCheckTimer > 0.3f && OnTimerElapsed.IsBound())
		{
			OnTimerElapsed.Execute(Browser->GetUrl());
			URLCheckTimer = 0.f;
		}
		URLCheckTimer = URLCheckTimer + DeltaTime;
	}
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
		Browser->ExecuteJavascript(
			SetElementUsernameScript + LoginPayload.Username + "');" + DispatchUsernameChangeEventScript);
	}
	else
	{
		Browser->ExecuteJavascript(SetElementEmailScript + LoginPayload.Email + "');" + DispatchEmailChangeEventScript);
	}
	Browser->ExecuteJavascript(
		SetElementPasswordScript + LoginPayload.Password + "');" + DispatchPasswordChangeEventScript);
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
	if (const FPlayerSettings PlayerSettings = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))
		->
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
	if (URLCheckAttempts > 15)
	{
		bTimerActive = false;
		OnURLLoaded.Broadcast(false);
		UserProfileURL = "";
		URLCheckAttempts = 0;
		return;
	}
	if (LastURL.IsEmpty())
	{
		bTimerActive = true;
		return;
	}
	if (UKismetStringLibrary::StartsWith(LastURL, UserProfileURL, ESearchCase::IgnoreCase))
	{
		bTimerActive = false;
		UserProfileURL = "";
		URLCheckAttempts = 0;
		OnURLLoaded.Broadcast(true);
	}
}
