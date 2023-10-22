// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CGMWC_Base.h"
#include "CGMWC_Activation.generated.h"

class USliderTextBoxOptionWidget;
class UEditableTextBoxOptionWidget;
class UCheckBoxOptionWidget;
class UComboBoxOptionWidget;

UCLASS()
class USERINTERFACE_API UCGMWC_Activation : public UCGMWC_Base
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void UpdateAllOptionsValid() override;
	virtual void UpdateOptionsFromConfig() override;
	void SetupWarningTooltipCallbacks();

	/** Updates options that depend on the value selection of TargetActivationResponses */
	void UpdateDependentOptions_TargetActivationResponses(const TArray<ETargetActivationResponse>& InResponses,
		const bool bUseConstantTargetSpeed);

	/** Updates options that depend on the value selection of TargetDistributionPolicy */
	void UpdateDependentOptions_TargetDistributionPolicy(const ETargetDistributionPolicy& Policy);

	UFUNCTION()
	void OnCheckStateChanged_AllowActivationWhileActivated(const bool bChecked);

	void OnSliderTextBoxValueChanged(USliderTextBoxOptionWidget* Widget, const float Value);
	
	void OnMinMaxMenuOptionChanged(UConstantMinMaxMenuOptionWidget* Widget,
	const bool bChecked, const float MinOrConstant, const float Max);

	UFUNCTION()
	void OnSelectionChanged_TargetActivationSelectionPolicy(const TArray<FString>& Selected,
		const ESelectInfo::Type SelectionType);

	FString GetComboBoxEntryTooltipStringTableKey_TargetActivationSelectionPolicy(const FString& EnumString);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_MaxNumActivatedTargetsAtOnce;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UConstantMinMaxMenuOptionWidget* MenuOption_NumTargetsToActivateAtOnce;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBoxOptionWidget* CheckBoxOption_AllowActivationWhileActivated;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxOptionWidget* ComboBoxOption_TargetActivationSelectionPolicy;
};
