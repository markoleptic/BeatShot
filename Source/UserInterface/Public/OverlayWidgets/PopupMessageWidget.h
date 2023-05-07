// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PopupMessageWidget.generated.h"

class UButton;
class UTextBlock;
class UWidgetAnimation;

/** Provides a simple pop up dialog for other widgets */
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
	UButton* Button_1;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Button_2;
	void ChangeMessageText(const FText& MessageInput);

protected:
	FWidgetAnimationDynamicEvent FadeOutPopupMessageDelegate;
	UFUNCTION()
	void OnFadeOutPopupMessageFinish();
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_Button1;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_Button2;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_Title;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_Message;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutMessage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeInMessage;
};
