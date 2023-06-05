// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "BSWidgetInterface.h"
#include "Components/EditableTextBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetStringLibrary.h"
#include "WidgetComponents/TooltipImage.h"
#include "WidgetComponents/TooltipWidget.h"

float IBSWidgetInterface::OnEditableTextBoxChanged(const FText& NewTextValue, UEditableTextBox* TextBoxToChange, USlider* SliderToChange, const float GridSnapSize, const float Min, const float Max)
{
	const FString StringTextValue = UKismetStringLibrary::Replace(NewTextValue.ToString(), ",", "");
	const float ClampedValue = FMath::Clamp(FCString::Atof(*StringTextValue), Min, Max);
	const float SnappedValue = FMath::GridSnap(ClampedValue, GridSnapSize);
	TextBoxToChange->SetText(FText::AsNumber(SnappedValue));
	SliderToChange->SetValue(SnappedValue);
	return SnappedValue;
}

float IBSWidgetInterface::OnSliderChanged(const float NewValue, UEditableTextBox* TextBoxToChange, const float GridSnapSize)
{
	const float ReturnValue = FMath::GridSnap(NewValue, GridSnapSize);
	TextBoxToChange->SetText(FText::AsNumber(ReturnValue));
	return ReturnValue;
}

void IBSWidgetInterface::OnTooltipImageHovered(UTooltipImage* TooltipImage, const FTooltipData& InTooltipData)
{
	if (!GetTooltipWidget())
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid tooltip widget"));
		return;
	}
	if (!TooltipImage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid TooltipImage."));
		return;
	}
	UTooltipWidget* Tooltip = GetTooltipWidget();
	Tooltip->TooltipDescriptor->SetText(InTooltipData.TooltipText);
	Tooltip->TooltipDescriptor->SetAutoWrapText(InTooltipData.bAllowTextWrap);
	TooltipImage->SetToolTip(Tooltip);
}

void IBSWidgetInterface::SetupTooltip(UTooltipImage* TooltipImage, const FText& TooltipText, const bool bInAllowTextWrap)
{
	if (!TooltipImage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid TooltipImage."));
	}
	if (TooltipText.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Empty Tooltip Text for %s."), *TooltipImage->GetOuter()->GetName());
	}
	
	TooltipImage->SetupTooltipImage(TooltipText, bInAllowTextWrap);
	if (!TooltipImage->OnTooltipHovered.IsBound())
	{
		TooltipImage->OnTooltipHovered.AddDynamic(this, &IBSWidgetInterface::OnTooltipImageHovered);
	}
}

void IBSWidgetInterface::UpdateTooltip(UTooltipImage* TooltipImage, const FText& TooltipText, const bool bInAllowTextWrap)
{
	if (!TooltipImage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid TooltipImage."));
	}
	if (TooltipText.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Empty Tooltip Text for %s."), *TooltipImage->GetOuter()->GetName());
	}
	TooltipImage->SetupTooltipImage(TooltipText, bInAllowTextWrap);
}
