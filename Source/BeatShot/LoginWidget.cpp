// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginWidget.h"

#include "DefaultGameInstance.h"
#include "Components/HorizontalBox.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Overlay.h"
#include "Components/BackgroundBlur.h"
#include "Kismet/GameplayStatics.h"

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Login->OnClicked.AddDynamic(this, &ULoginWidget::LoginButtonClicked);
	Login->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeOutLogin);
	
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
	if ((UsernameTextBox->GetText().IsEmpty() && EmailTextBox->GetText().IsEmpty()) || PasswordTextBox->GetText().IsEmpty())
	{
		ErrorBox->SetVisibility(ESlateVisibility::Visible);
		ErrorText->SetText(FText::FromString(MissingInfoError));
		return;
	}
	OnLoginButtonClicked.Broadcast(UsernameTextBox->GetText().ToString(), EmailTextBox->GetText().ToString(), PasswordTextBox->GetText().ToString()); 
}

void ULoginWidget::OnLoginSuccess()
{
	PlayFadeInLoggedIn();
}

void ULoginWidget::ShowRegisterScreen(const bool bIsPopupScreen)
{
	if (!bIsPopupScreen)
	{
		BackgroundBlur->SetVisibility(ESlateVisibility::Collapsed);
		NoRegister->SetVisibility(ESlateVisibility::Collapsed);
		NoLogin->SetVisibility(ESlateVisibility::Collapsed);
	}
	SetVisibility(ESlateVisibility::Visible);
	PlayFadeInRegister();
}

void ULoginWidget::ShowLoginScreen(const bool bIsPopupScreen)
{
	if (Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->LoadPlayerSettings().HasLoggedInHttp)
	{
		LoginErrorBox->SetVisibility(ESlateVisibility::Visible);
		NoRegisterCancel->OnClicked.RemoveDynamic(this, &ULoginWidget::PlayFadeInRegister);
		NoRegisterCancel->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeInLogin);
		ContinueWithoutTitleText->SetText(FText::FromStringTable("/Game/UI/StringTables/ST_Login.ST_Login", "ContinueWithoutTitleTextLogin"));
		ContinueWithoutBodyText->SetText(FText::FromStringTable("/Game/UI/StringTables/ST_Login.ST_Login", "ContinueWithoutBodyTextLogin"));
		ContinueWithoutCancelButtonText->SetText(FText::FromStringTable("/Game/UI/StringTables/ST_Login.ST_Login", "ContinueWithoutCancelButtonTextLogin"));
	}
	else
	{
		LoginErrorBox->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (!bIsPopupScreen)
	{
		BackgroundBlur->SetVisibility(ESlateVisibility::Collapsed);
		NoLogin->SetVisibility(ESlateVisibility::Collapsed);
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
