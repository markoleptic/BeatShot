// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Utilities/Buttons/MenuButton.h"

void UMenuButton::SetDefaults(UWidget* WidgetToShow, UMenuButton* NextButton)
{
	AssociatedWidget = WidgetToShow;
	Next = NextButton;
	SetHasSetDefaults(true);
}
