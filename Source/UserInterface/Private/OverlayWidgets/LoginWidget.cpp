// Fill out your copyright notice in the Description page of Project Settings.


#include "OverlayWidgets/LoginWidget.h"
#include "SaveGamePlayerSettings.h"
#include "Components/HorizontalBox.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/BackgroundBlur.h"

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Login->OnClicked.AddDynamic(this, &ULoginWidget::LoginButtonClicked);

	Register->OnClicked.AddDynamic(this, &ULoginWidget::LaunchRegisterURL);
	Register->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeOutRegister);
	Register->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeInLogin);

	GotoLogin->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeOutRegister);
	GotoLogin->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeInLogin);

	GotoRegister->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeOutLogin);
	GotoRegister->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeInRegister);

	NoLogin->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeOutLogin);
	NoLogin->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeInContinueWithout);

	NoRegister->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeOutRegister);
	NoRegister->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeInContinueWithout);

	NoRegisterCancel->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeOutContinueWithout);
	NoRegisterCancel->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeInRegister);

	NoRegisterConfirm->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeOutContinueWithout);
	NoRegisterConfirm->OnClicked.AddDynamic(this, &ULoginWidget::InitializeExit);

	OkayButton->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeOutLoggedIn);
	OkayButton->OnClicked.AddDynamic(this, &ULoginWidget::InitializeExit);

	UsernameTextBox->OnTextChanged.AddDynamic(this, &ULoginWidget::ClearErrorText);
	EmailTextBox->OnTextChanged.AddDynamic(this, &ULoginWidget::ClearErrorText);
	PasswordTextBox->OnTextChanged.AddDynamic(this, &ULoginWidget::ClearErrorText);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ULoginWidget::LoginButtonClicked()
{
	if ((UsernameTextBox->GetText().IsEmpty() && EmailTextBox->GetText().IsEmpty()) || PasswordTextBox->GetText().
		IsEmpty())
	{
		ErrorBox->SetVisibility(ESlateVisibility::Visible);
		SetErrorText("MissingInfoErrorText");
		return;
	}
	const FLoginPayload LoginPayload = FLoginPayload(UsernameTextBox->GetText().ToString(),
	                                                 EmailTextBox->GetText().ToString(),
	                                                 PasswordTextBox->GetText().ToString());
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
		NoRegister->SetVisibility(ESlateVisibility::Collapsed);
		NoLogin->SetVisibility(ESlateVisibility::Collapsed);
	}
	SetVisibility(ESlateVisibility::Visible);
	PlayFadeInRegister();
}

void ULoginWidget::ShowLoginScreen(const FString& Key)
{
	if (LoadPlayerSettings().HasLoggedInHttp)
	{
		NoRegisterCancel->OnClicked.RemoveDynamic(this, &ULoginWidget::PlayFadeInRegister);
		NoRegisterCancel->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeInLogin);
		ContinueWithoutTitleText->SetText(
			FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "Login_ContinueWithoutTitleTextLogin"));
		ContinueWithoutBodyText->SetText(
			FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "Login_ContinueWithoutBodyTextLogin"));
		ContinueWithoutCancelButtonText->SetText(
			FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "Login_ContinueWithoutCancelButtonTextLogin"));
	}
	if (!Key.IsEmpty())
	{
		ErrorBox->SetVisibility(ESlateVisibility::Visible);
		SetErrorText(Key);
	}
	if (!bIsPopup)
	{
		BackgroundBlur->SetVisibility(ESlateVisibility::Collapsed);
		NoLogin->SetVisibility(ESlateVisibility::Collapsed);
		GotoRegister->OnClicked.RemoveAll(this);
		GotoRegister->OnClicked.AddDynamic(this, &ULoginWidget::LaunchRegisterURL);
	}
	SetVisibility(ESlateVisibility::Visible);
	PlayFadeInLogin();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ULoginWidget::ClearErrorText(const FText& Text)
{
	ErrorBox->SetVisibility(ESlateVisibility::Collapsed);
	ErrorText->SetText(FText());
}

void ULoginWidget::SetErrorText(const FString& Key)
{
	ErrorText->SetText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", Key));
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
