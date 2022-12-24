// Fill out your copyright notice in the Description page of Project Settings.

#include "VideoSettingButton.h"
#include "Components/Button.h"
#include <Blueprint/UserWidget.h>

void UVideoSettingButton::NativeConstruct()
{
	Super::NativeConstruct();
	Button->OnClicked.AddUniqueDynamic(this, &UVideoSettingButton::OnButtonClickedCallback);
}

void UVideoSettingButton::OnButtonClickedCallback()
{
	OnButtonClicked.Broadcast(this);
}

