// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/BSSettingCategoryWidget.h"
#include "WidgetComponents/BSVerticalBox.h"
#include "WidgetComponents/TooltipWidget.h"

void UBSSettingCategoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetTooltipWidget(ConstructTooltipWidget());
	InitSettingCategoryWidget();
}

void UBSSettingCategoryWidget::InitSettingCategoryWidget()
{
	UpdateBrushColors();
}

UTooltipWidget* UBSSettingCategoryWidget::ConstructTooltipWidget()
{
	return CreateWidget<UTooltipWidget>(this, TooltipWidgetClass);
}

void UBSSettingCategoryWidget::AddWidgetBoxPair(UWidget* InWidget, UBSVerticalBox* InBox)
{
	MainContainer->AddWidgetBoxPair(InWidget, InBox);
}

void UBSSettingCategoryWidget::UpdateBrushColors() const
{
	MainContainer->UpdateBrushColors();
}
