// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "BSWidgetInterface.h"
#include "Components/EditableTextBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetStringLibrary.h"
#include "WidgetComponents/TooltipImage.h"
#include "WidgetComponents/TooltipWidget.h"

void IBSWidgetInterface::SetTooltipWidget(const UTooltipWidget* InTooltipWidget)
{
	if (InTooltipWidget)
	{
		TooltipWidget = InTooltipWidget;
	}
}

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

UTooltipWidget* IBSWidgetInterface::GetTooltipWidget() const
{
	return TooltipWidget.Get();
}

TArray<FTooltipData>& IBSWidgetInterface::GetTooltipData()
{
	return TooltipData;
}

void IBSWidgetInterface::OnTooltipImageHovered(UTooltipImage* HoveredTooltipImage)
{
	if (!TooltipWidget.IsValid() || HoveredTooltipImage == nullptr)
	{
		return;
	}
	
	FTooltipData Data;
	Data.TooltipImage = HoveredTooltipImage;
	
	if (const int32 Index = GetTooltipData().Find(Data); Index != INDEX_NONE)
	{
		TooltipWidget->TooltipDescriptor->SetText(GetTooltipData()[Index].TooltipText);
		TooltipWidget->TooltipDescriptor->SetAutoWrapText(GetTooltipData()[Index].bAllowTextWrap);
	}
	HoveredTooltipImage->SetToolTip(TooltipWidget.Get());
}

void IBSWidgetInterface::AddToTooltipData(UTooltipImage* TooltipImage, const FText& TooltipText, const bool bInAllowTextWrap)
{
	TooltipImage->OnTooltipHovered.AddDynamic(this, &IBSWidgetInterface::OnTooltipImageHovered);
	GetTooltipData().AddUnique(FTooltipData(TooltipImage, TooltipText, bInAllowTextWrap));
}

void IBSWidgetInterface::AddToTooltipData(const FTooltipData& InToolTipData)
{
	GetTooltipData().Emplace(InToolTipData);
}

void IBSWidgetInterface::EditTooltipText(const UTooltipImage* TooltipImage, const FText& TooltipText)
{
	if (TooltipImage == nullptr)
	{
		return;
	}
	FTooltipData Data;
	Data.TooltipImage = TooltipImage;
	if (const int32 Index = GetTooltipData().Find(Data); Index != INDEX_NONE)
	{
		GetTooltipData()[Index].TooltipText = TooltipText;
	}
}
