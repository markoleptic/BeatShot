// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Overlays/HitTimingWidget.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Utilities/BSWidgetInterface.h"

FNumberFormattingOptions UHitTimingWidget::NumberFormattingOptions = FNumberFormattingOptions().
	SetMaximumFractionalDigits(0);

void UHitTimingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Image_LogoTick->GetDynamicMaterial())
	{
		DynamicImageMaterial = Image_LogoTick->GetDynamicMaterial();
	}

	OnTickTransition.BindDynamic(this, &ThisClass::TickTransition);
	Tick_Transition_Timeline.AddInterpFloat(Tick_Current_Curve, OnTickTransition);
	Tick_Transition_Timeline.SetPlayRate(4.f);
}

void UHitTimingWidget::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	Tick_Transition_Timeline.TickTimeline(InDeltaTime);
}

void UHitTimingWidget::Init(const float MinTickValueInMilliseconds, const float MaxTickValueInMilliseconds)
{
	static const FTextFormat MillisecondFormat = FTextFormat(
		IBSWidgetInterface::GetWidgetTextFromKey("Scores_MillisecondFormat"));

	Tick_Min->SetText(FText::Format(MillisecondFormat,
	                                FText::AsNumber(MinTickValueInMilliseconds, &NumberFormattingOptions)));
	Tick_Max->SetText(FText::Format(MillisecondFormat,
	                                FText::AsNumber(MaxTickValueInMilliseconds, &NumberFormattingOptions)));
	Tick_Mid->SetText(FText::Format(MillisecondFormat, FText::AsNumber(0, &NumberFormattingOptions)));
}

void UHitTimingWidget::UpdateHitTiming(const float TimeOffsetNormalized, const float TimeOffsetRaw)
{
	if (TimeOffsetNormalized > 0.f && DynamicImageMaterial)
	{
		DynamicImageMaterial->SetScalarParameterValue(FName("LastTimeOffset"), LastTimeOffset);
		DynamicImageMaterial->SetScalarParameterValue(FName("TimeOffset"), TimeOffsetNormalized);

		Tick_Current->SetText(MakeCurrentTickText(TimeOffsetRaw));
		Tick_Current_Offset_Start = Tick_Current_Offset_End;
		Tick_Current_Offset_End = -300.f + TimeOffsetNormalized * (300.f - -300.f);
		LastTimeOffset = TimeOffsetNormalized;
		Tick_Transition_Timeline.PlayFromStart();
	}
}

FText UHitTimingWidget::MakeCurrentTickText(const float TimeOffsetRaw)
{
	static const FTextFormat TimeForFormat = FTextFormat(
		IBSWidgetInterface::GetWidgetTextFromKey("Scores_MillisecondFormat"));
	return FText::Format(TimeForFormat, FText::AsNumber(TimeOffsetRaw * 1000.f, &NumberFormattingOptions));
}

void UHitTimingWidget::TickTransition(const float Value)
{
	DynamicImageMaterial->SetScalarParameterValue(FName("EaseInOut"), Value);
	const float LerpValue = Tick_Current_Offset_Start + Value * (Tick_Current_Offset_End - Tick_Current_Offset_Start);
	Cast<UHorizontalBoxSlot>(Tick_Current->Slot)->SetPadding(FMargin(LerpValue, 0.f, 0.f, 0.f));
}
