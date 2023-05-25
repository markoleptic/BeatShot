// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "OverlayWidgets/LoginWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/BackgroundBlur.h"
#include "WidgetComponents/BSButton.h"

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	Button_Login->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	Button_Register->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	Button_GotoLogin->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	Button_GotoRegister->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	Button_NoLogin->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	Button_NoRegister->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	Button_NoRegisterCancel->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	Button_NoRegisterConfirm->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	OkayButton->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	
	Value_Username->OnTextChanged.AddDynamic(this, &ULoginWidget::ClearErrorText);
	Value_Email->OnTextChanged.AddDynamic(this, &ULoginWidget::ClearErrorText);
	Value_Password->OnTextChanged.AddDynamic(this, &ULoginWidget::ClearErrorText);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ULoginWidget::LoginButtonClicked()
{
	if ((Value_Username->GetText().IsEmpty() && Value_Email->GetText().IsEmpty()) || Value_Password->GetText().IsEmpty())
	{
		Box_Error->SetVisibility(ESlateVisibility::Visible);
		SetErrorText("MissingInfoErrorText");
		return;
	}
	const FLoginPayload LoginPayload = FLoginPayload(Value_Username->GetText().ToString(), Value_Email->GetText().ToString(), Value_Password->GetText().ToString());
	OnLoginButtonClicked.Broadcast(LoginPayload, bIsPopup);
	PlayFadeOutLogin();
}

void ULoginWidget::OnLoginSuccess()
{
	PlayFadeInLoggedIn();
}

void ULoginWidget::ShowRegisterScreen()
{
	if (!bIsPopup)
	{
		BackgroundBlur->SetVisibility(ESlateVisibility::Collapsed);
		Button_NoRegister->SetVisibility(ESlateVisibility::Collapsed);
		Button_NoLogin->SetVisibility(ESlateVisibility::Collapsed);
	}
	SetVisibility(ESlateVisibility::Visible);
	PlayFadeInRegister();
}

void ULoginWidget::ShowLoginScreen(const FString& Key)
{
	if (LoadPlayerSettings().User.HasLoggedInHttp)
	{
		TextBlock_ContinueWithoutTitle->SetText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "Login_ContinueWithoutTitleTextLogin"));
		TextBlock_ContinueWithoutBody->SetText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "Login_ContinueWithoutBodyTextLogin"));
		Button_NoRegisterCancel->ChangeButtonText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "Login_ContinueWithoutCancelButtonTextLogin"));
	}
	if (!Key.IsEmpty())
	{
		Box_Error->SetVisibility(ESlateVisibility::Visible);
		SetErrorText(Key);
	}
	if (!bIsPopup)
	{
		BackgroundBlur->SetVisibility(ESlateVisibility::Collapsed);
		Button_NoLogin->SetVisibility(ESlateVisibility::Collapsed);
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
	if (IsAnimationPlaying(FadeOutLoggedIn))
	{
		FadeOutLoggedInDelegate.BindDynamic(this, &ULoginWidget::RemoveFromParent);
		BindToAnimationFinished(FadeOutLoggedIn, FadeOutLoggedInDelegate);
	}
	else if (IsAnimationPlaying(FadeOutContinueWithout))
	{
		FadeOutContinueWithoutDelegate.BindDynamic(this, &ULoginWidget::RemoveFromParent);
		BindToAnimationFinished(FadeOutContinueWithout, FadeOutContinueWithoutDelegate);
	}
	else
	{
		RemoveFromParent();
	}
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
		PlayFadeOutRegister();
		PlayFadeInLogin();
	}
	else if (Button == Button_GotoLogin)
	{
		PlayFadeOutRegister();
		PlayFadeInLogin();
	}
	else if (Button == Button_GotoRegister)
	{
		if (!bIsPopup)
		{
			LaunchRegisterURL();
			return;
		}
		PlayFadeOutLogin();
		PlayFadeInRegister();
	}
	else if (Button == Button_NoLogin)
	{
		PlayFadeOutLogin();
		PlayFadeInContinueWithout();
	}
	else if (Button == Button_NoRegister)
	{
		PlayFadeOutRegister();
		PlayFadeInContinueWithout();
	}
	else if (Button == Button_NoRegisterCancel)
	{
		PlayFadeOutContinueWithout();
		if (LoadPlayerSettings().User.HasLoggedInHttp)
		{
			PlayFadeInLogin();
		}
		else
		{
			PlayFadeInRegister();
		}
	}
	else if (Button == Button_NoRegisterConfirm)
	{
		PlayFadeOutContinueWithout();
		InitializeExit();
	}
	else if (Button == OkayButton)
	{
		PlayFadeOutLoggedIn();
		InitializeExit();
	}
}
