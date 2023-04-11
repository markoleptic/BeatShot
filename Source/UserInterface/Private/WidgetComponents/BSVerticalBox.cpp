// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/BSVerticalBox.h"

#include "GlobalConstants.h"
#include "WidgetComponents/BSHorizontalBox.h"

TArray<TObjectPtr<UPanelSlot>>& UBSVerticalBox::GetBSHorizontalBoxSlots()
{
	return Slots;
}

void UBSVerticalBox::SetHorizontalBoxBorders()
{
	for (const TObjectPtr<UPanelSlot>& BoxSlot : GetBSHorizontalBoxSlots())
	{
		if (Cast<UBSHorizontalBox>(BoxSlot->Content))
		{
			Cast<UBSHorizontalBox>(BoxSlot->Content)->SetBorders();
		}
	}
}

void UBSVerticalBox::UpdateBackgroundColors()
{
	bool bLastLeftBorderDark = false;
	for (const TObjectPtr<UPanelSlot>& BoxSlot : GetBSHorizontalBoxSlots())
	{
		if (BoxSlot->Content->GetVisibility() == ESlateVisibility::Collapsed || !Cast<UBSHorizontalBox>(BoxSlot->Content))
		{
			continue;
		}
		const UBSHorizontalBox* Box = Cast<UBSHorizontalBox>(BoxSlot->Content);
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
}
