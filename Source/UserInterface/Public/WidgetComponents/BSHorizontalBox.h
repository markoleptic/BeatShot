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
	
	virtual void PostLoad() override;
	
public:
	/** Sets the Brush tint for the Left Border */
	void SetLeftBorderBrushTint(const FLinearColor& Color);
	
	/** Sets the Brush tint for the Right Border */
	void SetRightBorderBrushTint(const FLinearColor& Color);

protected:
	/** Sets the LeftBorder and RightBorder pointers */
	void SetBorders();
	
	/* Pointer to the left border inside the horizontal box */
	TObjectPtr<UBorder> LeftBorder;
	
	/* Pointer to the right border inside the horizontal box */
	TObjectPtr<UBorder> RightBorder;
};
