// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/VerticalBox.h"
#include "BSVerticalBox.generated.h"

/** Container holding BSHorizontalBoxes. Primarily used to update brush tints if some boxes have been collapsed */
UCLASS()
class USERINTERFACE_API UBSVerticalBox : public UVerticalBox
{
	GENERATED_BODY()

	friend class UBSSettingCategoryWidget;
	
public:
	
	/** Iterates through all BSHorizontalBoxes, alternating the Brush Tints based on the last set Brush Tint, skipping collapsed boxes */
	void UpdateBrushColors();

	/** Adds a widget-box pair to the WidgetBoxMap. Any widget-box pairs will be called when updating
	 *  the background colors so that other child widgets that contain BSVerticalBoxes will match this one */
	void AddWidgetBoxPair(UWidget* InWidget, UBSVerticalBox* InBox);

private:
	/** Returns whether or not the widget in the vertical box slot is in the WidgetBoxMap */
	bool IsPanelSlotInAdditionalWidgetChildren(const TObjectPtr<UPanelSlot>& BoxSlot) const;

	/** Iterates through all slots and changes the Brush Color of the borders. If a slot's content widget is contained in WidgetBoxMap,
	 *  it calls this function on the widget's associated BSVerticalBox */
	bool IterateThroughSlots_UpdateBrushColors(TArray<TObjectPtr<UPanelSlot>>& InSlots, bool bLastLeftBorderDark);

	/** Returns the BSVerticalBox corresponding to a widget inside this container's slots */
	UBSVerticalBox* GetBSBoxFromChildWidget(const TObjectPtr<UWidget>& InWidgetPtr) const;

	/** A map containing additional widget-box pairs that may be nested inside the Vertical Box. The widget should be a slot in this BSVerticalBox, and the BSVerticalBox associated with it
	 *  should contain all the widget's content, namely BSHorizontalBoxes and BSVerticalBoxes */
	TMap<TObjectPtr<UWidget>, UBSVerticalBox*> WidgetBoxMap;
};


