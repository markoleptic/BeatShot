// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CGMWC_Base.h"
#include "CGMWC_General.generated.h"

class USliderTextBoxCheckBoxOptionWidget;
class UCheckBoxOptionWidget;
class USliderTextBoxOptionWidget;
class UComboBoxOptionWidget;

UCLASS()
class USERINTERFACE_API UCGMWC_General : public UCGMWC_Base
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void UpdateAllOptionsValid() override;
	virtual void UpdateOptionsFromConfig() override;
	void SetupWarningTooltipCallbacks();

	/** Updates options that depend on the value selection of RecentTargetMemoryPolicy */
	void UpdateDependentOptions_RecentTargetMemoryPolicy(const ERecentTargetMemoryPolicy& InRecentTargetMemoryPolicy);

	void UpdateDependentOptions_DeactivationConditions(const TArray<ETargetDeactivationCondition>& Conditions);

	/** Updates options that depend on the value selection of bEnableReinforcementLearning */
	void UpdateDependentOptions_EnableAI(const bool bInEnableReinforcementLearning,
		const EReinforcementLearningHyperParameterMode HyperParameterMode);

	/** Updates options that depend on the value selection of HyperParameterMode */
	void UpdateDependentOptions_HyperParameterMode(const bool bInEnableReinforcementLearning,
		const EReinforcementLearningHyperParameterMode HyperParameterMode);
	
	UFUNCTION()
	void OnCheckStateChanged_EnableAI(const bool bChecked);

	void OnSliderTextBoxValueChanged(USliderTextBoxOptionWidget* Widget, const float Value);
	void OnSliderTextBoxCheckBoxOptionChanged(USliderTextBoxCheckBoxOptionWidget* Widget, const bool bChecked,
		const float Value);
	
	UFUNCTION()
	void OnSelectionChanged_RecentTargetMemoryPolicy(const TArray<FString>& Selected,
		const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_DamageType(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_HyperParameterMode(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);

	FString GetComboBoxEntryTooltipStringTableKey_TargetActivationSelectionPolicy(const FString& EnumString);
	FString GetComboBoxEntryTooltipStringTableKey_DamageType(const FString& EnumString);
	FString GetComboBoxEntryTooltipStringTableKey_HyperParameterMode(const FString& EnumString);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_SpawnBeatDelay;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_TargetSpawnCD;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxOptionWidget* ComboBoxOption_RecentTargetMemoryPolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_MaxNumRecentTargets;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_RecentTargetTimeLength;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBoxOptionWidget* CheckBoxOption_EnableAI;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxOptionWidget* ComboBoxOption_HyperParameterMode;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_Alpha;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_Epsilon;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_Gamma;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxCheckBoxOptionWidget* MenuOption_TargetLifespan;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxCheckBoxOptionWidget* MenuOption_TargetHealth;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_ExpirationHealthPenalty;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_DeactivationHealthLostThreshold;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxOptionWidget* ComboBoxOption_DamageType;
};
