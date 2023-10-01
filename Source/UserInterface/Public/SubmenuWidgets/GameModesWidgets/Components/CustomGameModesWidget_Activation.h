﻿// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CustomGameModesWidgetComponent.h"
#include "CustomGameModesWidget_Activation.generated.h"

class USliderTextBoxOptionWidget;
class UEditableTextBoxOptionWidget;
class UCheckBoxOptionWidget;
class UComboBoxOptionWidget;

UCLASS()
class USERINTERFACE_API UCustomGameModesWidget_Activation : public UCustomGameModesWidgetComponent
{
	GENERATED_BODY()
	
public:
	virtual void InitComponent(FBSConfig* InConfigPtr, TObjectPtr<UCustomGameModesWidgetComponent> InNext) override;

protected:
	virtual void NativeConstruct() override;
	virtual void UpdateAllOptionsValid() override;
	virtual void UpdateOptionsFromConfig() override;
	void SetupWarningTooltipCallbacks();
	
	/** Updates options that depend on the value selection of ConstantNumTargetsToActivateAtOnce */
	void UpdateDependentOptions_ConstantNumTargetsToActivateAtOnce(const bool bInConstant);

	/** Updates options that depend on the value selection of TargetActivationResponses */
	void UpdateDependentOptions_TargetActivationResponses(const TArray<ETargetActivationResponse>& InResponses, const bool bUseConstantTargetSpeed);

	/** Updates options that depend on the value selection of TargetActivationResponses and ConstantTargetSpeed */
	void UpdateDependentOptions_ConstantTargetSpeed(const TArray<ETargetActivationResponse>& InResponses, const bool bUseConstantTargetSpeed);

	/** Updates options that depend on the value selection of TargetDistributionPolicy */
	void UpdateDependentOptions_TargetDistributionPolicy(const ETargetDistributionPolicy& Policy);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_MaxNumActivatedTargetsAtOnce;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBoxOptionWidget* CheckBoxOption_ConstantNumTargetsToActivateAtOnce;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_NumTargetsToActivateAtOnce;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_MinNumTargetsToActivateAtOnce;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_MaxNumTargetsToActivateAtOnce;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxOptionWidget* ComboBoxOption_TargetActivationSelectionPolicy;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxOptionWidget* ComboBoxOption_TargetActivationResponses;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_LifetimeTargetScaleMultiplier;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBoxOptionWidget* CheckBoxOption_ConstantActivatedTargetVelocity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_ActivatedTargetVelocity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_MinActivatedTargetVelocity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_MaxActivatedTargetVelocity;

	UFUNCTION()
	void OnCheckStateChanged_ConstantNumTargetsToActivateAtOnce(const bool bChecked);
	UFUNCTION()
	void OnCheckStateChanged_ConstantActivatedTargetVelocity(const bool bChecked);

	void OnSliderTextBoxValueChanged(USliderTextBoxOptionWidget* Widget, const float Value);
	
	UFUNCTION()
	void OnSelectionChanged_TargetActivationSelectionPolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_TargetActivationResponses(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);
	
	FString GetComboBoxEntryTooltipStringTableKey_TargetActivationSelectionPolicy(const FString& EnumString);
	FString GetComboBoxEntryTooltipStringTableKey_TargetActivationResponses(const FString& EnumString);
};