// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/GameModesWidgets/Components/CustomGameModesWidget_Spawning.h"
#include "Components/CheckBox.h"
#include "WidgetComponents/Boxes/BSComboBoxString.h"
#include "WidgetComponents/MenuOptionWidgets/CheckBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/ComboBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/SliderTextBoxOptionWidget.h"


void UCustomGameModesWidget_Spawning::NativeConstruct()
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
	ComboBoxOption_TargetSpawnResponses->ComboBox->OnSelectionChanged.AddUniqueDynamic(this,
		&ThisClass::OnSelectionChanged_TargetSpawnResponses);
	ComboBoxOption_RuntimeTargetSpawningLocationSelectionMode->ComboBox->OnSelectionChanged.AddUniqueDynamic(this,
		&ThisClass::OnSelectionChanged_RuntimeTargetSpawningLocationSelectionMode);

	ComboBoxOption_TargetSpawningPolicy->GetComboBoxEntryTooltipStringTableKey.BindUObject(this,
		&ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetSpawningPolicy);
	ComboBoxOption_TargetSpawnResponses->GetComboBoxEntryTooltipStringTableKey.BindUObject(this,
		&ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetSpawnResponses);
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
	ComboBoxOption_TargetSpawnResponses->ComboBox->ClearOptions();
	ComboBoxOption_RuntimeTargetSpawningLocationSelectionMode->ComboBox->ClearOptions();

	TArray<FString> Options;

	for (const ETargetSpawningPolicy& Method : TEnumRange<ETargetSpawningPolicy>())
	{
		Options.Add(GetStringFromEnum_FromTagMap(Method));
	}
	ComboBoxOption_TargetSpawningPolicy->SortAndAddOptions(Options);
	ComboBoxOption_TargetSpawningPolicy->SetEnumType<ETargetSpawningPolicy>();
	Options.Empty();

	for (const ETargetSpawnResponse& Method : TEnumRange<ETargetSpawnResponse>())
	{
		Options.Add(GetStringFromEnum_FromTagMap(Method));
	}
	ComboBoxOption_TargetSpawnResponses->SortAndAddOptions(Options);
	ComboBoxOption_TargetSpawnResponses->SetEnumType<ETargetSpawnResponse>();
	Options.Empty();

	for (const ERuntimeTargetSpawningLocationSelectionMode& Method : TEnumRange<
		     ERuntimeTargetSpawningLocationSelectionMode>())
	{
		Options.Add(GetStringFromEnum_FromTagMap(Method));
	}
	ComboBoxOption_RuntimeTargetSpawningLocationSelectionMode->SortAndAddOptions(Options);
	ComboBoxOption_RuntimeTargetSpawningLocationSelectionMode->SetEnumType<
		ERuntimeTargetSpawningLocationSelectionMode>();
	Options.Empty();

	SliderTextBoxOption_NumUpfrontTargetsToSpawn->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_NumRuntimeTargetsToSpawn->SetVisibility(ESlateVisibility::Collapsed);

	SetupWarningTooltipCallbacks();
	UpdateBrushColors();
}

void UCustomGameModesWidget_Spawning::UpdateAllOptionsValid()
{
	Super::UpdateAllOptionsValid();
}

void UCustomGameModesWidget_Spawning::UpdateOptionsFromConfig()
{
	UpdateValueIfDifferent(ComboBoxOption_RuntimeTargetSpawningLocationSelectionMode,
		GetStringFromEnum_FromTagMap(BSConfig->TargetConfig.RuntimeTargetSpawningLocationSelectionMode));
	UpdateValueIfDifferent(ComboBoxOption_TargetSpawningPolicy,
		GetStringFromEnum_FromTagMap(BSConfig->TargetConfig.TargetSpawningPolicy));
	UpdateValueIfDifferent(ComboBoxOption_TargetSpawnResponses,
		GetStringArrayFromEnumArray_FromTagMap(BSConfig->TargetConfig.TargetSpawnResponses));

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
		BSConfig->TargetConfig.bUseBatchSpawning,
		BSConfig->TargetConfig.bAllowSpawnWithoutActivation);
	UpdateDependentOptions_TargetSpawnResponses(BSConfig->TargetConfig.TargetSpawnResponses);
	UpdateBrushColors();
}

void UCustomGameModesWidget_Spawning::SetupWarningTooltipCallbacks()
{
	ComboBoxOption_TargetSpawnResponses->AddWarningTooltipData(FTooltipData("Invalid_Velocity_MTDM_None",
		ETooltipImageType::Caution)).BindLambda([this]()
	{
		return BSConfig->TargetConfig.TargetSpawnResponses.Contains(ETargetSpawnResponse::ChangeVelocity) &&
			BSConfig->TargetConfig.MovingTargetDirectionMode == EMovingTargetDirectionMode::None;
	});
	ComboBoxOption_TargetSpawnResponses->AddWarningTooltipData(FTooltipData("Invalid_Direction_MTDM_None",
	ETooltipImageType::Caution)).BindLambda([this]()
	{
		return BSConfig->TargetConfig.MovingTargetDirectionMode == EMovingTargetDirectionMode::None && BSConfig->
			TargetConfig.TargetSpawnResponses.Contains(ETargetSpawnResponse::ChangeDirection);
	});
}

void UCustomGameModesWidget_Spawning::UpdateDependentOptions_TargetSpawningPolicy(
const ETargetSpawningPolicy& InTargetSpawningPolicy, const bool bUseBatchSpawning,
const bool bAllowSpawnWithoutActivation)
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
	}
}

void UCustomGameModesWidget_Spawning::UpdateDependentOptions_TargetSpawnResponses(
	const TArray<ETargetSpawnResponse>& InTargetSpawnResponses)
{
}

void UCustomGameModesWidget_Spawning::OnCheckStateChanged_AllowSpawnWithoutActivation(const bool bChecked)
{
	BSConfig->TargetConfig.bAllowSpawnWithoutActivation = bChecked;
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_Spawning::OnCheckStateChanged_BatchSpawning(const bool bChecked)
{
	BSConfig->TargetConfig.bUseBatchSpawning = bChecked;
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_Spawning::OnCheckStateChanged_SpawnAtOriginWheneverPossible(const bool bChecked)
{
	BSConfig->TargetConfig.bSpawnAtOriginWheneverPossible = bChecked;
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_Spawning::OnCheckStateChanged_SpawnEveryOtherTargetInCenter(const bool bChecked)
{
	BSConfig->TargetConfig.bSpawnEveryOtherTargetInCenter = bChecked;
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_Spawning::OnSliderTextBoxValueChanged(USliderTextBoxOptionWidget* Widget, const float Value)
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

void UCustomGameModesWidget_Spawning::OnSelectionChanged_RuntimeTargetSpawningLocationSelectionMode(
	const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || Selected.Num() != 1) return;

	BSConfig->TargetConfig.RuntimeTargetSpawningLocationSelectionMode = GetEnumFromString<
		ERuntimeTargetSpawningLocationSelectionMode>(Selected[0]);

	UpdateBrushColors();
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_Spawning::OnSelectionChanged_TargetSpawningPolicy(const TArray<FString>& Selected,
	const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || Selected.Num() != 1) return;


	BSConfig->TargetConfig.TargetSpawningPolicy = GetEnumFromString<ETargetSpawningPolicy>(Selected[0]);

	UpdateDependentOptions_TargetSpawningPolicy(BSConfig->TargetConfig.TargetSpawningPolicy,
		BSConfig->TargetConfig.bUseBatchSpawning, BSConfig->TargetConfig.bSpawnEveryOtherTargetInCenter);
	UpdateBrushColors();
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_Spawning::OnSelectionChanged_TargetSpawnResponses(const TArray<FString>& Selected,
	const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || Selected.Num() < 1) return;

	BSConfig->TargetConfig.TargetSpawnResponses = GetEnumArrayFromStringArray<ETargetSpawnResponse>(Selected);
	UpdateDependentOptions_TargetSpawnResponses(BSConfig->TargetConfig.TargetSpawnResponses);
	UpdateBrushColors();
	UpdateAllOptionsValid();
}

FString
UCustomGameModesWidget_Spawning::GetComboBoxEntryTooltipStringTableKey_RuntimeTargetSpawningLocationSelectionMode(
	const FString& EnumString)
{
	const ERuntimeTargetSpawningLocationSelectionMode EnumValue = GetEnumFromString<
		ERuntimeTargetSpawningLocationSelectionMode>(EnumString);
	return GetStringTableKeyNameFromEnum(EnumValue);
}

FString UCustomGameModesWidget_Spawning::GetComboBoxEntryTooltipStringTableKey_TargetSpawningPolicy(
	const FString& EnumString)
{
	const ETargetSpawningPolicy EnumValue = GetEnumFromString<ETargetSpawningPolicy>(EnumString);
	return GetStringTableKeyNameFromEnum(EnumValue);
}

FString UCustomGameModesWidget_Spawning::GetComboBoxEntryTooltipStringTableKey_TargetSpawnResponses(
	const FString& EnumString)
{
	const ETargetSpawnResponse EnumValue = GetEnumFromString<ETargetSpawnResponse>(EnumString);
	return GetStringTableKeyNameFromEnum(EnumValue);
}
