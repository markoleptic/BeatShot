// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PopupMessageWidget.generated.h"

class UBSButton;
class UTextBlock;
class UWidgetAnimation;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPopUpButtonPressed);
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
	void InitPopup(const FText& TitleInput, const FText& MessageInput, const FText& Button1TextInput, const FText& Button2TextInput = FText::GetEmpty()) const;
	UFUNCTION()
	void FadeIn();
	UFUNCTION()
	void FadeOut();

	FOnPopUpButtonPressed OnButton1Pressed;
	FOnPopUpButtonPressed OnButton2Pressed;
	
	void ChangeMessageText(const FText& MessageInput);
	
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_1;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_2;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_Title;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_Message;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutMessage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeInMessage;

	FWidgetAnimationDynamicEvent FadeOutPopupMessageDelegate;

	UFUNCTION()
	void OnButtonClicked_BSButton(const UBSButton* Button);
	
	UFUNCTION()
	void OnFadeOutPopupMessageFinish();
};
