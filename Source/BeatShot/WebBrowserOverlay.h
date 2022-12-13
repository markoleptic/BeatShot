// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WebBrowserWidget.h"
#include "Blueprint/UserWidget.h"
#include "WebBrowserOverlay.generated.h"

class UTextBlock;
class UOverlay;

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
	UFUNCTION(BlueprintCallable)
	void SetOverlayText(const FString& Key);
	
	UFUNCTION(BlueprintCallable)
	void FadeOut();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutOverlay;
	
private:
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UTextBlock* OverlayText;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UOverlay* BrowserOverlay;
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UWebBrowserWidget* BrowserWidget;
	
	UPROPERTY()
	FWidgetAnimationDynamicEvent FadeOutDelegate;
};
