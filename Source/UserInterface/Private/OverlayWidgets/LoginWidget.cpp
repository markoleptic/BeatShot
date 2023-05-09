// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "OverlayWidgets/LoginWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/BackgroundBlur.h"

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Button_Login->OnClicked.AddDynamic(this, &ULoginWidget::LoginButtonClicked);

	Button_Register->OnClicked.AddDynamic(this, &ULoginWidget::LaunchRegisterURL);
	Button_Register->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeOutRegister);
	Button_Register->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeInLogin);

	Button_GotoLogin->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeOutRegister);
	Button_GotoLogin->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeInLogin);

	Button_GotoRegister->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeOutLogin);
	Button_GotoRegister->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeInRegister);

	Button_NoLogin->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeOutLogin);
	Button_NoLogin->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeInContinueWithout);

	Button_NoRegister->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeOutRegister);
	Button_NoRegister->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeInContinueWithout);

	Button_NoRegisterCancel->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeOutContinueWithout);
	Button_NoRegisterCancel->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeInRegister);

	Button_NoRegisterConfirm->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeOutContinueWithout);
	Button_NoRegisterConfirm->OnClicked.AddDynamic(this, &ULoginWidget::InitializeExit);

	OkayButton->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeOutLoggedIn);
	OkayButton->OnClicked.AddDynamic(this, &ULoginWidget::InitializeExit);

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
		Button_NoRegisterCancel->OnClicked.RemoveDynamic(this, &ULoginWidget::PlayFadeInRegister);
		Button_NoRegisterCancel->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeInLogin);
		TextBlock_ContinueWithoutTitle->SetText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "Login_ContinueWithoutTitleTextLogin"));
		TextBlock_ContinueWithoutBody->SetText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "Login_ContinueWithoutBodyTextLogin"));
		TextBlock_ContinueWithoutCancelButton->SetText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "Login_ContinueWithoutCancelButtonTextLogin"));
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
		Button_GotoRegister->OnClicked.RemoveAll(this);
		Button_GotoRegister->OnClicked.AddDynamic(this, &ULoginWidget::LaunchRegisterURL);
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
