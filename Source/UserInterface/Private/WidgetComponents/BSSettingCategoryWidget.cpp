// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/BSSettingCategoryWidget.h"
#include "WidgetComponents/BSVerticalBox.h"

void UBSSettingCategoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	InitSettingCategoryWidget();
}

void UBSSettingCategoryWidget::InitSettingCategoryWidget()
{
	UpdateBrushColors();
}

void UBSSettingCategoryWidget::AddWidgetBoxPair(UWidget* InWidget, UBSVerticalBox* InBox)
{
	MainContainer->AddWidgetBoxPair(InWidget, InBox);
}

void UBSSettingCategoryWidget::UpdateBrushColors() const
{
	MainContainer->UpdateBrushColors();
}
