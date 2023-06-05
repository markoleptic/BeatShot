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
	IterateThroughSlots_UpdateBrushColors(Slots, false);
}

bool UBSVerticalBox::IterateThroughSlots_UpdateBrushColors(TArray<TObjectPtr<UPanelSlot>>& InSlots, bool bLastLeftBorderDark)
{
	for (const TObjectPtr<UPanelSlot>& BoxSlot : InSlots)
	{
		/*if (Cast<UPanelWidget>(BoxSlot))
		{
			for (UPanelSlot* PanelSlot : Cast<UPanelWidget>(BoxSlot)->GetSlots())
			{
				if (Cast<UPanelWidget>(BoxSlot)->HasAnyChildren())
			}
		}
		if (UBSVerticalBox* SubVerticalBox = Cast<UBSVerticalBox>(BoxSlot->Content))
		{
			bLastLeftBorderDark = IterateThroughSlots_UpdateBrushColors(SubVerticalBox->Slots, bLastLeftBorderDark);
			continue;
		}*/
		/*if (IsPanelSlotInWidgetBoxMap(BoxSlot))
		{
			if (UBSVerticalBox* SubVerticalBox = GetBSBoxFromChildWidget(BoxSlot->Content))
			{
				bLastLeftBorderDark = SubVerticalBox->IterateThroughSlots_UpdateBrushColors(SubVerticalBox->Slots, bLastLeftBorderDark);
			}
			continue;
		}*/
		if (UUserWidget* UserWidget = Cast<UUserWidget>(BoxSlot->Content))
		{
			UserWidget->WidgetTree->ForEachWidget([&bLastLeftBorderDark] (UWidget* Widget)
			{
				if (Cast<UBSVerticalBox>(Widget))
				{
					bLastLeftBorderDark = Cast<UBSVerticalBox>(Widget)->IterateThroughSlots_UpdateBrushColors(Cast<UBSVerticalBox>(Widget)->Slots, bLastLeftBorderDark);
				}
			});
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

/*TArray<UBSHorizontalBox*> Boxes;
for (const TObjectPtr<UPanelSlot>& PanelSlot : InSlots)
{
	if (UPanelWidget* PanelWidget = Cast<UPanelWidget>(PanelSlot->Content))
	{
		Boxes.Append(IBSWidgetInterface::DescendWidget<UBSHorizontalBox>(PanelWidget));
	}
	else if (UUserWidget* UserWidget = Cast<UUserWidget>(PanelSlot->Content))
	{
		UserWidget->WidgetTree->ForEachWidget([&Boxes] (UWidget* Widget)
		{
			if (Cast<UBSHorizontalBox>(Widget))
			{
				Boxes.Add(Cast<UBSHorizontalBox>(Widget));
			}
		});
	}
}*/
