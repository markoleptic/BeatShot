// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/TooltipImage.h"
#include "Components/Button.h"

void UTooltipImage::NativeConstruct()
{
	Super::NativeConstruct();
	Button->OnHovered.AddUniqueDynamic(this, &UTooltipImage::OnTooltipImageHoveredCallback);
}

void UTooltipImage::OnTooltipImageHoveredCallback()
{
	OnTooltipImageHovered.Broadcast(this, TooltipText);
}
