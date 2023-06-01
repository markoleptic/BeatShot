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

void IBSWidgetInterface::OnTooltipImageHovered(UTooltipImage* HoveredTooltipImage)
{
	if (!TooltipWidget.IsValid() || HoveredTooltipImage == nullptr)
	{
		return;
	}
	
	FTooltipData Data;
	Data.TooltipImage = HoveredTooltipImage;
	
	if (const int32 Index = TooltipData.Find(Data); Index != INDEX_NONE)
	{
		TooltipWidget->TooltipDescriptor->SetText(TooltipData[Index].TooltipText);
		TooltipWidget->TooltipDescriptor->SetAutoWrapText(TooltipData[Index].bAllowTextWrap);
	}
	HoveredTooltipImage->SetToolTip(TooltipWidget.Get());
}

void IBSWidgetInterface::AddToTooltipData(UTooltipImage* TooltipImage, const FText& TooltipText, const bool bInAllowTextWrap)
{
	if (TooltipData.Contains(FTooltipData(TooltipImage, FText())))
	{
		EditTooltipText(TooltipImage, TooltipText);
		return;
	}
	TooltipImage->OnTooltipHovered.AddDynamic(this, &IBSWidgetInterface::OnTooltipImageHovered);
	TooltipData.AddUnique(FTooltipData(TooltipImage, TooltipText, bInAllowTextWrap));
}

void IBSWidgetInterface::AddToTooltipData(const FTooltipData& InToolTipData)
{
	TooltipData.Emplace(InToolTipData);
}

void IBSWidgetInterface::EditTooltipText(const UTooltipImage* TooltipImage, const FText& TooltipText)
{
	FTooltipData* Found = TooltipData.FindByPredicate([&] (const FTooltipData& Data)
	{
		if (Data.TooltipImage)
		{
			if (Data.TooltipImage.Get() == TooltipImage)
			{
				return true;
			}
		}
		return false;
	});
	if (Found)
	{
		Found->TooltipText = TooltipText;
	}
}
