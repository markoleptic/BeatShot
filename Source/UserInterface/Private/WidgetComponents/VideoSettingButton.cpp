// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/VideoSettingButton.h"

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
