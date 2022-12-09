// Fill out your copyright notice in the Description page of Project Settings.


#include "PopupMessageWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetTextLibrary.h"

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

void UPopupMessageWidget::InitPopup(const FString TitleInput, const FString MessageInput, const FString Button1TextInput,
                                    const FString Button2TextInput) const
{
	TitleText->SetText(UKismetTextLibrary::Conv_StringToText(TitleInput));
	MessageText->SetText(UKismetTextLibrary::Conv_StringToText(MessageInput));
	Button1Text->SetText(UKismetTextLibrary::Conv_StringToText(Button1TextInput));
	if (!Button2TextInput.IsEmpty())
	{
		Button2->SetVisibility(ESlateVisibility::Visible);
		Button2Text->SetText(UKismetTextLibrary::Conv_StringToText(Button2TextInput));
	}
	else
	{
		Button2->SetVisibility(ESlateVisibility::Collapsed);
	}
}
