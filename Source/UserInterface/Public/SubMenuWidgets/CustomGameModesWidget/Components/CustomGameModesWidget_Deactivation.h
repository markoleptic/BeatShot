﻿// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CustomGameModesWidgetComponent.h"
#include "CustomGameModesWidget_Deactivation.generated.h"

class UComboBoxOptionWidget;
class USliderTextBoxWidget;

UCLASS()
class USERINTERFACE_API UCustomGameModesWidget_Deactivation : public UCustomGameModesWidgetComponent
{
	GENERATED_BODY()
	
public:
	virtual void InitComponent(FBSConfig* InConfigPtr, TObjectPtr<UCustomGameModesWidgetComponent> InNext) override;

protected:
	virtual void NativeConstruct() override;
	virtual bool UpdateAllOptionsValid() override;
	virtual void UpdateOptionsFromConfig() override;

	/** If Persistant, empties and disables Target Deactivation Responses Combo Box */
	void UpdateDependentOptions_TargetDeactivationConditions(const TArray<ETargetDeactivationCondition>& Conditions, const TArray<ETargetDeactivationResponse>& Responses);

	/** If Persistant, empties and disables Target Deactivation Responses Combo Box */
	void UpdateDependentOptions_TargetDeactivationResponses(const TArray<ETargetDeactivationCondition>& Conditions, const TArray<ETargetDeactivationResponse>& Responses);
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxOptionWidget* ComboBoxOption_TargetDeactivationConditions;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxOptionWidget* ComboBoxOption_TargetDeactivationResponses;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxWidget* SliderTextBoxOption_DeactivatedTargetScaleMultiplier;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxOptionWidget* ComboBoxOption_TargetDestructionConditions;

	void OnSliderTextBoxValueChanged(USliderTextBoxWidget* Widget, const float Value);
	
	UFUNCTION()
	void OnSelectionChanged_TargetDeactivationConditions(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_TargetDeactivationResponses(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_TargetDestructionConditions(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);
	
	FString GetComboBoxEntryTooltipStringTableKey_TargetDeactivationConditions(const FString& EnumString);
	FString GetComboBoxEntryTooltipStringTableKey_TargetDeactivationResponses(const FString& EnumString);
	FString GetComboBoxEntryTooltipStringTableKey_TargetDestructionConditions(const FString& EnumString);
};
