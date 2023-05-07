// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LoginWidget.generated.h"

class UBackgroundBlur;
class UOverlay;
class UEditableTextBox;
class UTextBlock;
class UButton;
class UHorizontalBox;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoginButtonClicked, const FLoginPayload, LoginPayload, const bool, bIsPopup);

/** Widget used to prompt a user to login and handle login through HTTP requests */
UCLASS()
class USERINTERFACE_API ULoginWidget : public UUserWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:
	/** Since LoginWidget is not responsible for logging the user into the Beatshot website,
	 *  WebBrowserOverlay or its parent widget will call this function to show success message */
	UFUNCTION()
	void OnLoginSuccess();

	/** Shows the register screen */
	UFUNCTION()
	void ShowRegisterScreen();

	/** Shows the login screen */
	UFUNCTION()
	void ShowLoginScreen(const FString& Key);

	/** Sets the Text in the Box_Error given a key for the ST_Login string table */
	void SetErrorText(const FString& Key);

	/** Broadcasts the user's login info to WebBrowserOverlay in order to log them into the web browser */
	UPROPERTY(BlueprintAssignable)
	FOnLoginButtonClicked OnLoginButtonClicked;

	/** WebBrowserOverlay binds to this button's OnClick event */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UButton* OkayButton;

	/** Whether or not this widget is a popup (MainMenu direct child) or a WebBrowserOverlay child */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Default", meta = (ExposeOnSpawn="true"))
	bool bIsPopup;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UBackgroundBlur* BackgroundBlur;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* Box_Error;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UOverlay* Overlay_LoginRegisterSwitcher;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UOverlay* Overlay_Register;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UOverlay* Overlay_ContinueWithout;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UOverlay* Overlay_Login;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UOverlay* Overlay_LoggedIn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_Username;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_Email;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_Password;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_Error;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_NoLoginButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_ContinueWithoutTitle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_ContinueWithoutBody;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_ContinueWithoutCancelButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UButton* Button_Register;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UButton* Button_GotoLogin;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UButton* Button_NoRegister;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UButton* Button_Login;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UButton* Button_GotoRegister;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UButton* Button_NoLogin;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UButton* Button_NoRegisterConfirm;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UButton* Button_NoRegisterCancel;

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
	void InitializeExit();
	UFUNCTION()
	void LaunchRegisterURL() { UKismetSystemLibrary::LaunchURL("https://beatshot.gg/register"); }
	
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
};
