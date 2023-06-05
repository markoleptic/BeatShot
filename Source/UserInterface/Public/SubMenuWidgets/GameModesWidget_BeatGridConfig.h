// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetComponents/DoubleSyncedSliderAndTextBox.h"
#include "GlobalStructs.h"
#include "WidgetComponents/BSSettingCategoryWidget.h"
#include "GameModesWidget_BeatGridConfig.generated.h"

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

DECLARE_MULTICAST_DELEGATE(FOnBeatGridUpdate_SaveStartButtonStates);

class UTooltipWidget;
class UBSHorizontalBox;
class UCheckBox;
class UEditableTextBox;
class USlider;
class UHorizontalBox;

/** SettingCategoryWidget for the GameModesWidget that holds BeatGrid configuration settings */
UCLASS()
class USERINTERFACE_API UGameModesWidget_BeatGridConfig : public UBSSettingCategoryWidget
{
	GENERATED_BODY()
	
	friend class UGameModesWidget;
	virtual void NativeConstruct() override;
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void InitSettingCategoryWidget() override;

public:
	void InitializeBeatGrid(const FBS_GridConfig& InBeatGridConfig, const UHorizontalBox* InTargetScaleBox);
	FBS_GridConfig GetBeatGridConfig() const;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Classes | Tooltip")
	TSubclassOf<UTooltipImage> BeatGridWarningEMarkClass;
	UPROPERTY(EditDefaultsOnly, Category = "Classes | Target Spread")
	TSubclassOf<UDoubleSyncedSliderAndTextBox> BeatGridSpreadConstrainedClass;
	
	TSoftObjectPtr<UHorizontalBox> TargetScaleBox;
	TSoftObjectPtr<UDoubleSyncedSliderAndTextBox> BeatGridSpreadConstrained;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	UBSHorizontalBox* BSBox_RandomizeNextTarget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	UBSHorizontalBox* BSBox_NumHorizontalTargets;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	UBSHorizontalBox* BSBox_NumVerticalTargets;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	UCheckBox* CheckBox_RandomizeNextTarget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	UCheckBox* CheckBox_NumHorizontalTargetsLock;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	UCheckBox* CheckBox_NumVerticalTargetsLock;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	USlider* Slider_NumHorizontalTargets;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	USlider* Slider_NumVerticalTargets;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	UEditableTextBox* Value_NumHorizontalTargets;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	UEditableTextBox* Value_NumVerticalTargets;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	UHorizontalBox* Box_NumHorizontalTargetsTextTooltip;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	UHorizontalBox* Box_NumVerticalTargetsTextTooltip;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* QMark_BeatGridAdjacentOnly;
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
	UFUNCTION()
	void OnCheckStateChanged_MinLock(const bool bIsLocked);
	UFUNCTION()
	void OnCheckStateChanged_MaxLock(const bool bIsLocked);
	UFUNCTION()
	void OnBeatGridUpdate_MaxTargetScale(const float NewMaxTargetScale);

	/** Checks all constraints, adjusting values if allowed to, and constructs the tooltip text for all constraints if needed.
	 *  Also calls UpdateAllWarningTooltips in parent widget by executing delegate */
	void CheckBeatGridConstraints(const EBeatGridConstraintType LastChangedConstraint);
	
	/** Constructs and returns BeatGrid warning Tooltip widget, places inside corresponding HorizontalBox, and binds the mouseover event */
	UTooltipImage* ConstructOrEditBeatGridWarningEMarkWidget(UTooltipImage* TooltipImage, const EBeatGridConstraintType ConstraintType);

	/** Shows or hides a BeatGrid warning Tooltip widget and updates the TooltipText. Constructs the widget if it does not exist */
	void HandleBeatGridWarningDisplay(UTooltipImage* TooltipImage, const EBeatGridConstraintType ConstraintType, const FText& TooltipText, const bool bDisplay);

	/** Calls HandleBeatGridWarningDisplay for each parameter that should receive a tooltip */
	void UpdateAllWarningTooltips(const FBeatGridConstraints& BeatGridConstraints);
	
	/** Returns a copy of a given tooltip text array without any empty text entries and optionally without the specific index's own text entry.
	 *  Will return a last resort text if there are no applicable parameters */
	static TArray<FText> FilterTooltipText(const TArray<FText>& TooltipTextArray, const int32 Index, const bool bShowSelfOnTooltip);

	int32 GetMaxAllowedNumHorizontalTargets() const;
	int32 GetMaxAllowedNumVerticalTargets() const;
	float GetMaxAllowedTargetScale() const;
	float GetMaxAllowedHorizontalSpacing() const;
	float GetMaxAllowedVerticalSpacing() const;
	float GetMinRequiredHorizontalSpread() const;
	float GetMinRequiredVerticalSpread() const;

	/** Stores the current slider values along with the HorizontalSpread & HorizontalSpread */
	FBS_GridConfig CurrentValues;

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
	
	const FText SuggestStart = GetTooltipTextFromKey("Grid_SuggestStart");
	const FText HorizontalSuggest = GetTooltipTextFromKey("Grid_NumHorizontalSuggest");
	const FText VerticalSuggest = GetTooltipTextFromKey("Grid_NumVerticalSuggest");
	const FText ScaleSuggest = GetTooltipTextFromKey("Grid_ScaleSuggest");
	const FText HorizontalSpacingSuggest = GetTooltipTextFromKey("Grid_HorizontalSpacingSuggest");
	const FText VerticalSpacingSuggest = GetTooltipTextFromKey("Grid_VerticalSpacingSuggest");
};
