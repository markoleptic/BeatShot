// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ConstrainedSlider.h"
#include "ConstrainedSlider_BeatGrid.generated.h"

USTRUCT(BlueprintType)
struct FBeatGridConstraints
{
	GENERATED_BODY()
	
	int32 MaxAllowedNumHorizontalTargets;
	int32 MaxAllowedNumVerticalTargets;
	float MinRequiredHorizontalSpread;
	float MinRequiredVerticalSpread;
	float MaxAllowedTargetScale;
	
	FBeatGridConstraints()
	{
		MaxAllowedNumHorizontalTargets = -1;
		MaxAllowedNumVerticalTargets = -1;
		MinRequiredHorizontalSpread = -1;
		MinRequiredVerticalSpread = -1;
		MaxAllowedTargetScale = -1;
	}
};

DECLARE_DELEGATE_OneParam(FBeatGridUpdate, const float NewValue);
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
	void UpdateBeatGridConstraints(const int32 NewNumHorizontalTargets, const int32 NewNumVerticalTargets, const float NewHorizontalSpread, const float NewVerticalSpread, const float NewMaxTargetScale);
	UFUNCTION()
	void OnBeatGridUpdate_NumVerticalTargets(const int32 NewNumVerticalTargets);
	UFUNCTION()
	void OnBeatGridUpdate_NumHorizontalTargets(const int32 NewNumHorizontalTargets);
	UFUNCTION()
	void OnBeatGridUpdate_HorizontalSpread(const float NewHorizontalSpread);
	UFUNCTION()
	void OnBeatGridUpdate_VerticalSpread(const float NewVerticalSpread);
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
	float HorizontalSpread;
	float VerticalSpread;
	float MaxTargetSize;
	
	/** The diameter of a target */
	const float SphereDiameter = 100.f;
};
