// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CustomGameModesWidgetComponent.h"
#include "CustomGameModesWidget_General.generated.h"

class UCheckBoxOptionWidget;
class USliderTextBoxWidget;
class UComboBoxOptionWidget;

UCLASS()
class USERINTERFACE_API UCustomGameModesWidget_General : public UCustomGameModesWidgetComponent
{
	GENERATED_BODY()

public:
	virtual void InitComponent(FBSConfig* InConfigPtr, TObjectPtr<UCustomGameModesWidgetComponent> InNext) override;

protected:
	virtual void NativeConstruct() override;
	virtual bool UpdateAllOptionsValid() override;
	virtual void UpdateOptionsFromConfig() override;
	
	/** Updates options that depend on the value selection of RecentTargetMemoryPolicy */
	void UpdateDependentOptions_RecentTargetMemoryPolicy(const ERecentTargetMemoryPolicy& InRecentTargetMemoryPolicy);
	
	/** Updates options that depend on the value selection of bEnableReinforcementLearning */
	void UpdateDependentOptions_EnableAI(const bool bInEnableReinforcementLearning);

	/** Updates options that depend on the value selection of UnlimitedTargetHealth */
	void UpdateDependentOptions_UnlimitedTargetHealth(const bool bInUnlimitedTargetHealth);
	
	/** Updates options that depend on the value selection of ConsecutiveTargetScalePolicy */
	void UpdateDependentOptions_ConsecutiveTargetScalePolicy(const EConsecutiveTargetScalePolicy InConsecutiveTargetScalePolicy);
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxWidget* SliderTextBoxOption_SpawnBeatDelay;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxWidget* SliderTextBoxOption_TargetSpawnCD;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxOptionWidget* ComboBoxOption_RecentTargetMemoryPolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxWidget* SliderTextBoxOption_MaxNumRecentTargets;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxWidget* SliderTextBoxOption_RecentTargetTimeLength;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBoxOptionWidget* CheckBoxOption_EnableAI;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxWidget* SliderTextBoxOption_Alpha;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxWidget* SliderTextBoxOption_Epsilon;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxWidget* SliderTextBoxOption_Gamma;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxWidget* SliderTextBoxOption_TargetMaxLifeSpan;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBoxOptionWidget* CheckBoxOption_UnlimitedTargetHealth;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxWidget* SliderTextBoxOption_MaxHealth;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxWidget* SliderTextBoxOption_ExpirationHealthPenalty;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxOptionWidget* ComboBoxOption_ConsecutiveTargetScalePolicy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxWidget* SliderTextBoxOption_TargetScale;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxWidget* SliderTextBoxOption_MinTargetScale;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxWidget* SliderTextBoxOption_MaxTargetScale;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxOptionWidget* ComboBoxOption_DamageType;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxOptionWidget* ComboBoxOption_MovingTargetDirectionMode;
	
	UFUNCTION()
	void OnCheckStateChanged_EnableAI(const bool bChecked);
	UFUNCTION()
	void OnCheckStateChanged_UnlimitedTargetHealth(const bool bChecked);
	
	void OnSliderTextBoxValueChanged(USliderTextBoxWidget* Widget, const float Value);

	UFUNCTION()
	void OnSelectionChanged_RecentTargetMemoryPolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_DamageType(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_ConsecutiveTargetScalePolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_MovingTargetDirectionMode(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);
	
	FString GetComboBoxEntryTooltipStringTableKey_TargetActivationSelectionPolicy(const FString& EnumString);
	FString GetComboBoxEntryTooltipStringTableKey_DamageType(const FString& EnumString);
	FString GetComboBoxEntryTooltipStringTableKey_ConsecutiveTargetScalePolicy(const FString& EnumString);
	FString GetComboBoxEntryTooltipStringTableKey_MovingTargetDirectionMode(const FString& EnumString);
};
