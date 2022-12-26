// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PopupMessageWidget.generated.h"

class UButton;
class UTextBlock;
class UWidgetAnimation;

/**
 * 
 */
UCLASS()
class BEATSHOT_API UPopupMessageWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	
	UFUNCTION(BlueprintCallable)
	void InitPopup(FString TitleInput, FString MessageInput, FString Button1TextInput, FString Button2TextInput = "") const;
	UFUNCTION()
	void FadeIn();
	UFUNCTION()
	void FadeOut();
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Button1;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Button2;

protected:
	
	FWidgetAnimationDynamicEvent FadeOutPopupMessageDelegate;
	UFUNCTION()
	void OnFadeOutPopupMessageFinish();
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* Button1Text;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* Button2Text;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TitleText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* MessageText;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutMessage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeInMessage;
};


