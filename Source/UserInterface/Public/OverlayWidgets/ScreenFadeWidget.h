// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "ScreenFadeWidget.generated.h"

/**
 * 
 */
DECLARE_MULTICAST_DELEGATE(FOnFadeFromBlackFinish);

DECLARE_MULTICAST_DELEGATE(FOnFadeToBlackFinish);

UCLASS()
class USERINTERFACE_API UScreenFadeWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:

	FOnFadeFromBlackFinish OnFadeFromBlackFinish;
	FOnFadeToBlackFinish OnFadeToBlackFinish;
	
	UFUNCTION(BlueprintCallable)
	void FadeFromBlack();
	UFUNCTION(BlueprintCallable)
	void FadeToBlack();

private:
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UImage* BlackScreen;
	UPROPERTY(EditDefaultsOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutBlackScreen;
	UPROPERTY(EditDefaultsOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeInBlackScreen;
	FWidgetAnimationDynamicEvent FadeFromBlackFinishDelegate;
	FWidgetAnimationDynamicEvent FadeToBlackFinishDelegate;
	UFUNCTION()
	void FadeFromBlackAnimFinished();
	UFUNCTION()
	void FadeToBlackAnimFinished();
};
