// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/BSVerticalBox.h"
#include "GlobalConstants.h"
#include "WidgetComponents/BSHorizontalBox.h"
#include "WidgetComponents/BSSettingCategoryWidget.h"

UBSVerticalBox* UBSVerticalBox::GetBSBoxFromChildWidget(const TObjectPtr<UWidget>& InWidgetPtr) const
{
	if (UBSVerticalBox* Box = WidgetBoxMap.FindRef(InWidgetPtr); Box != nullptr)
	{
		return Box;
	}
	return nullptr;
}

void UBSVerticalBox::UpdateBrushColors()
{
	IterateThroughSlots_UpdateBrushColors(Slots, false);
}

bool UBSVerticalBox::IterateThroughSlots_UpdateBrushColors(TArray<TObjectPtr<UPanelSlot>>& InSlots, bool bLastLeftBorderDark)
{
	for (const TObjectPtr<UPanelSlot>& BoxSlot : InSlots)
	{
		if (IsPanelSlotInWidgetBoxMap(BoxSlot))
		{
			if (UBSVerticalBox* SubVerticalBox = GetBSBoxFromChildWidget(BoxSlot->Content))
			{
				bLastLeftBorderDark = SubVerticalBox->IterateThroughSlots_UpdateBrushColors(SubVerticalBox->Slots, bLastLeftBorderDark);
			}
			continue;
		}

		if (!Cast<UBSHorizontalBox>(BoxSlot->Content) || BoxSlot->Content->GetVisibility() == ESlateVisibility::Collapsed)
		{
			continue;
		}
		
		UBSHorizontalBox* Box = Cast<UBSHorizontalBox>(BoxSlot->Content);
		if (bLastLeftBorderDark)
		{
			Box->SetLeftBorderBrushTint(Constants::LightMenuBrushColor);
			Box->SetRightBorderBrushTint(Constants::DarkMenuBrushColor);
			bLastLeftBorderDark = false;
		}
		else
		{
			Box->SetLeftBorderBrushTint(Constants::DarkMenuBrushColor);
			Box->SetRightBorderBrushTint(Constants::LightMenuBrushColor);
			bLastLeftBorderDark = true;
		}
	}
	return bLastLeftBorderDark;
}

void UBSVerticalBox::AddWidgetBoxPair(UWidget* InWidget, UBSVerticalBox* InBox)
{
	if (InWidget && InBox)
	{
		WidgetBoxMap.Add(InWidget, InBox);
	}
}

bool UBSVerticalBox::IsPanelSlotInWidgetBoxMap(const TObjectPtr<UPanelSlot>& BoxSlot) const
{
	if (!BoxSlot || WidgetBoxMap.IsEmpty())
	{
		return false;
	}
	
	if (WidgetBoxMap.Contains(BoxSlot->Content))
	{
		return true;
	}
	return false;
}
