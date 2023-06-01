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
	Containers.AddUnique(MainContainer);
	UpdateBrushColors();
}

UTooltipWidget* UBSSettingCategoryWidget::ConstructTooltipWidget()
{
	return CreateWidget<UTooltipWidget>(this, TooltipWidgetClass);
}

void UBSSettingCategoryWidget::AddAdditionalContainers(const TArray<TObjectPtr<UBSVerticalBox>>& InContainers)
{
	for (const TObjectPtr<UBSVerticalBox>& Container : InContainers)
	{
		Containers.AddUnique(Container);
	}
}

void UBSSettingCategoryWidget::AddWidgetBoxPair(UWidget* InWidget, UBSVerticalBox* InBox) const
{
	MainContainer->AddWidgetBoxPair(InWidget, InBox);
}

void UBSSettingCategoryWidget::UpdateBrushColors() const
{
	for (const TObjectPtr<UBSVerticalBox>& Container : Containers)
	{
		if (Container)
		{
			Container->UpdateBrushColors();
		}
	}
}
