// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/BSComboBoxEntry_Tagged.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"


void UBSComboBoxEntry_Tagged::AddGameModeCategoryTagWidget(UUserWidget* InGameModeCategoryTagWidget)
{
	UHorizontalBoxSlot* BoxSlot = Box_TagWidgets->AddChildToHorizontalBox(InGameModeCategoryTagWidget);
	BoxSlot->SetPadding(Padding_TagWidget);
	BoxSlot->SetHorizontalAlignment(HorizontalAlignment_TagWidget);
	BoxSlot->SetVerticalAlignment(VerticalAlignment_TagWidget);
}
