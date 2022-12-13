// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginWidget.h"

#include "DefaultGameInstance.h"
#include "Components/HorizontalBox.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Overlay.h"
#include "Kismet/GameplayStatics.h"

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
	PlayFadeOutLogin();
	PlayFadeInLoggedIn();
}

void ULoginWidget::ShowRegisterScreen()
{
	SetVisibility(ESlateVisibility::Visible);
	PlayFadeInRegister();
}

void ULoginWidget::ShowLoginScreen(const bool bIsPopupScreen)
{
	if (Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->LoadPlayerSettings().HasLoggedInHttp)
	{
		DefaultSignInText->SetVisibility(ESlateVisibility::Collapsed);
		HasSignedInBeforeText->SetVisibility(ESlateVisibility::Visible);
		ContinueWithoutLoginText->SetText(FText::FromString("Continue Without Signing In"));
		ContinueWithoutTitleText->SetText(FText::FromString("Are you sure you want to continue without signing in?"));
		ContinueWithoutBodyText->SetText(FText::FromString("You won't be able to see your played game data, but it will still be saved locally when you decide to sign in later."));
		ContinueWithoutCancelText->SetText(FText::FromString("Go Back and Sign In"));
		NoRegisterCancel->OnClicked.RemoveDynamic(this, &ULoginWidget::PlayFadeInRegister);
		NoRegisterCancel->OnClicked.AddDynamic(this, &ULoginWidget::PlayFadeInLogin);
	}
	else
	{
		DefaultSignInText->SetVisibility(ESlateVisibility::Visible);
		HasSignedInBeforeText->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (!bIsPopupScreen)
	{
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
