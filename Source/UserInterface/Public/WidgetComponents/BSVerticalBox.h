// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/VerticalBox.h"
#include "BSVerticalBox.generated.h"

class UBSHorizontalBox;
/** Container holding BSHorizontalBoxes. Primarily used to update brush tints if some boxes have been collapsed */
UCLASS()
class USERINTERFACE_API UBSVerticalBox : public UVerticalBox
{
	GENERATED_BODY()

	friend class UBSSettingCategoryWidget;

	virtual TSharedRef<SWidget> RebuildWidget() override;
	
public:
	/** Iterates through all BSHorizontalBoxes, alternating the Brush Tints based on the last set Brush Tint, skipping collapsed boxes */
	void UpdateBrushColors();

private:
	/** Iterates through all slots and changes the Brush Color of the borders. If a slot's content widget is contained in WidgetBoxMap,
	 *  it calls this function on the widget's associated BSVerticalBox */
	bool IterateThroughSlots_UpdateBrushColors(TArray<TObjectPtr<UPanelSlot>>& InSlots, bool bLastLeftBorderDark);

	/** A map containing additional widget-box pairs that may be nested inside the Vertical Box. The widget should be a slot in this BSVerticalBox, and the BSVerticalBox associated with it
	 *  should contain all the widget's content, namely BSHorizontalBoxes and BSVerticalBoxes */
	TMap<TObjectPtr<UWidget>, UBSVerticalBox*> WidgetBoxMap;
};


