// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "WidgetComponents/ConstrainedSlider_BeatGrid.h"
#include "Components/Slider.h"
#include "Misc/DefaultValueHelper.h"

void UConstrainedSlider_BeatGrid::UpdateBeatGridConstraints(const int32 NewNumHorizontalTargets, const int32 NewNumVerticalTargets, const float NewHorizontalSpread, const float NewVerticalSpread, const float NewMaxTargetScale)
{
	NumHorizontalTargets = NewNumHorizontalTargets;
	NumVerticalTargets = NewNumVerticalTargets;
	HorizontalSpread = NewHorizontalSpread;
	VerticalSpread = NewVerticalSpread;
	MaxTargetSize = NewMaxTargetScale;
	SliderStruct.MinConstraintUpper = GetMaxAllowedHorizontalSpacing();
	SliderStruct.MaxConstraintUpper = GetMaxAllowedVerticalSpacing();
	MinSlider->SetMaxValue(SliderStruct.MinConstraintUpper);
	MaxSlider->SetMaxValue(SliderStruct.MaxConstraintUpper);
}

void UConstrainedSlider_BeatGrid::OnBeatGridUpdate_NumVerticalTargets(const int32 NewNumVerticalTargets)
{
	NumVerticalTargets = NewNumVerticalTargets;
	CheckBeatGridConstraints();
}

void UConstrainedSlider_BeatGrid::OnBeatGridUpdate_NumHorizontalTargets(const int32 NewNumHorizontalTargets)
{
	NumHorizontalTargets = NewNumHorizontalTargets;
	CheckBeatGridConstraints();
}

void UConstrainedSlider_BeatGrid::CheckBeatGridConstraints()
{
	SliderStruct.MinConstraintUpper = GetMaxAllowedHorizontalSpacing();
	SliderStruct.MaxConstraintUpper = GetMaxAllowedVerticalSpacing();
	MinSlider->SetMaxValue(SliderStruct.MinConstraintUpper);
	MaxSlider->SetMaxValue(SliderStruct.MaxConstraintUpper);

	FBeatGridConstraints BeatGridConstraints;
	if (MinSlider->GetValue() > SliderStruct.MinConstraintUpper)
	{
		OnMinSliderChanged(SliderStruct.MinConstraintUpper);
	}
	if (MaxSlider->GetValue() > SliderStruct.MaxConstraintUpper)
	{
		OnMaxSliderChanged(SliderStruct.MaxConstraintUpper);
	}
	
	if (GetMinRequiredHorizontalSpread() > HorizontalSpread)
	{
		BeatGridConstraints.MinRequiredHorizontalSpread = GetMinRequiredHorizontalSpread();
		UE_LOG(LogTemp, Display, TEXT("GetMinRequiredHorizontalSpread() < HorizontalSpread %f %f "), GetMinRequiredHorizontalSpread(), HorizontalSpread);
	}
	if (GetMinRequiredVerticalSpread() > VerticalSpread)
	{
		BeatGridConstraints.MinRequiredVerticalSpread = GetMinRequiredVerticalSpread();
		UE_LOG(LogTemp, Display, TEXT("GetMinRequiredVerticalSpread() < VerticalSpread %f %f "), GetMinRequiredVerticalSpread(), VerticalSpread);
	}
	if (GetMaxAllowedTargetScale() < (MaxTargetSize / SphereDiameter))
	{
		BeatGridConstraints.MaxAllowedTargetScale = GetMaxAllowedTargetScale();
		UE_LOG(LogTemp, Display, TEXT("GetMaxAllowedTargetScale() > (MaxTargetSize / SphereDiameter) %f %f "), GetMaxAllowedTargetScale(), (MaxTargetSize / SphereDiameter));
	}
	if (GetMaxAllowedNumHorizontalTargets() < NumHorizontalTargets)
	{
		BeatGridConstraints.MaxAllowedNumHorizontalTargets = GetMaxAllowedNumHorizontalTargets();
		UE_LOG(LogTemp, Display, TEXT("GetMaxAllowedNumHorizontalTargets() > NumHorizontalTargets %d %d "), GetMaxAllowedNumHorizontalTargets(), NumHorizontalTargets);
	}
	if (GetMaxAllowedNumVerticalTargets() < NumVerticalTargets)
	{
		BeatGridConstraints.MaxAllowedNumVerticalTargets = GetMaxAllowedNumVerticalTargets();
		UE_LOG(LogTemp, Display, TEXT("GetMaxAllowedNumVerticalTargets() > NumVerticalTargets %d %d "), GetMaxAllowedNumVerticalTargets(), NumVerticalTargets);
	}
	OnBeatGridSpacingConstrained.Execute(BeatGridConstraints);

	/*if (TargetSize >= ((TotalWidth - 200 - HSpacing * MaxTargets + HSpacing) / MaxTargets) ||
		TargetSize >= ((TotalHeight - 200 - VSpacing * MaxTargets + VSpacing) / MaxTargets) ||
		TotalWidth <= (TargetSize * MaxTargets + HSpacing * MaxTargets - HSpacing + 200) ||
		TotalHeight <= (TargetSize * MaxTargets + VSpacing * MaxTargets - VSpacing + 200) ||
		MaxTargets >= ((TotalWidth - 200 - HSpacing *MaxTargets + HSpacing) / TargetSize) ||
		MaxTargets >= ((TotalHeight - 200 - VSpacing * MaxTargets + VSpacing) / TargetSize) ||
		HSpacing >= ((TotalWidth - TargetSize * MaxTargets - 200) / (MaxTargets - 1)) ||
		VSpacing >= ((TotalHeight - TargetSize * MaxTargets - 200) / (MaxTargets - 1)))
	{
		/* change minmax values #1#
	}*/
	/*if (BaseGameModeComboBox->GetSelectedOption() != "Beat Grid")
	{
		return;
	}
	const float Width = round(HorizontalSpreadSlider->GetValue());
	const float TargetWidth = ceil(TargetScaleConstrained->MaxSlider->GetValue() * SphereDiameter * 100) / 100;
	const float HSpacing = BeatGridHorizontalSpacingSlider->GetValue();
	const float Height = round(VerticalSpreadSlider->GetValue());
	const float TargetHeight = ceil(TargetScaleConstrained->MaxSlider->GetValue() * SphereDiameter * 100) / 100;
	const float VSpacing = BeatGridVerticalSpacingSlider->GetValue();
	const FString ToConvert = NumBeatGridTargetsComboBox->GetSelectedOption();
	int32 MaxTargets;
	FDefaultValueHelper::ParseInt(ToConvert, MaxTargets);
	MaxTargets = sqrt(MaxTargets);
	// WidthORHeight = TargetScale*MaxTargets + Spacing*MaxTargets - Spacing + 200

	if (MaxTargets >= (Width - 200 - HSpacing * MaxTargets + HSpacing) / TargetWidth)
	{
		UE_LOG(LogTemp, Display, TEXT("MaxTargets height contraint"));
	}
	if (MaxTargets >= (Height - 200 - VSpacing * MaxTargets + VSpacing) / TargetHeight)
	{
		UE_LOG(LogTemp, Display, TEXT("MaxTargets width contraint"));
	}*/
}

float UConstrainedSlider_BeatGrid::GetMaxAllowedHorizontalSpacing() const
{
	const float TotalTargetWidth = MaxTargetSize * NumHorizontalTargets;
	return (HorizontalSpread - TotalTargetWidth) / (NumHorizontalTargets - 1);
}

float UConstrainedSlider_BeatGrid::GetMaxAllowedVerticalSpacing() const
{
	const float TotalTargetHeight = MaxTargetSize * NumVerticalTargets;
	return (VerticalSpread - TotalTargetHeight) / (NumVerticalTargets - 1);
}

float UConstrainedSlider_BeatGrid::GetMinRequiredHorizontalSpread() const
{
	const float TotalTargetWidth = MaxTargetSize * NumHorizontalTargets;
	const float HorizontalSpacing = MinSlider->GetValue();
	return HorizontalSpacing * (NumHorizontalTargets - 1) + TotalTargetWidth;
}

float UConstrainedSlider_BeatGrid::GetMinRequiredVerticalSpread() const
{
	const float TotalTargetHeight = MaxTargetSize * NumVerticalTargets;
	const float VerticalSpacing = MaxSlider->GetValue();
	return VerticalSpacing * (NumVerticalTargets - 1) + TotalTargetHeight;
}

float UConstrainedSlider_BeatGrid::GetMaxAllowedTargetScale() const
{
	// TotalTargetWidth = TargetScale * SphereDiameter * NumHorizontalTargets;
	// HSpacing = (HorizontalSpread - TotalTargetWidth) / (NumHorizontalTargets - 1);
	// HSpacing = (HorizontalSpread - (TargetScale * SphereDiameter * NumHorizontalTargets)) / (NumHorizontalTargets - 1);
	// HSpacing * (NumHorizontalTargets - 1) = HorizontalSpread - (TargetScale * SphereDiameter  * NumHorizontalTargets);
	// (HSpacing * (NumHorizontalTargets - 1) - HorizontalSpread) / - (SphereDiameter  * NumHorizontalTargets) = TargetScale;
	const float HorizontalSpacing = MinSlider->GetValue();
	const float VerticalSpacing = MaxSlider->GetValue();
	const float MaxAllowedHorizontal = (HorizontalSpacing * (NumHorizontalTargets - 1) - HorizontalSpread) / -(SphereDiameter * NumHorizontalTargets);
	const float MaxAllowedVertical = (VerticalSpacing * (NumVerticalTargets - 1) - VerticalSpread) / -(SphereDiameter * NumVerticalTargets);
	if (MaxAllowedVertical > MaxAllowedHorizontal)
	{
		return MaxAllowedVertical;
	}
	return MaxAllowedHorizontal;
}

int32 UConstrainedSlider_BeatGrid::GetMaxAllowedNumHorizontalTargets() const
{
	// NumHorizontalTargets = (HorizontalSpread - TotalTargetWidth + HSpacing) / HSpacing;
	const float TotalTargetWidth = MaxTargetSize * NumHorizontalTargets;
	const float HorizontalSpacing = MinSlider->GetValue();
	return (HorizontalSpread - TotalTargetWidth + HorizontalSpacing) / HorizontalSpacing;
}

int32 UConstrainedSlider_BeatGrid::GetMaxAllowedNumVerticalTargets() const
{
	// NumHorizontalTargets = (HorizontalSpread - TotalTargetWidth + HSpacing) / HSpacing;
	const float TotalTargetHeight = MaxTargetSize * NumVerticalTargets;
	const float VerticalSpacing = MaxSlider->GetValue();
	return (VerticalSpread - TotalTargetHeight + VerticalSpacing) / VerticalSpacing;
}

void UConstrainedSlider_BeatGrid::OnBeatGridUpdate_HorizontalSpread(const float NewHorizontalSpread)
{
	HorizontalSpread = NewHorizontalSpread;
	CheckBeatGridConstraints();
}

void UConstrainedSlider_BeatGrid::OnBeatGridUpdate_VerticalSpread(const float NewVerticalSpread)
{
	VerticalSpread = NewVerticalSpread;
	CheckBeatGridConstraints();
}

void UConstrainedSlider_BeatGrid::OnBeatGridUpdate_MaxTargetScale(const float NewMaxTargetScale)
{
	MaxTargetSize = roundf(NewMaxTargetScale * SphereDiameter * 100) / 100;
	CheckBeatGridConstraints();
}
