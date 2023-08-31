// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "OverlayWidgets/PopupMessageWidget.h"
#include "Components/TextBlock.h"
#include "WidgetComponents/Buttons/BSButton.h"

void UPopupMessageWidget::NativeConstruct()
{
	Super::NativeConstruct();
	Button_1->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	Button_2->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
}

void UPopupMessageWidget::InitPopup(const FText& TitleInput, const FText& MessageInput, const FText& Button1TextInput, const FText& Button2TextInput) const
{
	TextBlock_Title->SetText(TitleInput);
	if (MessageInput.IsEmpty())
	{
		TextBlock_Message->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		TextBlock_Message->SetText(MessageInput);
		TextBlock_Message->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	Button_1->SetButtonText(Button1TextInput);
	
	if (Button2TextInput.IsEmpty())
	{
		Button_2->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		Button_2->SetButtonText(Button2TextInput);
		Button_2->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	
}

void UPopupMessageWidget::FadeIn()
{
	PlayAnimationForward(FadeInMessage);
}

void UPopupMessageWidget::FadeOut()
{
	FadeOutPopupMessageDelegate.BindDynamic(this, &UPopupMessageWidget::OnFadeOutPopupMessageFinish);
	BindToAnimationFinished(FadeOutMessage, FadeOutPopupMessageDelegate);
	PlayAnimationForward(FadeOutMessage);
}

void UPopupMessageWidget::ChangeMessageText(const FText& MessageInput)
{
	if (MessageInput.IsEmpty())
	{
		TextBlock_Message->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		TextBlock_Message->SetText(MessageInput);
		TextBlock_Message->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UPopupMessageWidget::OnButtonClicked_BSButton(const UBSButton* Button)
{
	if (Button == Button_1)
	{
		if (OnButton1Pressed.IsBound())
		{
			OnButton1Pressed.Broadcast();
		}
		if (OnButton1Pressed_NonDynamic.IsBound())
		{
			OnButton1Pressed_NonDynamic.Broadcast();
		}
		return;
	}
	if (Button == Button_2)
	{
		if (OnButton2Pressed.IsBound())
		{
			OnButton2Pressed.Broadcast();
		}
		if (OnButton2Pressed_NonDynamic.IsBound())
		{
			OnButton2Pressed_NonDynamic.Broadcast();
		}
	}
}

void UPopupMessageWidget::OnFadeOutPopupMessageFinish()
{
	if (bRemoveFromParentOnFadeOut)
	{
		RemoveFromParent();
	}
}
