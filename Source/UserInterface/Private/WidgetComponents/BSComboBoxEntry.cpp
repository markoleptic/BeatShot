// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/BSComboBoxEntry.h"

#include "Components/TextBlock.h"
#include "WidgetComponents/TooltipImage.h"

void UBSComboBoxEntry::SetText(const FText& InText)
{
	TextBlock->SetText(InText);
}

void UBSComboBoxEntry::ToggleTooltipImageVisibility(const bool bIsVisible)
{
	if (bIsVisible)
	{
		TooltipImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		TooltipImage->SetVisibility(ESlateVisibility::Collapsed);
	}
}
