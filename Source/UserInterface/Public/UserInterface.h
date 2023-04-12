// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UserInterface.generated.h"

class USlider;
class UEditableTextBox;

UCLASS()
class USERINTERFACE_API UUserInterface : public UObject
{
	GENERATED_BODY()
	
public:
	
	/** Clamps NewTextValue, updates associated Slider value while rounding to the GridSnapSize */
	static float OnEditableTextBoxChanged(const FText& NewTextValue, UEditableTextBox* TextBoxToChange, USlider* SliderToChange, const float GridSnapSize, const float Min, const float Max);

	/** Updates associated TextBoxToChange with result of rounding to the GridSnapSize */
	static float OnSliderChanged(const float NewValue, UEditableTextBox* TextBoxToChange, const float GridSnapSize);
};
