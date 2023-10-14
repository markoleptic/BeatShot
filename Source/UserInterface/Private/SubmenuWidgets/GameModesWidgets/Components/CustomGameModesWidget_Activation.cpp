// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/GameModesWidgets/Components/CustomGameModesWidget_Activation.h"
#include "Components/CheckBox.h"
#include "Components/Slider.h"
#include "WidgetComponents/Boxes/BSComboBoxString.h"
#include "WidgetComponents/MenuOptionWidgets/CheckBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/ComboBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/SliderTextBoxOptionWidget.h"


void UCustomGameModesWidget_Activation::NativeConstruct()
{
	Super::NativeConstruct();

	SliderTextBoxOption_MaxNumActivatedTargetsAtOnce->SetValues(MinValue_MaxNumActivatedTargetsAtOnce,
		MaxValue_MaxNumActivatedTargetsAtOnce, SnapSize_MaxNumActivatedTargetsAtOnce);
	SliderTextBoxOption_NumTargetsToActivateAtOnce->SetValues(MinValue_MaxNumActivatedTargetsAtOnce,
		MaxValue_MaxNumActivatedTargetsAtOnce, SnapSize_MaxNumActivatedTargetsAtOnce);
	SliderTextBoxOption_MinNumTargetsToActivateAtOnce->SetValues(MinValue_MaxNumActivatedTargetsAtOnce,
		MaxValue_MaxNumActivatedTargetsAtOnce, SnapSize_MaxNumActivatedTargetsAtOnce);
	SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->SetValues(MinValue_MaxNumActivatedTargetsAtOnce,
		MaxValue_MaxNumActivatedTargetsAtOnce, SnapSize_MaxNumActivatedTargetsAtOnce);

	SliderTextBoxOption_MaxNumActivatedTargetsAtOnce->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_NumTargetsToActivateAtOnce->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MinNumTargetsToActivateAtOnce->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	
	CheckBoxOption_ConstantNumTargetsToActivateAtOnce->CheckBox->OnCheckStateChanged.AddUniqueDynamic(this,
		&ThisClass::OnCheckStateChanged_ConstantNumTargetsToActivateAtOnce);
	CheckBoxOption_AllowActivationWhileActivated->CheckBox->OnCheckStateChanged.AddUniqueDynamic(this,
		&ThisClass::OnCheckStateChanged_AllowActivationWhileActivated);

	ComboBoxOption_TargetActivationSelectionPolicy->ComboBox->OnSelectionChanged.AddUniqueDynamic(this,
		&ThisClass::OnSelectionChanged_TargetActivationSelectionPolicy);
	ComboBoxOption_TargetActivationResponses->ComboBox->OnSelectionChanged.AddUniqueDynamic(this,
		&ThisClass::OnSelectionChanged_TargetActivationResponses);
	ComboBoxOption_TargetActivationSelectionPolicy->GetComboBoxEntryTooltipStringTableKey.BindUObject(this,
		&ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetActivationSelectionPolicy);
	ComboBoxOption_TargetActivationResponses->GetComboBoxEntryTooltipStringTableKey.BindUObject(this,
		&ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetActivationResponses);

	ComboBoxOption_TargetActivationSelectionPolicy->ComboBox->ClearOptions();
	ComboBoxOption_TargetActivationResponses->ComboBox->ClearOptions();

	TArray<FString> Options;

	for (const ETargetActivationSelectionPolicy& Method : TEnumRange<ETargetActivationSelectionPolicy>())
	{
		Options.Add(GetStringFromEnum_FromTagMap(Method));
	}
	ComboBoxOption_TargetActivationSelectionPolicy->SortAddOptionsAndSetEnumType<
		ETargetActivationSelectionPolicy>(Options);
	Options.Empty();

	for (const ETargetActivationResponse& Method : TEnumRange<ETargetActivationResponse>())
	{
		// Deprecated
		if (Method != ETargetActivationResponse::ChangeScale)
		{
			Options.Add(GetStringFromEnum_FromTagMap(Method));
		}
	}
	ComboBoxOption_TargetActivationResponses->SortAddOptionsAndSetEnumType<ETargetActivationResponse>(Options);
	Options.Empty();

	SliderTextBoxOption_MinNumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::Collapsed);
	
	SetupWarningTooltipCallbacks();
	UpdateBrushColors();
}

void UCustomGameModesWidget_Activation::UpdateAllOptionsValid()
{
	Super::UpdateAllOptionsValid();
}

void UCustomGameModesWidget_Activation::UpdateOptionsFromConfig()
{
	const bool bConstantNumTargetsToActivateAtOnce = BSConfig->TargetConfig.MinNumTargetsToActivateAtOnce == BSConfig->
		TargetConfig.MaxNumTargetsToActivateAtOnce;
	const bool bConstantTargetSpeed = BSConfig->TargetConfig.MinActivatedTargetSpeed == BSConfig->TargetConfig.
		MaxActivatedTargetSpeed;

	UpdateValueIfDifferent(CheckBoxOption_ConstantNumTargetsToActivateAtOnce, bConstantNumTargetsToActivateAtOnce);
	UpdateValueIfDifferent(CheckBoxOption_AllowActivationWhileActivated, BSConfig->TargetConfig.bAllowActivationWhileActivated);

	UpdateValueIfDifferent(SliderTextBoxOption_MaxNumActivatedTargetsAtOnce,
		BSConfig->TargetConfig.MaxNumActivatedTargetsAtOnce);
	UpdateValueIfDifferent(SliderTextBoxOption_NumTargetsToActivateAtOnce,
		BSConfig->TargetConfig.MinNumTargetsToActivateAtOnce);
	UpdateValueIfDifferent(SliderTextBoxOption_MinNumTargetsToActivateAtOnce,
		BSConfig->TargetConfig.MinNumTargetsToActivateAtOnce);
	UpdateValueIfDifferent(SliderTextBoxOption_MaxNumTargetsToActivateAtOnce,
		BSConfig->TargetConfig.MaxNumTargetsToActivateAtOnce);

	UpdateValueIfDifferent(ComboBoxOption_TargetActivationResponses,
		GetStringArrayFromEnumArray_FromTagMap(BSConfig->TargetConfig.TargetActivationResponses));
	UpdateValueIfDifferent(ComboBoxOption_TargetActivationSelectionPolicy,
		GetStringFromEnum_FromTagMap(BSConfig->TargetConfig.TargetActivationSelectionPolicy));

	UpdateDependentOptions_ConstantNumTargetsToActivateAtOnce(bConstantNumTargetsToActivateAtOnce);
	UpdateDependentOptions_TargetActivationResponses(BSConfig->TargetConfig.TargetActivationResponses,
		bConstantTargetSpeed);

	UpdateDependentOptions_TargetDistributionPolicy(BSConfig->TargetConfig.TargetDistributionPolicy);
	
	UpdateBrushColors();
}

void UCustomGameModesWidget_Activation::SetupWarningTooltipCallbacks()
{
	ComboBoxOption_TargetActivationResponses->AddWarningTooltipData(FTooltipData("Invalid_Velocity_MTDM_None",
		ETooltipImageType::Caution)).BindLambda([this]()
	{
		return BSConfig->TargetConfig.MovingTargetDirectionMode == EMovingTargetDirectionMode::None && BSConfig->
			TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::ChangeVelocity);
	});
	ComboBoxOption_TargetActivationResponses->AddWarningTooltipData(FTooltipData("Invalid_Direction_MTDM_None",
		ETooltipImageType::Caution)).BindLambda([this]()
	{
		return BSConfig->TargetConfig.MovingTargetDirectionMode == EMovingTargetDirectionMode::None && BSConfig->
			TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::ChangeDirection);
	});
}

void UCustomGameModesWidget_Activation::UpdateDependentOptions_ConstantNumTargetsToActivateAtOnce(
	const bool bInConstant)
{
	if (bInConstant)
	{
		SliderTextBoxOption_NumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_MinNumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		SliderTextBoxOption_NumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MinNumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UCustomGameModesWidget_Activation::UpdateDependentOptions_TargetActivationResponses(
	const TArray<ETargetActivationResponse>& InResponses, const bool bUseConstantTargetSpeed)
{
}

void UCustomGameModesWidget_Activation::UpdateDependentOptions_TargetDistributionPolicy(
	const ETargetDistributionPolicy& Policy)
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

void UCustomGameModesWidget_Activation::OnCheckStateChanged_ConstantNumTargetsToActivateAtOnce(const bool bChecked)
{
	if (bChecked)
	{
		BSConfig->TargetConfig.MinNumTargetsToActivateAtOnce = SliderTextBoxOption_NumTargetsToActivateAtOnce->
			GetSliderValue();
		BSConfig->TargetConfig.MaxNumTargetsToActivateAtOnce = SliderTextBoxOption_NumTargetsToActivateAtOnce->
			GetSliderValue();
	}
	else
	{
		BSConfig->TargetConfig.MinNumTargetsToActivateAtOnce = SliderTextBoxOption_MinNumTargetsToActivateAtOnce->
			GetSliderValue();
		BSConfig->TargetConfig.MaxNumTargetsToActivateAtOnce = SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->
			GetSliderValue();
	}

	UpdateDependentOptions_ConstantNumTargetsToActivateAtOnce(bChecked);
	UpdateBrushColors();
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_Activation::OnCheckStateChanged_AllowActivationWhileActivated(const bool bChecked)
{
	BSConfig->TargetConfig.bAllowActivationWhileActivated = bChecked;
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_Activation::OnSliderTextBoxValueChanged(USliderTextBoxOptionWidget* Widget,
	const float Value)
{
	if (Widget == SliderTextBoxOption_MaxNumActivatedTargetsAtOnce)
	{
		BSConfig->TargetConfig.MaxNumActivatedTargetsAtOnce = Value;
	}
	if (Widget == SliderTextBoxOption_NumTargetsToActivateAtOnce && SliderTextBoxOption_NumTargetsToActivateAtOnce->
		GetVisibility() != ESlateVisibility::Collapsed)
	{
		BSConfig->TargetConfig.MaxNumTargetsToActivateAtOnce = Value;
		BSConfig->TargetConfig.MinNumTargetsToActivateAtOnce = Value;
	}
	else if (Widget == SliderTextBoxOption_MinNumTargetsToActivateAtOnce)
	{
		BSConfig->TargetConfig.MinNumTargetsToActivateAtOnce = Value;
	}
	else if (Widget == SliderTextBoxOption_MaxNumTargetsToActivateAtOnce)
	{
		BSConfig->TargetConfig.MaxNumTargetsToActivateAtOnce = Value;
	}
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_Activation::OnSelectionChanged_TargetActivationSelectionPolicy(
	const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || Selected.Num() != 1)
	{
		return;
	}

	BSConfig->TargetConfig.TargetActivationSelectionPolicy = GetEnumFromString<
		ETargetActivationSelectionPolicy>(Selected[0]);
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_Activation::OnSelectionChanged_TargetActivationResponses(const TArray<FString>& Selected,
	const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct)
	{
		return;
	}

	BSConfig->TargetConfig.TargetActivationResponses = GetEnumArrayFromStringArray<ETargetActivationResponse>(Selected);
	UpdateDependentOptions_TargetActivationResponses(BSConfig->TargetConfig.TargetActivationResponses,
		BSConfig->TargetConfig.MinActivatedTargetSpeed == BSConfig->TargetConfig.MaxActivatedTargetSpeed);
	UpdateBrushColors();
	UpdateAllOptionsValid();
}

FString UCustomGameModesWidget_Activation::GetComboBoxEntryTooltipStringTableKey_TargetActivationSelectionPolicy(
	const FString& EnumString)
{
	const ETargetActivationSelectionPolicy EnumValue = GetEnumFromString<ETargetActivationSelectionPolicy>(EnumString);
	return GetStringTableKeyNameFromEnum(EnumValue);
}

FString UCustomGameModesWidget_Activation::GetComboBoxEntryTooltipStringTableKey_TargetActivationResponses(
	const FString& EnumString)
{
	const ETargetActivationResponse EnumValue = GetEnumFromString<ETargetActivationResponse>(EnumString);
	return GetStringTableKeyNameFromEnum(EnumValue);
}
