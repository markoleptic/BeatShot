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
class UBSButton;
class UHorizontalBox;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoginButtonClicked, const FLoginPayload, LoginPayload);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExitAnimationCompleted);

/** Widget used to prompt a user to login and handle login through HTTP requests */
UCLASS()
class USERINTERFACE_API ULoginWidget : public UUserWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:

	/** Shows the login screen */
	UFUNCTION()
	void ShowLoginScreen(const FString& Key);

	/** Sets the Text in the Box_Error given a key for the ST_Login string table */
	void SetErrorText(const FString& Key);

	/** Broadcasts the user's login info to WebBrowserOverlay in order to log them into the web browser */
	UPROPERTY(BlueprintAssignable)
	FOnLoginButtonClicked OnLoginButtonClicked;

	FOnExitAnimationCompleted OnExitAnimationCompletedDelegate;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UBackgroundBlur* BackgroundBlur;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* Box_Error;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UOverlay* Overlay_ContinueWithout;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UOverlay* Overlay_Login;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_UsernameEmail;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_Password;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_Error;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_ContinueWithoutTitle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_ContinueWithoutBody;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_Register;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_Login;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_NoLogin;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_NoLoginConfirm;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_NoLoginCancel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutLogin;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutContinueWithout;

	FWidgetAnimationDynamicEvent FadeOutContinueWithoutDelegate;
	
	UFUNCTION()
	void LoginButtonClicked();
	UFUNCTION()
	void ClearErrorText(const FText& Text);
	UFUNCTION()
	void InitializeExit();
	UFUNCTION()
	void OnExitAnimationCompleted();
	
	static void LaunchRegisterURL() { UKismetSystemLibrary::LaunchURL("https://beatshot.gg/register"); }

	UFUNCTION()
	void OnButtonClicked_BSButton(const UBSButton* Button);
	
	UFUNCTION()
	void PlayFadeInLogin() { PlayAnimationReverse(FadeOutLogin); }
	UFUNCTION()
	void PlayFadeOutLogin() { PlayAnimationForward(FadeOutLogin); }
	UFUNCTION()
	void PlayFadeInContinueWithout() { PlayAnimationReverse(FadeOutContinueWithout); }
	UFUNCTION()
	void PlayFadeOutContinueWithout() { PlayAnimationForward(FadeOutContinueWithout); }
};
