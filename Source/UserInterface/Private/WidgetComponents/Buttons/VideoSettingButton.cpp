// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/Buttons/VideoSettingButton.h"

void UVideoSettingButton::SetDefaults(const EVideoSettingType InVideoSettingType, const uint8 VideoSettingQuality, UBSButton* NextButton)
{
	SettingType = InVideoSettingType;
	Quality = VideoSettingQuality;
	Next = NextButton;
	SetHasSetDefaults(true);
}