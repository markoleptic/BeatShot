// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TooltipImage.h"
#include "Blueprint/UserWidget.h"
#include "ConstrainedSlider.generated.h"

class UHorizontalBox;
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
	bool bShowMinLock;
	bool bShowMaxLock;
	
	FConstrainedSliderStruct()
	{
		CheckboxText = FText();
		MinText = FText();
		MaxText = FText();
		MinConstraintLower = 0.f;
		MinConstraintUpper = 0.f;
		MaxConstraintLower = 0.f;
		MaxConstraintUpper = 0.f;
		DefaultMinValue = 0.f;
		DefaultMaxValue = 0.f;
		bSyncSlidersAndValues = false;
		GridSnapSize = 0.f;
		bShowMinLock = false;
		bShowMaxLock = false;
	}
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMinValueChanged, float MinValue);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMaxValueChanged, float MaxValue);

UCLASS()
class USERINTERFACE_API UConstrainedSlider : public UUserWidget
{
	friend class UGameModesWidget;
	
	GENERATED_BODY()
	
protected:
	/** The Tooltip image for the Checkbox. The parent widget will bind to this widget's OnTooltipImageHoveredLocal delegate to display tooltip information */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* CheckboxQMark;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* MinSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* MaxSlider;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* TextTooltipBox_Min;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* TextTooltipBox_Max;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBox* MinLock;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBox* MaxLock;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* MinValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* MaxValue;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* CheckboxText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* MinText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* MaxText;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBox* Checkbox;
	
	virtual void NativeConstruct() override;
	
	/** Initializes the Sliders and EditableTextBoxes' with text and values */
	virtual void InitConstrainedSlider(const FConstrainedSliderStruct& InStruct);
	
	/** Updates the Slider Values by calling OnSliderChanged_Min and OnSliderChanged_Max, and updates the Checkbox checked state if necessary */
	void UpdateDefaultValues(const float NewMinValue, const float NewMaxValue);
	
	/** Executed when MinSlider or MinValue is changed */
	FOnMinValueChanged OnMinValueChanged;
	
	/** Executed when MaxSlider or MaxValue is changed */
	FOnMaxValueChanged OnMaxValueChanged;
	
	/** Updates the Checkbox checked state, and calls the appropriate OnSliderChanged functions to update the values of the Sliders and EditableTextBoxes */
	UFUNCTION()
	void OnCheckStateChanged(const bool bIsChecked);

	UFUNCTION()
	virtual void OnCheckStateChanged_MinLock(const bool bIsLocked);

	UFUNCTION()
	virtual void OnCheckStateChanged_MaxLock(const bool bIsLocked);
	
	/** Checks the constraints for the MinSlider, changes the MinValue text, and executes OnMinValueChanged. Changes MaxSlider, MaxValue, and calls OnMaxValueChanged if bSyncSlidersAndValues is true */
	UFUNCTION()
	void OnSliderChanged_Min(const float NewMin);
	
	/** Checks the constraints for the MaxSlider, changes the MaxValue text, and executes OnMaxValueChanged. Changes MinSlider, MinValue, and calls OnMinValueChanged if bSyncSlidersAndValues is true */
	UFUNCTION()
	void OnSliderChanged_Max(const float NewMax);
	
	/** Checks the constraints for the MinValue, changes the value for MinSlider, and calls MinSliderChanged */
	UFUNCTION()
	void OnTextCommitted_Min(const FText& NewMin, ETextCommit::Type CommitType);
	
	/** Checks the constraints for the MaxValue, changes the value for MaxSlider, and calls MaxSliderChanged */
	UFUNCTION()
	void OnTextCommitted_Max(const FText& NewMax, ETextCommit::Type CommitType);

	/** Struct containing information for the Sliders and EditableTextBoxes */
	FConstrainedSliderStruct SliderStruct;
	
	/** Returns the rounded value of ValueToRound according to the GridSnapSize */
	float RoundValue(const float ValueToRound) const;
	
	/** Clamps the NewValue to the appropriate Slider Min and Max Values, and returns the new rounded value */
	virtual float CheckConstraints(const float NewValue, const bool bIsMin);

	/** Overrides a slider's max value */
	void OverrideMaxValue(const bool bIsMin, const float ValueToOverride);
};
