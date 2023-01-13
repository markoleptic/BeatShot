// Fill out your copyright notice in the Description page of Project Settings.


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

