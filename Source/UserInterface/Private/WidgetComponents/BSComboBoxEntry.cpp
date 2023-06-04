// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/BSComboBoxEntry.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "WidgetComponents/TooltipImage.h"

void UBSComboBoxEntry::SetEntryText(const FText& InText) const
{
	TextBlock_Entry->SetText(InText);
}

void UBSComboBoxEntry::SetTooltipImageVisibility(const bool bIsVisible) const
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

FString UBSComboBoxEntry::GetEntryTextAsString() const
{
	return TextBlock_Entry->GetText().ToString();
}
