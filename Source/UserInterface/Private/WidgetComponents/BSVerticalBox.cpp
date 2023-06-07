// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/BSVerticalBox.h"
#include "GlobalConstants.h"
#include "BSWidgetInterface.h"
#include "Blueprint/WidgetTree.h"
#include "WidgetComponents/BSHorizontalBox.h"

TSharedRef<SWidget> UBSVerticalBox::RebuildWidget()
{
	return Super::RebuildWidget();
}

void UBSVerticalBox::UpdateBrushColors()
{
	UpdateHorizontalBoxBrushColors(Slots, false);
}

bool UBSVerticalBox::UpdateHorizontalBoxBrushColors(TArray<TObjectPtr<UPanelSlot>>& InSlots, bool bLastLeftBorderDark)
{
	for (const TObjectPtr<UPanelSlot>& BoxSlot : InSlots)
	{
		// Slot is a BSVerticalBox
		if (UBSVerticalBox* SubVerticalBox = Cast<UBSVerticalBox>(BoxSlot->Content))
		{
			// First slot of every Vertical Box has the same border color
			bLastLeftBorderDark = false;
			bLastLeftBorderDark = UpdateHorizontalBoxBrushColors(SubVerticalBox->Slots, bLastLeftBorderDark);
			continue;
		}

		// Slot is a UUserWidget
		if (const UUserWidget* UserWidget = Cast<UUserWidget>(BoxSlot->Content))
		{
			UserWidget->WidgetTree->ForEachWidget([&bLastLeftBorderDark] (UWidget* Widget)
			{
				if (Cast<UBSVerticalBox>(Widget))
				{
					bLastLeftBorderDark = Cast<UBSVerticalBox>(Widget)->UpdateHorizontalBoxBrushColors(Cast<UBSVerticalBox>(Widget)->Slots, bLastLeftBorderDark);
				}
			});
			continue;
		}
		
		// Slot is a subclass of PanelWidget
		if (UPanelWidget* PanelWidget = Cast<UPanelWidget>(BoxSlot->Content))
		{
			if (TArray<UBSVerticalBox*> Boxes = IBSWidgetInterface::DescendWidget<UBSVerticalBox>(PanelWidget); !Boxes.IsEmpty())
			{
				for (UBSVerticalBox* Box : Boxes)
				{
					// First slot of every Vertical Box has the same border color
					bLastLeftBorderDark = false;
					bLastLeftBorderDark = UpdateHorizontalBoxBrushColors(Box->Slots, bLastLeftBorderDark);
				}
				continue;
			}
		}

		// Slot is a BSHorizontalBox
		if (UBSHorizontalBox* HorizontalBox = Cast<UBSHorizontalBox>(BoxSlot->Content))
		{
			if (HorizontalBox->GetVisibility() != ESlateVisibility::Collapsed)
			{
				if (bLastLeftBorderDark)
				{
					HorizontalBox->SetLeftBorderBrushTint(Constants::LightMenuBrushColor);
					HorizontalBox->SetRightBorderBrushTint(Constants::DarkMenuBrushColor);
					bLastLeftBorderDark = false;
				}
				else
				{
					HorizontalBox->SetLeftBorderBrushTint(Constants::DarkMenuBrushColor);
					HorizontalBox->SetRightBorderBrushTint(Constants::LightMenuBrushColor);
					bLastLeftBorderDark = true;
				}
			}
		}
	}
	return bLastLeftBorderDark;
}