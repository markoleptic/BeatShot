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

	SliderTextBoxOption_MaxNumTargetsAtOnce->SetValues(MinValue_MaxNumTargetsAtOnce, MaxValue_MaxNumTargetsAtOnce, SnapSize_MaxNumTargetsAtOnce);
	SliderTextBoxOption_NumUpfrontTargetsToSpawn->SetValues(MinValue_NumUpfrontTargetsToSpawn, MaxValue_NumUpfrontTargetsToSpawn, SnapSize_NumUpfrontTargetsToSpawn);
	SliderTextBoxOption_NumRuntimeTargetsToSpawn->SetValues(MinValue_NumRuntimeTargetsToSpawn, MaxValue_NumRuntimeTargetsToSpawn, SnapSize_NumRuntimeTargetsToSpawn);
	SliderTextBoxOption_SpawnedTargetVelocity->SetValues(MinValue_TargetSpeed, MaxValue_TargetSpeed, SnapSize_TargetSpeed);
	SliderTextBoxOption_MinSpawnedTargetVelocity->SetValues(MinValue_TargetSpeed, MaxValue_TargetSpeed, SnapSize_TargetSpeed);
	SliderTextBoxOption_MaxSpawnedTargetVelocity->SetValues(MinValue_TargetSpeed, MaxValue_TargetSpeed, SnapSize_TargetSpeed);

	SliderTextBoxOption_MaxNumTargetsAtOnce->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_NumUpfrontTargetsToSpawn->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_NumRuntimeTargetsToSpawn->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_SpawnedTargetVelocity->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MinSpawnedTargetVelocity->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MaxSpawnedTargetVelocity->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	
	ComboBoxOption_TargetSpawningPolicy->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_TargetSpawningPolicy);
	ComboBoxOption_TargetSpawningPolicy->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetSpawningPolicy);
	
	CheckBoxOption_AllowSpawnWithoutActivation->CheckBox->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckStateChanged_AllowSpawnWithoutActivation);
	CheckBoxOption_BatchSpawning->CheckBox->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckStateChanged_BatchSpawning);
	CheckBoxOption_ApplyImmunityOnSpawn->CheckBox->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckStateChanged_ApplyImmunityOnSpawn);
	CheckBoxOption_SpawnAtOriginWheneverPossible->CheckBox->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckStateChanged_SpawnAtOriginWheneverPossible);
	CheckBoxOption_SpawnEveryOtherTargetInCenter->CheckBox->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckStateChanged_SpawnEveryOtherTargetInCenter);
	CheckBoxOption_ApplyVelocityWhenSpawned->CheckBox->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckStateChanged_ApplyVelocityWhenSpawned);
	CheckBoxOption_ConstantSpawnedTargetVelocity->CheckBox->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckStateChanged_ConstantSpawnedTargetVelocity);
	
	ComboBoxOption_TargetSpawningPolicy->ComboBox->ClearOptions();
	
	TArray<FString> Options;
	for (const ETargetSpawningPolicy& Method : TEnumRange<ETargetSpawningPolicy>())
	{
		Options.Add(GetStringFromEnum(Method));
	}
	ComboBoxOption_TargetSpawningPolicy->SortAndAddOptions(Options);
	Options.Empty();

	SliderTextBoxOption_NumUpfrontTargetsToSpawn->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_NumRuntimeTargetsToSpawn->SetVisibility(ESlateVisibility::Collapsed);

	CheckBoxOption_ConstantSpawnedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_SpawnedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MinSpawnedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MaxSpawnedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);

	UpdateBrushColors();
}

void UCustomGameModesWidget_Spawning::UpdateAllOptionsValid()
{
	TArray<FTooltipData> UpdateArray;
	bool bRequestComponentUpdate = false;
	uint32 NumWarnings = 0;
	uint32 NumCautions = 0;

	// CheckBoxOption_EnableAI
	if (BSConfig->TargetConfig.bApplyVelocityWhenSpawned)
	{
		if (BSConfig->TargetConfig.MovingTargetDirectionMode == EMovingTargetDirectionMode::None)
		{
			NumCautions++;
			UpdateArray.Emplace("Invalid_Velocity_MTDM_None", ETooltipImageType::Caution);
		}
	}
	bRequestComponentUpdate = UpdateWarningTooltips(CheckBoxOption_ApplyVelocityWhenSpawned, UpdateArray) || bRequestComponentUpdate;
	UpdateArray.Empty();

	CustomGameModeCategoryInfo.Update(NumCautions, NumWarnings);
	
	if (bRequestComponentUpdate)
	{
		RequestComponentUpdate.Broadcast();
	}
}

void UCustomGameModesWidget_Spawning::UpdateOptionsFromConfig()
{
	UpdateValueIfDifferent(ComboBoxOption_TargetSpawningPolicy, GetStringFromEnum(BSConfig->TargetConfig.TargetSpawningPolicy));

	UpdateValueIfDifferent(SliderTextBoxOption_MaxNumTargetsAtOnce, BSConfig->TargetConfig.MaxNumTargetsAtOnce);
	UpdateValueIfDifferent(SliderTextBoxOption_NumUpfrontTargetsToSpawn, BSConfig->TargetConfig.NumUpfrontTargetsToSpawn);
	UpdateValueIfDifferent(SliderTextBoxOption_NumRuntimeTargetsToSpawn, BSConfig->TargetConfig.NumRuntimeTargetsToSpawn);
	
	UpdateValueIfDifferent(CheckBoxOption_AllowSpawnWithoutActivation, BSConfig->TargetConfig.bAllowSpawnWithoutActivation);
	UpdateValueIfDifferent(CheckBoxOption_BatchSpawning, BSConfig->TargetConfig.bUseBatchSpawning);
	UpdateValueIfDifferent(CheckBoxOption_ApplyImmunityOnSpawn, BSConfig->TargetConfig.bApplyImmunityOnSpawn);
	UpdateValueIfDifferent(CheckBoxOption_SpawnAtOriginWheneverPossible, BSConfig->TargetConfig.bSpawnAtOriginWheneverPossible);
	UpdateValueIfDifferent(CheckBoxOption_SpawnEveryOtherTargetInCenter, BSConfig->TargetConfig.bSpawnEveryOtherTargetInCenter);
	
	UpdateValueIfDifferent(CheckBoxOption_ApplyVelocityWhenSpawned, BSConfig->TargetConfig.bApplyVelocityWhenSpawned);
	UpdateValueIfDifferent(CheckBoxOption_ConstantSpawnedTargetVelocity, BSConfig->TargetConfig.MinSpawnedTargetSpeed == BSConfig->TargetConfig.MaxSpawnedTargetSpeed);
	UpdateValueIfDifferent(SliderTextBoxOption_SpawnedTargetVelocity, BSConfig->TargetConfig.MinSpawnedTargetSpeed);
	UpdateValueIfDifferent(SliderTextBoxOption_MinSpawnedTargetVelocity, BSConfig->TargetConfig.MinSpawnedTargetSpeed);
	UpdateValueIfDifferent(SliderTextBoxOption_MaxSpawnedTargetVelocity, BSConfig->TargetConfig.MaxSpawnedTargetSpeed);
	
	UpdateDependentOptions_TargetSpawningPolicy(BSConfig->TargetConfig.TargetSpawningPolicy);
	UpdateDependentOptions_ApplyVelocityWhenSpawned(BSConfig->TargetConfig.bApplyVelocityWhenSpawned, BSConfig->TargetConfig.MinSpawnedTargetSpeed == BSConfig->TargetConfig.MaxSpawnedTargetSpeed);
	
	UpdateBrushColors();
	
}

void UCustomGameModesWidget_Spawning::UpdateDependentOptions_TargetSpawningPolicy(const ETargetSpawningPolicy& InTargetSpawningPolicy)
{
	if (InTargetSpawningPolicy == ETargetSpawningPolicy::RuntimeOnly)
	{
		CheckBoxOption_BatchSpawning->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		CheckBoxOption_SpawnAtOriginWheneverPossible->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		CheckBoxOption_SpawnEveryOtherTargetInCenter->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		
		SliderTextBoxOption_NumUpfrontTargetsToSpawn->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_NumRuntimeTargetsToSpawn->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

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
	}
}

void UCustomGameModesWidget_Spawning::UpdateDependentOptions_ApplyVelocityWhenSpawned(const bool bApplyVelocityWhenSpawned, const bool bConstantSpawnedTargetVelocity)
{
	if (bApplyVelocityWhenSpawned)
	{
		CheckBoxOption_ConstantSpawnedTargetVelocity->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		CheckBoxOption_ConstantSpawnedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
	}
	UpdateDependentOptions_ConstantSpawnedTargetVelocity(bApplyVelocityWhenSpawned, bConstantSpawnedTargetVelocity);
}

void UCustomGameModesWidget_Spawning::UpdateDependentOptions_ConstantSpawnedTargetVelocity(const bool bApplyVelocityWhenSpawned, const bool bInConstant)
{
	if (bApplyVelocityWhenSpawned)
	{
		if (bInConstant)
		{
			SliderTextBoxOption_SpawnedTargetVelocity->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			SliderTextBoxOption_MinSpawnedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
			SliderTextBoxOption_MaxSpawnedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			SliderTextBoxOption_SpawnedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
			SliderTextBoxOption_MinSpawnedTargetVelocity->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			SliderTextBoxOption_MaxSpawnedTargetVelocity->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
	else
	{
		SliderTextBoxOption_SpawnedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MinSpawnedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MaxSpawnedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
	}
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

void UCustomGameModesWidget_Spawning::OnCheckStateChanged_ApplyImmunityOnSpawn(const bool bChecked)
{
	BSConfig->TargetConfig.bApplyImmunityOnSpawn = bChecked;
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

void UCustomGameModesWidget_Spawning::OnCheckStateChanged_ApplyVelocityWhenSpawned(const bool bChecked)
{
	BSConfig->TargetConfig.bApplyVelocityWhenSpawned = bChecked;
	UpdateDependentOptions_ApplyVelocityWhenSpawned(BSConfig->TargetConfig.bApplyVelocityWhenSpawned, BSConfig->TargetConfig.MinSpawnedTargetSpeed == BSConfig->TargetConfig.MaxSpawnedTargetSpeed);
	UpdateBrushColors();
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_Spawning::OnCheckStateChanged_ConstantSpawnedTargetVelocity(const bool bChecked)
{
	if (bChecked)
	{
		BSConfig->TargetConfig.MinSpawnedTargetSpeed = SliderTextBoxOption_SpawnedTargetVelocity->GetSliderValue();
		BSConfig->TargetConfig.MaxSpawnedTargetSpeed = SliderTextBoxOption_SpawnedTargetVelocity->GetSliderValue();
	}
	else
	{
		BSConfig->TargetConfig.MinSpawnedTargetSpeed = SliderTextBoxOption_MinSpawnedTargetVelocity->GetSliderValue();
		BSConfig->TargetConfig.MaxSpawnedTargetSpeed = SliderTextBoxOption_MaxSpawnedTargetVelocity->GetSliderValue();
	}
	
	UpdateDependentOptions_ConstantSpawnedTargetVelocity(BSConfig->TargetConfig.bApplyVelocityWhenSpawned, bChecked);
	UpdateBrushColors();
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_Spawning::OnSliderTextBoxValueChanged(USliderTextBoxWidget* Widget, const float Value)
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
	else if (Widget == SliderTextBoxOption_SpawnedTargetVelocity && SliderTextBoxOption_SpawnedTargetVelocity->GetVisibility() != ESlateVisibility::Collapsed)
	{
		BSConfig->TargetConfig.MinSpawnedTargetSpeed = Value;
		BSConfig->TargetConfig.MaxSpawnedTargetSpeed = Value;
	}
	else if (Widget == SliderTextBoxOption_MinSpawnedTargetVelocity)
	{
		BSConfig->TargetConfig.MinSpawnedTargetSpeed = Value;
	}
	else if (Widget == SliderTextBoxOption_MaxSpawnedTargetVelocity)
	{
		BSConfig->TargetConfig.MaxSpawnedTargetSpeed = Value;
	}
	
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_Spawning::OnSelectionChanged_TargetSpawningPolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct)
	{
		return;
	}
	
	if (Selected.Num() != 1)
	{
		
		return;
	}
	
	BSConfig->TargetConfig.TargetSpawningPolicy = GetEnumFromString<ETargetSpawningPolicy>(Selected[0]);
	
	UpdateDependentOptions_TargetSpawningPolicy(BSConfig->TargetConfig.TargetSpawningPolicy);
	UpdateBrushColors();
	UpdateAllOptionsValid();
}

FString UCustomGameModesWidget_Spawning::GetComboBoxEntryTooltipStringTableKey_TargetSpawningPolicy(const FString& EnumString)
{
	const ETargetSpawningPolicy EnumValue = GetEnumFromString<ETargetSpawningPolicy>(EnumString);
	return GetStringTableKeyNameFromEnum(EnumValue);
}