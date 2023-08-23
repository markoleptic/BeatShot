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

	SliderTextBoxOption_NumUpfrontTargetsToSpawn->SetValues(MinValue_NumUpfrontTargetsToSpawn, MaxValue_NumUpfrontTargetsToSpawn, SnapSize_NumUpfrontTargetsToSpawn);
	SliderTextBoxOption_NumRuntimeTargetsToSpawn->SetValues(MinValue_NumRuntimeTargetsToSpawn, MaxValue_NumRuntimeTargetsToSpawn, SnapSize_NumRuntimeTargetsToSpawn);

	SliderTextBoxOption_NumUpfrontTargetsToSpawn->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_NumRuntimeTargetsToSpawn->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);

	ComboBoxOption_TargetSpawningPolicy->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_TargetSpawningPolicy);
	ComboBoxOption_TargetSpawningPolicy->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetSpawningPolicy);

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
	ComboBoxOption_TargetSpawningPolicy->ComboBox->SetSelectedOption(UEnum::GetDisplayValueAsText(BSConfig->TargetConfig.TargetSpawningPolicy).ToString());
	CheckBoxOption_AllowSpawnWithoutActivation->CheckBox->SetIsChecked(BSConfig->TargetConfig.bAllowSpawnWithoutActivation);
	CheckBoxOption_BatchSpawning->CheckBox->SetIsChecked(BSConfig->TargetConfig.bUseBatchSpawning);
	SliderTextBoxOption_NumUpfrontTargetsToSpawn->SetValue(BSConfig->TargetConfig.NumUpfrontTargetsToSpawn);
	SliderTextBoxOption_NumRuntimeTargetsToSpawn->SetValue(BSConfig->TargetConfig.NumRuntimeTargetsToSpawn);

	if (ComboBoxOption_TargetSpawningPolicy->ComboBox->GetSelectedOption() == UEnum::GetDisplayValueAsText(ETargetSpawningPolicy::RuntimeOnly).ToString())
	{
		SliderTextBoxOption_NumUpfrontTargetsToSpawn->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_NumRuntimeTargetsToSpawn->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else if (ComboBoxOption_TargetSpawningPolicy->ComboBox->GetSelectedOption() == UEnum::GetDisplayValueAsText(ETargetSpawningPolicy::UpfrontOnly).ToString())
	{
		SliderTextBoxOption_NumUpfrontTargetsToSpawn->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_NumRuntimeTargetsToSpawn->SetVisibility(ESlateVisibility::Collapsed);
	}

	SetAllOptionsValid(UpdateAllOptionsValid());
	UpdateBrushColors();
}

void UCustomGameModesWidget_Spawning::OnCheckStateChanged_AllowSpawnWithoutActivation(const bool bChecked)
{
	SetAllOptionsValid(UpdateAllOptionsValid());
	BSConfig->TargetConfig.bAllowSpawnWithoutActivation = bChecked;
}

void UCustomGameModesWidget_Spawning::OnCheckStateChanged_BatchSpawning(const bool bChecked)
{
	SetAllOptionsValid(UpdateAllOptionsValid());
	BSConfig->TargetConfig.bUseBatchSpawning = bChecked;
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
	
	const ETargetSpawningPolicy Policy = GetEnumFromString<ETargetSpawningPolicy>(Selected[0], ETargetSpawningPolicy::None);

	if (Policy == ETargetSpawningPolicy::RuntimeOnly)
	{
		SliderTextBoxOption_NumUpfrontTargetsToSpawn->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_NumRuntimeTargetsToSpawn->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else if (Policy == ETargetSpawningPolicy::UpfrontOnly)
	{
		SliderTextBoxOption_NumUpfrontTargetsToSpawn->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_NumRuntimeTargetsToSpawn->SetVisibility(ESlateVisibility::Collapsed);
	}

	BSConfig->TargetConfig.TargetSpawningPolicy = Policy;
	SetAllOptionsValid(UpdateAllOptionsValid());
	UpdateBrushColors();
}

FString UCustomGameModesWidget_Spawning::GetComboBoxEntryTooltipStringTableKey_TargetSpawningPolicy(const FString& EnumString)
{
	const ETargetSpawningPolicy EnumValue = GetEnumFromString<ETargetSpawningPolicy>(EnumString);
	return GetStringTableKeyNameFromEnum(EnumValue);
}