// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "WidgetComponents/ConstrainedSlider_BeatGrid.h"
#include "Components/Slider.h"

void UConstrainedSlider_BeatGrid::UpdateBeatGridConstraints(const int32 NewNumHorizontalTargets, const int32 NewNumVerticalTargets, const float NewMaxTargetScale)
{
	NumHorizontalTargets = NewNumHorizontalTargets;
	NumVerticalTargets = NewNumVerticalTargets;
	MaxTargetSize = NewMaxTargetScale;
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
	/*SliderStruct.MinConstraintUpper = GetMaxAllowedHorizontalSpacing();
	SliderStruct.MaxConstraintUpper = GetMaxAllowedVerticalSpacing();
	MinSlider->SetMaxValue(SliderStruct.MinConstraintUpper);
	MaxSlider->SetMaxValue(SliderStruct.MaxConstraintUpper);
	if (MinSlider->GetValue() > SliderStruct.MinConstraintUpper)
	{
		OnMinSliderChanged(SliderStruct.MinConstraintUpper);
	}
	if (MaxSlider->GetValue() > SliderStruct.MaxConstraintUpper)
	{
		OnMaxSliderChanged(SliderStruct.MaxConstraintUpper);
	}*/

	FBeatGridConstraints BeatGridConstraints;
	
	if (GetMaxAllowedHorizontalSpacing() < MinSlider->GetValue())
	{
		BeatGridConstraints.HorizontalSpacing = GetMaxAllowedHorizontalSpacing();
	}
	if (GetMaxAllowedVerticalSpacing() < MaxSlider->GetValue())
	{
		BeatGridConstraints.VerticalSpacing = GetMaxAllowedVerticalSpacing();
	}
	if (GetMaxAllowedTargetScale() < MaxTargetSize / SphereDiameter)
	{
		BeatGridConstraints.TargetScale = GetMaxAllowedTargetScale();
	}
	if (GetMaxAllowedNumHorizontalTargets() < NumHorizontalTargets)
	{
		BeatGridConstraints.NumHorizontalTargets = GetMaxAllowedNumHorizontalTargets();
	}
	if (GetMaxAllowedNumVerticalTargets() < NumVerticalTargets)
	{
		BeatGridConstraints.NumVerticalTargets = GetMaxAllowedNumVerticalTargets();
	}
	OnBeatGridSpacingConstrained.Execute(BeatGridConstraints);
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
	// HorizontalSpread = TargetScale * SphereDiameter * NumHorizontalTargets + HorizontalSpacing * (NumHorizontalTargets - 1)
	// HorizontalSpread - HorizontalSpacing * (NumHorizontalTargets - 1)= TargetScale * SphereDiameter * NumHorizontalTargets 
	// HorizontalSpread - HorizontalSpacing * NumHorizontalTargets + HorizontalSpacing = TargetScale * SphereDiameter * NumHorizontalTargets 
	// (HorizontalSpread - HorizontalSpacing * NumHorizontalTargets + HorizontalSpacing) / (SphereDiameter * NumHorizontalTargets) = TargetScale
	
	const float HorizontalSpacing = MinSlider->GetValue();
	const float VerticalSpacing = MaxSlider->GetValue();
	const float MaxAllowedHorizontal = (HorizontalSpread - HorizontalSpacing * NumHorizontalTargets + HorizontalSpacing) / (SphereDiameter * NumHorizontalTargets);
	const float MaxAllowedVertical = (VerticalSpread - VerticalSpacing * NumVerticalTargets + VerticalSpacing) / (SphereDiameter * NumVerticalTargets);
	if (MaxAllowedVertical < MaxAllowedHorizontal)
	{
		return MaxAllowedVertical;
	}
	return MaxAllowedHorizontal;
}

int32 UConstrainedSlider_BeatGrid::GetMaxAllowedNumHorizontalTargets() const
{
	// HorizontalSpread = MaxTargetSize * NumHorizontalTargets + HorizontalSpacing * (NumHorizontalTargets - 1)
	// HorizontalSpread = MaxTargetSize * NumHorizontalTargets + HorizontalSpacing * NumHorizontalTargets - HorizontalSpacing
	// HorizontalSpread + HorizontalSpacing = MaxTargetSize * NumHorizontalTargets + HorizontalSpacing * NumHorizontalTargets
	// HorizontalSpread + HorizontalSpacing = NumHorizontalTargets(MaxTargetSize + HorizontalSpacing)
	// (HorizontalSpread + HorizontalSpacing) / (MaxTargetSize + HorizontalSpacing) = NumHorizontalTargets
	const float HorizontalSpacing = MinSlider->GetValue();
	return (HorizontalSpread + HorizontalSpacing) / (MaxTargetSize + HorizontalSpacing);
}

int32 UConstrainedSlider_BeatGrid::GetMaxAllowedNumVerticalTargets() const
{
	const float VerticalSpacing = MaxSlider->GetValue();
	return (VerticalSpread + VerticalSpacing) / (MaxTargetSize + VerticalSpacing);
}

void UConstrainedSlider_BeatGrid::OnBeatGridUpdate_MaxTargetScale(const float NewMaxTargetScale)
{
	MaxTargetSize = NewMaxTargetScale * SphereDiameter;
	CheckBeatGridConstraints();
}
