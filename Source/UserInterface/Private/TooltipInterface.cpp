// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "TooltipInterface.h"
#include "Components/TextBlock.h"
#include "WidgetComponents/TooltipImage.h"
#include "WidgetComponents/TooltipWidget.h"

void ITooltipInterface::SetTooltipWidget(const UTooltipWidget* InTooltipWidget)
{
	if (InTooltipWidget)
	{
		TooltipWidget = InTooltipWidget;
	}
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
