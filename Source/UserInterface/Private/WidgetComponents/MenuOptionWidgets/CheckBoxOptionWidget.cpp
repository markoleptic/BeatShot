// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/MenuOptionWidgets/CheckBoxOptionWidget.h"
#include "Components/CheckBox.h"

void UCheckBoxOptionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CheckBox->OnCheckStateChanged.AddUniqueDynamic(this, &ThisClass::OnCheckStateChanged_CheckBox);
}

void UCheckBoxOptionWidget::OnCheckStateChanged_CheckBox(const bool bChecked)
{
}
