// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "TooltipInterface.h"

#include "Components/EditableTextBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetStringLibrary.h"
#include "WidgetComponents/TooltipImage.h"
#include "WidgetComponents/TooltipWidget.h"

void ITooltipInterface::SetTooltipWidget(const UTooltipWidget* InTooltipWidget)
{
	TooltipWidget = InTooltipWidget;
}

UTooltipWidget* ITooltipInterface::GetTooltipWidget() const
{
	return TooltipWidget.Get();
}

TArray<FTooltipData>& ITooltipInterface::GetTooltipData()
{
	return TooltipData;
}

void ITooltipInterface::OnTooltipImageHovered(UTooltipImage* HoveredTooltipImage)
{
	if (!TooltipWidget.IsValid())
	{
		return;
	}
	if (const int32 Index = GetTooltipData().Find(HoveredTooltipImage); Index != INDEX_NONE)
	{
		TooltipWidget->TooltipDescriptor->SetText(GetTooltipData()[Index].TooltipText);
		TooltipWidget->TooltipDescriptor->SetAutoWrapText(GetTooltipData()[Index].bAllowTextWrap);
	}
	HoveredTooltipImage->SetToolTip(TooltipWidget.Get());
}

float ITooltipInterface::OnEditableTextBoxChanged(const FText& NewTextValue, UEditableTextBox* TextBoxToChange, USlider* SliderToChange, const float GridSnapSize, const float Min,
	const float Max) const
{
	const FString StringTextValue = UKismetStringLibrary::Replace(NewTextValue.ToString(), ",", "");
	const float ClampedValue = FMath::Clamp(FCString::Atof(*StringTextValue), Min, Max);
	const float SnappedValue = FMath::GridSnap(ClampedValue, GridSnapSize);
	TextBoxToChange->SetText(FText::AsNumber(SnappedValue));
	SliderToChange->SetValue(SnappedValue);
	return SnappedValue;
}

float ITooltipInterface::OnSliderChanged(const float NewValue, UEditableTextBox* TextBoxToChange, const float GridSnapSize) const
{
	const float ReturnValue = FMath::GridSnap(NewValue, GridSnapSize);
	TextBoxToChange->SetText(FText::AsNumber(ReturnValue));
	return ReturnValue;
}

void ITooltipInterface::AddToTooltipData(UTooltipImage* TooltipImage, const FText& TooltipText, const bool bInAllowTextWrap)
{
	TooltipImage->OnTooltipHovered.AddDynamic(this, &ITooltipInterface::OnTooltipImageHovered);
	GetTooltipData().AddUnique(FTooltipData(TooltipImage, TooltipText, bInAllowTextWrap));
}

void ITooltipInterface::AddToTooltipData(const FTooltipData& InToolTipData)
{
	GetTooltipData().Emplace(InToolTipData);
}

void ITooltipInterface::EditTooltipText(const UTooltipImage* TooltipImage, const FText& TooltipText)
{
	if (const int32 Index = GetTooltipData().Find(TooltipImage); Index != INDEX_NONE)
	{
		GetTooltipData()[Index].TooltipText = TooltipText;
	}
}
