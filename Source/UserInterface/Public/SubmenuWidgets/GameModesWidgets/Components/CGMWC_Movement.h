// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CustomGameModesWidgetComponent.h"
#include "CGMWC_Movement.generated.h"

/**
 * 
 */
UCLASS()
class USERINTERFACE_API UCGMWC_Movement : public UCustomGameModesWidgetComponent
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void UpdateAllOptionsValid() override;
	virtual void UpdateOptionsFromConfig() override;
	void SetupWarningTooltipCallbacks();

	/** Updates options that depend on TargetSpawnResponses (and ConstantSpawnedTargetVelocity) */
	void UpdateDependentOptions_SpawnResponses(const TArray<ETargetSpawnResponse>& Responses, const bool bConstant);

	/** Updates options that depend on TargetActivationResponses (and ConstantActivationTargetVelocity) */
	void UpdateDependentOptions_ActivationResponses(const TArray<ETargetActivationResponse>& Responses,
		const bool bConstant);

	/** Updates options that depend on TargetDeactivationResponses (and ConstantDeactivatedTargetVelocity) */
	void UpdateDependentOptions_DeactivationResponses(const TArray<ETargetDeactivationResponse>& Responses,
		const bool bConstant);

	UFUNCTION()
	void OnSelectionChanged_MovingTargetDirectionMode(const TArray<FString>& Selected,
		const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnCheckStateChanged_ConstantSpawnedTargetVelocity(const bool bChecked);
	UFUNCTION()
	void OnCheckStateChanged_ConstantActivatedTargetVelocity(const bool bChecked);
	UFUNCTION()
	void OnCheckStateChanged_ConstantDeactivatedTargetVelocity(const bool bChecked);

	void OnSliderTextBoxValueChanged(USliderTextBoxOptionWidget* Widget, const float Value);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBoxOptionWidget* CheckBoxOption_ConstantActivatedTargetVelocity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_ActivatedTargetVelocity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_MinActivatedTargetVelocity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_MaxActivatedTargetVelocity;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBoxOptionWidget* CheckBoxOption_ConstantDeactivatedTargetVelocity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_DeactivatedTargetVelocity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_MinDeactivatedTargetVelocity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_MaxDeactivatedTargetVelocity;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBoxOptionWidget* CheckBoxOption_ConstantSpawnedTargetVelocity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_SpawnedTargetVelocity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_MinSpawnedTargetVelocity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_MaxSpawnedTargetVelocity;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxOptionWidget* ComboBoxOption_MovingTargetDirectionMode;

	FString GetComboBoxEntryTooltipStringTableKey_MovingTargetDirectionMode(const FString& EnumString);
};
