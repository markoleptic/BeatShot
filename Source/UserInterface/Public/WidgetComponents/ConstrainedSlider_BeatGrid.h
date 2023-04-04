// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ConstrainedSlider.h"
#include "ConstrainedSlider_BeatGrid.generated.h"

USTRUCT(BlueprintType)
struct FBeatGridConstraints
{
	GENERATED_BODY()

	/* Max allowed NumHorizontalTargets */
	int32 NumHorizontalTargets;
	/* Max allowed NumVerticalTargets */
	int32 NumVerticalTargets;
	/* Max allowed TargetScale */
	float TargetScale;
	/* Max allowed HorizontalSpacing */
	float HorizontalSpacing;
	/* Max allowed VerticalSpacing */
	float VerticalSpacing;
	
	FBeatGridConstraints()
	{
		NumHorizontalTargets = -1;
		NumVerticalTargets = -1;
		TargetScale = -1;
		HorizontalSpacing = -1;
		VerticalSpacing = -1;
	}
};

DECLARE_DELEGATE_OneParam(FBeatGridUpdate_NumTargets, const int32 NewNumTargets);
DECLARE_DELEGATE_OneParam(FOnBeatGridSpacingConstrained, const FBeatGridConstraints& BeatGridConstraints);

class UButton;
class UCheckBox;
class USlider;
class UTextBlock;
class UEditableTextBox;
UCLASS()
class USERINTERFACE_API UConstrainedSlider_BeatGrid : public UConstrainedSlider
{
	GENERATED_BODY()

public:
	void UpdateBeatGridConstraints(const int32 NewNumHorizontalTargets, const int32 NewNumVerticalTargets, const float NewMaxTargetScale);
	UFUNCTION()
	void OnBeatGridUpdate_NumVerticalTargets(const int32 NewNumVerticalTargets);
	UFUNCTION()
	void OnBeatGridUpdate_NumHorizontalTargets(const int32 NewNumHorizontalTargets);
	UFUNCTION()
	void OnBeatGridUpdate_MaxTargetScale(const float NewMaxTargetScale);

	FOnBeatGridSpacingConstrained OnBeatGridSpacingConstrained;

protected:
	void CheckBeatGridConstraints();
	float GetMaxAllowedHorizontalSpacing() const;
	float GetMaxAllowedVerticalSpacing() const;
	float GetMinRequiredHorizontalSpread() const;
	float GetMinRequiredVerticalSpread() const;
	float GetMaxAllowedTargetScale() const;
	int32 GetMaxAllowedNumHorizontalTargets() const;
	int32 GetMaxAllowedNumVerticalTargets() const;
	
	int32 NumHorizontalTargets;
	int32 NumVerticalTargets;
	const float HorizontalSpread = 3200.f;
	const float VerticalSpread = 1000.f;
	float MaxTargetSize;
	
	/** The diameter of a target */
	const float SphereDiameter = 100.f;
};
