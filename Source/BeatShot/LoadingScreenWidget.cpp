// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingScreenWidget.h"

void ULoadingScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BindToAnimationFinished(FadeOutLoadingScreen, FadeOutLoadingScreenDelegate);
	BindToAnimationFinished(FadeInLoadingScreen, FadeInLoadingScreenDelegate);
}

void ULoadingScreenWidget::FadeOut()
{
	PlayAnimationForward(FadeOutLoadingScreen);
}

void ULoadingScreenWidget::FadeIn()
{
	PlayAnimationForward(FadeInLoadingScreen);
}

void ULoadingScreenWidget::BindToFadeOutFinish(UObject* Object, FName FunctionName)
{
	FadeOutLoadingScreenDelegate.BindUFunction(Object, FunctionName);
}
