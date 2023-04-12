// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "UserInterface.h"
#include "Components/EditableTextBox.h"
#include "Components/Slider.h"
#include "Kismet/KismetStringLibrary.h"

float UUserInterface::OnEditableTextBoxChanged(const FText& NewTextValue, UEditableTextBox* TextBoxToChange, USlider* SliderToChange, const float GridSnapSize, const float Min,
                                               const float Max) 
{
	const FString StringTextValue = UKismetStringLibrary::Replace(NewTextValue.ToString(), ",", "");
	const float ClampedValue = FMath::Clamp(FCString::Atof(*StringTextValue), Min, Max);
	const float SnappedValue = FMath::GridSnap(ClampedValue, GridSnapSize);
	TextBoxToChange->SetText(FText::AsNumber(SnappedValue));
	SliderToChange->SetValue(SnappedValue);
	return SnappedValue;
}

float UUserInterface::OnSliderChanged(const float NewValue, UEditableTextBox* TextBoxToChange, const float GridSnapSize) 
{
	const float ReturnValue = FMath::GridSnap(NewValue, GridSnapSize);
	TextBoxToChange->SetText(FText::AsNumber(ReturnValue));
	return ReturnValue;
}
