// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TooltipImage.h"
#include "Blueprint/UserWidget.h"
#include "Delegates/DelegateCombinations.h"
#include "ConstrainedSlider.generated.h"

class UButton;
class UCheckBox;
class USlider;
class UTextBlock;
class UEditableTextBox;

struct FConstrainedSliderStruct
{
	FText CheckboxText;
	FText MinText;
	FText MaxText;
	float MinConstraintLower;
	float MinConstraintUpper;
	float MaxConstraintLower;
	float MaxConstraintUpper;
	float DefaultMinValue;
	float DefaultMaxValue;
	bool bSyncSlidersAndValues;
	float GridSnapSize;
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnMinValueChanged, float, MinValue);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnMaxValueChanged, float, MaxValue);

UCLASS()
class BEATSHOT_API UConstrainedSlider : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:
	
	/** Initializes the Sliders and EditableTextBoxes' with text and values */
	void InitConstrainedSlider(const FConstrainedSliderStruct InStruct);
	/** Updates the Slider Values by calling OnMinSliderChanged and OnMaxSliderChanged, and updates the Checkbox checked state if necessary */
	void UpdateDefaultValues(const float NewMinValue, const float NewMaxValue);
	/** Executed when MinSlider or MinValue is changed */
	UPROPERTY()
	FOnMinValueChanged OnMinValueChanged;
	/** Executed when MaxSlider or MaxValue is changed */
	UPROPERTY()
	FOnMaxValueChanged OnMaxValueChanged;
	/** The Tooltip image for the Checkbox. The parent widget will bind to this widget's OnTooltipImageHovered delegate to display tooltip information */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* CheckboxQMark;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* MinSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* MaxSlider;
	
protected:
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* CheckboxText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* MinText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* MaxText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBox* Checkbox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* MinValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* MaxValue;

	/** Updates the Checkbox checked state, and calls the appropriate OnSliderChanged functions to update the values of the Sliders and EditableTextBoxes */
	UFUNCTION()
	void OnCheckStateChanged(const bool bIsChecked);
	/** Checks the constraints for the MinSlider, changes the MinValue text, and executes OnMinValueChanged. Changes MaxSlider, MaxValue, and calls OnMaxValueChanged if bSyncSlidersAndValues is true */
	UFUNCTION()
	void OnMinSliderChanged(const float NewMin);
	/** Checks the constraints for the MaxSlider, changes the MaxValue text, and executes OnMaxValueChanged. Changes MinSlider, MinValue, and calls OnMinValueChanged if bSyncSlidersAndValues is true */
	UFUNCTION()
	void OnMaxSliderChanged(const float NewMax);
	/** Checks the constraints for the MinValue, changes the value for MinSlider, and calls MinSliderChanged */
	UFUNCTION()
	void OnMinValueCommitted(const FText& NewMin, ETextCommit::Type CommitType);
	/** Checks the constraints for the MaxValue, changes the value for MaxSlider, and calls MaxSliderChanged */
	UFUNCTION()
	void OnMaxValueCommitted(const FText& NewMax, ETextCommit::Type CommitType);
	
	/** Struct containing information for the Sliders and EditableTextBoxes */
	FConstrainedSliderStruct SliderStruct;
	/** Returns the rounded value of ValueToRound according to the GridSnapSize */
	float RoundValue(const float ValueToRound) const;
	/** Clamps the NewValue to the appropriate Slider Min and Max Values, and returns the new rounded value */
	float CheckConstraints(const float NewValue, const bool bIsMin) const;

};


