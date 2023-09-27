// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PopupMessageWidget.generated.h"

class UBSButton;
class UTextBlock;
class UWidgetAnimation;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPopUpButtonPressed);
DECLARE_MULTICAST_DELEGATE(FOnPopUpButtonPressed_NonDynamic);

/** Provides a simple pop up dialog for other widgets */
UCLASS()
class USERINTERFACE_API UPopupMessageWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	virtual void InitPopup(const FText& TitleInput, const FText& MessageInput, const FText& Button1TextInput, const FText& Button2TextInput = FText::GetEmpty()) const;
	UFUNCTION()
	virtual void FadeIn();
	UFUNCTION()
	virtual void FadeOut();

	void SetShouldRemoveFromParentOnFadeOut(const bool bShouldRemove) { bRemoveFromParentOnFadeOut = bShouldRemove; }

	/** Broadcast when Button1 is pressed */
	FOnPopUpButtonPressed OnButton1Pressed;
	/** Broadcast when Button2 is pressed */
	FOnPopUpButtonPressed OnButton2Pressed;
	/** Broadcast when Button1 is pressed */
	FOnPopUpButtonPressed_NonDynamic OnButton1Pressed_NonDynamic;
	/** Broadcast when Button2 is pressed */
	FOnPopUpButtonPressed_NonDynamic OnButton2Pressed_NonDynamic;
	
	virtual void ChangeMessageText(const FText& MessageInput);
	
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

	bool bRemoveFromParentOnFadeOut = true;
};
