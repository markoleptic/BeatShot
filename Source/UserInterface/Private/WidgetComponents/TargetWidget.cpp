// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/TargetWidget.h"
#include "Components/Image.h"
#include "Components/OverlaySlot.h"

void UTargetWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (TargetImage && TargetImage->GetDynamicMaterial())
	{
		TargetImageMaterial = TargetImage->GetDynamicMaterial();
	}
}

void UTargetWidget::SetTargetScale(const FVector& NewScale) const
{
	if (TargetImage)
	{
		TargetImage->SetDesiredSizeOverride(FVector2d(100.f * NewScale.X, 100.f * NewScale.Y));
	}
}

void UTargetWidget::SetTargetColor(const FLinearColor& Color) const
{
	if (TargetImageMaterial)
	{
		TargetImageMaterial->SetVectorParameterValue(FName("TargetColor"), Color);
	}
}

void UTargetWidget::SetTargetPosition(const FVector2d& InPosition) const
{
	if (TargetImage)
	{
		const FMargin NewPadding(FMath::Clamp(InPosition.X, -1700, 1700), 0, 0, FMath::Clamp(InPosition.Y, -600, 600));
		if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(Slot))
		{
			OverlaySlot->SetPadding(NewPadding);
		}
	}
}
