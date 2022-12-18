// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DefaultGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Delegates/DelegateCombinations.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LoginWidget.generated.h"

class UBackgroundBlur;
class UOverlay;
class UEditableTextBox;
class UTextBlock;
class UButton;
class UHorizontalBox;
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoginButtonClicked, const FLoginPayload, LoginPayload, const bool, bIsPopup);

UCLASS()
class BEATSHOT_API ULoginWidget : public UUserWidget
{
	GENERATED_BODY()
	virtual void NativeConstruct() override;
	
public:
	
	/** Since LoginWidget is not responsible for logging the user into the Beatshot website,
	 * WebBrowserOverlay or its parent widget will call this function to show success message */
	UFUNCTION(BlueprintCallable)
	void OnLoginSuccess();

	/** Shows the register screen */
	UFUNCTION(BlueprintCallable)
	void ShowRegisterScreen();

	/** Shows the login screen */
	UFUNCTION(BlueprintCallable)
	void ShowLoginScreen();

	/** Broadcasts the user's login info to WebBrowserOverlay in order to log them into the web browser.
	 * The parameters are stateless, the only one that would be saved is the username */
	UPROPERTY(BlueprintAssignable)
	FOnLoginButtonClicked OnLoginButtonClicked;

	/** WebBrowserOverlay binds to this button's OnClick event */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UButton* OkayButton;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Default", meta = (ExposeOnSpawn="true"))
	bool bIsPopup;
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UBackgroundBlur* BackgroundBlur;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* LoginErrorBox;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* ErrorBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UOverlay* LoginRegisterSwitcher;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UOverlay* RegisterOverlay;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UOverlay* ContinueWithoutOverlay;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UOverlay* LoginOverlay;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UOverlay* LoggedInOverlay;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* UsernameTextBox;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* EmailTextBox;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* PasswordTextBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ErrorText;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* NoLoginButtonText;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ContinueWithoutTitleText;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ContinueWithoutBodyText;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ContinueWithoutCancelButtonText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UButton* Register;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UButton* GotoLogin;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UButton* NoRegister;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UButton* Login;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UButton* GotoRegister;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UButton* NoLogin;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UButton* NoRegisterConfirm;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UButton* NoRegisterCancel;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutLogin;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutRegister;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutContinueWithout;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutLoggedIn;
	
	FWidgetAnimationDynamicEvent FadeOutContinueWithoutDelegate;
	FWidgetAnimationDynamicEvent FadeOutLoggedInDelegate;

	UFUNCTION()
	void LoginButtonClicked();
	UFUNCTION()
	void ClearErrorText(const FText& Text);
	UFUNCTION()
	void PlayFadeInLogin() { PlayAnimationReverse(FadeOutLogin); }
	UFUNCTION()
	void PlayFadeOutLogin() { PlayAnimationForward(FadeOutLogin); }
	UFUNCTION()
	void PlayFadeInRegister() { PlayAnimationReverse(FadeOutRegister); }
	UFUNCTION()
	void PlayFadeOutRegister() { PlayAnimationForward(FadeOutRegister); }
	UFUNCTION()
	void PlayFadeInContinueWithout() { PlayAnimationReverse(FadeOutContinueWithout); }
	UFUNCTION()
	void PlayFadeOutContinueWithout() { PlayAnimationForward(FadeOutContinueWithout); }
	UFUNCTION()
	void PlayFadeInLoggedIn() { PlayAnimationReverse(FadeOutLoggedIn); }
	UFUNCTION()
	void PlayFadeOutLoggedIn() { PlayAnimationForward(FadeOutLoggedIn); }
	UFUNCTION()
	// ReSharper disable once CppMemberFunctionMayBeStatic
	void LaunchRegisterURL() { UKismetSystemLibrary::LaunchURL("https://beatshot.gg/register"); }
	UFUNCTION()
	void InitializeExit();
	
	const FString MissingInfoError = "Please enter your username/email and password to login.";
};


