// Fill out your copyright notice in the Description page of Project Settings.


#include "OverlayWidgets/PopupMessageWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UPopupMessageWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPopupMessageWidget::NativeDestruct()
{
	Button1->OnClicked.Clear();
	Button2->OnClicked.Clear();
	Super::NativeDestruct();
}

void UPopupMessageWidget::InitPopup(const FText& TitleInput, const FText& MessageInput, const FText& Button1TextInput, FText Button2TextInput) const
{
	TitleText->SetText(TitleInput);
	MessageText->SetText(MessageInput);
	Button1Text->SetText(Button1TextInput);
	if (!Button2TextInput.IsEmpty())
	{
		Button2->SetVisibility(ESlateVisibility::Visible);
		Button2Text->SetText(Button2TextInput);
	}
	else
	{
		Button2->SetVisibility(ESlateVisibility::Collapsed);
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

void UPopupMessageWidget::OnFadeOutPopupMessageFinish()
{
	RemoveFromParent();
}