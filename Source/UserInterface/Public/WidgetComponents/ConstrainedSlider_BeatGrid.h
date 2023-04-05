// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ConstrainedSlider.h"
#include "ConstrainedSlider_BeatGrid.generated.h"

/** Enum representing the different BeatGrid constraints */
UENUM(BlueprintType)
enum class EBeatGridConstraintType : uint8
{
	NumHorizontalTargets	UMETA(DisplayName="NumHorizontalTargets"),
	NumVerticalTargets		UMETA(DisplayName="NumVerticalTargets"),
	TargetScale				UMETA(DisplayName="TargetScale"),
	HorizontalSpacing		UMETA(DisplayName="HorizontalSpacing"),
	VerticalSpacing			UMETA(DisplayName="VerticalSpacing")
};
ENUM_RANGE_BY_FIRST_AND_LAST(EBeatGridConstraintType, EBeatGridConstraintType::NumHorizontalTargets, EBeatGridConstraintType::VerticalSpacing);

/** Struct containing which BeatGrid parameters are constrained and the tooltip text to show for all related parameters */
USTRUCT(BlueprintType)
struct FBeatGridConstraints
{
	GENERATED_BODY()

	/* Max allowed NumHorizontalTargets */
	FText Tooltip_NumHorizontalTargets;
	/* Max allowed NumVerticalTargets */
	FText Tooltip_NumVerticalTargets;
	/* Max allowed TargetScale */
	FText Tooltip_TargetScale;
	/* Max allowed HorizontalSpacing */
	FText Tooltip_HorizontalSpacing;
	/* Max allowed VerticalSpacing */
	FText Tooltip_VerticalSpacing;

	UPROPERTY()
	TArray<EBeatGridConstraintType> ConstraintTypes;
	
	FBeatGridConstraints()
	{
		Tooltip_NumHorizontalTargets = FText();
		Tooltip_NumVerticalTargets = FText();
		Tooltip_TargetScale = FText();
		Tooltip_HorizontalSpacing = FText();
		Tooltip_VerticalSpacing = FText();
		ConstraintTypes = TArray<EBeatGridConstraintType>();
	}
};

DECLARE_DELEGATE_TwoParams(FBeatGridUpdate_NumTargets, const bool bIsLocked, const int32 NewNumTargets);
DECLARE_DELEGATE_OneParam(FOnBeatGridSpacingConstrained, const FBeatGridConstraints& BeatGridConstraints);

UCLASS()
class USERINTERFACE_API UConstrainedSlider_BeatGrid : public UConstrainedSlider
{
	GENERATED_BODY()
public:
	void UpdateBeatGridConstraints(const int32 NewNumHorizontalTargets, const int32 NewNumVerticalTargets, const float NewMaxTargetScale);
	UFUNCTION()
	void OnBeatGridUpdate_NumVerticalTargets(const bool bIsLocked, const int32 NewNumVerticalTargets);
	UFUNCTION()
	void OnBeatGridUpdate_NumHorizontalTargets(const bool bIsLocked, const int32 NewNumHorizontalTargets);
	UFUNCTION()
	void OnBeatGridUpdate_MaxTargetScale(const float NewMaxTargetScale);

	FOnBeatGridSpacingConstrained OnBeatGridSpacingConstrained;

private:
	virtual void NativeConstruct() override;
	UFUNCTION()
	void OnHorizontalSpacingChanged(const float NewHorizontalSpacing);
	UFUNCTION()
	void OnVerticalSpacingChanged(const float NewVerticalSpacing);

	virtual void OnCheckStateChanged_MinLock(const bool bIsLocked) override;
	virtual void OnCheckStateChanged_MaxLock(const bool bIsLocked) override;

	/* Checks all constraints and sets the tooltip text for all constraints. Broadcasts OnBeatGridSpacingConstrained */
	void CheckBeatGridConstraints() const;

	/* Returns a copy of the array without the corresponding index. Used to exclude the tooltip info for the corresponding index */
	static TArray<FText> FilterTooltipText(const TArray<FText>& TooltipTextArray, const int32 Index);
	
	int32 GetMaxAllowedNumHorizontalTargets() const;
	int32 GetMaxAllowedNumVerticalTargets() const;
	float GetMaxAllowedTargetScale() const;
	float GetMaxAllowedHorizontalSpacing() const;
	float GetMaxAllowedVerticalSpacing() const;
	float GetMinRequiredHorizontalSpread() const;
	float GetMinRequiredVerticalSpread() const;
	
	int32 NumHorizontalTargets;
	int32 NumVerticalTargets;
	float MaxTargetSize;
	float HorizontalSpacing;
	float VerticalSpacing;

	bool bNumHorizontalTargetsLocked = false;
	bool bNumVerticalTargetsLocked = false;
	bool bTargetSizeLocked = false;
	bool bHorizontalSpacingLocked = false;
	bool bVerticalSpacingLocked = false;

	const float StaticHorizontalSpread = 3200.f;
	const float StaticVerticalSpread = 1000.f;
	
	float HorizontalSpread = 3200.f;
	float VerticalSpread = 1000.f;

	const FText OrDelimit = FText::FromString(" or ");
	const FText NewLineDelimit = FText::FromString("\n");
	const FText SpaceDelimit = FText::FromString(" ");
	const FText SuggestStart = FText::FromString("Not enough space! Change one or more of the following:");
	const FText HorizontalSuggest = FText::FromString("- Number of Horizontal Targets <=");
	const FText VerticalSuggest = FText::FromString("- Number of Vertical Targets to <=");
	const FText ScaleSuggest = FText::FromString("- Maximum Target Scale to <=");
	const FText HorizontalSpacingSuggest = FText::FromString("- Horizontal Spacing to <=");
	const FText VerticalSpacingSuggest = FText::FromString("- Vertical Spacing to <=");
};
