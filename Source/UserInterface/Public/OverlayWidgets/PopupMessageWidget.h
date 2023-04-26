// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PopupMessageWidget.generated.h"

class UButton;
class UTextBlock;
class UWidgetAnimation;

UCLASS()
class USERINTERFACE_API UPopupMessageWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UFUNCTION(BlueprintCallable)
	void InitPopup(const FText& TitleInput, const FText& MessageInput, const FText& Button1TextInput, FText Button2TextInput = FText::GetEmpty()) const;
	UFUNCTION()
	void FadeIn();
	UFUNCTION()
	void FadeOut();
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Button1;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Button2;
	void ChangeMessageText(const FText& MessageInput);

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
