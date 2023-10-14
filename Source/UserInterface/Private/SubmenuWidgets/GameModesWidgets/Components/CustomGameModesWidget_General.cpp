// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/GameModesWidgets/Components/CustomGameModesWidget_General.h"
#include "Components/CheckBox.h"
#include "WidgetComponents/Boxes/BSComboBoxString.h"
#include "WidgetComponents/MenuOptionWidgets/CheckBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/ComboBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/SliderTextBoxOptionWidget.h"


void UCustomGameModesWidget_General::NativeConstruct()
{
	Super::NativeConstruct();

	SliderTextBoxOption_SpawnBeatDelay->SetValues(MinValue_PlayerDelay, MaxValue_PlayerDelay, SnapSize_PlayerDelay);
	SliderTextBoxOption_TargetSpawnCD->
		SetValues(MinValue_TargetSpawnCD, MaxValue_TargetSpawnCD, SnapSize_TargetSpawnCD);
	SliderTextBoxOption_MaxNumRecentTargets->SetValues(MinValue_MaxNumRecentTargets, MaxValue_MaxNumRecentTargets,
		SnapSize_MaxNumRecentTargets);
	SliderTextBoxOption_RecentTargetTimeLength->SetValues(MinValue_RecentTargetTimeLength,
		MaxValue_RecentTargetTimeLength, SnapSize_RecentTargetTimeLength);
	SliderTextBoxOption_TargetLifespan->SetValues(MinValue_Lifespan, MaxValue_Lifespan, SnapSize_Lifespan);
	SliderTextBoxOption_MaxHealth->SetValues(MinValue_MaxHealth, MaxValue_MaxHealth, SnapSize_MaxHealth);
	SliderTextBoxOption_ExpirationHealthPenalty->SetValues(MinValue_ExpirationHealthPenalty,
		MaxValue_ExpirationHealthPenalty, SnapSize_ExpirationHealthPenalty);
	SliderTextBoxOption_Alpha->SetValues(MinValue_Alpha, MaxValue_Alpha, SnapSize_Alpha);
	SliderTextBoxOption_Epsilon->SetValues(MinValue_Epsilon, MaxValue_Epsilon, SnapSize_Epsilon);
	SliderTextBoxOption_Gamma->SetValues(MinValue_Gamma, MaxValue_Gamma, SnapSize_Gamma);
	
	SliderTextBoxOption_SpawnBeatDelay->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_TargetSpawnCD->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MaxNumRecentTargets->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_RecentTargetTimeLength->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_Alpha->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_Epsilon->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_Gamma->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_TargetLifespan->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MaxHealth->OnSliderTextBoxValueChanged.
	                               AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_ExpirationHealthPenalty->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);


	CheckBoxOption_EnableAI->CheckBox->OnCheckStateChanged.AddUniqueDynamic(this,
		&ThisClass::OnCheckStateChanged_EnableAI);
	CheckBoxOption_InfiniteTargetHealth->CheckBox->OnCheckStateChanged.AddUniqueDynamic(this,
		&ThisClass::OnCheckStateChanged_InfiniteTargetHealth);
	CheckBoxOption_InfiniteTargetLifespan->CheckBox->OnCheckStateChanged.AddUniqueDynamic(this,
		&ThisClass::OnCheckStateChanged_InfiniteTargetLifespan);

	ComboBoxOption_RecentTargetMemoryPolicy->ComboBox->OnSelectionChanged.AddUniqueDynamic(this,
		&ThisClass::OnSelectionChanged_RecentTargetMemoryPolicy);
	ComboBoxOption_DamageType->ComboBox->OnSelectionChanged.AddUniqueDynamic(this,
		&ThisClass::OnSelectionChanged_DamageType);
	ComboBoxOption_HyperParameterMode->ComboBox->OnSelectionChanged.AddUniqueDynamic(this,
		&ThisClass::OnSelectionChanged_HyperParameterMode);
	
	ComboBoxOption_RecentTargetMemoryPolicy->GetComboBoxEntryTooltipStringTableKey.BindUObject(this,
		&ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetActivationSelectionPolicy);
	ComboBoxOption_DamageType->GetComboBoxEntryTooltipStringTableKey.BindUObject(this,
		&ThisClass::GetComboBoxEntryTooltipStringTableKey_DamageType);
	ComboBoxOption_HyperParameterMode->GetComboBoxEntryTooltipStringTableKey.BindUObject(this,
		&ThisClass::GetComboBoxEntryTooltipStringTableKey_HyperParameterMode);

	ComboBoxOption_RecentTargetMemoryPolicy->ComboBox->ClearOptions();
	ComboBoxOption_DamageType->ComboBox->ClearOptions();
	ComboBoxOption_HyperParameterMode->ComboBox->ClearOptions();

	TArray<FString> Options;
	for (const ERecentTargetMemoryPolicy& Method : TEnumRange<ERecentTargetMemoryPolicy>())
	{
		Options.Add(GetStringFromEnum_FromTagMap(Method));
	}
	ComboBoxOption_RecentTargetMemoryPolicy->SortAddOptionsAndSetEnumType<ERecentTargetMemoryPolicy>(Options);
	Options.Empty();

	for (const ETargetDamageType& Method : TEnumRange<ETargetDamageType>())
	{
		Options.Add(GetStringFromEnum_FromTagMap(Method));
	}
	ComboBoxOption_DamageType->SortAddOptionsAndSetEnumType<ETargetDamageType>(Options);
	Options.Empty();
	
	for (const EReinforcementLearningHyperParameterMode& Method : TEnumRange<
		     EReinforcementLearningHyperParameterMode>())
	{
		Options.Add(GetStringFromEnum_FromTagMap(Method));
	}
	ComboBoxOption_HyperParameterMode->SortAddOptionsAndSetEnumType<EReinforcementLearningHyperParameterMode>(Options);
	Options.Empty();

	SliderTextBoxOption_RecentTargetTimeLength->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MaxNumRecentTargets->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_Alpha->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_Epsilon->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_Gamma->SetVisibility(ESlateVisibility::Collapsed);
	ComboBoxOption_HyperParameterMode->SetVisibility(ESlateVisibility::Collapsed);

	SetupWarningTooltipCallbacks();
	UpdateBrushColors();
}

void UCustomGameModesWidget_General::UpdateAllOptionsValid()
{
	Super::UpdateAllOptionsValid();
}

void UCustomGameModesWidget_General::UpdateOptionsFromConfig()
{
	UpdateValueIfDifferent(SliderTextBoxOption_SpawnBeatDelay, BSConfig->TargetConfig.SpawnBeatDelay);
	UpdateValueIfDifferent(SliderTextBoxOption_TargetSpawnCD, BSConfig->TargetConfig.TargetSpawnCD);
	UpdateValueIfDifferent(SliderTextBoxOption_MaxNumRecentTargets, BSConfig->TargetConfig.MaxNumRecentTargets);
	UpdateValueIfDifferent(SliderTextBoxOption_RecentTargetTimeLength, BSConfig->TargetConfig.RecentTargetTimeLength);
	UpdateValueIfDifferent(SliderTextBoxOption_Alpha, BSConfig->AIConfig.Alpha);
	UpdateValueIfDifferent(SliderTextBoxOption_Epsilon, BSConfig->AIConfig.Epsilon);
	UpdateValueIfDifferent(SliderTextBoxOption_Gamma, BSConfig->AIConfig.Gamma);
	UpdateValueIfDifferent(SliderTextBoxOption_TargetLifespan, BSConfig->TargetConfig.TargetMaxLifeSpan);
	UpdateValueIfDifferent(SliderTextBoxOption_MaxHealth, BSConfig->TargetConfig.MaxHealth);
	UpdateValueIfDifferent(SliderTextBoxOption_ExpirationHealthPenalty, BSConfig->TargetConfig.ExpirationHealthPenalty);
	
	UpdateValueIfDifferent(ComboBoxOption_HyperParameterMode,
		GetStringFromEnum_FromTagMap(BSConfig->AIConfig.HyperParameterMode));
	UpdateValueIfDifferent(ComboBoxOption_RecentTargetMemoryPolicy,
		GetStringFromEnum_FromTagMap(BSConfig->TargetConfig.RecentTargetMemoryPolicy));
	UpdateValueIfDifferent(ComboBoxOption_DamageType,
		GetStringFromEnum_FromTagMap(BSConfig->TargetConfig.TargetDamageType));
	
	UpdateValueIfDifferent(CheckBoxOption_EnableAI, BSConfig->AIConfig.bEnableReinforcementLearning);
	UpdateValueIfDifferent(CheckBoxOption_InfiniteTargetHealth, BSConfig->TargetConfig.MaxHealth <= 0.f);
	UpdateValueIfDifferent(CheckBoxOption_InfiniteTargetLifespan, BSConfig->TargetConfig.TargetMaxLifeSpan <= 0.f);

	UpdateDependentOptions_InfiniteTargetHealth(BSConfig->TargetConfig.MaxHealth <= 0.f);
	UpdateDependentOptions_InfiniteTargetLifespan(BSConfig->TargetConfig.TargetMaxLifeSpan <= 0.f);
	UpdateDependentOptions_RecentTargetMemoryPolicy(BSConfig->TargetConfig.RecentTargetMemoryPolicy);
	UpdateDependentOptions_EnableAI(BSConfig->AIConfig.bEnableReinforcementLearning,
		BSConfig->AIConfig.HyperParameterMode);

	UpdateBrushColors();
}

void UCustomGameModesWidget_General::SetupWarningTooltipCallbacks()
{
	CheckBoxOption_EnableAI->AddWarningTooltipData(FTooltipData("Invalid_HeadshotHeightOnly_AI",
		ETooltipImageType::Warning)).BindLambda([this]()
	{
		return BSConfig->TargetConfig.TargetDistributionPolicy == ETargetDistributionPolicy::Grid && BSConfig->AIConfig.
			bEnableReinforcementLearning && BSConfig->TargetConfig.TargetDistributionPolicy ==
			ETargetDistributionPolicy::HeadshotHeightOnly;
	});
	CheckBoxOption_EnableAI->AddWarningTooltipData(FTooltipData("Invalid_Tracking_AI", ETooltipImageType::Warning)).
	                         BindLambda([this]()
	                         {
		                         return BSConfig->TargetConfig.TargetDistributionPolicy ==
			                         ETargetDistributionPolicy::Grid && BSConfig->AIConfig.bEnableReinforcementLearning
			                         && BSConfig->TargetConfig.TargetDamageType == ETargetDamageType::Tracking;
	                         });
	ComboBoxOption_DamageType->AddWarningTooltipData(FTooltipData("Invalid_Tracking_AI", ETooltipImageType::Warning)).
	                           BindLambda([this]()
	                           {
		                           return BSConfig->AIConfig.bEnableReinforcementLearning && BSConfig->TargetConfig.
			                           TargetDamageType == ETargetDamageType::Tracking;
	                           });
}

void UCustomGameModesWidget_General::UpdateDependentOptions_RecentTargetMemoryPolicy(
	const ERecentTargetMemoryPolicy& InRecentTargetMemoryPolicy)
{
	switch (InRecentTargetMemoryPolicy)
	{
	case ERecentTargetMemoryPolicy::CustomTimeBased:
		SliderTextBoxOption_RecentTargetTimeLength->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_MaxNumRecentTargets->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case ERecentTargetMemoryPolicy::NumTargetsBased:
		SliderTextBoxOption_RecentTargetTimeLength->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MaxNumRecentTargets->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		break;
	case ERecentTargetMemoryPolicy::None:
	case ERecentTargetMemoryPolicy::UseTargetSpawnCD: default:
		SliderTextBoxOption_RecentTargetTimeLength->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MaxNumRecentTargets->SetVisibility(ESlateVisibility::Collapsed);
		break;
	}
}

void UCustomGameModesWidget_General::UpdateDependentOptions_EnableAI(const bool bInEnableReinforcementLearning,
	const EReinforcementLearningHyperParameterMode HyperParameterMode)
{
	if (bInEnableReinforcementLearning)
	{
		ComboBoxOption_HyperParameterMode->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		ComboBoxOption_HyperParameterMode->SetVisibility(ESlateVisibility::Collapsed);
	}
	UpdateDependentOptions_HyperParameterMode(bInEnableReinforcementLearning, HyperParameterMode);
}

void UCustomGameModesWidget_General::UpdateDependentOptions_HyperParameterMode(
	const bool bInEnableReinforcementLearning, const EReinforcementLearningHyperParameterMode HyperParameterMode)
{
	if (!bInEnableReinforcementLearning)
	{
		SliderTextBoxOption_Alpha->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_Epsilon->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_Gamma->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	switch (HyperParameterMode)
	{
	case EReinforcementLearningHyperParameterMode::None:
	case EReinforcementLearningHyperParameterMode::Auto:
		SliderTextBoxOption_Alpha->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_Epsilon->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_Gamma->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case EReinforcementLearningHyperParameterMode::Custom:
		SliderTextBoxOption_Alpha->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_Epsilon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_Gamma->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		break;
	}
}

void UCustomGameModesWidget_General::UpdateDependentOptions_InfiniteTargetHealth(const bool bInUnlimitedTargetHealth)
{
	if (bInUnlimitedTargetHealth)
	{
		SliderTextBoxOption_MaxHealth->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		SliderTextBoxOption_MaxHealth->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UCustomGameModesWidget_General::UpdateDependentOptions_InfiniteTargetLifespan(const bool bInfiniteTargetLifespan)
{
	if (bInfiniteTargetLifespan)
	{
		SliderTextBoxOption_TargetLifespan->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		SliderTextBoxOption_TargetLifespan->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UCustomGameModesWidget_General::OnCheckStateChanged_EnableAI(const bool bChecked)
{
	BSConfig->AIConfig.bEnableReinforcementLearning = bChecked;
	UpdateDependentOptions_EnableAI(BSConfig->AIConfig.bEnableReinforcementLearning,
		BSConfig->AIConfig.HyperParameterMode);

	UpdateBrushColors();
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_General::OnCheckStateChanged_InfiniteTargetHealth(const bool bChecked)
{
	BSConfig->TargetConfig.MaxHealth = bChecked ? -1.f : SliderTextBoxOption_MaxHealth->GetSliderValue();
	UpdateDependentOptions_InfiniteTargetHealth(bChecked);

	UpdateBrushColors();
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_General::OnCheckStateChanged_InfiniteTargetLifespan(const bool bChecked)
{
	BSConfig->TargetConfig.TargetMaxLifeSpan = bChecked ? -1.f : SliderTextBoxOption_TargetLifespan->GetSliderValue();
	UpdateDependentOptions_InfiniteTargetLifespan(bChecked);

	UpdateBrushColors();
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_General::OnSliderTextBoxValueChanged(USliderTextBoxOptionWidget* Widget, const float Value)
{
	if (Widget == SliderTextBoxOption_SpawnBeatDelay)
	{
		BSConfig->TargetConfig.SpawnBeatDelay = Value;
	}
	else if (Widget == SliderTextBoxOption_TargetSpawnCD)
	{
		BSConfig->TargetConfig.TargetSpawnCD = Value;
	}
	else if (Widget == SliderTextBoxOption_MaxNumRecentTargets)
	{
		BSConfig->TargetConfig.MaxNumRecentTargets = Value;
	}
	else if (Widget == SliderTextBoxOption_RecentTargetTimeLength)
	{
		BSConfig->TargetConfig.RecentTargetTimeLength = Value;
	}
	else if (Widget == SliderTextBoxOption_Alpha)
	{
		BSConfig->AIConfig.Alpha = Value;
	}
	else if (Widget == SliderTextBoxOption_Epsilon)
	{
		BSConfig->AIConfig.Epsilon = Value;
	}
	else if (Widget == SliderTextBoxOption_Gamma)
	{
		BSConfig->AIConfig.Gamma = Value;
	}
	else if (Widget == SliderTextBoxOption_TargetLifespan)
	{
		BSConfig->TargetConfig.TargetMaxLifeSpan = Value;
	}
	else if (Widget == SliderTextBoxOption_MaxHealth)
	{
		BSConfig->TargetConfig.MaxHealth = Value;
	}
	else if (Widget == SliderTextBoxOption_ExpirationHealthPenalty)
	{
		BSConfig->TargetConfig.ExpirationHealthPenalty = Value;
	}
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_General::OnSelectionChanged_RecentTargetMemoryPolicy(const TArray<FString>& Selected,
	const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct)
	{
		return;
	}

	if (Selected.Num() != 1)
	{
		return;
	}

	BSConfig->TargetConfig.RecentTargetMemoryPolicy = GetEnumFromString<ERecentTargetMemoryPolicy>(Selected[0]);
	UpdateDependentOptions_RecentTargetMemoryPolicy(BSConfig->TargetConfig.RecentTargetMemoryPolicy);
	UpdateBrushColors();
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_General::OnSelectionChanged_DamageType(const TArray<FString>& Selected,
	const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || Selected.Num() != 1)
	{
		return;
	}

	BSConfig->TargetConfig.TargetDamageType = GetEnumFromString<ETargetDamageType>(Selected[0]);
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_General::OnSelectionChanged_HyperParameterMode(const TArray<FString>& Selected,
	const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || Selected.Num() != 1)
	{
		return;
	}
	BSConfig->AIConfig.HyperParameterMode = GetEnumFromString<EReinforcementLearningHyperParameterMode>(Selected[0]);
	UpdateDependentOptions_EnableAI(BSConfig->AIConfig.bEnableReinforcementLearning,
		BSConfig->AIConfig.HyperParameterMode);

	UpdateBrushColors();
	UpdateAllOptionsValid();
}

FString UCustomGameModesWidget_General::GetComboBoxEntryTooltipStringTableKey_HyperParameterMode(
	const FString& EnumString)
{
	const EReinforcementLearningHyperParameterMode EnumValue = GetEnumFromString<
		EReinforcementLearningHyperParameterMode>(EnumString);
	return GetStringTableKeyNameFromEnum(EnumValue);
}

FString UCustomGameModesWidget_General::GetComboBoxEntryTooltipStringTableKey_TargetActivationSelectionPolicy(
	const FString& EnumString)
{
	return GetStringTableKeyNameFromEnum(GetEnumFromString<ERecentTargetMemoryPolicy>(EnumString));
}

FString UCustomGameModesWidget_General::GetComboBoxEntryTooltipStringTableKey_DamageType(const FString& EnumString)
{
	const ETargetDamageType EnumValue = GetEnumFromString<ETargetDamageType>(EnumString);
	return GetStringTableKeyNameFromEnum(EnumValue);
}
