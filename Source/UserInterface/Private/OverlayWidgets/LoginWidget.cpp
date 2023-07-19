// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "OverlayWidgets/LoginWidget.h"
#include "Animation/WidgetAnimation.h"
#include "Components/HorizontalBox.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "WidgetComponents/BSButton.h"

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	Button_RetrySteamLogin->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	Button_FromSteam_ToLegacyLogin->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	Button_NoSteamLogin->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	
	Button_Login->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	Button_NoLogin->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	Button_Register->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	Button_FromLogin_ToSteam->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	
	Button_NoLoginCancel->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	Button_NoLoginConfirm->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	
	Value_UsernameEmail->OnTextChanged.AddDynamic(this, &ULoginWidget::ClearErrorText);
	Value_Password->OnTextChanged.AddDynamic(this, &ULoginWidget::ClearErrorText);
}

void ULoginWidget::ShowLoginScreen(const FString& Key)
{
	TextBlock_ContinueWithoutTitle->SetText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "Login_ContinueWithoutTitleTextLogin"));
	TextBlock_ContinueWithoutBody->SetText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "Login_ContinueWithoutBodyTextLogin"));
	Button_NoLoginCancel->ChangeButtonText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "Login_ContinueWithoutCancelButtonTextLogin"));
	
	if (!Key.IsEmpty())
	{
		Box_Error->SetVisibility(ESlateVisibility::Visible);
		SetErrorText(Key);
	}
	SetVisibility(ESlateVisibility::Visible);
	PlayFadeInLogin();
}

void ULoginWidget::ShowSteamLoginScreen()
{
	SetVisibility(ESlateVisibility::Visible);
	PlayFadeInSteamLogin();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ULoginWidget::ClearErrorText(const FText& Text)
{
	Box_Error->SetVisibility(ESlateVisibility::Collapsed);
	TextBlock_Error->SetText(FText());
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ULoginWidget::SetErrorText(const FString& Key)
{
	TextBlock_Error->SetText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", Key));
}

void ULoginWidget::InitializeExit()
{
	if (IsAnimationPlaying(FadeOutContinueWithout))
	{
		FadeOutDelegate.BindDynamic(this, &ULoginWidget::OnExitAnimationCompleted);
		BindToAnimationFinished(FadeOutContinueWithout, FadeOutDelegate);
		UE_LOG(LogTemp, Display, TEXT("FadeOutContinueWithout"));
	}
	else if (IsAnimationPlaying(FadeOutLogin))
	{
		FadeOutDelegate.BindDynamic(this, &ULoginWidget::OnExitAnimationCompleted);
		BindToAnimationFinished(FadeOutLogin, FadeOutDelegate);
		UE_LOG(LogTemp, Display, TEXT("FadeOutLogin"));
	}
	else if (IsAnimationPlaying(FadeOutSteam))
	{
		FadeOutDelegate.BindDynamic(this, &ULoginWidget::OnExitAnimationCompleted);
		BindToAnimationFinished(FadeOutSteam, FadeOutDelegate);
		UE_LOG(LogTemp, Display, TEXT("FadeOutSteam"));
	}
	else
	{
		OnExitAnimationCompleted();
		UE_LOG(LogTemp, Display, TEXT("No animatino playing"));
	}
}

void ULoginWidget::OnExitAnimationCompleted()
{
	OnExitAnimationCompletedDelegate.Broadcast();
	UnbindFromAnimationFinished(FadeOutContinueWithout, FadeOutDelegate);
	UnbindFromAnimationFinished(FadeOutSteam, FadeOutDelegate);
	UnbindFromAnimationFinished(FadeOutLogin, FadeOutDelegate);
	SetVisibility(ESlateVisibility::Collapsed);
}

void ULoginWidget::OnButtonClicked_BSButton(const UBSButton* Button)
{
	if (Button == Button_RetrySteamLogin)
	{
		PlayFadeOutSteamLogin();
		InitializeExit();
	}
	else if (Button == Button_FromSteam_ToLegacyLogin)
	{
		PlayFadeOutSteamLogin();
		PlayFadeInLogin();
	}
	else if (Button == Button_NoSteamLogin)
	{
		PlayFadeOutSteamLogin();
		PlayFadeInContinueWithout();
	}
	else if (Button == Button_Login)
	{
		LoginButtonClicked();
	}
	else if (Button == Button_Register)
	{
		LaunchRegisterURL();
	}
	else if (Button == Button_NoLogin)
	{
		PlayFadeOutLogin();
		PlayFadeInContinueWithout();
	}
	else if (Button == Button_FromLogin_ToSteam)
	{
		PlayFadeOutLogin();
		PlayFadeInSteamLogin();
	}
	else if (Button == Button_NoLoginCancel)
	{
		PlayFadeOutContinueWithout();
		PlayFadeInSteamLogin();
	}
	else if (Button == Button_NoLoginConfirm)
	{
		PlayFadeOutContinueWithout();
		InitializeExit();
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ULoginWidget::LoginButtonClicked()
{
	if (Value_UsernameEmail->GetText().IsEmpty() || Value_Password->GetText().IsEmpty())
	{
		Box_Error->SetVisibility(ESlateVisibility::Visible);
		SetErrorText("MissingInfoErrorText");
		return;
	}
	
	const FRegexPattern EmailPattern("[a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*@(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?");
	if (FRegexMatcher EmailMatch(EmailPattern, Value_UsernameEmail->GetText().ToString()); EmailMatch.FindNext())
	{
		OnLoginButtonClicked.Broadcast(FLoginPayload("", Value_UsernameEmail->GetText().ToString(), Value_Password->GetText().ToString()));
	}
	else
	{
		OnLoginButtonClicked.Broadcast(FLoginPayload(Value_UsernameEmail->GetText().ToString(), "", Value_Password->GetText().ToString()));
	}
	
	PlayFadeOutLogin();
	InitializeExit();
}