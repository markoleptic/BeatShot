// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/MenuButton.h"

void UMenuButton::SetDefaults(UVerticalBox* BoxToShow, UMenuButton* NextButton)
{
	Box = BoxToShow;
	Next = NextButton;
	SetHasSetDefaults(true);
}
