﻿// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CGMWC_Base.h"
#include "CGMWC_ConditionsAndResponses.generated.h"

UCLASS()
class USERINTERFACE_API UCGMWC_ConditionsAndResponses : public UCGMWC_Base
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void UpdateAllOptionsValid() override;
	virtual void UpdateOptionsFromConfig() override;
	void SetupWarningTooltipCallbacks();
	
	void UpdateDependentOptions_TargetSpawnResponses(const TArray<ETargetSpawnResponse>& Responses);
	void UpdateDependentOptions_TargetActivationResponses(const TArray<ETargetActivationResponse>& Responses);
	void UpdateDependentOptions_TargetDeactivationResponses(const TArray<ETargetDeactivationResponse>& Responses);

	UFUNCTION()
	void OnSelectionChanged_TargetSpawnResponses(const TArray<FString>& Selected,
		const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_TargetActivationResponses(const TArray<FString>& Selected,
		const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_TargetDeactivationConditions(const TArray<FString>& Selected,
		const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_TargetDeactivationResponses(const TArray<FString>& Selected,
		const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_TargetDestructionConditions(const TArray<FString>& Selected,
		const ESelectInfo::Type SelectionType);
	
	FString GetComboBoxEntryTooltipStringTableKey_TargetSpawnResponses(const FString& EnumString);
	FString GetComboBoxEntryTooltipStringTableKey_TargetActivationResponses(const FString& EnumString);
	FString GetComboBoxEntryTooltipStringTableKey_TargetDeactivationConditions(const FString& EnumString);
	FString GetComboBoxEntryTooltipStringTableKey_TargetDeactivationResponses(const FString& EnumString);
	FString GetComboBoxEntryTooltipStringTableKey_TargetDestructionConditions(const FString& EnumString);
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxOptionWidget* ComboBoxOption_TargetSpawnResponses;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxOptionWidget* ComboBoxOption_TargetActivationResponses;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxOptionWidget* ComboBoxOption_TargetDeactivationConditions;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxOptionWidget* ComboBoxOption_TargetDeactivationResponses;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxOptionWidget* ComboBoxOption_TargetDestructionConditions;
};
