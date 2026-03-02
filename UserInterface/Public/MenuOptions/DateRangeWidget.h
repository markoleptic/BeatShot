// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MenuOptionWidget.h"
#include "DateRangeWidget.generated.h"

class UCommonTextBlock;

UCLASS()
class USERINTERFACE_API UDateRangeWidget : public UMenuOptionWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	virtual void SetStyling() override;

public:
	/** Sets the enabled state of the menu option, optionally adding a tooltip to the entire widget. By default, this
	 *  sets the enabled states of the two main horizontal boxes of a MenuOptionWidget, but subclasses may choose to
	 *  modify this behavior.
	 *  @param State the state to set the widget to.
	 *  @param TooltipText text to add to a tooltip for the entire widget (if provided and State is DependentMissing).
	 *  Otherwise, the tooltip will be cleared.
	 */
	virtual void
	SetMenuOptionEnabledState(const EMenuOptionEnabledState State, const FText& TooltipText = FText()) override;

	/** Returns the value for the MinOrConstant slider. */
	FDateTime GetMinSliderValue(const bool bClamped) const;

	/** Returns the value for the Max slider, NOT the max value of a slider. */
	FDateTime GetMaxSliderValue(const bool bClamped) const;

	TTuple<FDateTime, FDateTime> GetValues(const bool bClamped) const;

	/** Sets the Min and Max values of the sliders and sets the grid snap size. */
	void SetValues(const FDateTime& Min, const FDateTime& Max, const float SnapSize);

	/** Sets the value for the min slider & EditableTextBox. */
	void SetValue_Min(const FDateTime& Value) const;

	/** Sets the value for the max slider & EditableTextBox. */
	void SetValue_Max(const FDateTime& Value) const;

	/** Locks or unlocks the slider and sets the EditableTextBox to read-only or not. */
	void SetSliderAndTextBoxEnabledStates(const bool bEnabled) const;

	/** Returns the current snap size */
	float GetSnapSize() const { return GridSnapSize; }

	/** Broadcast when the slider value changes or the EditableTextBox has text committed to it. */
	TDelegate<void(const FDateTime& Min, const FDateTime& Max)> OnMinMaxMenuOptionChanged;

protected:
	float GetSliderValueFromDateTime(const FDateTime& DateTime) const;

	FDateTime GetDateTimeFromSliderValue(float Value) const;

	UFUNCTION()
	void OnSliderChanged_Min(const float Value);
	UFUNCTION()
	void OnSliderChanged_Max(const float Value);
	UFUNCTION()
	void OnTextCommitted_Min(const FText& Text, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnTextCommitted_Max(const FText& Text, ETextCommit::Type CommitType);

	float GridSnapSize = 1.f;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonTextBlock* TextBlock_Min;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_Min;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* EditableTextBox_Min;

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

	UPROPERTY(EditInstanceOnly, Category = "DualRangeInputWidget")
	FText MinText;

	UPROPERTY(EditInstanceOnly, Category = "DualRangeInputWidget")
	FText MaxText;

	mutable FText LastValidMinText;
	mutable FText LastValidMaxText;
	TMap<FDateTime, float> DatesToSliderValues;
};
