// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "OverlayWidgets/PopupMessageWidget.h"
#include "Components/TextBlock.h"
#include "WidgetComponents/BSButton.h"

void UPopupMessageWidget::NativeConstruct()
{
	Super::NativeConstruct();
	Button_1->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	Button_2->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
}

void UPopupMessageWidget::NativeDestruct()
{
	Button_1->OnBSButtonPressed.Clear();
	Button_2->OnBSButtonPressed.Clear();
	Super::NativeDestruct();
}

void UPopupMessageWidget::InitPopup(const FText& TitleInput, const FText& MessageInput, const FText& Button1TextInput, const FText& Button2TextInput) const
{
	TextBlock_Title->SetText(TitleInput);
	TextBlock_Message->SetText(MessageInput);
	Button_1->ChangeButtonText(Button1TextInput);
	if (!Button2TextInput.IsEmpty())
	{
		TextBlock_Button2->SetText(Button2TextInput);
		Button_2->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Button_2->SetVisibility(ESlateVisibility::Collapsed);
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
	TextBlock_Message->SetText(MessageInput);
}

void UPopupMessageWidget::OnButtonClicked_BSButton(const UBSButton* Button)
{
	if (Button == Button_1)
	{
		OnButton1Pressed.Broadcast();
		return;
	}
	if (Button == Button_2)
	{
		OnButton2Pressed.Broadcast();
	}
}

void UPopupMessageWidget::OnFadeOutPopupMessageFinish()
{
	RemoveFromParent();
}
