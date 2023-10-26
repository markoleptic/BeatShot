// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/GameModesWidgets/Components/CGMWC_Spawning.h"
#include "Components/CheckBox.h"
#include "WidgetComponents/Boxes/BSComboBoxString.h"
#include "WidgetComponents/MenuOptionWidgets/CheckBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/ComboBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/SliderTextBoxOptionWidget.h"


void UCGMWC_Spawning::NativeConstruct()
{
	Super::NativeConstruct();

	SliderTextBoxOption_MaxNumTargetsAtOnce->SetValues(MinValue_MaxNumTargetsAtOnce, MaxValue_MaxNumTargetsAtOnce,
		SnapSize_MaxNumTargetsAtOnce);
	SliderTextBoxOption_NumUpfrontTargetsToSpawn->SetValues(MinValue_NumUpfrontTargetsToSpawn,
		MaxValue_NumUpfrontTargetsToSpawn, SnapSize_NumUpfrontTargetsToSpawn);
	SliderTextBoxOption_NumRuntimeTargetsToSpawn->SetValues(MinValue_NumRuntimeTargetsToSpawn,
		MaxValue_NumRuntimeTargetsToSpawn, SnapSize_NumRuntimeTargetsToSpawn);

	SliderTextBoxOption_MaxNumTargetsAtOnce->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_NumUpfrontTargetsToSpawn->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_NumRuntimeTargetsToSpawn->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);

	ComboBoxOption_TargetSpawningPolicy->ComboBox->OnSelectionChanged.AddUniqueDynamic(this,
		&ThisClass::OnSelectionChanged_TargetSpawningPolicy);
	ComboBoxOption_RuntimeTargetSpawningLocationSelectionMode->ComboBox->OnSelectionChanged.AddUniqueDynamic(this,
		&ThisClass::OnSelectionChanged_RuntimeTargetSpawningLocationSelectionMode);

	ComboBoxOption_TargetSpawningPolicy->GetComboBoxEntryTooltipStringTableKey.BindUObject(this,
		&ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetSpawningPolicy);
	ComboBoxOption_RuntimeTargetSpawningLocationSelectionMode->GetComboBoxEntryTooltipStringTableKey.BindUObject(this,
		&ThisClass::GetComboBoxEntryTooltipStringTableKey_RuntimeTargetSpawningLocationSelectionMode);

	CheckBoxOption_AllowSpawnWithoutActivation->CheckBox->OnCheckStateChanged.AddDynamic(this,
		&ThisClass::OnCheckStateChanged_AllowSpawnWithoutActivation);
	CheckBoxOption_BatchSpawning->CheckBox->OnCheckStateChanged.AddDynamic(this,
		&ThisClass::OnCheckStateChanged_BatchSpawning);
	CheckBoxOption_SpawnAtOriginWheneverPossible->CheckBox->OnCheckStateChanged.AddDynamic(this,
		&ThisClass::OnCheckStateChanged_SpawnAtOriginWheneverPossible);
	CheckBoxOption_SpawnEveryOtherTargetInCenter->CheckBox->OnCheckStateChanged.AddDynamic(this,
		&ThisClass::OnCheckStateChanged_SpawnEveryOtherTargetInCenter);

	ComboBoxOption_TargetSpawningPolicy->ComboBox->ClearOptions();
	ComboBoxOption_RuntimeTargetSpawningLocationSelectionMode->ComboBox->ClearOptions();

	TArray<FString> Options;
	for (const ETargetSpawningPolicy& Method : TEnumRange<ETargetSpawningPolicy>())
	{
		Options.Add(GetStringFromEnum_FromTagMap(Method));
	}
	ComboBoxOption_TargetSpawningPolicy->SortAddOptionsAndSetEnumType<ETargetSpawningPolicy>(Options);
	Options.Empty();

	for (const ERuntimeTargetSpawningLocationSelectionMode& Method : TEnumRange<
		     ERuntimeTargetSpawningLocationSelectionMode>())
	{
		Options.Add(GetStringFromEnum_FromTagMap(Method));
	}
	ComboBoxOption_RuntimeTargetSpawningLocationSelectionMode->SortAddOptionsAndSetEnumType<
		ERuntimeTargetSpawningLocationSelectionMode>(Options);
	Options.Empty();

	SliderTextBoxOption_NumUpfrontTargetsToSpawn->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_NumRuntimeTargetsToSpawn->SetVisibility(ESlateVisibility::Collapsed);

	SetupWarningTooltipCallbacks();
	UpdateBrushColors();
}

void UCGMWC_Spawning::UpdateAllOptionsValid()
{
	Super::UpdateAllOptionsValid();
}

void UCGMWC_Spawning::UpdateOptionsFromConfig()
{
	UpdateValueIfDifferent(ComboBoxOption_RuntimeTargetSpawningLocationSelectionMode,
		GetStringFromEnum_FromTagMap(BSConfig->TargetConfig.RuntimeTargetSpawningLocationSelectionMode));
	UpdateValueIfDifferent(ComboBoxOption_TargetSpawningPolicy,
		GetStringFromEnum_FromTagMap(BSConfig->TargetConfig.TargetSpawningPolicy));

	UpdateValueIfDifferent(SliderTextBoxOption_MaxNumTargetsAtOnce, BSConfig->TargetConfig.MaxNumTargetsAtOnce);
	UpdateValueIfDifferent(SliderTextBoxOption_NumUpfrontTargetsToSpawn,
		BSConfig->TargetConfig.NumUpfrontTargetsToSpawn);
	UpdateValueIfDifferent(SliderTextBoxOption_NumRuntimeTargetsToSpawn,
		BSConfig->TargetConfig.NumRuntimeTargetsToSpawn);

	UpdateValueIfDifferent(CheckBoxOption_AllowSpawnWithoutActivation,
		BSConfig->TargetConfig.bAllowSpawnWithoutActivation);
	UpdateValueIfDifferent(CheckBoxOption_BatchSpawning, BSConfig->TargetConfig.bUseBatchSpawning);
	UpdateValueIfDifferent(CheckBoxOption_SpawnAtOriginWheneverPossible,
		BSConfig->TargetConfig.bSpawnAtOriginWheneverPossible);
	UpdateValueIfDifferent(CheckBoxOption_SpawnEveryOtherTargetInCenter,
		BSConfig->TargetConfig.bSpawnEveryOtherTargetInCenter);

	UpdateDependentOptions_TargetSpawningPolicy(BSConfig->TargetConfig.TargetSpawningPolicy,
		BSConfig->TargetConfig.bUseBatchSpawning, BSConfig->TargetConfig.bAllowSpawnWithoutActivation);

	UpdateBrushColors();
}

void UCGMWC_Spawning::SetupWarningTooltipCallbacks()
{
	CheckBoxOption_SpawnEveryOtherTargetInCenter->AddWarningTooltipData(
		FTooltipData("Invalid_SpawnEveryOtherTargetInCenter_BatchSpawning", ETooltipImageType::Warning)).BindLambda(
		[this]()
		{
			return BSConfig->TargetConfig.bSpawnEveryOtherTargetInCenter && BSConfig->TargetConfig.bUseBatchSpawning;
		});
	CheckBoxOption_SpawnEveryOtherTargetInCenter->AddWarningTooltipData(
		FTooltipData("Invalid_SpawnEveryOtherTargetInCenter_AllowSpawnWithoutActivation",
			ETooltipImageType::Warning)).BindLambda([this]()
	{
		return BSConfig->TargetConfig.bSpawnEveryOtherTargetInCenter && BSConfig->TargetConfig.
			bAllowSpawnWithoutActivation;
	});
	CheckBoxOption_BatchSpawning->AddWarningTooltipData(
		FTooltipData("Invalid_SpawnEveryOtherTargetInCenter_BatchSpawning2", ETooltipImageType::Warning)).BindLambda(
		[this]()
		{
			return BSConfig->TargetConfig.bSpawnEveryOtherTargetInCenter && BSConfig->TargetConfig.bUseBatchSpawning;
		});
	CheckBoxOption_AllowSpawnWithoutActivation->AddWarningTooltipData(
		FTooltipData("Invalid_SpawnEveryOtherTargetInCenter_AllowSpawnWithoutActivation2",
			ETooltipImageType::Warning)).BindLambda([this]()
	{
		return BSConfig->TargetConfig.bSpawnEveryOtherTargetInCenter && BSConfig->TargetConfig.
			bAllowSpawnWithoutActivation;
	});
}

void UCGMWC_Spawning::UpdateDependentOptions_TargetSpawningPolicy(const ETargetSpawningPolicy& InTargetSpawningPolicy,
	const bool bUseBatchSpawning, const bool bAllowSpawnWithoutActivation)
{
	if (InTargetSpawningPolicy == ETargetSpawningPolicy::RuntimeOnly)
	{
		CheckBoxOption_BatchSpawning->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		CheckBoxOption_SpawnAtOriginWheneverPossible->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		// Do not allow force spawning every other target in center if these conditions are not true
		if (!bUseBatchSpawning && !bAllowSpawnWithoutActivation)
		{
			CheckBoxOption_SpawnEveryOtherTargetInCenter->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			CheckBoxOption_SpawnEveryOtherTargetInCenter->SetVisibility(ESlateVisibility::Collapsed);
		}

		SliderTextBoxOption_NumUpfrontTargetsToSpawn->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_NumRuntimeTargetsToSpawn->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		if (BSConfig->TargetConfig.TargetDistributionPolicy == ETargetDistributionPolicy::Grid)
		{
			ComboBoxOption_RuntimeTargetSpawningLocationSelectionMode->ComboBox->SetIsEnabled(true);
		}
		else
		{
			BSConfig->TargetConfig.RuntimeTargetSpawningLocationSelectionMode =
				ERuntimeTargetSpawningLocationSelectionMode::Random;
			ComboBoxOption_RuntimeTargetSpawningLocationSelectionMode->ComboBox->SetSelectedOption(
				GetStringFromEnum_FromTagMap(BSConfig->TargetConfig.RuntimeTargetSpawningLocationSelectionMode));
			ComboBoxOption_RuntimeTargetSpawningLocationSelectionMode->ComboBox->SetIsEnabled(false);
		}

		ComboBoxOption_RuntimeTargetSpawningLocationSelectionMode->
			SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else if (InTargetSpawningPolicy == ETargetSpawningPolicy::UpfrontOnly)
	{
		BSConfig->TargetConfig.bUseBatchSpawning = false;
		BSConfig->TargetConfig.bSpawnEveryOtherTargetInCenter = false;
		BSConfig->TargetConfig.bSpawnAtOriginWheneverPossible = false;

		UpdateValueIfDifferent(CheckBoxOption_BatchSpawning, false);
		UpdateValueIfDifferent(CheckBoxOption_SpawnAtOriginWheneverPossible, false);
		UpdateValueIfDifferent(CheckBoxOption_SpawnEveryOtherTargetInCenter, false);

		CheckBoxOption_BatchSpawning->SetVisibility(ESlateVisibility::Collapsed);
		CheckBoxOption_SpawnAtOriginWheneverPossible->SetVisibility(ESlateVisibility::Collapsed);
		CheckBoxOption_SpawnEveryOtherTargetInCenter->SetVisibility(ESlateVisibility::Collapsed);

		SliderTextBoxOption_NumUpfrontTargetsToSpawn->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_NumRuntimeTargetsToSpawn->SetVisibility(ESlateVisibility::Collapsed);

		ComboBoxOption_RuntimeTargetSpawningLocationSelectionMode->SetVisibility(ESlateVisibility::Collapsed);

		if (BSConfig->TargetConfig.TargetDistributionPolicy == ETargetDistributionPolicy::Grid)
		{
			SliderTextBoxOption_NumUpfrontTargetsToSpawn->SetSliderAndTextBoxEnabledStates(false);
			SliderTextBoxOption_NumUpfrontTargetsToSpawn->SetValue(BSConfig->GridConfig.NumHorizontalGridTargets * BSConfig->GridConfig.NumVerticalGridTargets);
			SliderTextBoxOption_MaxNumTargetsAtOnce->SetSliderAndTextBoxEnabledStates(false);
			SliderTextBoxOption_MaxNumTargetsAtOnce->SetValue(-1.f);
		}
		else
		{
			SliderTextBoxOption_NumUpfrontTargetsToSpawn->SetSliderAndTextBoxEnabledStates(true);
			SliderTextBoxOption_MaxNumTargetsAtOnce->SetSliderAndTextBoxEnabledStates(true);
		}
	}
}

void UCGMWC_Spawning::OnCheckStateChanged_AllowSpawnWithoutActivation(const bool bChecked)
{
	BSConfig->TargetConfig.bAllowSpawnWithoutActivation = bChecked;
	UpdateAllOptionsValid();
}

void UCGMWC_Spawning::OnCheckStateChanged_BatchSpawning(const bool bChecked)
{
	BSConfig->TargetConfig.bUseBatchSpawning = bChecked;
	UpdateAllOptionsValid();
}

void UCGMWC_Spawning::OnCheckStateChanged_SpawnAtOriginWheneverPossible(const bool bChecked)
{
	BSConfig->TargetConfig.bSpawnAtOriginWheneverPossible = bChecked;
	UpdateAllOptionsValid();
}

void UCGMWC_Spawning::OnCheckStateChanged_SpawnEveryOtherTargetInCenter(const bool bChecked)
{
	BSConfig->TargetConfig.bSpawnEveryOtherTargetInCenter = bChecked;
	UpdateAllOptionsValid();
}

void UCGMWC_Spawning::OnSliderTextBoxValueChanged(USliderTextBoxOptionWidget* Widget, const float Value)
{
	if (Widget == SliderTextBoxOption_MaxNumTargetsAtOnce)
	{
		BSConfig->TargetConfig.MaxNumTargetsAtOnce = Value;
	}
	else if (Widget == SliderTextBoxOption_NumUpfrontTargetsToSpawn)
	{
		BSConfig->TargetConfig.NumUpfrontTargetsToSpawn = Value;
	}
	else if (Widget == SliderTextBoxOption_NumRuntimeTargetsToSpawn)
	{
		BSConfig->TargetConfig.NumRuntimeTargetsToSpawn = Value;
	}

	UpdateAllOptionsValid();
}

void UCGMWC_Spawning::OnSelectionChanged_RuntimeTargetSpawningLocationSelectionMode(const TArray<FString>& Selected,
	const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || Selected.Num() != 1) return;

	BSConfig->TargetConfig.RuntimeTargetSpawningLocationSelectionMode = GetEnumFromString_FromTagMap<
		ERuntimeTargetSpawningLocationSelectionMode>(Selected[0]);

	UpdateBrushColors();
	UpdateAllOptionsValid();
}

void UCGMWC_Spawning::OnSelectionChanged_TargetSpawningPolicy(const TArray<FString>& Selected,
	const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || Selected.Num() != 1) return;

	BSConfig->TargetConfig.TargetSpawningPolicy = GetEnumFromString_FromTagMap<ETargetSpawningPolicy>(Selected[0]);

	UpdateDependentOptions_TargetSpawningPolicy(BSConfig->TargetConfig.TargetSpawningPolicy,
		BSConfig->TargetConfig.bUseBatchSpawning, BSConfig->TargetConfig.bSpawnEveryOtherTargetInCenter);
	UpdateBrushColors();
	UpdateAllOptionsValid();
}

FString UCGMWC_Spawning::GetComboBoxEntryTooltipStringTableKey_RuntimeTargetSpawningLocationSelectionMode(
	const FString& EnumString)
{
	const ERuntimeTargetSpawningLocationSelectionMode EnumValue = GetEnumFromString_FromTagMap<
		ERuntimeTargetSpawningLocationSelectionMode>(EnumString);
	return GetStringTableKeyNameFromEnum(EnumValue);
}

FString UCGMWC_Spawning::GetComboBoxEntryTooltipStringTableKey_TargetSpawningPolicy(const FString& EnumString)
{
	const ETargetSpawningPolicy EnumValue = GetEnumFromString_FromTagMap<ETargetSpawningPolicy>(EnumString);
	return GetStringTableKeyNameFromEnum(EnumValue);
}
