// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CGMWC_Base.h"
#include "CGMWC_Spawning.generated.h"

class USliderTextBoxOptionWidget;
class UCheckBoxOptionWidget;
class UComboBoxOptionWidget;

UCLASS()
class USERINTERFACE_API UCGMWC_Spawning : public UCGMWC_Base
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void UpdateAllOptionsValid() override;
	virtual void UpdateOptionsFromConfig() override;
	void SetupWarningTooltipCallbacks();

	void UpdateDependentOptions_TargetSpawningPolicy(const ETargetSpawningPolicy& InTargetSpawningPolicy);
	
	UFUNCTION()
	void OnCheckStateChanged_AllowSpawnWithoutActivation(const bool bChecked);
	UFUNCTION()
	void OnCheckStateChanged_BatchSpawning(const bool bChecked);
	UFUNCTION()
	void OnCheckStateChanged_SpawnAtOriginWheneverPossible(const bool bChecked);
	UFUNCTION()
	void OnCheckStateChanged_SpawnEveryOtherTargetInCenter(const bool bChecked);

	void OnSliderTextBoxValueChanged(USliderTextBoxOptionWidget* Widget, const float Value);

	UFUNCTION()
	void OnSelectionChanged_RuntimeTargetSpawningLocationSelectionMode(const TArray<FString>& Selected,
		const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_TargetSpawningPolicy(const TArray<FString>& Selected,
		const ESelectInfo::Type SelectionType);
	
	FString GetComboBoxEntryTooltipStringTableKey_TargetSpawningPolicy(const FString& EnumString);
	FString GetComboBoxEntryTooltipStringTableKey_RuntimeTargetSpawningLocationSelectionMode(const FString& EnumString);
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_MaxNumTargetsAtOnce;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxOptionWidget* ComboBoxOption_TargetSpawningPolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxOptionWidget* ComboBoxOption_RuntimeTargetSpawningLocationSelectionMode;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBoxOptionWidget* CheckBoxOption_AllowSpawnWithoutActivation;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBoxOptionWidget* CheckBoxOption_BatchSpawning;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBoxOptionWidget* CheckBoxOption_SpawnAtOriginWheneverPossible;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBoxOptionWidget* CheckBoxOption_SpawnEveryOtherTargetInCenter;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_NumUpfrontTargetsToSpawn;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_NumRuntimeTargetsToSpawn;
};
