// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/GameModesWidgets/Components/CustomGameModesWidget_Deactivation.h"
#include "Components/CheckBox.h"
#include "WidgetComponents/Boxes/BSComboBoxString.h"
#include "WidgetComponents/MenuOptionWidgets/CheckBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/ComboBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/SliderTextBoxOptionWidget.h"

void UCustomGameModesWidget_Deactivation::InitComponent(FBSConfig* InConfigPtr,
	TObjectPtr<UCustomGameModesWidgetComponent> InNext)
{
	Super::InitComponent(InConfigPtr, InNext);
}

void UCustomGameModesWidget_Deactivation::NativeConstruct()
{
	Super::NativeConstruct();

	SliderTextBoxOption_DeactivatedTargetScaleMultiplier->SetValues(MinValue_ConsecutiveChargeScaleMultiplier,
		MaxValue_ConsecutiveChargeScaleMultiplier, SnapSize_ConsecutiveChargeScaleMultiplier);
	SliderTextBoxOption_DeactivatedTargetVelocity->SetValues(MinValue_TargetSpeed, MaxValue_TargetSpeed,
		SnapSize_TargetSpeed);
	SliderTextBoxOption_MinDeactivatedTargetVelocity->SetValues(MinValue_TargetSpeed, MaxValue_TargetSpeed,
		SnapSize_TargetSpeed);
	SliderTextBoxOption_MaxDeactivatedTargetVelocity->SetValues(MinValue_TargetSpeed, MaxValue_TargetSpeed,
		SnapSize_TargetSpeed);

	SliderTextBoxOption_DeactivatedTargetScaleMultiplier->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_DeactivatedTargetVelocity->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MinDeactivatedTargetVelocity->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MaxDeactivatedTargetVelocity->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);

	ComboBoxOption_TargetDeactivationConditions->ComboBox->OnSelectionChanged.AddUniqueDynamic(this,
		&ThisClass::OnSelectionChanged_TargetDeactivationConditions);
	ComboBoxOption_TargetDeactivationResponses->ComboBox->OnSelectionChanged.AddUniqueDynamic(this,
		&ThisClass::OnSelectionChanged_TargetDeactivationResponses);
	ComboBoxOption_TargetDestructionConditions->ComboBox->OnSelectionChanged.AddUniqueDynamic(this,
		&ThisClass::OnSelectionChanged_TargetDestructionConditions);

	ComboBoxOption_TargetDeactivationConditions->GetComboBoxEntryTooltipStringTableKey.BindUObject(this,
		&ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetDeactivationConditions);
	ComboBoxOption_TargetDeactivationResponses->GetComboBoxEntryTooltipStringTableKey.BindUObject(this,
		&ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetDeactivationResponses);
	ComboBoxOption_TargetDestructionConditions->GetComboBoxEntryTooltipStringTableKey.BindUObject(this,
		&ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetDestructionConditions);

	CheckBoxOption_ConstantDeactivatedTargetVelocity->CheckBox->OnCheckStateChanged.AddDynamic(this,
		&ThisClass::OnCheckStateChanged_ConstantDeactivatedTargetVelocity);

	ComboBoxOption_TargetDeactivationConditions->ComboBox->ClearOptions();
	ComboBoxOption_TargetDeactivationResponses->ComboBox->ClearOptions();
	ComboBoxOption_TargetDestructionConditions->ComboBox->ClearOptions();

	TArray<FString> Options;
	for (const ETargetDeactivationCondition& Method : TEnumRange<ETargetDeactivationCondition>())
	{
		Options.Add(GetStringFromEnum_FromTagMap(Method));
	}
	ComboBoxOption_TargetDeactivationConditions->SortAddOptionsAndSetEnumType<ETargetDeactivationCondition>(Options);
	Options.Empty();

	for (const ETargetDeactivationResponse& Method : TEnumRange<ETargetDeactivationResponse>())
	{
		Options.Add(GetStringFromEnum_FromTagMap(Method));
	}
	ComboBoxOption_TargetDeactivationResponses->SortAddOptionsAndSetEnumType<ETargetDeactivationResponse>(Options);
	Options.Empty();

	for (const ETargetDestructionCondition& Method : TEnumRange<ETargetDestructionCondition>())
	{
		Options.Add(GetStringFromEnum_FromTagMap(Method));
	}
	ComboBoxOption_TargetDestructionConditions->SortAddOptionsAndSetEnumType<ETargetDestructionCondition>(Options);
	Options.Empty();

	CheckBoxOption_ConstantDeactivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_DeactivatedTargetScaleMultiplier->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_DeactivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MinDeactivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MaxDeactivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);

	SetupWarningTooltipCallbacks();
	UpdateBrushColors();
}

void UCustomGameModesWidget_Deactivation::UpdateAllOptionsValid()
{
	Super::UpdateAllOptionsValid();
}

void UCustomGameModesWidget_Deactivation::UpdateOptionsFromConfig()
{
	UpdateValueIfDifferent(ComboBoxOption_TargetDeactivationConditions,
		GetStringArrayFromEnumArray_FromTagMap(BSConfig->TargetConfig.TargetDeactivationConditions));
	UpdateValueIfDifferent(ComboBoxOption_TargetDeactivationResponses,
		GetStringArrayFromEnumArray_FromTagMap(BSConfig->TargetConfig.TargetDeactivationResponses));
	UpdateValueIfDifferent(SliderTextBoxOption_DeactivatedTargetScaleMultiplier,
		BSConfig->TargetConfig.ConsecutiveChargeScaleMultiplier);
	UpdateValueIfDifferent(ComboBoxOption_TargetDestructionConditions,
		GetStringArrayFromEnumArray_FromTagMap(BSConfig->TargetConfig.TargetDestructionConditions));

	UpdateValueIfDifferent(CheckBoxOption_ConstantDeactivatedTargetVelocity,
		BSConfig->TargetConfig.MinDeactivatedTargetSpeed == BSConfig->TargetConfig.MaxDeactivatedTargetSpeed);
	UpdateValueIfDifferent(SliderTextBoxOption_DeactivatedTargetVelocity,
		BSConfig->TargetConfig.MinDeactivatedTargetSpeed);
	UpdateValueIfDifferent(SliderTextBoxOption_MinDeactivatedTargetVelocity,
		BSConfig->TargetConfig.MinDeactivatedTargetSpeed);
	UpdateValueIfDifferent(SliderTextBoxOption_MaxDeactivatedTargetVelocity,
		BSConfig->TargetConfig.MaxDeactivatedTargetSpeed);

	UpdateDependentOptions_TargetDeactivationConditions(BSConfig->TargetConfig.TargetDeactivationConditions,
		BSConfig->TargetConfig.TargetDeactivationResponses);

	UpdateBrushColors();
}

void UCustomGameModesWidget_Deactivation::SetupWarningTooltipCallbacks()
{
	ComboBoxOption_TargetDeactivationResponses->AddWarningTooltipData(FTooltipData("Invalid_Velocity_MTDM_None",
		ETooltipImageType::Caution)).BindLambda([this]()
	{
		return BSConfig->TargetConfig.MovingTargetDirectionMode == EMovingTargetDirectionMode::None && BSConfig->
			TargetConfig.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::ChangeVelocity);
	});
	ComboBoxOption_TargetDeactivationResponses->AddWarningTooltipData(FTooltipData("Invalid_Direction_MTDM_None",
		ETooltipImageType::Caution)).BindLambda([this]()
	{
		return BSConfig->TargetConfig.MovingTargetDirectionMode == EMovingTargetDirectionMode::None && BSConfig->
			TargetConfig.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::ChangeDirection);
	});
}

void UCustomGameModesWidget_Deactivation::UpdateDependentOptions_TargetDeactivationConditions(
	const TArray<ETargetDeactivationCondition>& Conditions, const TArray<ETargetDeactivationResponse>& Responses)
{
	// Persistant Deactivation Condition
	if (Conditions.Contains(ETargetDeactivationCondition::Persistant))
	{
		if (!BSConfig->TargetConfig.TargetDeactivationResponses.IsEmpty())
		{
			BSConfig->TargetConfig.TargetDeactivationResponses.Empty();
		}
		UpdateValueIfDifferent(ComboBoxOption_TargetDeactivationResponses, TArray<FString>());
		ComboBoxOption_TargetDeactivationResponses->ComboBox->SetIsEnabled(false);
	}
	// No Persistant Deactivation Condition
	else
	{
		ComboBoxOption_TargetDeactivationResponses->ComboBox->SetIsEnabled(true);
	}
	UpdateDependentOptions_TargetDeactivationResponses(Conditions, Responses);
}

void UCustomGameModesWidget_Deactivation::UpdateDependentOptions_TargetDeactivationResponses(
	const TArray<ETargetDeactivationCondition>& Conditions, const TArray<ETargetDeactivationResponse>& Responses)
{
	// Collapse any Responses dependent upon Conditions
	if (Conditions.Contains(ETargetDeactivationCondition::Persistant))
	{
		SliderTextBoxOption_DeactivatedTargetScaleMultiplier->SetVisibility(ESlateVisibility::Collapsed);
	}

	// Show any Responses dependent upon Conditions
	if (!Conditions.Contains(ETargetDeactivationCondition::Persistant))
	{
		if (Responses.Contains(ETargetDeactivationResponse::ApplyDeactivatedTargetScaleMultiplier))
		{
			SliderTextBoxOption_DeactivatedTargetScaleMultiplier->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			SliderTextBoxOption_DeactivatedTargetScaleMultiplier->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// Change Velocity
	if (Responses.Contains(ETargetDeactivationResponse::ChangeVelocity))
	{
		CheckBoxOption_ConstantDeactivatedTargetVelocity->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		CheckBoxOption_ConstantDeactivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
	}

	UpdateDependentOptions_ConstantDeactivatedTargetVelocity(Responses,
		BSConfig->TargetConfig.MinDeactivatedTargetSpeed == BSConfig->TargetConfig.MaxDeactivatedTargetSpeed);
}

void UCustomGameModesWidget_Deactivation::UpdateDependentOptions_ConstantDeactivatedTargetVelocity(
	const TArray<ETargetDeactivationResponse>& Responses, const bool bInConstant)
{
	// Hide all speed options if Change Velocity not selected as a TargetDeactivationResponse
	if (!Responses.Contains(ETargetDeactivationResponse::ChangeVelocity))
	{
		SliderTextBoxOption_DeactivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MinDeactivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MaxDeactivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	// Change Velocity is selected as a TargetDeactivationResponse
	if (bInConstant)
	{
		SliderTextBoxOption_DeactivatedTargetVelocity->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_MinDeactivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MaxDeactivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		SliderTextBoxOption_DeactivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MinDeactivatedTargetVelocity->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_MaxDeactivatedTargetVelocity->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UCustomGameModesWidget_Deactivation::OnCheckStateChanged_ConstantDeactivatedTargetVelocity(const bool bChecked)
{
	if (bChecked)
	{
		BSConfig->TargetConfig.MinDeactivatedTargetSpeed = SliderTextBoxOption_DeactivatedTargetVelocity->
			GetSliderValue();
		BSConfig->TargetConfig.MaxDeactivatedTargetSpeed = SliderTextBoxOption_DeactivatedTargetVelocity->
			GetSliderValue();
	}
	else
	{
		BSConfig->TargetConfig.MinDeactivatedTargetSpeed = SliderTextBoxOption_MinDeactivatedTargetVelocity->
			GetSliderValue();
		BSConfig->TargetConfig.MaxDeactivatedTargetSpeed = SliderTextBoxOption_MaxDeactivatedTargetVelocity->
			GetSliderValue();
	}

	UpdateDependentOptions_ConstantDeactivatedTargetVelocity(BSConfig->TargetConfig.TargetDeactivationResponses,
		bChecked);
	UpdateBrushColors();
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_Deactivation::OnSliderTextBoxValueChanged(USliderTextBoxOptionWidget* Widget,
	const float Value)
{
	if (Widget == SliderTextBoxOption_DeactivatedTargetScaleMultiplier)
	{
		BSConfig->TargetConfig.ConsecutiveChargeScaleMultiplier = Value;
	}
	else if (Widget == SliderTextBoxOption_DeactivatedTargetVelocity && SliderTextBoxOption_DeactivatedTargetVelocity->
		GetVisibility() != ESlateVisibility::Collapsed)
	{
		BSConfig->TargetConfig.MinDeactivatedTargetSpeed = Value;
		BSConfig->TargetConfig.MaxDeactivatedTargetSpeed = Value;
	}
	else if (Widget == SliderTextBoxOption_MinDeactivatedTargetVelocity)
	{
		BSConfig->TargetConfig.MinDeactivatedTargetSpeed = Value;
	}
	else if (Widget == SliderTextBoxOption_MaxDeactivatedTargetVelocity)
	{
		BSConfig->TargetConfig.MaxDeactivatedTargetSpeed = Value;
	}
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_Deactivation::OnSelectionChanged_TargetDeactivationConditions(
	const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (Selected.Num() < 1 || SelectionType == ESelectInfo::Type::Direct)
	{
		return;
	}
	BSConfig->TargetConfig.TargetDeactivationConditions = GetEnumArrayFromStringArray<
		ETargetDeactivationCondition>(Selected);
	UpdateDependentOptions_TargetDeactivationConditions(BSConfig->TargetConfig.TargetDeactivationConditions,
		BSConfig->TargetConfig.TargetDeactivationResponses);
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_Deactivation::OnSelectionChanged_TargetDeactivationResponses(
	const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (Selected.Num() < 1 || SelectionType == ESelectInfo::Type::Direct)
	{
		return;
	}

	BSConfig->TargetConfig.TargetDeactivationResponses = GetEnumArrayFromStringArray<
		ETargetDeactivationResponse>(Selected);
	UpdateDependentOptions_TargetDeactivationResponses(BSConfig->TargetConfig.TargetDeactivationConditions,
		BSConfig->TargetConfig.TargetDeactivationResponses);
	UpdateBrushColors();
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_Deactivation::OnSelectionChanged_TargetDestructionConditions(
	const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (Selected.Num() < 1 || SelectionType == ESelectInfo::Type::Direct)
	{
		return;
	}

	BSConfig->TargetConfig.TargetDestructionConditions = GetEnumArrayFromStringArray<
		ETargetDestructionCondition>(Selected);
	UpdateAllOptionsValid();
}

FString UCustomGameModesWidget_Deactivation::GetComboBoxEntryTooltipStringTableKey_TargetDeactivationConditions(
	const FString& EnumString)
{
	return GetStringTableKeyNameFromEnum(GetEnumFromString<ETargetDeactivationCondition>(EnumString));
}

FString UCustomGameModesWidget_Deactivation::GetComboBoxEntryTooltipStringTableKey_TargetDeactivationResponses(
	const FString& EnumString)
{
	return GetStringTableKeyNameFromEnum(GetEnumFromString<ETargetDeactivationResponse>(EnumString));
}

FString UCustomGameModesWidget_Deactivation::GetComboBoxEntryTooltipStringTableKey_TargetDestructionConditions(
	const FString& EnumString)
{
	return GetStringTableKeyNameFromEnum(GetEnumFromString<ETargetDestructionCondition>(EnumString));
}
