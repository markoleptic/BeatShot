// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

// ReSharper disable CppUE4CodingStandardNamingViolationWarning
#pragma once

#include "CoreMinimal.h"
#include "CGMWC_Base.h"
#include "CGMWC_TargetScaling.generated.h"

UCLASS()
class USERINTERFACE_API UCGMWC_TargetScaling : public UCGMWC_Base
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void UpdateAllOptionsValid() override;
	virtual void UpdateOptionsFromConfig() override;
	void SetupWarningTooltipCallbacks();
	
	/** Updates options that depend on the value selection of TargetActivationResponses */
	void UpdateDependentOptions_TargetActivationResponses(const TArray<ETargetActivationResponse>& InResponses);
	
	/** If Persistent, empties and disables Target Deactivation Responses Combo Box */
	void UpdateDependentOptions_TargetDeactivationResponses(const TArray<ETargetDeactivationCondition>& Conditions,
		const TArray<ETargetDeactivationResponse>& Responses);
	
	/** Updates options that depend on the value selection of ConsecutiveTargetScalePolicy */
	void UpdateDependentOptions_ConsecutiveTargetScalePolicy(
		const EConsecutiveTargetScalePolicy InConsecutiveTargetScalePolicy);

	void OnSliderTextBoxValueChanged(USliderTextBoxOptionWidget* Widget, const float Value);

	void OnMinMaxMenuOptionChanged(UConstantMinMaxMenuOptionWidget* Widget,
		const bool bChecked, const float MinOrConstant, const float Max);
	
	UFUNCTION()
	void OnSelectionChanged_ConsecutiveTargetScalePolicy(const TArray<FString>& Selected,
		const ESelectInfo::Type SelectionType);
	
	FString GetComboBoxEntryTooltipStringTableKey_ConsecutiveTargetScalePolicy(const FString& EnumString);
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_DeactivatedTargetScaleMultiplier;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UConstantMinMaxMenuOptionWidget* MenuOption_TargetScale;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_StartThreshold;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_EndThreshold;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_DecrementAmount;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_LifetimeTargetScaleMultiplier;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxOptionWidget* ComboBoxOption_ConsecutiveTargetScalePolicy;

	EConsecutiveTargetScalePolicy LastSelectedConsecutiveTargetScalePolicy = EConsecutiveTargetScalePolicy::None;
};


