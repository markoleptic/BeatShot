// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/HorizontalBox.h"
#include "BSHorizontalBox.generated.h"

class UBorder;

/** Container holding two Borders. Used primarily to change background colors (brush tints) at runtime */
UCLASS()
class USERINTERFACE_API UBSHorizontalBox : public UHorizontalBox
{
	GENERATED_BODY()
	
public:
	/** Sets the Brush tint for the Left Border */
	void SetLeftBorderBrushTint(const FLinearColor& Color) const;
	
	/** Sets the Brush tint for the Right Border */
	void SetRightBorderBrushTint(const FLinearColor& Color) const;
	
	/** Sets the LeftBorder and RightBorder pointers */
	void SetBorders();
	
protected:
	/* Pointer to the left border inside the horizontal box */
	TObjectPtr<UBorder> LeftBorder;
	
	/* Pointer to the right border inside the horizontal box */
	TObjectPtr<UBorder> RightBorder;
};
