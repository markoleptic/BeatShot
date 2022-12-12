// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadingScreenWidget.generated.h"

/**
 * 
 */
UCLASS()
class BEATSHOT_API ULoadingScreenWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:

	UPROPERTY(BlueprintReadWrite)
	float Time;
	UFUNCTION(BlueprintCallable)
	void FadeOut();
	UFUNCTION(BlueprintCallable)
	void FadeIn();
	UPROPERTY(EditDefaultsOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutLoadingScreen;
	UPROPERTY(EditDefaultsOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeInLoadingScreen;
	UPROPERTY()
	FWidgetAnimationDynamicEvent FadeOutLoadingScreenDelegate;
	UPROPERTY()
	FWidgetAnimationDynamicEvent FadeInLoadingScreenDelegate;
	UFUNCTION()
	void BindToFadeOutFinish(UObject* Object, FName FunctionName);
	
};
