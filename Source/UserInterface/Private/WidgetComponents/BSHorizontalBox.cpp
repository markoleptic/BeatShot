// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/BSHorizontalBox.h"

#include "Components/Border.h"

void UBSHorizontalBox::PostLoad()
{
	Super::PostLoad();
	SetBorders();
}

void UBSHorizontalBox::SetLeftBorderBrushTint(const FLinearColor& Color)
{
	if (!LeftBorder)
	{
		SetBorders();
		if (!LeftBorder)
		{
			SetBorders();
			return;
		}
	}
	LeftBorder->SetBrushColor(Color);
}

void UBSHorizontalBox::SetRightBorderBrushTint(const FLinearColor& Color)
{
	if (!RightBorder)
	{
		SetBorders();
		if (!RightBorder)
		{
			return;
		}
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
