// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MenuOptionWidget.h"
#include "ConstantMinMaxMenuOptionWidget.generated.h"

class UCommonTextBlock;

UENUM(BlueprintType)
enum class EConstantMinMaxType : uint8
{
	Constant UMETA(DisplayName="Constant"),
	Min UMETA(DisplayName="Min"),
	Max UMETA(DisplayName="Max"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(EConstantMinMaxType, EConstantMinMaxType::Constant,
	EConstantMinMaxType::Max);

class UConstantMinMaxMenuOptionWidget;
DECLARE_MULTICAST_DELEGATE_FourParams(FOnMinMaxMenuOptionChanged, UConstantMinMaxMenuOptionWidget* Widget,
	const bool bChecked, const float MinOrConstantValue, const float MaxValue);

UCLASS()
class USERINTERFACE_API UConstantMinMaxMenuOptionWidget : public UMenuOptionWidget
{
	GENERATED_BODY()

public:
	virtual void SetMenuOptionEnabledState(const EMenuOptionEnabledState EnabledState) override;
	
	/** Returns the value for the MinOrConstant slider */
	float GetMinOrConstantSliderValue() const;

	/** Returns the value for the Max slider, NOT the max value of a slider */
	float GetMaxSliderValue() const;

	/** Returns the value for the MinOrConstant EditableTextBox */
	float GetMinOrConstantEditableTextBoxValue() const;

	/** Returns the value for the Max EditableTextBox */
	float GetMaxEditableTextBoxValue() const;

	/** Returns true if the CheckBox is checked */
	bool GetIsChecked() const;
	
	/** Sets the Min and Max values of the sliders and sets the grid snap size */
	void SetValues(const float Min, const float Max, const float SnapSize);

	/** Sets the value for the ConstantOrMin slider & EditableTextBox */
	void SetValue_ConstantOrMin(const float Value) const;

	/** Sets the value for the Max slider & EditableTextBox */
	void SetValue_Max(const float Value) const;

	/** Sets the checked state for the Checkbox and calls UpdateMinMaxDependencies to update visibility stuff */
	void SetIsChecked(const bool bIsChecked) const;
	
	/** Locks or unlocks the slider and sets the EditableTextBox to read-only or not */
	void SetSliderAndTextBoxEnabledStates(const bool bEnabled) const;

	/** Broadcast when the slider value changes or the EditableTextBox has text committed to it */
	FOnMinMaxMenuOptionChanged OnMinMaxMenuOptionChanged;

protected:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	virtual void SetStyling() override;
		
	/** If true, collapses BSBox_Max and MinText. Otherwise sets them visible and sets the MinText */
	void UpdateMinMaxDependencies(const bool bConstant) const;

	UFUNCTION()
	void OnSliderChanged_ConstantOrMin(const float Value);
	UFUNCTION()
	void OnSliderChanged_Max(const float Value);
	UFUNCTION()
	void OnTextCommitted_ConstantOrMin(const FText& Text, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnTextCommitted_Max(const FText& Text, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnCheckStateChanged_CheckBox(const bool bChecked);
	
	float GridSnapSize = 1.f;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_ConstantOrMin;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* Box_Left_ConstantOrMin;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* Box_Right_ConstantOrMin;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USpacer* Indent_Left_ConstantOrMin;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonTextBlock* TextBlock_Description_ConstantOrMin;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonTextBlock* TextBlock_ConstantOrMin;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_ConstantOrMin;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* EditableTextBox_ConstantOrMin;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_Max;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* Box_Left_Max;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* Box_Right_Max;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USpacer* Indent_Left_Max;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonTextBlock* TextBlock_Description_Max;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonTextBlock* TextBlock_Max;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_Max;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* EditableTextBox_Max;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonTextBlock* TextBlock_CheckBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBox* CheckBox;

	UPROPERTY(EditInstanceOnly, Category = "ConstantMinMaxMenuOptionWidget")
	FText MinText = FText::FromString("Min");

	UPROPERTY(EditInstanceOnly, Category = "ConstantMinMaxMenuOptionWidget")
	FText MaxText = FText::FromString("Max");
};
