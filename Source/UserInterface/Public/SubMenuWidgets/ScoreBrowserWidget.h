// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HttpRequestInterface.h"
#include "Blueprint/UserWidget.h"
#include "SaveLoadInterface.h"
#include "ScoreBrowserWidget.generated.h"

class UBSButton;
class UImage;
class UWebBrowserWidget;
class ULoginWidget;
class UTextBlock;
class UOverlay;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnURLChangedResult, const bool, bSuccess);

/** Container that wraps around WebBrowserWidget providing additional functionality */
UCLASS()
class USERINTERFACE_API UScoreBrowserWidget : public UUserWidget, public ISaveLoadInterface, public IHttpRequestInterface
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

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

	/** WebBrowserWidget, only public so that parent widget can load urls easily */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UWebBrowserWidget* BrowserWidget;

public:
	/** Broadcast when a login state has changed, either in InitScoreBrowser or OnURLLoaded */
	FOnURLChangedResult OnURLChangedResult;

	/** Initializes a score browser based on the type */
	void InitScoreBrowser(const EScoreBrowserType InScoreBrowserType, const EPostScoresResponse& Response = EPostScoresResponse::NoAccount);
	
	/** Logs in to BeatShot website using the LoginPayload and UserID */
	void LoginUserBrowser(const FLoginPayload LoginPayload, const FString UserID);

	/** Logs in to BeatShot website using a SteamAuthTicket */
	void LoginUserBrowser(const FString SteamAuthTicket);

	/** Fade Out the loading screen overlay and show the web browser */
	UFUNCTION()
	void FadeOutLoadingOverlay();

private:
	/** Handles the response from LoginWidget LoginUserToBeatShotWebsite */
	UFUNCTION()
	void OnURLLoaded(const bool bLoadedSuccessfully);

	/** Changes the overlay text and plays FadeInText animation */
	UFUNCTION()
	void SetOverlayTextAndFadeIn(const FString& Key);
	
	/** Delegate used to remove the overlay from WebBrowserOverlay after FadeOut */
	UPROPERTY()
	FWidgetAnimationDynamicEvent FadeOutDelegate;

	/** Total DeltaTime pass to Loading Icon Dynamic Material */
	float TotalDeltaTime = 0;

	/** The type of Score Browser */
	EScoreBrowserType ScoreBrowserType;
};
