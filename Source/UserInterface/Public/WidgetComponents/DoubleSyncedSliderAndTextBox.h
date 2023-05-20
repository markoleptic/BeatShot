// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TooltipImage.h"
#include "Blueprint/UserWidget.h"
#include "DoubleSyncedSliderAndTextBox.generated.h"

class UBSVerticalBox;
class UBSHorizontalBox;
class UHorizontalBox;
class UButton;
class UCheckBox;
class USlider;
class UTextBlock;
class UEditableTextBox;

struct FSyncedSlidersParams
{
	/* Text to show beside the Checkbox */
	FText CheckboxText;
	/* Text to show beside Slider_Min and Value_Min */
	FText MinText;
	/* Text to show beside Slider_Max and Value_Max */
	FText MaxText;
	/* Min value for Slider_Min and Value_Min */
	float MinConstraintLower;
	/* Min value for Slider_Max and Value_Max */
	float MinConstraintUpper;
	/* Max value for Slider_Min and Value_Min */
	float MaxConstraintLower;
	/* Max value for Slider_Max and Value_Max */
	float MaxConstraintUpper;
	/* Default value for Slider_Min and Value_Min */
	float DefaultMinValue;
	/* Default value for Slider_Max and Value_Max */
	float DefaultMaxValue;
	/* Whether or not to start synced */
	bool bSyncSlidersAndValues;
	/* Snap size for all values */
	float GridSnapSize;
	/* Whether or not to show the Checkbox */
	bool bShowCheckBox;
	/* Whether or not to show the MinLock beside Slider_Min and Value_Min */
	bool bShowMinLock;
	/* Whether or not to show the MinLock beside Slider_Max and Value_Max */
	bool bShowMaxLock;
	/* Whether or not to start with MinLock set to locked */
	bool bStartMinLocked;
	/* Whether or not to start with MaxLock set to locked */
	bool bStartMaxLocked;
	
	FSyncedSlidersParams()
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
		bShowCheckBox = true;
		bShowMinLock = false;
		bShowMaxLock = false;
		bStartMinLocked = false;
		bStartMaxLocked = false;
	}
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnValueChanged_Synced, float Value);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCheckStateChanged_Synced, bool bIsChecked);

/** Widget that syncs two sliders and two editable text boxes depending on whether or not a checkbox is checked. Useful for getting min/max values from user */
UCLASS()
class USERINTERFACE_API UDoubleSyncedSliderAndTextBox : public UUserWidget
{
	
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSVerticalBox* MainContainer;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_Min;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_Max;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_CheckBox;
	
	/** The Tooltip image for the Checkbox_SyncSlidersAndValues. The parent widget will bind to this widget's OnTooltipImageHoveredLocal delegate to display tooltip information */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_Checkbox;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_Min;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_Max;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* TextTooltipBox_Min;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* TextTooltipBox_Max;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_Min;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_Max;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* Text_Checkbox_SyncSlidersAndValues;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* Text_Min;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* Text_Max;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBox* Checkbox_MinLock;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBox* Checkbox_MaxLock;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBox* Checkbox_SyncSlidersAndValues;
	
public:

	/** Initializes the Sliders and EditableTextBoxes' with text and values */
	virtual void InitConstrainedSlider(const FSyncedSlidersParams& InParams);

	/** Overrides a slider's max value */
	void OverrideMaxValue(const bool bIsMin, const float ValueToOverride);
	
	/** Updates the Slider Values by calling OnSliderChanged_Min and OnSliderChanged_Max, and updates the Checkbox_SyncSlidersAndValues checked state if necessary */
	void UpdateDefaultValues(const float NewMinValue, const float NewMaxValue, const bool bSync);
	
	/** Executed when Slider_Min or Value_Min is changed */
	FOnValueChanged_Synced OnValueChanged_Min;
	
	/** Executed when Slider_Max or Value_Max is changed */
	FOnValueChanged_Synced OnValueChanged_Max;

	/** Executed when Checkbox_MinLock check state is changed */
	FOnCheckStateChanged_Synced OnCheckStateChanged_Min;

	/** Executed when Checkbox_MaxLock check state is changed */
	FOnCheckStateChanged_Synced OnCheckStateChanged_Max;
	
	/** Executed when the Checkbox_SyncSlidersAndValues check state is changed */
	FOnCheckStateChanged_Synced OnCheckStateChanged_Sync;

	UTooltipImage* GetCheckBoxQMark() const { return QMark_Checkbox; }
	UBSVerticalBox* GetMainContainer() const { return MainContainer; }
	UHorizontalBox* GetTextTooltipBox_Min() const { return TextTooltipBox_Min; }
	UHorizontalBox* GetTextTooltipBox_Max() const { return TextTooltipBox_Max; }
	float GetMinValue() const;
	float GetMaxValue() const;

	/** Returns whether or not the min and max values are being synced */
	bool GetIsSynced() const;

private:
	virtual void NativeConstruct() override;

	/** Function to handle changing of Checkbox_SyncSlidersAndValues */
	void SyncSlidersAndValues(const bool bSync);
	
	/** Calls SyncSlidersAndValues and broadcast OnCheckStateChanged_Sync */
	UFUNCTION()
	void OnCheckStateChanged_SyncSlidersAndValues(const bool bIsChecked);

	UFUNCTION()
	virtual void OnCheckStateChanged_MinLock(const bool bIsLocked);

	UFUNCTION()
	virtual void OnCheckStateChanged_MaxLock(const bool bIsLocked);

	/** Function to handle changing of slider values. Broadcasts relevant delegates depending on sync mode */
	void OnSliderChanged(const bool bIsSlider_Min, const float NewValue);
	
	UFUNCTION()
	void OnSliderChanged_Min(const float NewMin);
	
	UFUNCTION()
	void OnSliderChanged_Max(const float NewMax);

	/** Function to handle changing of text values. Broadcasts relevant delegates depending on sync mode */
	void OnTextCommitted(const bool bIsValue_Min, const FText& NewValue);
	
	UFUNCTION()
	void OnTextCommitted_Min(const FText& NewMin, ETextCommit::Type CommitType);
	
	UFUNCTION()
	void OnTextCommitted_Max(const FText& NewMax, ETextCommit::Type CommitType);
	
	/** Struct containing information for the Sliders and EditableTextBoxes */
	FSyncedSlidersParams SliderStruct;
	
	/** Returns the rounded value of ValueToRound according to the GridSnapSize */
	float RoundValue(const float ValueToRound) const;
	
	/** Clamps the NewValue to the appropriate Slider Min and Max Values, and returns the new rounded value */
	virtual float CheckConstraints(const float NewValue, const bool bIsMin);

	float PreSyncedMinValue = -1.f;
	float PreSyncedMaxValue = -1.f;
};
