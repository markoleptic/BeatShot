// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/WebBrowserWidget.h"
#include "TimerManager.h"
#include "WebBrowser.h"
#include "Kismet/KismetStringLibrary.h"

void UWebBrowserWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UWebBrowserWidget::LoadCustomGameModesURL(const FString& Username)
{
	UserProfileURL = ProfileURL + Username;
	Browser->LoadURL(ProfileURL + Username + CustomModesString);
	OnURLChanged();
}

void UWebBrowserWidget::LoadDefaultGameModesURL(const FString& Username)
{
	UserProfileURL = ProfileURL + Username;
	Browser->LoadURL(ProfileURL + Username + DefaultModesString);
	OnURLChanged();
}

void UWebBrowserWidget::LoadPatchNotesURL() const
{
	Browser->LoadURL(PatchNotesURL);
}

void UWebBrowserWidget::LoadProfileURL(const FString& Username)
{
	UserProfileURL = ProfileURL + Username;
	Browser->LoadURL(ProfileURL + Username);
	OnURLChanged();
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

	CheckboxDelegate.BindLambda([this]
	{
		Browser->ExecuteJavascript(CheckPersistScript);
		GetWorld()->GetTimerManager().SetTimer(ClickLoginDelay, ClickLoginDelegate, 0.1f, false);
	});
	ClickLoginDelegate.BindLambda([this]
	{
		Browser->ExecuteJavascript(ClickLoginScript);
		OnURLChanged();
	});
	GetWorld()->GetTimerManager().SetTimer(CheckCheckboxDelay, CheckboxDelegate, 0.1f, false);
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

void UWebBrowserWidget::OnURLChanged()
{
	CheckURLDelegate.BindLambda([this]
	{
		const FString URL = Browser->GetUrl();
		URLCheckAttempts++;
		if (URLCheckAttempts > 6)
		{
			OnURLLoaded.Broadcast(false);
			UserProfileURL = "";
			URLCheckAttempts = 0;
			GetWorld()->GetTimerManager().ClearTimer(CheckURLTimer);
			return;
		}
		if (UKismetStringLibrary::StartsWith(URL, UserProfileURL, ESearchCase::IgnoreCase))
		{
			UserProfileURL = "";
			URLCheckAttempts = 0;
			OnURLLoaded.Broadcast(true);
			GetWorld()->GetTimerManager().ClearTimer(CheckURLTimer);
		}
	});
	GetWorld()->GetTimerManager().SetTimer(CheckURLTimer, CheckURLDelegate, 0.5f, true);
	// GetWorld()->GetTimerManager().SetTimer(CheckURLTimer, FTimerDelegate::CreateLambda([&]
	// {
	// 	const FString URL = Browser->GetUrl();
	// 	URLCheckAttempts++;
	// 	if (URLCheckAttempts > 6)
	// 	{
	// 		OnURLLoaded.Broadcast(false);
	// 		UserProfileURL = "";
	// 		URLCheckAttempts = 0;
	// 		GetWorld()->GetTimerManager().ClearTimer(CheckURLTimer);
	// 		return;
	// 	}
	// 	if (UKismetStringLibrary::StartsWith(URL, UserProfileURL, ESearchCase::IgnoreCase))
	// 	{
	// 		UserProfileURL = "";
	// 		URLCheckAttempts = 0;
	// 		OnURLLoaded.Broadcast(true);
	// 		GetWorld()->GetTimerManager().ClearTimer(CheckURLTimer);
	// 	}
	// }), 0.5f, true);
}
