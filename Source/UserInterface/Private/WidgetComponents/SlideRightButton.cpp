// Fill out your copyright notice in the Description page of Project Settings.



#include "WidgetComponents/SlideRightButton.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"

void USlideRightButton::NativeConstruct()
{
	Super::NativeConstruct();
	ButtonDisplayText->SetText(ButtonText);
	ButtonDisplayText->SetFont(DefaultFontInfo);
}

void USlideRightButton::SlideButton(const bool bIsActiveSelection)
{
	if (bIsActiveSelection)
	{
		SlideButtonRight();
	}
	else
	{
		/** If button is sliding left */
		if (IsAnimationPlaying(SlideButtonAnim) && !IsAnimationPlayingForward(SlideButtonAnim))
		{
			return;
		}
		if (HorizontalBox->RenderTransform.Translation.X > -40)
		{
			SlideButtonLeft();
		}
	}
}
