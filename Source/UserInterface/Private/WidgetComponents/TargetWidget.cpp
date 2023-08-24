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
		TargetImage->SetDesiredSizeOverride(FVector2d(50.f * NewScale.X, 50.f * NewScale.Y));
	}
}

void UTargetWidget::SetTargetColor(const FLinearColor& Color) const
{
	if (TargetImageMaterial)
	{
		TargetImageMaterial->SetVectorParameterValue(FName("TargetColor"), Color);
	}
}

void UTargetWidget::SetTargetOutlineColor(const FLinearColor& Color) const
{
	if (TargetImageMaterial)
	{
		TargetImageMaterial->SetVectorParameterValue(FName("TargetOutlineColor"), Color);
	}
}

void UTargetWidget::SetUseSeparateTargetOutlineColor(const float bUse) const
{
	if (TargetImageMaterial)
	{
		TargetImageMaterial->SetScalarParameterValue("bUseSeparateOutlineColor", bUse);
	}
}

void UTargetWidget::SetTargetPosition(const FVector2d& InPosition) const
{
	if (TargetImage)
	{
		//const FVector2d HalfSize = TargetImage->GetDesiredSize() / 2.f;
		const FMargin NewPadding(InPosition.X /*- HalfSize.X*/, 0, 0, InPosition.Y);
		if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(Slot))
		{
			OverlaySlot->SetPadding(NewPadding);
		}
	}
}
