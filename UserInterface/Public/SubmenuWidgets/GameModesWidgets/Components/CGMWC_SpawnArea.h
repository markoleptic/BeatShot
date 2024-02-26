// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CGMWC_Base.h"
#include "CGMWC_SpawnArea.generated.h"

class USliderTextBoxOptionWidget;
class UEditableTextBoxOptionWidget;
class UCheckBoxOptionWidget;
class UComboBoxOptionWidget;
class UConstantMinMaxMenuOptionWidget;

struct FSpreadWidgetState
{
	EMenuOptionEnabledState MenuOptionEnabledState;
	EMenuOptionEnabledState SubWidgetEnabledState;
	FString MenuOptionTooltipKey;
	FString SubWidgetTooltipKey;
	FSpreadWidgetState() = default;
	FSpreadWidgetState(const EMenuOptionEnabledState MenuState, const EMenuOptionEnabledState SubWidgetState,
		const FString& MenuKey, const FString& SubWidgetKey) :
	MenuOptionEnabledState(MenuState),
	SubWidgetEnabledState(SubWidgetState),
	MenuOptionTooltipKey(MenuKey),
	SubWidgetTooltipKey(SubWidgetKey)
	{}

	explicit FSpreadWidgetState(const EMenuOptionEnabledState AllState) :
	MenuOptionEnabledState(AllState),
	SubWidgetEnabledState(AllState),
	MenuOptionTooltipKey(""),
	SubWidgetTooltipKey("")
	{}
};

UCLASS()
class USERINTERFACE_API UCGMWC_SpawnArea : public UCGMWC_Base
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void UpdateAllOptionsValid() override;
	virtual void UpdateOptionsFromConfig() override;
	void SetupWarningTooltipCallbacks();
	void UpdateDependentOptions_TargetDistributionPolicy();
	void UpdateSpread();
	void UpdateSpreadWidgetState(UConstantMinMaxMenuOptionWidget* Widget, const bool bTracking,
		const bool bHeadShotHeightOnly, const bool bGrid);
	void UpdateDependentOption_BoundsScalingPolicy();
	void OnSliderTextBoxValueChanged(USliderTextBoxOptionWidget* Widget, const float Value);
	void OnMinMaxValueChanged(UConstantMinMaxMenuOptionWidget* Widget,
		const bool bChecked, const float Min, const float Max);
	
	UFUNCTION()
	void OnSelectionChanged_TargetDistributionPolicy(const TArray<FString>& Selected,
		const ESelectInfo::Type SelectionType);
	
	FString GetComboBoxEntryTooltipStringTableKey_TargetDistributionPolicy(const FString& EnumString);
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxOptionWidget* ComboBoxOption_TargetDistributionPolicy;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_StartThreshold;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_EndThreshold;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_DecrementAmount;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_NumHorizontalGridTargets;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_NumVerticalGridTargets;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_HorizontalSpacing;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_VerticalSpacing;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UConstantMinMaxMenuOptionWidget* MenuOption_HorizontalSpread;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UConstantMinMaxMenuOptionWidget* MenuOption_VerticalSpread;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UConstantMinMaxMenuOptionWidget* MenuOption_ForwardSpread;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxOptionWidget* SliderTextBoxOption_FloorDistance;
};