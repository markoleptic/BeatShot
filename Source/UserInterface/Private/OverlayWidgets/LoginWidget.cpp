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
	
	Button_Login->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	Button_NoLogin->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	Button_Register->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	Button_NoLoginCancel->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	Button_NoLoginConfirm->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	
	Value_UsernameEmail->OnTextChanged.AddDynamic(this, &ULoginWidget::ClearErrorText);
	Value_Password->OnTextChanged.AddDynamic(this, &ULoginWidget::ClearErrorText);
}

void ULoginWidget::ShowLoginScreen(const FString& Key)
{
	if (LoadPlayerSettings().User.bHasLoggedInBefore)
	{
		TextBlock_ContinueWithoutTitle->SetText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "Login_ContinueWithoutTitleTextLogin"));
		TextBlock_ContinueWithoutBody->SetText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "Login_ContinueWithoutBodyTextLogin"));
		Button_NoLoginCancel->ChangeButtonText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "Login_ContinueWithoutCancelButtonTextLogin"));
	}
	if (!Key.IsEmpty())
	{
		Box_Error->SetVisibility(ESlateVisibility::Visible);
		SetErrorText(Key);
	}
	SetVisibility(ESlateVisibility::Visible);
	PlayFadeInLogin();
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
		FadeOutContinueWithoutDelegate.BindDynamic(this, &ULoginWidget::OnExitAnimationCompleted);
		BindToAnimationFinished(FadeOutContinueWithout, FadeOutContinueWithoutDelegate);
	}
	else
	{
		OnExitAnimationCompleted();
	}
}

void ULoginWidget::OnExitAnimationCompleted()
{
	OnExitAnimationCompletedDelegate.Broadcast();
	UnbindFromAnimationFinished(FadeOutContinueWithout, FadeOutContinueWithoutDelegate);
	SetVisibility(ESlateVisibility::Collapsed);
}

void ULoginWidget::OnButtonClicked_BSButton(const UBSButton* Button)
{
	if (Button == Button_Login)
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
	else if (Button == Button_NoLoginCancel)
	{
		PlayFadeOutContinueWithout();
		PlayFadeInLogin();
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
}