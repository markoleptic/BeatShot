// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/HitTimingWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UHitTimingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (Image_LogoTick->GetDynamicMaterial())
	{
		DynamicImageMaterial = Image_LogoTick->GetDynamicMaterial();
	}
}

void UHitTimingWidget::Init(const FText MinTickValue, const FText MaxTickValue)
{
	Tick_Min->SetText(MinTickValue);
	Tick_Max->SetText(MaxTickValue);
	Tick_Mid->SetText(FText::FromString(FString::FromInt(0) + "ms"));
}

void UHitTimingWidget::UpdateHitTiming(const float TimeOffsetNormalized, const float TimeOffsetRaw)
{
	if (TimeOffsetNormalized > 0.f && DynamicImageMaterial)
	{
		DynamicImageMaterial->SetScalarParameterValue(FName("LastTimeOffset"), LastTimeOffset);
		DynamicImageMaterial->SetScalarParameterValue(FName("TimeOffset"), TimeOffsetNormalized);
		LastTimeOffset = TimeOffsetNormalized;
		Tick_Current->SetText(MakeCurrentTickText(TimeOffsetRaw));
		PlayAnimationForward(EaseInNewOffset);
	}
}

FText UHitTimingWidget::MakeCurrentTickText(const float TimeOffsetRaw) const
{
	const FString Offset = FString::FromInt(static_cast<int32>(TimeOffsetRaw * 100.f)) + "ms";
	return FText::FromString(Offset);
}




