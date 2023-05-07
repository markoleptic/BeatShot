// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "OverlayWidgets/PopupMessageWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UPopupMessageWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPopupMessageWidget::NativeDestruct()
{
	Button_1->OnClicked.Clear();
	Button_2->OnClicked.Clear();
	Super::NativeDestruct();
}

void UPopupMessageWidget::InitPopup(const FText& TitleInput, const FText& MessageInput, const FText& Button1TextInput, FText Button2TextInput) const
{
	TextBlock_Title->SetText(TitleInput);
	TextBlock_Message->SetText(MessageInput);
	TextBlock_Button1->SetText(Button1TextInput);
	if (!Button2TextInput.IsEmpty())
	{
		Button_2->SetVisibility(ESlateVisibility::Visible);
		TextBlock_Button2->SetText(Button2TextInput);
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

void UPopupMessageWidget::OnFadeOutPopupMessageFinish()
{
	RemoveFromParent();
}
