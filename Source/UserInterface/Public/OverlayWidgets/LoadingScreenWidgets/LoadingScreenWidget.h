// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LoadingProcessInterface.h"
#include "Blueprint/UserWidget.h"
#include "LoadingScreenWidget.generated.h"

class ULoadingProcessTask;

UCLASS(BlueprintType)
class USERINTERFACE_API ULoadingScreenWidget : public UUserWidget, public ILoadingProcessInterface
{
	GENERATED_BODY()

public:
	/** ~ILoadingProcessInterface begin */
	virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
	virtual void BindToLoadingScreenDelegates(FOnLoadingScreenVisibilityChangedDelegate& OnLoadingScreenVisibilityChanged, FOnReadyToHideLoadingScreenDelegate& OnReadyToHideLoadingScreen) override;
	/** ~ILoadingProcessInterface end */
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UFUNCTION()
	void FadeOut(float AnimPlaybackLength);
	
	UPROPERTY(EditDefaultsOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutAnim;
	UPROPERTY(EditDefaultsOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeInAnim;
	
	const FString Reason = "LoadScreenAnimation";
	bool bShowLoadingScreen = false;
};
