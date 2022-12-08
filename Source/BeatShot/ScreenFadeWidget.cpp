// Fill out your copyright notice in the Description page of Project Settings.


#include "ScreenFadeWidget.h"

#include "DefaultPlayerController.h"
#include "Kismet/GameplayStatics.h"

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
