// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WebBrowserWidget.h"
#include "Blueprint/UserWidget.h"
#include "Delegates/DelegateCombinations.h"

#include "WebBrowserOverlay.generated.h"

class ULoginWidget;
class UTextBlock;
class UOverlay;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoginAttempted, bool, bSuccess, bool, bHasRegistered);

/**
 * 
 */
UCLASS()
class BEATSHOT_API UWebBrowserOverlay : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:

	/* Called from the parent widget of any instance of this widget */
	UFUNCTION(BlueprintCallable)
	void InitializeScoringOverlay();

	/* Called to change the overlay text in PostGameMenuWidget */
	UFUNCTION(BlueprintCallable)
	void SetOverlayText(const FString& Key) const;

	/* Called from MainMenu if the user uses the pop to login instead of the WebBrowserOverlay Login widget */
	UFUNCTION(BlueprintCallable)
	void LoginUserFromPopup(const FString Username, const FString Email, const FString Password);

	/* Fade Out the loading screen and show the web browser */
	UFUNCTION(BlueprintCallable)
	void FadeOut();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutOverlay;
	
	UPROPERTY(BlueprintAssignable);
	FOnLoginAttempted OnLoginAttempted;
	
private:

	/* Calls HandleLoginUser function in LoginWidget */
	UFUNCTION()
	void LoginUser(const FString Username, const FString Email, const FString Password);

	/* Handles the response from LoginWidget HandleUserLogin */
	UFUNCTION()
	void OnURLLoaded(const bool bLoadedSuccessfully);

	/** Handles the response from LoginWidget HandleUserLogin,
	 * but does not show a success message so the user does not receive
	 * two success messages if they were logging in from the Popup in MainMenu */
	UFUNCTION()
	void OnURLLoadedFromPopup(const bool bLoadedSuccessfully);
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UTextBlock* OverlayText;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UOverlay* BrowserOverlay;
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UWebBrowserWidget* BrowserWidget;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	ULoginWidget* LoginWidget;
	
	UPROPERTY()
	FWidgetAnimationDynamicEvent FadeOutDelegate;

	/** Whether or no the user was explicitly logging in with the login screen,
	 * rather than just loading the user profile in BrowserWidget */
	bool bIsLoggingIn = false;
};


