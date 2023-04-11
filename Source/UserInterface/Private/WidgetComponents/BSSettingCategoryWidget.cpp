// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/BSSettingCategoryWidget.h"
#include "WidgetComponents/BSVerticalBox.h"

void UBSSettingCategoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	MainContainer->SetHorizontalBoxBorders();
	UpdateBrushColors();
}

void UBSSettingCategoryWidget::UpdateBrushColors() const
{
	MainContainer->UpdateBackgroundColors();
}
