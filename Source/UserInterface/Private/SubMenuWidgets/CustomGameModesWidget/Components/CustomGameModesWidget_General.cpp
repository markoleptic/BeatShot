// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_General.h"
#include "Components/CheckBox.h"
#include "WidgetComponents/BSComboBoxString.h"
#include "WidgetComponents/MenuOptionWidgets/CheckBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/ComboBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/SliderTextBoxWidget.h"

void UCustomGameModesWidget_General::InitComponent(FBSConfig* InConfigPtr, TObjectPtr<UCustomGameModesWidgetComponent> InNext)
{
	Super::InitComponent(InConfigPtr, InNext);
}

void UCustomGameModesWidget_General::NativeConstruct()
{
	Super::NativeConstruct();

	SetupTooltip(SliderTextBoxOption_SpawnBeatDelay->GetTooltipImage(), SliderTextBoxOption_SpawnBeatDelay->GetTooltipRegularText());
	SetupTooltip(SliderTextBoxOption_TargetSpawnCD->GetTooltipImage(), SliderTextBoxOption_TargetSpawnCD->GetTooltipRegularText());
	SetupTooltip(ComboBoxOption_RecentTargetMemoryPolicy->GetTooltipImage(), ComboBoxOption_RecentTargetMemoryPolicy->GetTooltipRegularText());
	SetupTooltip(SliderTextBoxOption_MaxNumRecentTargets->GetTooltipImage(), SliderTextBoxOption_MaxNumRecentTargets->GetTooltipRegularText());
	SetupTooltip(SliderTextBoxOption_RecentTargetTimeLength->GetTooltipImage(), SliderTextBoxOption_RecentTargetTimeLength->GetTooltipRegularText());
	SetupTooltip(SliderTextBoxOption_MaxNumTargetsAtOnce->GetTooltipImage(), SliderTextBoxOption_MaxNumTargetsAtOnce->GetTooltipRegularText());
	SetupTooltip(CheckBoxOption_EnableAI->GetTooltipImage(), CheckBoxOption_EnableAI->GetTooltipRegularText());
	SetupTooltip(SliderTextBoxOption_Alpha->GetTooltipImage(), SliderTextBoxOption_Alpha->GetTooltipRegularText());
	SetupTooltip(SliderTextBoxOption_Epsilon->GetTooltipImage(), SliderTextBoxOption_Epsilon->GetTooltipRegularText());
	SetupTooltip(SliderTextBoxOption_Gamma->GetTooltipImage(), SliderTextBoxOption_Gamma->GetTooltipRegularText());

	SliderTextBoxOption_SpawnBeatDelay->SetValues(MinValue_PlayerDelay, MaxValue_PlayerDelay, SnapSize_PlayerDelay);
	SliderTextBoxOption_TargetSpawnCD->SetValues(MinValue_TargetSpawnCD, MaxValue_TargetSpawnCD, SnapSize_TargetSpawnCD);
	SliderTextBoxOption_MaxNumRecentTargets->SetValues(MinValue_MaxNumRecentTargets, MaxValue_MaxNumRecentTargets, SnapSize_MaxNumRecentTargets);
	SliderTextBoxOption_RecentTargetTimeLength->SetValues(MinValue_RecentTargetTimeLength, MaxValue_RecentTargetTimeLength, SnapSize_RecentTargetTimeLength);
	SliderTextBoxOption_MaxNumTargetsAtOnce->SetValues(MinValue_MaxNumTargetsAtOnce, MaxValue_MaxNumTargetsAtOnce, SnapSize_MaxNumTargetsAtOnce);
	SliderTextBoxOption_Alpha->SetValues(MinValue_Alpha, MaxValue_Alpha, SnapSize_Alpha);
	SliderTextBoxOption_Epsilon->SetValues(MinValue_Epsilon, MaxValue_Epsilon, SnapSize_Epsilon);
	SliderTextBoxOption_Gamma->SetValues(MinValue_Gamma, MaxValue_Gamma, SnapSize_Gamma);

	SliderTextBoxOption_SpawnBeatDelay->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_TargetSpawnCD->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MaxNumRecentTargets->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_RecentTargetTimeLength->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MaxNumTargetsAtOnce->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_Alpha->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_Epsilon->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_Gamma->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);

	CheckBoxOption_EnableAI->CheckBox->OnCheckStateChanged.AddUniqueDynamic(this, &ThisClass::OnCheckStateChanged_EnableAI);
	ComboBoxOption_RecentTargetMemoryPolicy->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_RecentTargetMemoryPolicy);
	ComboBoxOption_RecentTargetMemoryPolicy->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetActivationSelectionPolicy);

	ComboBoxOption_RecentTargetMemoryPolicy->ComboBox->ClearOptions();

	for (const ERecentTargetMemoryPolicy& Method : TEnumRange<ERecentTargetMemoryPolicy>())
	{
		ComboBoxOption_RecentTargetMemoryPolicy->ComboBox->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}

	SliderTextBoxOption_RecentTargetTimeLength->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MaxNumTargetsAtOnce->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_Alpha->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_Epsilon->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_Gamma->SetVisibility(ESlateVisibility::Collapsed);

	UpdateBrushColors();
}

bool UCustomGameModesWidget_General::UpdateAllOptionsValid()
{
	return true;
}

void UCustomGameModesWidget_General::UpdateOptions()
{
	SliderTextBoxOption_SpawnBeatDelay->SetValue(ConfigPtr->TargetConfig.SpawnBeatDelay);
	SliderTextBoxOption_TargetSpawnCD->SetValue(ConfigPtr->TargetConfig.TargetSpawnCD);
	SliderTextBoxOption_MaxNumTargetsAtOnce->SetValue(ConfigPtr->TargetConfig.MaxNumTargetsAtOnce);

	ComboBoxOption_RecentTargetMemoryPolicy->ComboBox->SetSelectedOption(UEnum::GetDisplayValueAsText(ConfigPtr->TargetConfig.RecentTargetMemoryPolicy).ToString());
	SliderTextBoxOption_MaxNumRecentTargets->SetValue(ConfigPtr->TargetConfig.MaxNumRecentTargets);
	SliderTextBoxOption_RecentTargetTimeLength->SetValue(ConfigPtr->TargetConfig.RecentTargetTimeLength);

	switch (ConfigPtr->TargetConfig.RecentTargetMemoryPolicy)
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

	if (ConfigPtr->AIConfig.bEnableReinforcementLearning)
	{
		if (ConfigPtr->TargetConfig.TargetDamageType == ETargetDamageType::Tracking)
		{
			CheckBoxOption_EnableAI->CheckBox->SetIsEnabled(false);
			CheckBoxOption_EnableAI->CheckBox->SetIsChecked(false);
			ConfigPtr->AIConfig.bEnableReinforcementLearning = false;
			RequestUpdateAfterConfigChange.Broadcast();
		}
		else
		{
			CheckBoxOption_EnableAI->CheckBox->SetIsEnabled(true);
			CheckBoxOption_EnableAI->CheckBox->SetIsChecked(ConfigPtr->AIConfig.bEnableReinforcementLearning);
		}
	}

	if (ConfigPtr->AIConfig.bEnableReinforcementLearning)
	{
		SliderTextBoxOption_Alpha->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_Epsilon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_Gamma->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		SliderTextBoxOption_Alpha->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_Epsilon->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_Gamma->SetVisibility(ESlateVisibility::Collapsed);
	}

	SliderTextBoxOption_Alpha->SetValue(ConfigPtr->AIConfig.Alpha);
	SliderTextBoxOption_Epsilon->SetValue(ConfigPtr->AIConfig.Epsilon);
	SliderTextBoxOption_Gamma->SetValue(ConfigPtr->AIConfig.Gamma);

	SetAllOptionsValid(UpdateAllOptionsValid());
	UpdateBrushColors();
}

void UCustomGameModesWidget_General::OnCheckStateChanged_EnableAI(const bool bChecked)
{
	if (bChecked)
	{
		SliderTextBoxOption_Alpha->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_Epsilon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_Gamma->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		SliderTextBoxOption_Alpha->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_Epsilon->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_Gamma->SetVisibility(ESlateVisibility::Collapsed);
	}
	ConfigPtr->AIConfig.bEnableReinforcementLearning = bChecked;
	SetAllOptionsValid(UpdateAllOptionsValid());
	UpdateBrushColors();
}

void UCustomGameModesWidget_General::OnSelectionChanged_RecentTargetMemoryPolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (Selected.Num() != 1)
	{
		SetAllOptionsValid(UpdateAllOptionsValid());
		return;
	}

	const ERecentTargetMemoryPolicy Policy = GetEnumFromString<ERecentTargetMemoryPolicy>(Selected[0], ERecentTargetMemoryPolicy::None);

	switch (Policy)
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
	case ERecentTargetMemoryPolicy::UseTargetSpawnCD:
		default:
		SliderTextBoxOption_RecentTargetTimeLength->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MaxNumRecentTargets->SetVisibility(ESlateVisibility::Collapsed);
		break;
	}

	ConfigPtr->TargetConfig.RecentTargetMemoryPolicy = Policy;
	SetAllOptionsValid(UpdateAllOptionsValid());
	UpdateBrushColors();
}

FString UCustomGameModesWidget_General::GetComboBoxEntryTooltipStringTableKey_TargetActivationSelectionPolicy(const FString& EnumString)
{
	const ERecentTargetMemoryPolicy EnumValue = GetEnumFromString<ERecentTargetMemoryPolicy>(EnumString, ERecentTargetMemoryPolicy::None);
	return GetStringTableKeyNameFromEnum(EnumValue);
}

void UCustomGameModesWidget_General::OnSliderTextBoxValueChanged(USliderTextBoxWidget* Widget, const float Value)
{
	if (Widget == SliderTextBoxOption_SpawnBeatDelay)
	{
		ConfigPtr->TargetConfig.SpawnBeatDelay = Value;
	}
	else if (Widget == SliderTextBoxOption_TargetSpawnCD)
	{
		ConfigPtr->TargetConfig.TargetSpawnCD = Value;
	}
	else if (Widget == SliderTextBoxOption_MaxNumRecentTargets)
	{
		ConfigPtr->TargetConfig.MaxNumRecentTargets = Value;
	}
	else if (Widget == SliderTextBoxOption_RecentTargetTimeLength)
	{
		ConfigPtr->TargetConfig.RecentTargetTimeLength = Value;
	}
	else if (Widget == SliderTextBoxOption_MaxNumTargetsAtOnce)
	{
		ConfigPtr->TargetConfig.MaxNumTargetsAtOnce = Value;
	}
	else if (Widget == SliderTextBoxOption_Alpha)
	{
		ConfigPtr->AIConfig.Alpha = Value;
	}
	else if (Widget == SliderTextBoxOption_Epsilon)
	{
		ConfigPtr->AIConfig.Epsilon = Value;
	}
	else if (Widget == SliderTextBoxOption_Gamma)
	{
		ConfigPtr->AIConfig.Gamma = Value;
	}
}
