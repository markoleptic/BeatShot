// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/HitTimingWidget.h"
#include "Components/Image.h"

void UHitTimingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (Image_LogoTick->GetDynamicMaterial())
	{
		DynamicImageMaterial = Image_LogoTick->GetDynamicMaterial();
	}
}

void UHitTimingWidget::UpdateHitTiming(const float TimeOffsetNormalized)
{
	if (TimeOffsetNormalized > 0.f && DynamicImageMaterial)
	{
		DynamicImageMaterial->SetScalarParameterValue(FName("LastTimeOffset"), LastTimeOffset);
		DynamicImageMaterial->SetScalarParameterValue(FName("TimeOffset"), TimeOffsetNormalized);
		LastTimeOffset = TimeOffsetNormalized;
		PlayAnimationForward(EaseInNewOffset);
	}
}


