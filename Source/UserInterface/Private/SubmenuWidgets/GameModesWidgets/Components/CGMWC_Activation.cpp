// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/GameModesWidgets/Components/CGMWC_Activation.h"
#include "Components/CheckBox.h"
#include "Components/Slider.h"
#include "WidgetComponents/Boxes/BSComboBoxString.h"
#include "WidgetComponents/MenuOptionWidgets/CheckBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/ComboBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/ConstantMinMaxMenuOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/SliderTextBoxOptionWidget.h"


void UCGMWC_Activation::NativeConstruct()
{
	Super::NativeConstruct();

	SliderTextBoxOption_MaxNumActivatedTargetsAtOnce->SetValues(MinValue_MaxNumActivatedTargetsAtOnce,
		MaxValue_MaxNumActivatedTargetsAtOnce, SnapSize_MaxNumActivatedTargetsAtOnce);
	MenuOption_NumTargetsToActivateAtOnce->SetValues(MinValue_MaxNumActivatedTargetsAtOnce,
		MaxValue_MaxNumActivatedTargetsAtOnce, SnapSize_MaxNumActivatedTargetsAtOnce);

	SliderTextBoxOption_MaxNumActivatedTargetsAtOnce->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	MenuOption_NumTargetsToActivateAtOnce->OnMinMaxMenuOptionChanged.AddUObject(this,
		&ThisClass::OnMinMaxMenuOptionChanged);

	CheckBoxOption_AllowActivationWhileActivated->CheckBox->OnCheckStateChanged.AddUniqueDynamic(this,
		&ThisClass::OnCheckStateChanged_AllowActivationWhileActivated);

	ComboBoxOption_TargetActivationSelectionPolicy->ComboBox->OnSelectionChanged.AddUniqueDynamic(this,
		&ThisClass::OnSelectionChanged_TargetActivationSelectionPolicy);

	ComboBoxOption_TargetActivationSelectionPolicy->GetComboBoxEntryTooltipStringTableKey.BindUObject(this,
		&ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetActivationSelectionPolicy);

	ComboBoxOption_TargetActivationSelectionPolicy->ComboBox->ClearOptions();

	TArray<FString> Options;
	for (const ETargetActivationSelectionPolicy& Method : TEnumRange<ETargetActivationSelectionPolicy>())
	{
		Options.Add(GetStringFromEnum_FromTagMap(Method));
	}
	ComboBoxOption_TargetActivationSelectionPolicy->SortAddOptionsAndSetEnumType<
		ETargetActivationSelectionPolicy>(Options);
	Options.Empty();

	SetupWarningTooltipCallbacks();
	UpdateBrushColors();
}

void UCGMWC_Activation::UpdateAllOptionsValid()
{
	Super::UpdateAllOptionsValid();
}

void UCGMWC_Activation::UpdateOptionsFromConfig()
{
	const bool bConstantNumTargetsToActivateAtOnce = BSConfig->TargetConfig.MinNumTargetsToActivateAtOnce == BSConfig->
		TargetConfig.MaxNumTargetsToActivateAtOnce;
	const bool bConstantTargetSpeed = BSConfig->TargetConfig.MinActivatedTargetSpeed == BSConfig->TargetConfig.
		MaxActivatedTargetSpeed;

	UpdateValueIfDifferent(CheckBoxOption_AllowActivationWhileActivated,
		BSConfig->TargetConfig.bAllowActivationWhileActivated);
	UpdateValueIfDifferent(SliderTextBoxOption_MaxNumActivatedTargetsAtOnce,
		BSConfig->TargetConfig.MaxNumActivatedTargetsAtOnce);
	UpdateValuesIfDifferent(MenuOption_NumTargetsToActivateAtOnce, bConstantNumTargetsToActivateAtOnce,
		BSConfig->TargetConfig.MinNumTargetsToActivateAtOnce, BSConfig->TargetConfig.MaxNumActivatedTargetsAtOnce);

	UpdateValueIfDifferent(ComboBoxOption_TargetActivationSelectionPolicy,
		GetStringFromEnum_FromTagMap(BSConfig->TargetConfig.TargetActivationSelectionPolicy));

	UpdateDependentOptions_TargetActivationResponses(BSConfig->TargetConfig.TargetActivationResponses,
		bConstantTargetSpeed);
	UpdateDependentOptions_TargetDistributionPolicy(BSConfig->TargetConfig.TargetDistributionPolicy);

	UpdateBrushColors();
}

void UCGMWC_Activation::SetupWarningTooltipCallbacks()
{
}

void UCGMWC_Activation::UpdateDependentOptions_TargetActivationResponses(
	const TArray<ETargetActivationResponse>& InResponses, const bool bUseConstantTargetSpeed)
{
}

void UCGMWC_Activation::UpdateDependentOptions_TargetDistributionPolicy(const ETargetDistributionPolicy& Policy)
{
	switch (Policy)
	{
	case ETargetDistributionPolicy::Grid:
		ComboBoxOption_TargetActivationSelectionPolicy->ComboBox->SetIsEnabled(true);
		break;
	case ETargetDistributionPolicy::None:
	case ETargetDistributionPolicy::HeadshotHeightOnly:
	case ETargetDistributionPolicy::EdgeOnly:
	case ETargetDistributionPolicy::FullRange:
		BSConfig->TargetConfig.TargetActivationSelectionPolicy = ETargetActivationSelectionPolicy::Random;
		UpdateValueIfDifferent(ComboBoxOption_TargetActivationSelectionPolicy,
			GetStringFromEnum_FromTagMap(BSConfig->TargetConfig.TargetActivationSelectionPolicy));
		ComboBoxOption_TargetActivationSelectionPolicy->ComboBox->SetIsEnabled(false);
		break;
	}
}

void UCGMWC_Activation::OnCheckStateChanged_AllowActivationWhileActivated(const bool bChecked)
{
	BSConfig->TargetConfig.bAllowActivationWhileActivated = bChecked;
	UpdateAllOptionsValid();
}

void UCGMWC_Activation::OnSliderTextBoxValueChanged(USliderTextBoxOptionWidget* Widget, const float Value)
{
	if (Widget == SliderTextBoxOption_MaxNumActivatedTargetsAtOnce)
	{
		BSConfig->TargetConfig.MaxNumActivatedTargetsAtOnce = Value;
	}
	UpdateAllOptionsValid();
}

void UCGMWC_Activation::OnMinMaxMenuOptionChanged(UConstantMinMaxMenuOptionWidget* Widget, const bool bChecked,
	const float MinOrConstant, const float Max)
{
	if (Widget == MenuOption_NumTargetsToActivateAtOnce)
	{
		BSConfig->TargetConfig.MinNumTargetsToActivateAtOnce = MinOrConstant;
		BSConfig->TargetConfig.MaxNumTargetsToActivateAtOnce = bChecked ? MinOrConstant : Max;
	}
	UpdateBrushColors();
	UpdateAllOptionsValid();
}

void UCGMWC_Activation::OnSelectionChanged_TargetActivationSelectionPolicy(const TArray<FString>& Selected,
	const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || Selected.Num() != 1)
	{
		return;
	}

	BSConfig->TargetConfig.TargetActivationSelectionPolicy = GetEnumFromString<
		ETargetActivationSelectionPolicy>(Selected[0]);
	UpdateAllOptionsValid();
}

FString UCGMWC_Activation::GetComboBoxEntryTooltipStringTableKey_TargetActivationSelectionPolicy(
	const FString& EnumString)
{
	const ETargetActivationSelectionPolicy EnumValue = GetEnumFromString<ETargetActivationSelectionPolicy>(EnumString);
	return GetStringTableKeyNameFromEnum(EnumValue);
}
