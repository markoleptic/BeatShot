// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginWidget.h"

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();
	EndDelegate.BindDynamic(this, &ULoginWidget::FadeOutAnimFinished);
	BindToAnimationFinished(FadeOutAll, EndDelegate);
}

void ULoginWidget::FadeOutAnimFinished()
{
	ExitLogin.Broadcast();
}