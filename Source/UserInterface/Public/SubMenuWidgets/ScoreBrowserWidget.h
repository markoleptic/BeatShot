// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HttpRequestInterface.h"
#include "Blueprint/UserWidget.h"
#include "SaveLoadInterface.h"
#include "ScoreBrowserWidget.generated.h"

class UImage;
class UWebBrowserWidget;
class ULoginWidget;
class UTextBlock;
class UOverlay;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoginStateChange, const ELoginState&, LoginState, bool, bIsPopup);

/** Container that wraps around WebBrowserWidget providing additional functionality */
UCLASS()
class USERINTERFACE_API UScoreBrowserWidget : public UUserWidget, public ISaveLoadInterface, public IHttpRequestInterface
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	/** Called from the parent widget of any instance of this widget */
	UFUNCTION()
	void InitializeScoringOverlay();

	/** Called from the parent widget of any instance of this widget */
	UFUNCTION()
	void InitializePostGameScoringOverlay(const ELoginState& LoginState);

	/** Sends a login request to make sure we can save scores to database, which also saves the refresh token */
	UFUNCTION()
	void LoginUserHttp(const FLoginPayload LoginPayload, const bool bIsPopup);

	/** Function called when there a response is returned for logging in user through Http */
	UFUNCTION()
	void OnHttpLoginResponse(const FPlayerSettings& PlayerSettings, FString ResponseMsg, int32 ResponseCode);

	/** Called to change the overlay text in PostGameMenuWidget */
	UFUNCTION()
	void SetOverlayText(const FString& Key);

	/** Fade Out the loading screen overlay and show the web browser */
	UFUNCTION()
	void FadeOutLoadingOverlay();

	/** Fade in the OverlayText and fade out the loading icon */
	UFUNCTION()
	void FadeInText();

	/** Broadcast when a login state has changed, either in InitializeScoringOverlay or OnURLLoaded */
	FOnLoginStateChange OnLoginStateChange;

	/** WebBrowserWidget, only public so that parent widget can pass in ticks */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UWebBrowserWidget* BrowserWidget;

	/** Whether or not the instance of this widget is a child of PostGameMenuWidget, vs being a child widget of MainMenuWidget */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Default", meta = (ExposeOnSpawn="true"))
	bool bIsPostGameScoringOverlay;

protected:
	/** Animation to fade out the overlay and show the web browser */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutOverlay;

	/** Animation to fade in the OverlayText and fade out the loading icon */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeInOverlayText;

	/** Animation to fade in the patch notes */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeInPatchNotes;

	/** Loading Icon Dynamic Material */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UImage* LoadingIcon;

	/** Loading Icon Dynamic Material */
	UPROPERTY(EditDefaultsOnly, Category = "Materials")
	UMaterialInstanceDynamic* MID_LoadingIcon;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UTextBlock* OverlayText;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UOverlay* BrowserOverlay;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	ULoginWidget* LoginWidget;

private:
	/** Calls HandleLoginUser function in LoginWidget */
	UFUNCTION()
	void LoginUserBrowser(const FLoginPayload LoginPayload);

	/** Handles the response from LoginWidget HandleUserLogin */
	UFUNCTION()
	void OnURLLoaded(const bool bLoadedSuccessfully);

	/** Handles the response from OnRefreshTokenResponse */
	UFUNCTION()
	void OnAccessTokenResponse(const FString& AccessToken);

	/** Delegate used to remove the overlay from WebBrowserOverlay after FadeOut */
	UPROPERTY()
	FWidgetAnimationDynamicEvent FadeOutDelegate;

	FLoginPayload LoginInfo;

	/** Delegate passed to OnLoginResponse(), which calls OnHttpLoginResponse */
	FOnLoginResponse OnLoginResponse;

	/** Delegate passed to RequestAccessToken(), which calls OnAccessTokenResponse */
	FOnAccessTokenResponse OnAccessTokenResponseDelegate;

	/** Total DeltaTime pass to Loading Icon Dynamic Material */
	float TotalDeltaTime = 0;

	/** Whether or not LoginUserHttp was called through this widget's LoginWidget, or through the
	 *  MainMenu's Popup LoginWidget */
	bool bSignedInThroughPopup = false;
};
