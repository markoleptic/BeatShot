// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/BSComboBoxEntry.h"

#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "WidgetComponents/TooltipImage.h"

void UBSComboBoxEntry::SetText(const FText& InText) const
{
	TextBlock->SetText(InText);
}

void UBSComboBoxEntry::ToggleTooltipImageVisibility(const bool bIsVisible) const
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

void UBSComboBoxEntry::SetBackgroundBrushTint(const FLinearColor& Color) const
{
	Background->SetBrushColor(Color);
}

void UBSComboBoxEntry::SetIsSelectedOption(const bool bIsSelected) const
{
	bIsSelectedOption = bIsSelected;
	if (bIsSelected)
	{
		SetBackgroundBrushTint(SelectedColor);
	}
	else
	{
		SetBackgroundBrushTint(NotSelectedColor);
	}
}

FString UBSComboBoxEntry::GetTextBlockAsString() const
{
	return TextBlock->GetText().ToString();
}
