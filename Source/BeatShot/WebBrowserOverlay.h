// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WebBrowserWidget.h"
#include "Blueprint/UserWidget.h"
#include "DefaultGameInstance.h"
#include "Delegates/DelegateCombinations.h"

#include "WebBrowserOverlay.generated.h"

class ULoginWidget;
class UTextBlock;
class UOverlay;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnLoginStateChange, bool, bLoggedInHttp, bool, bLoggedInBrowser, bool, bIsPopup);

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
	/** Called from the parent widget of any instance of this widget */
	UFUNCTION(BlueprintCallable)
	void InitializeScoringOverlay();

	/** Called from the parent widget of any instance of this widget */
	UFUNCTION(BlueprintCallable)
	void InitializePostGameScoringOverlay(const FString& ResponseMessage, const int32 ResponseCode);

	/** Sends a login request to make sure we can save scores to database, which also saves the refresh token */
	UFUNCTION(BlueprintCallable)
	void LoginUserHttp(const FLoginPayload LoginPayload, const bool bIsPopup);

	/** Function called when there a response is returned for logging in user through Http */
	UFUNCTION(BlueprintCallable)
	void OnHttpLoginResponse(FString ResponseMsg, int32 ResponseCode);

	/** Called to change the overlay text in PostGameMenuWidget */
	UFUNCTION(BlueprintCallable)
	void SetOverlayText(const FString& Key);

	/** Fade Out the loading screen overlay and show the web browser */
	UFUNCTION(BlueprintCallable)
	void FadeOut();

	/** Fade in the OverlayText and fade out the loading icon */
	UFUNCTION(BlueprintCallable)
	void FadeInText();

	/** Whether or not the instance of this widget is a child of PostGameMenuWidget, vs being a child widget of MainMenuWidget */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Default", meta = (ExposeOnSpawn="true"))
	bool bIsPostGameScoringOverlay;

	/** Broadcast when a login state has changed, either in InitializeScoringOverlay or OnURLLoaded */
	UPROPERTY(BlueprintAssignable)
	FOnLoginStateChange OnLoginStateChange;

	/** Animation to fade out the overlay and show the web browser */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutOverlay;

	/** Animation to fade in the OverlayText and fade out the loading icon */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeInOverlayText;

	/** Animation to fade in the patch notes */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeInPatchNotes;

	/** WebBrowserWidget, only public so that parent widget can pass in ticks */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UWebBrowserWidget* BrowserWidget;

private:
	/** Calls HandleLoginUser function in LoginWidget */
	UFUNCTION()
	void LoginUserBrowser(const FLoginPayload LoginPayload);

	/** Handles the response from LoginWidget HandleUserLogin */
	UFUNCTION()
	void OnURLLoaded(const bool bLoadedSuccessfully);

	/** Handles the response from OnRefreshTokenResponse */
	UFUNCTION()
	void OnAccessTokenResponse(const bool Success);

	/** Handles the response from LoginWidget HandleUserLogin */
	UFUNCTION()
	void HttpDelay();

	/** Resets all instance variables at the end of a Login State Change */
	void Reset();

	/** The timer associated with looping OnURLLoaded over itself by binding to HttpDelay */
	UPROPERTY()
	FTimerHandle HttpTimerHandle;

	/** Delegate used to remove the overlay from WebBrowserOverlay after FadeOut */
	UPROPERTY()
	FWidgetAnimationDynamicEvent FadeOutDelegate;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UTextBlock* OverlayText;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UOverlay* BrowserOverlay;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	ULoginWidget* LoginWidget;

#pragma region InstanceVariables

	/** Whether or not the user was explicitly logging in with the login screen,
	 *  rather than just loading the user profile in BrowserWidget */
	bool bIsLoggingIn = false;

	/** Whether or not the http login request returned a status of 200 (OK) */
	bool bHttpResponseSuccess = false;

	/** Whether or not we've received an http response yet. Used to determine if
	 *  OnURLLoaded should continue execution or loop over itself until it
	 *  receives a response */
	bool bHttpResponse = false;

	/** Whether or not the URL load was successful in WebBrowser. Saved as variable in case
	 *  we need to wait for an Http Response */
	bool URLLoadSuccess = false;

	/** Whether or not LoginUserHttp was called through this widget's LoginWidget, or through the
	 *  MainMenu's Popup LoginWidget */
	bool bSignedInThroughPopup = false;

#pragma endregion
};
