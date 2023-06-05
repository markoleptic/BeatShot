// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/HorizontalBox.h"
#include "BSHorizontalBox.generated.h"

class UBorder;

/** Container that should hold two UBorders. Used primarily to change background colors (brush tints) at runtime */
UCLASS()
class USERINTERFACE_API UBSHorizontalBox : public UHorizontalBox
{
	GENERATED_BODY()

	virtual TSharedRef<SWidget> RebuildWidget() override;
	
public:
	/** Sets the Brush tint for the Left Border */
	void SetLeftBorderBrushTint(const FLinearColor& Color);
	
	/** Sets the Brush tint for the Right Border */
	void SetRightBorderBrushTint(const FLinearColor& Color);

	/** Searches for the first Border it finds for each slot in Slots, adding to Borders if it finds one */
	void RefreshBorders();

protected:
	/** Descends a PanelWidget, searching for the first Border it can find. Recursively calls itself on any children */
	UBorder* DescendPanelWidget(UPanelWidget* PanelWidget);
	
	/* Array of pointers to the first border found in every slot */
	TArray<TObjectPtr<UBorder>> Borders;
};
