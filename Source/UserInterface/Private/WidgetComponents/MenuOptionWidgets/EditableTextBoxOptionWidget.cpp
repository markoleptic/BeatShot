// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/MenuOptionWidgets/EditableTextBoxOptionWidget.h"
#include "Components/EditableTextBox.h"
#include "Styles/MenuOptionStyle.h"

void UEditableTextBoxOptionWidget::SetStyling()
{
	Super::SetStyling();
	if (MenuOptionStyle)
	{
		EditableTextBox->WidgetStyle.SetFont(MenuOptionStyle->Font_EditableText);
	}
}
