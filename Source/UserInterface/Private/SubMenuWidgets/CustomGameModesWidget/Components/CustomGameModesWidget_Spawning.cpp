// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_Spawning.h"
#include "Components/CheckBox.h"
#include "WidgetComponents/BSComboBoxString.h"
#include "WidgetComponents/MenuOptionWidgets/CheckBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/ComboBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/SliderTextBoxWidget.h"

void UCustomGameModesWidget_Spawning::InitComponent(FBSConfig* InConfigPtr, TObjectPtr<UCustomGameModesWidgetComponent> InNext)
{
	Super::InitComponent(InConfigPtr, InNext);
}

void UCustomGameModesWidget_Spawning::NativeConstruct()
{
	Super::NativeConstruct();

	SetupTooltip(ComboBoxOption_TargetSpawningPolicy->GetTooltipImage(), ComboBoxOption_TargetSpawningPolicy->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_NumUpfrontTargetsToSpawn->GetTooltipImage(), SliderTextBoxOption_NumUpfrontTargetsToSpawn->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_NumRuntimeTargetsToSpawn->GetTooltipImage(), SliderTextBoxOption_NumRuntimeTargetsToSpawn->GetTooltipImageText());
	SetupTooltip(CheckBoxOption_AllowSpawnWithoutActivation->GetTooltipImage(), CheckBoxOption_AllowSpawnWithoutActivation->GetTooltipImageText());
	SetupTooltip(CheckBoxOption_BatchSpawning->GetTooltipImage(), CheckBoxOption_BatchSpawning->GetTooltipImageText());
	SetupTooltip(CheckBoxOption_ApplyImmunityOnSpawn->GetTooltipImage(), CheckBoxOption_ApplyImmunityOnSpawn->GetTooltipImageText());
	SetupTooltip(CheckBoxOption_SpawnAtOriginWheneverPossible->GetTooltipImage(), CheckBoxOption_SpawnAtOriginWheneverPossible->GetTooltipImageText());
	SetupTooltip(CheckBoxOption_SpawnEveryOtherTargetInCenter->GetTooltipImage(), CheckBoxOption_SpawnEveryOtherTargetInCenter->GetTooltipImageText());

	SliderTextBoxOption_NumUpfrontTargetsToSpawn->SetValues(MinValue_NumUpfrontTargetsToSpawn, MaxValue_NumUpfrontTargetsToSpawn, SnapSize_NumUpfrontTargetsToSpawn);
	SliderTextBoxOption_NumRuntimeTargetsToSpawn->SetValues(MinValue_NumRuntimeTargetsToSpawn, MaxValue_NumRuntimeTargetsToSpawn, SnapSize_NumRuntimeTargetsToSpawn);

	SliderTextBoxOption_NumUpfrontTargetsToSpawn->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_NumRuntimeTargetsToSpawn->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	
	ComboBoxOption_TargetSpawningPolicy->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_TargetSpawningPolicy);
	ComboBoxOption_TargetSpawningPolicy->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetSpawningPolicy);
	
	CheckBoxOption_AllowSpawnWithoutActivation->CheckBox->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckStateChanged_AllowSpawnWithoutActivation);
	CheckBoxOption_BatchSpawning->CheckBox->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckStateChanged_BatchSpawning);
	CheckBoxOption_ApplyImmunityOnSpawn->CheckBox->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckStateChanged_BatchSpawning);
	CheckBoxOption_SpawnAtOriginWheneverPossible->CheckBox->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckStateChanged_SpawnAtOriginWheneverPossible);
	CheckBoxOption_SpawnEveryOtherTargetInCenter->CheckBox->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckStateChanged_SpawnEveryOtherTargetInCenter);
	
	ComboBoxOption_TargetSpawningPolicy->ComboBox->ClearOptions();

	for (const ETargetSpawningPolicy& Method : TEnumRange<ETargetSpawningPolicy>())
	{
		ComboBoxOption_TargetSpawningPolicy->ComboBox->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}

	SliderTextBoxOption_NumUpfrontTargetsToSpawn->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_NumRuntimeTargetsToSpawn->SetVisibility(ESlateVisibility::Collapsed);

	UpdateBrushColors();
}

bool UCustomGameModesWidget_Spawning::UpdateAllOptionsValid()
{
	if (ComboBoxOption_TargetSpawningPolicy->ComboBox->GetSelectedOptionCount() != 1)
	{
		return false;
	}
	return true;
}

void UCustomGameModesWidget_Spawning::UpdateOptionsFromConfig()
{
	UpdateValueIfDifferent(ComboBoxOption_TargetSpawningPolicy, GetStringFromEnum(BSConfig->TargetConfig.TargetSpawningPolicy));

	UpdateValueIfDifferent(SliderTextBoxOption_NumUpfrontTargetsToSpawn, BSConfig->TargetConfig.NumUpfrontTargetsToSpawn);
	UpdateValueIfDifferent(SliderTextBoxOption_NumRuntimeTargetsToSpawn, BSConfig->TargetConfig.NumRuntimeTargetsToSpawn);
	
	UpdateValueIfDifferent(CheckBoxOption_AllowSpawnWithoutActivation, BSConfig->TargetConfig.bAllowSpawnWithoutActivation);
	UpdateValueIfDifferent(CheckBoxOption_BatchSpawning, BSConfig->TargetConfig.bUseBatchSpawning);
	UpdateValueIfDifferent(CheckBoxOption_ApplyImmunityOnSpawn, BSConfig->TargetConfig.bApplyImmunityOnSpawn);
	UpdateValueIfDifferent(CheckBoxOption_SpawnAtOriginWheneverPossible, BSConfig->TargetConfig.bSpawnAtOriginWheneverPossible);
	UpdateValueIfDifferent(CheckBoxOption_SpawnEveryOtherTargetInCenter, BSConfig->TargetConfig.bSpawnEveryOtherTargetInCenter);
	
	UpdateDependentOptions_TargetSpawningPolicy(BSConfig->TargetConfig.TargetSpawningPolicy);
	
	UpdateBrushColors();
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_Spawning::UpdateDependentOptions_TargetSpawningPolicy(const ETargetSpawningPolicy& InTargetSpawningPolicy)
{
	if (InTargetSpawningPolicy == ETargetSpawningPolicy::RuntimeOnly)
	{
		SliderTextBoxOption_NumUpfrontTargetsToSpawn->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_NumRuntimeTargetsToSpawn->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		CheckBoxOption_BatchSpawning->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		CheckBoxOption_SpawnAtOriginWheneverPossible->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		CheckBoxOption_SpawnEveryOtherTargetInCenter->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else if (InTargetSpawningPolicy == ETargetSpawningPolicy::UpfrontOnly)
	{
		SliderTextBoxOption_NumUpfrontTargetsToSpawn->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_NumRuntimeTargetsToSpawn->SetVisibility(ESlateVisibility::Collapsed);
		CheckBoxOption_BatchSpawning->SetVisibility(ESlateVisibility::Collapsed);
		CheckBoxOption_SpawnAtOriginWheneverPossible->SetVisibility(ESlateVisibility::Collapsed);
		CheckBoxOption_SpawnEveryOtherTargetInCenter->SetVisibility(ESlateVisibility::Collapsed);
		CheckBoxOption_BatchSpawning->CheckBox->SetIsChecked(false);
		CheckBoxOption_SpawnAtOriginWheneverPossible->CheckBox->SetIsChecked(false);
		CheckBoxOption_SpawnEveryOtherTargetInCenter->CheckBox->SetIsChecked(false);
		BSConfig->TargetConfig.bUseBatchSpawning = false;
	}
}

void UCustomGameModesWidget_Spawning::OnCheckStateChanged_AllowSpawnWithoutActivation(const bool bChecked)
{
	BSConfig->TargetConfig.bAllowSpawnWithoutActivation = bChecked;
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_Spawning::OnCheckStateChanged_BatchSpawning(const bool bChecked)
{
	BSConfig->TargetConfig.bUseBatchSpawning = bChecked;
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_Spawning::OnCheckStateChanged_ApplyImmunityOnSpawn(const bool bChecked)
{
	BSConfig->TargetConfig.bApplyImmunityOnSpawn = bChecked;
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_Spawning::OnCheckStateChanged_SpawnAtOriginWheneverPossible(const bool bChecked)
{
	BSConfig->TargetConfig.bSpawnAtOriginWheneverPossible = bChecked;
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_Spawning::OnCheckStateChanged_SpawnEveryOtherTargetInCenter(const bool bChecked)
{
	BSConfig->TargetConfig.bSpawnEveryOtherTargetInCenter = bChecked;
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_Spawning::OnSliderTextBoxValueChanged(USliderTextBoxWidget* Widget, const float Value)
{
	if (Widget == SliderTextBoxOption_NumUpfrontTargetsToSpawn)
	{
		BSConfig->TargetConfig.NumUpfrontTargetsToSpawn = Value;
	}
	else if (Widget == SliderTextBoxOption_NumRuntimeTargetsToSpawn)
	{
		BSConfig->TargetConfig.NumRuntimeTargetsToSpawn = Value;
	}
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_Spawning::OnSelectionChanged_TargetSpawningPolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct)
	{
		return;
	}
	
	if (Selected.Num() != 1)
	{
		SetAllOptionsValid(UpdateAllOptionsValid());
		return;
	}
	
	BSConfig->TargetConfig.TargetSpawningPolicy = GetEnumFromString<ETargetSpawningPolicy>(Selected[0]);
	
	UpdateDependentOptions_TargetSpawningPolicy(BSConfig->TargetConfig.TargetSpawningPolicy);
	UpdateBrushColors();
	SetAllOptionsValid(UpdateAllOptionsValid());
}

FString UCustomGameModesWidget_Spawning::GetComboBoxEntryTooltipStringTableKey_TargetSpawningPolicy(const FString& EnumString)
{
	const ETargetSpawningPolicy EnumValue = GetEnumFromString<ETargetSpawningPolicy>(EnumString);
	return GetStringTableKeyNameFromEnum(EnumValue);
}