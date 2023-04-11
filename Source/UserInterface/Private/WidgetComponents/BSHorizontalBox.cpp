// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/BSHorizontalBox.h"

#include "Components/Border.h"

void UBSHorizontalBox::SetLeftBorderBrushTint(const FLinearColor& Color) const
{
	if (!LeftBorder)
	{
		return;
	}
	LeftBorder->SetBrushColor(Color);
}

void UBSHorizontalBox::SetRightBorderBrushTint(const FLinearColor& Color) const
{
	if (!RightBorder)
	{
		return;
	}
	RightBorder->SetBrushColor(Color);
}

void UBSHorizontalBox::SetBorders()
{
	for (const TObjectPtr<UPanelSlot>& BorderSlot : Slots)
	{
		if (!LeftBorder && Cast<UBorder>(BorderSlot->Content))
		{
			LeftBorder = Cast<UBorder>(BorderSlot->Content);
			continue;
		}
		if (!RightBorder && Cast<UBorder>(BorderSlot->Content))
		{
			RightBorder = Cast<UBorder>(BorderSlot->Content);
			return;
		}
	}
}
