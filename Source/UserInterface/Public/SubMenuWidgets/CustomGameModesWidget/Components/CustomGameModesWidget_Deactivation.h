// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

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
	virtual void UpdateOptions() override;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxOptionWidget* ComboBoxOption_TargetDeactivationConditions;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxOptionWidget* ComboBoxOption_TargetDeactivationResponses;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxWidget* SliderTextBoxOption_DeactivatedTargetScaleMultiplier;

	UFUNCTION()
	void OnSelectionChanged_TargetDeactivationConditions(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_TargetDeactivationResponses(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);
	
	FString GetComboBoxEntryTooltipStringTableKey_TargetDeactivationConditions(const FString& EnumString);
	FString GetComboBoxEntryTooltipStringTableKey_TargetDeactivationResponses(const FString& EnumString);

	void OnSliderTextBoxValueChanged(USliderTextBoxWidget* Widget, const float Value);
};
