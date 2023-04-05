// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "WidgetComponents/ConstrainedSlider_BeatGrid.h"
#include "GlobalConstants.h"
#include "Components/Slider.h"

void UConstrainedSlider_BeatGrid::NativeConstruct()
{
	OnMinValueChanged.AddUObject(this, &UConstrainedSlider_BeatGrid::OnHorizontalSpacingChanged);
	OnMaxValueChanged.AddUObject(this, &UConstrainedSlider_BeatGrid::OnVerticalSpacingChanged);
}

void UConstrainedSlider_BeatGrid::UpdateBeatGridConstraints(const int32 NewNumHorizontalTargets, const int32 NewNumVerticalTargets, const float NewMaxTargetScale)
{
	NumHorizontalTargets = NewNumHorizontalTargets;
	NumVerticalTargets = NewNumVerticalTargets;
	MaxTargetSize = NewMaxTargetScale;
	HorizontalSpacing = MinSlider->GetValue();
	HorizontalSpacing = MaxSlider->GetValue();
	HorizontalSpread = StaticHorizontalSpread + MaxTargetSize;
	VerticalSpread = StaticVerticalSpread + MaxTargetSize;
	CheckBeatGridConstraints();
}

void UConstrainedSlider_BeatGrid::OnBeatGridUpdate_NumHorizontalTargets(const bool bIsLocked, const int32 NewNumHorizontalTargets)
{
	NumHorizontalTargets = NewNumHorizontalTargets;
	bNumHorizontalTargetsLocked = bIsLocked;
	CheckBeatGridConstraints();
}

void UConstrainedSlider_BeatGrid::OnBeatGridUpdate_NumVerticalTargets(const bool bIsLocked, const int32 NewNumVerticalTargets)
{
	NumVerticalTargets = NewNumVerticalTargets;
	bNumVerticalTargetsLocked = bIsLocked;
	CheckBeatGridConstraints();
}

void UConstrainedSlider_BeatGrid::OnBeatGridUpdate_MaxTargetScale(const float NewMaxTargetScale)
{
	MaxTargetSize = NewMaxTargetScale * Constants::SphereDiameter;
	CheckBeatGridConstraints();
}

void UConstrainedSlider_BeatGrid::OnCheckStateChanged_MinLock(const bool bIsLocked)
{
	Super::OnCheckStateChanged_MinLock(bIsLocked);
	bHorizontalSpacingLocked = bIsLocked;
}

void UConstrainedSlider_BeatGrid::OnCheckStateChanged_MaxLock(const bool bIsLocked)
{
	Super::OnCheckStateChanged_MaxLock(bIsLocked);
	bVerticalSpacingLocked = bIsLocked;
}

void UConstrainedSlider_BeatGrid::OnHorizontalSpacingChanged(const float NewHorizontalSpacing)
{
	HorizontalSpacing = NewHorizontalSpacing;
	CheckBeatGridConstraints();
}

void UConstrainedSlider_BeatGrid::OnVerticalSpacingChanged(const float NewVerticalSpacing)
{
	VerticalSpacing = NewVerticalSpacing;
	CheckBeatGridConstraints();
}

void UConstrainedSlider_BeatGrid::CheckBeatGridConstraints() const
{
	FBeatGridConstraints BeatGridConstraints;
	TArray<FText> SuggestionValueArray;
	SuggestionValueArray.Init(FText(), 5);
	
	if (NumHorizontalTargets > GetMaxAllowedNumHorizontalTargets())
	{
		BeatGridConstraints.ConstraintTypes.Add(EBeatGridConstraintType::NumHorizontalTargets);
		const int32 Value = GetMaxAllowedNumHorizontalTargets();
		if (Value >= Constants::MinValue_NumBeatGridHorizontalTargets && Value <= Constants::MaxValue_NumBeatGridHorizontalTargets)
		{
			SuggestionValueArray[0] = FText::Join(SpaceDelimit, HorizontalSuggest, FText::AsNumber(Value));
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("Over Threshold for NumHorizontalTargets at:"));
			UE_LOG(LogTemp, Display, TEXT("NumHorizontalTargets %d NumVerticalTargets %d TargetScale: %f HorizontalSpacing: %f VerticalSpacing: %f"),
				NumHorizontalTargets, NumVerticalTargets, MaxTargetSize / Constants::SphereDiameter, HorizontalSpacing, VerticalSpacing);
		}
	}
	if (NumVerticalTargets > GetMaxAllowedNumVerticalTargets())
	{
		BeatGridConstraints.ConstraintTypes.Add(EBeatGridConstraintType::NumVerticalTargets);
		const int32 Value = GetMaxAllowedNumVerticalTargets();
		if (Value >= Constants::MinValue_NumBeatGridVerticalTargets && Value <= Constants::MaxValue_NumBeatGridVerticalTargets)
		{
			SuggestionValueArray[1] = FText::Join(SpaceDelimit, VerticalSuggest,  FText::AsNumber(Value));
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("Over Threshold for NumVerticalTargets at:"));
			UE_LOG(LogTemp, Display, TEXT("NumHorizontalTargets %d NumVerticalTargets %d TargetScale: %f HorizontalSpacing: %f VerticalSpacing: %f"),
				NumHorizontalTargets, NumVerticalTargets, MaxTargetSize / Constants::SphereDiameter, HorizontalSpacing, VerticalSpacing);
		}
	}
	if (MaxTargetSize / Constants::SphereDiameter > GetMaxAllowedTargetScale())
	{
		BeatGridConstraints.ConstraintTypes.Add(EBeatGridConstraintType::TargetScale);
		const float Value = floorf(GetMaxAllowedTargetScale() * 100.f) / 100.f;
		if (Value >= Constants::MinValue_TargetScale && Value <= Constants::MaxValue_TargetScale)
		{
			SuggestionValueArray[2] = FText::Join(SpaceDelimit, ScaleSuggest, FText::AsNumber(Value));
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("Over Threshold for TargetScale at:"));
			UE_LOG(LogTemp, Display, TEXT("NumHorizontalTargets %d NumVerticalTargets %d TargetScale: %f HorizontalSpacing: %f VerticalSpacing: %f"),
				NumHorizontalTargets, NumVerticalTargets, MaxTargetSize / Constants::SphereDiameter, HorizontalSpacing, VerticalSpacing);
		}
	}
	if (HorizontalSpacing > GetMaxAllowedHorizontalSpacing())
	{
		BeatGridConstraints.ConstraintTypes.Add(EBeatGridConstraintType::HorizontalSpacing);
		const float Value = floorf(GetMaxAllowedHorizontalSpacing() * 100.f) / 100.f;
		if (Value >= Constants::MinValue_BeatGridHorizontalSpacing && Value <= Constants::MaxValue_BeatGridHorizontalSpacing)
		{
			SuggestionValueArray[3] = FText::Join(SpaceDelimit, HorizontalSpacingSuggest, FText::AsNumber(Value));
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("Over Threshold for HorizontalSpacing at:"));
			UE_LOG(LogTemp, Display, TEXT("NumHorizontalTargets %d NumVerticalTargets %d TargetScale: %f HorizontalSpacing: %f VerticalSpacing: %f"),
				NumHorizontalTargets, NumVerticalTargets, MaxTargetSize / Constants::SphereDiameter, HorizontalSpacing, VerticalSpacing);
		}
	}
	if (VerticalSpacing > GetMaxAllowedVerticalSpacing())
	{
		BeatGridConstraints.ConstraintTypes.Add(EBeatGridConstraintType::VerticalSpacing);
		const float Value = floorf(GetMaxAllowedVerticalSpacing() * 100.f) / 100.f;
		if (Value >= Constants::MinValue_BeatGridVerticalSpacing && Value <= Constants::MaxValue_BeatGridVerticalSpacing)
		{
			SuggestionValueArray[4] = FText::Join(SpaceDelimit, VerticalSpacingSuggest, FText::AsNumber(Value));
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("Over Threshold for VerticalSpacing at:"));
			UE_LOG(LogTemp, Display, TEXT("NumHorizontalTargets %d NumVerticalTargets %d TargetScale: %f HorizontalSpacing: %f VerticalSpacing: %f"),
				NumHorizontalTargets, NumVerticalTargets, MaxTargetSize / Constants::SphereDiameter, HorizontalSpacing, VerticalSpacing);
		}
	}
	
	for (const EBeatGridConstraintType ConstraintType : BeatGridConstraints.ConstraintTypes)
	{
		FText JoinedValues;
		switch (ConstraintType) {
		case EBeatGridConstraintType::NumHorizontalTargets:
			JoinedValues = FText::Join(NewLineDelimit, FilterTooltipText(SuggestionValueArray, 0));
			BeatGridConstraints.Tooltip_NumHorizontalTargets = FText::Join(NewLineDelimit, SuggestStart, JoinedValues);
			break;
		case EBeatGridConstraintType::NumVerticalTargets:
			JoinedValues = FText::Join(NewLineDelimit, FilterTooltipText(SuggestionValueArray, 1));
			BeatGridConstraints.Tooltip_NumVerticalTargets = FText::Join(NewLineDelimit, SuggestStart, JoinedValues);
			break;
		case EBeatGridConstraintType::TargetScale:
			JoinedValues = FText::Join(NewLineDelimit, FilterTooltipText(SuggestionValueArray, 2));
			BeatGridConstraints.Tooltip_TargetScale = FText::Join(NewLineDelimit, SuggestStart, JoinedValues);
			break;
		case EBeatGridConstraintType::HorizontalSpacing:
			JoinedValues = FText::Join(NewLineDelimit, FilterTooltipText(SuggestionValueArray, 3));
			BeatGridConstraints.Tooltip_HorizontalSpacing = FText::Join(NewLineDelimit, SuggestStart, JoinedValues);
			break;
		case EBeatGridConstraintType::VerticalSpacing:
			JoinedValues = FText::Join(NewLineDelimit, FilterTooltipText(SuggestionValueArray, 4));
			BeatGridConstraints.Tooltip_VerticalSpacing = FText::Join(NewLineDelimit, SuggestStart, JoinedValues);
			break;
		}
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
	return HorizontalSpacing * (NumHorizontalTargets - 1) + TotalTargetWidth;
}

float UConstrainedSlider_BeatGrid::GetMinRequiredVerticalSpread() const
{
	const float TotalTargetHeight = MaxTargetSize * NumVerticalTargets;
	return VerticalSpacing * (NumVerticalTargets - 1) + TotalTargetHeight;
}

float UConstrainedSlider_BeatGrid::GetMaxAllowedTargetScale() const
{
	// HorizontalSpread = TargetScale * SphereDiameter * NumHorizontalTargets + HorizontalSpacing * (NumHorizontalTargets - 1)
	// HorizontalSpread - HorizontalSpacing * (NumHorizontalTargets - 1)= TargetScale * SphereDiameter * NumHorizontalTargets 
	// HorizontalSpread - HorizontalSpacing * NumHorizontalTargets + HorizontalSpacing = TargetScale * SphereDiameter * NumHorizontalTargets 
	// (HorizontalSpread - HorizontalSpacing * NumHorizontalTargets + HorizontalSpacing) / (SphereDiameter * NumHorizontalTargets) = TargetScale
	const float MaxAllowedHorizontal = (HorizontalSpread - HorizontalSpacing * NumHorizontalTargets + HorizontalSpacing) / (Constants::SphereDiameter * NumHorizontalTargets);
	const float MaxAllowedVertical = (VerticalSpread - VerticalSpacing * NumVerticalTargets + VerticalSpacing) / (Constants::SphereDiameter * NumVerticalTargets);
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
	return (HorizontalSpread + HorizontalSpacing) / (MaxTargetSize + HorizontalSpacing);
}

int32 UConstrainedSlider_BeatGrid::GetMaxAllowedNumVerticalTargets() const
{
	return (VerticalSpread + VerticalSpacing) / (MaxTargetSize + VerticalSpacing);
}

TArray<FText> UConstrainedSlider_BeatGrid::FilterTooltipText(const TArray<FText>& TooltipTextArray, const int32 Index)
{
	const TArray<FText> NonEmptyTextEntries = TooltipTextArray.FilterByPredicate([] (const FText& Text)
	{
		if (Text.IsEmptyOrWhitespace())
		{
			return false;
		}
		return true;
	});
	const TArray<FText> ExcludingIndexEntries = NonEmptyTextEntries.FilterByPredicate([&TooltipTextArray, &Index] (const FText& Text)
	{
		if (Text.EqualTo(TooltipTextArray[Index]))
		{
			return false;
		}
		return true;
	});

	if (!ExcludingIndexEntries.IsEmpty())
	{
		return ExcludingIndexEntries;
	}
	if (!NonEmptyTextEntries.IsEmpty())
	{
		return NonEmptyTextEntries;
	}
	TArray<FText> LastResortArray = TArray<FText>();
	LastResortArray.Add(FText::FromString("- Any parameters with a warning beside them"));
	return LastResortArray;
}
