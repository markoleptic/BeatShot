// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubmenuWidgets/GameModesWidgets/Components/CGMWC_ConditionsAndResponses.h"

void UCGMWC_ConditionsAndResponses::NativeConstruct()
{
	Super::NativeConstruct();

	ComboBoxOption_TargetSpawnResponses->ComboBox->OnSelectionChanged.AddUniqueDynamic(this,
		&ThisClass::OnSelectionChanged_TargetSpawnResponses);
	ComboBoxOption_TargetActivationResponses->ComboBox->OnSelectionChanged.AddUniqueDynamic(this,
		&ThisClass::OnSelectionChanged_TargetActivationResponses);
	ComboBoxOption_TargetDeactivationConditions->ComboBox->OnSelectionChanged.AddUniqueDynamic(this,
		&ThisClass::OnSelectionChanged_TargetDeactivationConditions);
	ComboBoxOption_TargetDeactivationResponses->ComboBox->OnSelectionChanged.AddUniqueDynamic(this,
		&ThisClass::OnSelectionChanged_TargetDeactivationResponses);
	ComboBoxOption_TargetDestructionConditions->ComboBox->OnSelectionChanged.AddUniqueDynamic(this,
		&ThisClass::OnSelectionChanged_TargetDestructionConditions);

	ComboBoxOption_TargetSpawnResponses->GetComboBoxEntryTooltipStringTableKey.BindUObject(this,
		&ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetSpawnResponses);
	ComboBoxOption_TargetActivationResponses->GetComboBoxEntryTooltipStringTableKey.BindUObject(this,
		&ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetActivationResponses);
	ComboBoxOption_TargetDeactivationConditions->GetComboBoxEntryTooltipStringTableKey.BindUObject(this,
		&ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetDeactivationConditions);
	ComboBoxOption_TargetDeactivationResponses->GetComboBoxEntryTooltipStringTableKey.BindUObject(this,
		&ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetDeactivationResponses);
	ComboBoxOption_TargetDestructionConditions->GetComboBoxEntryTooltipStringTableKey.BindUObject(this,
		&ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetDestructionConditions);

	ComboBoxOption_TargetSpawnResponses->ComboBox->ClearOptions();
	ComboBoxOption_TargetActivationResponses->ComboBox->ClearOptions();
	ComboBoxOption_TargetDeactivationConditions->ComboBox->ClearOptions();
	ComboBoxOption_TargetDeactivationResponses->ComboBox->ClearOptions();
	ComboBoxOption_TargetDestructionConditions->ComboBox->ClearOptions();

	TArray<FString> Options;
	for (const ETargetSpawnResponse& Method : TEnumRange<ETargetSpawnResponse>())
	{
		Options.Add(GetStringFromEnum_FromTagMap(Method));
	}
	ComboBoxOption_TargetSpawnResponses->SortAddOptionsAndSetEnumType<ETargetSpawnResponse>(Options);
	Options.Empty();

	for (const ETargetActivationResponse& Method : TEnumRange<ETargetActivationResponse>())
	{
		if (Method != ETargetActivationResponse::ChangeScale) // Deprecated
		{
			Options.Add(GetStringFromEnum_FromTagMap(Method));
		}
	}
	ComboBoxOption_TargetActivationResponses->SortAddOptionsAndSetEnumType<ETargetActivationResponse>(Options);
	Options.Empty();

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

	SetupWarningTooltipCallbacks();
	UpdateBrushColors();
}

void UCGMWC_ConditionsAndResponses::UpdateAllOptionsValid()
{
	Super::UpdateAllOptionsValid();
}

void UCGMWC_ConditionsAndResponses::UpdateOptionsFromConfig()
{
	UpdateValueIfDifferent(ComboBoxOption_TargetSpawnResponses,
		GetStringArrayFromEnumArray_FromTagMap(BSConfig->TargetConfig.TargetSpawnResponses));
	UpdateValueIfDifferent(ComboBoxOption_TargetActivationResponses,
		GetStringArrayFromEnumArray_FromTagMap(BSConfig->TargetConfig.TargetActivationResponses));
	UpdateValueIfDifferent(ComboBoxOption_TargetDeactivationConditions,
		GetStringArrayFromEnumArray_FromTagMap(BSConfig->TargetConfig.TargetDeactivationConditions));
	UpdateValueIfDifferent(ComboBoxOption_TargetDeactivationResponses,
		GetStringArrayFromEnumArray_FromTagMap(BSConfig->TargetConfig.TargetDeactivationResponses));
	UpdateValueIfDifferent(ComboBoxOption_TargetDestructionConditions,
		GetStringArrayFromEnumArray_FromTagMap(BSConfig->TargetConfig.TargetDestructionConditions));

	UpdateDependentOptions_TargetSpawnResponses(BSConfig->TargetConfig.TargetSpawnResponses);
	UpdateDependentOptions_TargetActivationResponses(BSConfig->TargetConfig.TargetActivationResponses);
	UpdateDependentOptions_TargetDeactivationConditions(BSConfig->TargetConfig.TargetDeactivationConditions);
	UpdateDependentOptions_TargetDeactivationResponses(BSConfig->TargetConfig.TargetDeactivationResponses);

	UpdateBrushColors();
}

void UCGMWC_ConditionsAndResponses::SetupWarningTooltipCallbacks()
{
	ComboBoxOption_TargetSpawnResponses->AddWarningTooltipData(FTooltipData("Invalid_Velocity_MTDM_None",
		ETooltipImageType::Caution)).BindLambda([this]()
	{
		return BSConfig->TargetConfig.TargetSpawnResponses.Contains(ETargetSpawnResponse::ChangeVelocity) && BSConfig->
			TargetConfig.MovingTargetDirectionMode == EMovingTargetDirectionMode::None;
	});
	ComboBoxOption_TargetSpawnResponses->AddWarningTooltipData(FTooltipData("Invalid_Direction_MTDM_None",
		ETooltipImageType::Caution)).BindLambda([this]()
	{
		return BSConfig->TargetConfig.MovingTargetDirectionMode == EMovingTargetDirectionMode::None && BSConfig->
			TargetConfig.TargetSpawnResponses.Contains(ETargetSpawnResponse::ChangeDirection);
	});
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

void UCGMWC_ConditionsAndResponses::UpdateDependentOptions_TargetDeactivationConditions(
	const TArray<ETargetDeactivationCondition>& Conditions)
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
}

void UCGMWC_ConditionsAndResponses::UpdateDependentOptions_TargetDeactivationResponses(
	const TArray<ETargetDeactivationResponse>& Responses)
{
}

void UCGMWC_ConditionsAndResponses::UpdateDependentOptions_TargetSpawnResponses(
	const TArray<ETargetSpawnResponse>& Responses)
{
}

void UCGMWC_ConditionsAndResponses::UpdateDependentOptions_TargetActivationResponses(
	const TArray<ETargetActivationResponse>& Responses)
{
}

void UCGMWC_ConditionsAndResponses::OnSelectionChanged_TargetActivationResponses(const TArray<FString>& Selected,
	const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct) return;

	if (Selected.IsEmpty()) BSConfig->TargetConfig.TargetActivationResponses.Empty();
	else BSConfig->TargetConfig.TargetActivationResponses = GetEnumArrayFromStringArray<
		ETargetActivationResponse>(Selected);

	UpdateDependentOptions_TargetActivationResponses(BSConfig->TargetConfig.TargetActivationResponses);
	UpdateBrushColors();
	UpdateAllOptionsValid();
}

void UCGMWC_ConditionsAndResponses::OnSelectionChanged_TargetSpawnResponses(const TArray<FString>& Selected,
	const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct) return;

	if (Selected.IsEmpty()) BSConfig->TargetConfig.TargetSpawnResponses.Empty();
	else BSConfig->TargetConfig.TargetSpawnResponses = GetEnumArrayFromStringArray<ETargetSpawnResponse>(Selected);

	UpdateDependentOptions_TargetSpawnResponses(BSConfig->TargetConfig.TargetSpawnResponses);
	UpdateBrushColors();
	UpdateAllOptionsValid();
}

void UCGMWC_ConditionsAndResponses::OnSelectionChanged_TargetDeactivationConditions(const TArray<FString>& Selected,
	const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct) return;

	if (Selected.IsEmpty()) BSConfig->TargetConfig.TargetDeactivationConditions.Empty();
	else
		BSConfig->TargetConfig.TargetDeactivationConditions = GetEnumArrayFromStringArray<
			ETargetDeactivationCondition>(Selected);

	UpdateDependentOptions_TargetDeactivationConditions(BSConfig->TargetConfig.TargetDeactivationConditions);
	UpdateBrushColors();
	UpdateAllOptionsValid();
}

void UCGMWC_ConditionsAndResponses::OnSelectionChanged_TargetDeactivationResponses(const TArray<FString>& Selected,
	const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct) return;

	if (Selected.IsEmpty()) BSConfig->TargetConfig.TargetDeactivationResponses.Empty();
	else
		BSConfig->TargetConfig.TargetDeactivationResponses = GetEnumArrayFromStringArray<
			ETargetDeactivationResponse>(Selected);

	UpdateDependentOptions_TargetDeactivationResponses(BSConfig->TargetConfig.TargetDeactivationResponses);
	UpdateBrushColors();
	UpdateAllOptionsValid();
}

void UCGMWC_ConditionsAndResponses::OnSelectionChanged_TargetDestructionConditions(const TArray<FString>& Selected,
	const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct) return;

	if (Selected.IsEmpty()) BSConfig->TargetConfig.TargetDestructionConditions.Empty();
	else
		BSConfig->TargetConfig.TargetDestructionConditions = GetEnumArrayFromStringArray<
			ETargetDestructionCondition>(Selected);

	UpdateBrushColors();
	UpdateAllOptionsValid();
}

FString UCGMWC_ConditionsAndResponses::GetComboBoxEntryTooltipStringTableKey_TargetSpawnResponses(
	const FString& EnumString)
{
	return GetStringTableKeyNameFromEnum(GetEnumFromString_FromTagMap<ETargetSpawnResponse>(EnumString));
}

FString UCGMWC_ConditionsAndResponses::GetComboBoxEntryTooltipStringTableKey_TargetActivationResponses(
	const FString& EnumString)
{
	return GetStringTableKeyNameFromEnum(GetEnumFromString_FromTagMap<ETargetActivationResponse>(EnumString));
}

FString UCGMWC_ConditionsAndResponses::GetComboBoxEntryTooltipStringTableKey_TargetDeactivationConditions(
	const FString& EnumString)
{
	return GetStringTableKeyNameFromEnum(GetEnumFromString_FromTagMap<ETargetDeactivationCondition>(EnumString));
}

FString UCGMWC_ConditionsAndResponses::GetComboBoxEntryTooltipStringTableKey_TargetDeactivationResponses(
	const FString& EnumString)
{
	return GetStringTableKeyNameFromEnum(GetEnumFromString_FromTagMap<ETargetDeactivationResponse>(EnumString));
}

FString UCGMWC_ConditionsAndResponses::GetComboBoxEntryTooltipStringTableKey_TargetDestructionConditions(
	const FString& EnumString)
{
	return GetStringTableKeyNameFromEnum(GetEnumFromString_FromTagMap<ETargetDestructionCondition>(EnumString));
}
