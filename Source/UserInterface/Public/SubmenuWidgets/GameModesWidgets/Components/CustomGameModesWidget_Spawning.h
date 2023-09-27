// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CustomGameModesWidgetComponent.h"
#include "CustomGameModesWidget_Spawning.generated.h"

class USliderTextBoxOptionWidget;
class UCheckBoxOptionWidget;
class UComboBoxOptionWidget;

UCLASS()
class USERINTERFACE_API UCustomGameModesWidget_Spawning : public UCustomGameModesWidgetComponent
{
	GENERATED_BODY()

public:
	virtual void InitComponent(FBSConfig* InConfigPtr, TObjectPtr<UCustomGameModesWidgetComponent> InNext) override;

protected:
	virtual void NativeConstruct() override;
	virtual void UpdateAllOptionsValid() override;
	virtual void UpdateOptionsFromConfig() override;
	void SetupWarningTooltipCallbacks();
	
	void UpdateDependentOptions_TargetSpawningPolicy(const ETargetSpawningPolicy& InTargetSpawningPolicy);

	void UpdateDependentOptions_ApplyVelocityWhenSpawned(const bool bApplyVelocityWhenSpawned, const bool bConstantSpawnedTargetVelocity);
	
	/** Updates options that depend on the value selection of ConstantSpawnedTargetVelocity */
	void UpdateDependentOptions_ConstantSpawnedTargetVelocity(const bool bApplyVelocityWhenSpawned, const bool bInConstant);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_MaxNumTargetsAtOnce;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxOptionWidget* ComboBoxOption_TargetSpawningPolicy;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBoxOptionWidget* CheckBoxOption_AllowSpawnWithoutActivation;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBoxOptionWidget* CheckBoxOption_BatchSpawning;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBoxOptionWidget* CheckBoxOption_ApplyImmunityOnSpawn;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBoxOptionWidget* CheckBoxOption_SpawnAtOriginWheneverPossible;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBoxOptionWidget* CheckBoxOption_SpawnEveryOtherTargetInCenter;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_NumUpfrontTargetsToSpawn;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_NumRuntimeTargetsToSpawn;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBoxOptionWidget* CheckBoxOption_ApplyVelocityWhenSpawned;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBoxOptionWidget* CheckBoxOption_ConstantSpawnedTargetVelocity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_SpawnedTargetVelocity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_MinSpawnedTargetVelocity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_MaxSpawnedTargetVelocity;
	
	UFUNCTION()
	void OnCheckStateChanged_AllowSpawnWithoutActivation(const bool bChecked);
	UFUNCTION()
	void OnCheckStateChanged_BatchSpawning(const bool bChecked);
	UFUNCTION()
	void OnCheckStateChanged_ApplyImmunityOnSpawn(const bool bChecked);
	UFUNCTION()
	void OnCheckStateChanged_SpawnAtOriginWheneverPossible(const bool bChecked);
	UFUNCTION()
	void OnCheckStateChanged_SpawnEveryOtherTargetInCenter(const bool bChecked);
	UFUNCTION()
	void OnCheckStateChanged_ApplyVelocityWhenSpawned(const bool bChecked);
	UFUNCTION()
	void OnCheckStateChanged_ConstantSpawnedTargetVelocity(const bool bChecked);

	void OnSliderTextBoxValueChanged(USliderTextBoxOptionWidget* Widget, const float Value);
	
	UFUNCTION()
	void OnSelectionChanged_TargetSpawningPolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);
	
	FString GetComboBoxEntryTooltipStringTableKey_TargetSpawningPolicy(const FString& EnumString);
};
