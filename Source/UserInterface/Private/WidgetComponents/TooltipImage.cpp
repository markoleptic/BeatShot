// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/TooltipImage.h"
#include "Components/Button.h"

void UTooltipImage::NativeConstruct()
{
	Super::NativeConstruct();
	Button->OnHovered.AddDynamic(this, &UTooltipImage::OnTooltipImageHoveredCallback);
}

void UTooltipImage::OnTooltipImageHoveredCallback()
{
	OnTooltipHovered.Broadcast(this, TooltipData);
}

void UTooltipImage::SetupTooltipImage(const FText& InText, const bool bAllowTextWrap)
{
	TooltipData = FTooltipData(InText, bAllowTextWrap);
}
