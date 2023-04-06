// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ConstrainedSlider.h"
#include "GlobalStructs.h"
#include "TooltipInterface.h"
#include "BeatGridSettingsWidget.generated.h"

/** Enum representing the different BeatGrid constraints */
UENUM(BlueprintType)
enum class EBeatGridConstraintType : uint8
{
	None					UMETA(DisplayName="None"),
	NumHorizontalTargets	UMETA(DisplayName="NumHorizontalTargets"),
	NumVerticalTargets		UMETA(DisplayName="NumVerticalTargets"),
	TargetScale				UMETA(DisplayName="TargetScale"),
	HorizontalSpacing		UMETA(DisplayName="HorizontalSpacing"),
	VerticalSpacing			UMETA(DisplayName="VerticalSpacing")
};
ENUM_RANGE_BY_FIRST_AND_LAST(EBeatGridConstraintType, EBeatGridConstraintType::None, EBeatGridConstraintType::VerticalSpacing);

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

DECLARE_DELEGATE(FOnBeatGridUpdate_SaveStartButtonStates);

class UTooltipWidget;
class UCheckBox;
class UEditableTextBox;
class USlider;
class UHorizontalBox;

UCLASS()
class USERINTERFACE_API UBeatGridSettingsWidget : public UConstrainedSlider, public ITooltipInterface
{
	GENERATED_BODY()

	virtual UTooltipWidget* ConstructTooltipWidget() override;

	friend class UGameModesWidget;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Classes | Tooltip")
	TSubclassOf<UTooltipWidget> TooltipWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Classes | Tooltip")
	TSubclassOf<UTooltipImage> BeatGridWarningEMarkClass;
	UPROPERTY()
	TSoftObjectPtr<UHorizontalBox> TargetScaleBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	UCheckBox* CheckBox_RandomizeNextTarget;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	USlider* Slider_NumHorizontalTargets;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	UEditableTextBox* Value_NumHorizontalTargets;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	UCheckBox* CheckBox_NumHorizontalTargetsLock;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	USlider* Slider_NumVerticalTargets;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	UEditableTextBox* Value_NumVerticalTargets;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	UCheckBox* CheckBox_NumVerticalTargetsLock;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	UHorizontalBox* Box_NumHorizontalTargetsTextTooltip;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	UHorizontalBox* Box_NumVerticalTargetsTextTooltip;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* BeatGridAdjacentOnlyQMark;
	UPROPERTY(BlueprintReadOnly, Category = "Tooltip")
	UTooltipImage* WarningEMark_NumHorizontalTargets;
	UPROPERTY(BlueprintReadOnly, Category = "Tooltip")
	UTooltipImage* WarningEMark_NumVerticalTargets;
	UPROPERTY(BlueprintReadOnly, Category = "Tooltip")
	UTooltipImage* WarningEMark_MaxTargetScale;
	UPROPERTY(BlueprintReadOnly, Category = "Tooltip")
	UTooltipImage* WarningEMark_HorizontalSpacing;
	UPROPERTY(BlueprintReadOnly, Category = "Tooltip")
	UTooltipImage* WarningEMark_VerticalSpacing;

public:
	bool IsAnyParameterConstrained() const { return !ActiveConstraints.IsEmpty(); }
	
	/** Executed in CheckBeatGridConstraints */
	FOnBeatGridUpdate_SaveStartButtonStates OnBeatGridUpdate_SaveStartButtonStates;

private:
	virtual void NativeConstruct() override;
	void InitializeBeatGrid(const FBeatGridConfig& InBeatGridConfig, const UHorizontalBox* InTargetScaleBox);
	FBeatGridConfig GetBeatGridConfig() const;
	
	UFUNCTION()
	void OnConstrainedChanged_HorizontalSpacing(const float NewHorizontalSpacing);
	UFUNCTION()
	void OnConstrainedChanged_VerticalSpacing(const float NewVerticalSpacing);
	UFUNCTION()
	void OnSliderChanged_BeatGridNumHorizontalTargets(const float NewNumHorizontalTargets);
	UFUNCTION()
	void OnSliderChanged_BeatGridNumVerticalTargets(const float NewNumVerticalTargets);
	UFUNCTION()
	void OnTextCommitted_BeatGridNumHorizontalTargets(const FText& NewNumHorizontalTargets, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnTextCommitted_BeatGridNumVerticalTargets(const FText& NewNumVerticalTargets, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnCheckStateChanged_BeatGridNumHorizontalTargetsLock(const bool bIsLocked);
	UFUNCTION()
	void OnCheckStateChanged_BeatGridNumVerticalTargetsLock(const bool bIsLocked);
	UFUNCTION()
	void OnCheckStateChanged_RandomizeNextTarget(const bool bIsChecked);
	virtual void OnCheckStateChanged_MinLock(const bool bIsLocked) override;
	virtual void OnCheckStateChanged_MaxLock(const bool bIsLocked) override;
	UFUNCTION()
	void OnBeatGridUpdate_MaxTargetScale(const float NewMaxTargetScale);

	/** Checks all constraints, adjusting values if allowed to, and constructs the tooltip text for all constraints if needed.
	 *  Also calls UpdateAllWarningTooltips in parent widget by executing delegate */
	void CheckBeatGridConstraints(const EBeatGridConstraintType LastChangedConstraint);
	
	/** Constructs and returns BeatGrid warning Tooltip widget, places inside corresponding HorizontalBox, and binds the mouseover event */
	UTooltipImage* ConstructBeatGridWarningEMarkWidget(const EBeatGridConstraintType ConstraintType);

	/** Shows or hides a BeatGrid warning Tooltip widget and updates the TooltipText. Constructs the widget if it does not exist */
	void HandleBeatGridWarningDisplay(UTooltipImage* TooltipImage, const EBeatGridConstraintType ConstraintType, const FText& TooltipText, const bool bDisplay);

	/** Calls HandleBeatGridWarningDisplay for each parameter that should receive a tooltip */
	void UpdateAllWarningTooltips(const FBeatGridConstraints& BeatGridConstraints);
	
	/** Returns a copy of a given tooltip text array without any empty text entries and optionally without the specific index's own text entry.
	 *  Will return a last resort text if there are no applicable parameters */
	static TArray<FText> FilterTooltipText(const TArray<FText>& TooltipTextArray, const int32 Index, const bool bShowSelfOnTooltip);
	
	/** Clamps NewTextValue, updates associated Slider value while rounding to the GridSnapSize */
	float OnEditableTextBoxChanged(const FText& NewTextValue, UEditableTextBox* TextBoxToChange, USlider* SliderToChange, const float GridSnapSize, const float Min, const float Max) const;

	/** Updates associated TextBoxToChange with result of rounding to the GridSnapSize */
	float OnSliderChanged(const float NewValue, UEditableTextBox* TextBoxToChange, const float GridSnapSize) const;

	int32 GetMaxAllowedNumHorizontalTargets() const;
	int32 GetMaxAllowedNumVerticalTargets() const;
	float GetMaxAllowedTargetScale() const;
	float GetMaxAllowedHorizontalSpacing() const;
	float GetMaxAllowedVerticalSpacing() const;
	float GetMinRequiredHorizontalSpread() const;
	float GetMinRequiredVerticalSpread() const;

	/** Stores the current slider values along with the HorizontalSpread & HorizontalSpread */
	FBeatGridConfig CurrentValues;

	/** Stores any actively constraining BeatGrid parameters */
	TArray<EBeatGridConstraintType> ActiveConstraints;

	/** The max target size, update each time it is changed in parent widget */
	float MaxTargetSize;

	/** Width of spawn area, which is StaticHorizontalSpread - MaxTargetSize since targets are allowed to spawn with their center on the edge */
	float HorizontalSpread = 3200.f;
	/** Height of spawn area, which is StaticVerticalSpread - MaxTargetSize since targets are allowed to spawn with their center on the edge */
	float VerticalSpread = 1000.f;

	bool bNumHorizontalTargetsLocked = false;
	bool bNumVerticalTargetsLocked = false;
	bool bTargetSizeLocked = false;
	bool bHorizontalSpacingLocked = false;
	bool bVerticalSpacingLocked = false;
	
	/** Sizing mode for BeatGrid Warning Tooltip widget */
	FSlateChildSize TooltipWarningSizing;

	const float StaticHorizontalSpread = 3200.f;
	const float StaticVerticalSpread = 1000.f;
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
