// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LoadingProcessInterface.h"
#include "Blueprint/UserWidget.h"
#include "LoadingScreenWidget.generated.h"

UCLASS(BlueprintType)
class USERINTERFACE_API ULoadingScreenWidget : public UUserWidget, public ILoadingProcessInterface
{
	GENERATED_BODY()

public:
	/** ~ILoadingProcessInterface begin */
	virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
	virtual void CreateLoadingScreenTask(FOnLoadingScreenShown& OnLoadingScreenShown, FOnReadyToHideLoadingScreen& OnReadyToHideLoadingScreen) override;
	/** ~ILoadingProcessInterface end */
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnFadeOutFinished();
	
	UFUNCTION()
	void FadeIn();
	
	UFUNCTION()
	void FadeOut();
	
	UPROPERTY(EditDefaultsOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutAnim;
	UPROPERTY(EditDefaultsOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeInAnim;
	
	const FString Reason = "LoadScreenAnimation";
	bool bShowLoadingScreen = true;
};
