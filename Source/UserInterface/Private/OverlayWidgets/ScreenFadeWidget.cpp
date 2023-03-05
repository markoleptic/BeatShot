// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "OverlayWidgets/ScreenFadeWidget.h"

void UScreenFadeWidget::NativeConstruct()
{
	Super::NativeConstruct();
	FadeToBlackFinishDelegate.BindDynamic(this, &UScreenFadeWidget::FadeToBlackAnimFinished);
	BindToAnimationFinished(FadeInBlackScreen, FadeToBlackFinishDelegate);
	FadeFromBlackFinishDelegate.BindDynamic(this, &UScreenFadeWidget::FadeFromBlackAnimFinished);
	BindToAnimationFinished(FadeOutBlackScreen, FadeFromBlackFinishDelegate);
}

void UScreenFadeWidget::NativeDestruct()
{
	Super::NativeDestruct();
	FadeToBlackFinishDelegate.Unbind();
	FadeFromBlackFinishDelegate.Unbind();
}

void UScreenFadeWidget::FadeFromBlackAnimFinished()
{
	OnFadeFromBlackFinish.Broadcast();
}

void UScreenFadeWidget::FadeToBlackAnimFinished()
{
	OnFadeToBlackFinish.Broadcast();
}

void UScreenFadeWidget::FadeFromBlack()
{
	PlayAnimationForward(FadeOutBlackScreen);
}

void UScreenFadeWidget::FadeToBlack()
{
	PlayAnimationForward(FadeInBlackScreen);
}
