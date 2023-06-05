// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/BSHorizontalBox.h"

#include "Components/Border.h"
#include "Components/HorizontalBoxSlot.h"

TSharedRef<SWidget> UBSHorizontalBox::RebuildWidget()
{
	MyHorizontalBox = SNew(SHorizontalBox);
	
	for (const TObjectPtr<UPanelSlot>& PanelSlot : Slots)
	{
		if (UHorizontalBoxSlot* TypedSlot = Cast<UHorizontalBoxSlot>(PanelSlot))
		{
			TypedSlot->Parent = this;
			TypedSlot->BuildSlot(MyHorizontalBox.ToSharedRef());
		}
		if (UPanelWidget* PanelWidget = Cast<UPanelWidget>(PanelSlot->Content))
		{
			if (UBorder* FoundBorder = DescendPanelWidget(PanelWidget))
			{
				Borders.AddUnique(FoundBorder);
			}
		}
	}
	return MyHorizontalBox.ToSharedRef();
}

void UBSHorizontalBox::SetLeftBorderBrushTint(const FLinearColor& Color)
{
	if (Borders.IsValidIndex(0))
	{
		Borders[0]->SetBrushColor(Color);
	}
}

void UBSHorizontalBox::SetRightBorderBrushTint(const FLinearColor& Color)
{
	if (Borders.IsValidIndex(1))
	{
		Borders[1]->SetBrushColor(Color);
	}
	/*if (!RightBorder)
	{
		SetBorders();
		if (!RightBorder)
		{
			return;
		}
	}
	RightBorder->SetBrushColor(Color);*/
}

void UBSHorizontalBox::RefreshBorders()
{
	for (const TObjectPtr<UPanelSlot>& PanelSlot : Slots)
	{
		if (UPanelWidget* PanelWidget = Cast<UPanelWidget>(PanelSlot->Content))
		{
			if (UBorder* FoundBorder = DescendPanelWidget(PanelWidget))
			{
				Borders.AddUnique(FoundBorder);
			}
		}
	}
}

UBorder* UBSHorizontalBox::DescendPanelWidget(UPanelWidget* PanelWidget)
{
	// Check the PanelWidget first
	if (Cast<UBorder>(PanelWidget))
	{
		return Cast<UBorder>(PanelWidget);
	}
	
	// Search through children
	for (UWidget* Child : PanelWidget->GetAllChildren())
	{
		// Check child
		if (Cast<UBorder>(Child))
		{
			return Cast<UBorder>(Child);
		}
		if (UPanelWidget* ChildPanelWidget = Cast<UPanelWidget>(Child))
		{
			// Recursively call function on any child's children
			if (UBorder* FoundBorder = DescendPanelWidget(ChildPanelWidget))
			{
				return FoundBorder;
			}
		}
	}
	return nullptr;
}
