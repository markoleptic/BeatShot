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
	
	TArray<TObjectPtr<UPanelSlot>>& GetBSHorizontalBoxSlots();
	
public:
	/** Iterates through all BSHorizontalBoxes and assigns their left and right borders */
	void SetHorizontalBoxBorders();
	
	/** Iterates through all BSHorizontalBoxes, alternating the Brush Tints based on the last set Brush Tint, skipping collapsed boxes */
	void UpdateBackgroundColors();
};
