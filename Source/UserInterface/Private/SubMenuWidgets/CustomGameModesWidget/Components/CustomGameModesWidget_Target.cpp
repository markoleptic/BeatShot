// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_Target.h"
#include "Components/CheckBox.h"
#include "WidgetComponents/BSComboBoxString.h"
#include "WidgetComponents/MenuOptionWidgets/CheckBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/ComboBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/SliderTextBoxWidget.h"

void UCustomGameModesWidget_Target::InitComponent(FBSConfig* InConfigPtr, TObjectPtr<UCustomGameModesWidgetComponent> InNext)
{
	Super::InitComponent(InConfigPtr, InNext);
}

void UCustomGameModesWidget_Target::NativeConstruct()
{
	Super::NativeConstruct();
	
	SliderTextBoxOption_TargetMaxLifeSpan->SetValues(MinValue_Lifespan, MaxValue_Lifespan, SnapSize_Lifespan);
	SliderTextBoxOption_MaxHealth->SetValues(MinValue_MaxHealth, MaxValue_MaxHealth, SnapSize_MaxHealth);
	SliderTextBoxOption_ExpirationHealthPenalty->SetValues(MinValue_ExpirationHealthPenalty, MaxValue_ExpirationHealthPenalty, SnapSize_ExpirationHealthPenalty);
	SliderTextBoxOption_TargetScale->SetValues(MinValue_TargetScale, MaxValue_TargetScale, SnapSize_TargetScale);
	SliderTextBoxOption_MinTargetScale->SetValues(MinValue_TargetScale, MaxValue_TargetScale, SnapSize_TargetScale);
	SliderTextBoxOption_MaxTargetScale->SetValues(MinValue_TargetScale, MaxValue_TargetScale, SnapSize_TargetScale);
	
	SliderTextBoxOption_TargetScale->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MinTargetScale->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MaxTargetScale->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_TargetMaxLifeSpan->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MaxHealth->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_ExpirationHealthPenalty->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	ComboBoxOption_ConsecutiveTargetScalePolicy->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_ConsecutiveTargetScalePolicy);
	ComboBoxOption_DamageType->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_DamageType);
	ComboBoxOption_DamageType->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_DamageType);
	CheckBoxOption_UnlimitedTargetHealth->CheckBox->OnCheckStateChanged.AddUniqueDynamic(this, &ThisClass::OnCheckStateChanged_UnlimitedTargetHealth);
	ComboBoxOption_ConsecutiveTargetScalePolicy->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_ConsecutiveTargetScalePolicy);
	
	ComboBoxOption_DamageType->ComboBox->ClearOptions();
	ComboBoxOption_ConsecutiveTargetScalePolicy->ComboBox->ClearOptions();

	TArray<FString> Options;
	for (const ETargetDamageType& Method : TEnumRange<ETargetDamageType>())
	{
		Options.Add(GetStringFromEnum(Method));
	}
	ComboBoxOption_DamageType->SortAndAddOptions(Options);
	Options.Empty();
	
	for (const EConsecutiveTargetScalePolicy& Method : TEnumRange<EConsecutiveTargetScalePolicy>())
	{
		Options.Add(GetStringFromEnum(Method));
	}
	ComboBoxOption_ConsecutiveTargetScalePolicy->SortAndAddOptions(Options);
	Options.Empty();
	
	SliderTextBoxOption_TargetScale->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MinTargetScale->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MaxTargetScale->SetVisibility(ESlateVisibility::Collapsed);
	
	UpdateBrushColors();
}

bool UCustomGameModesWidget_Target::UpdateAllOptionsValid()
{
	TArray<FTooltipData> UpdateArray;
	bool bRequestComponentUpdate = false;
	
	if (BSConfig->TargetConfig.TargetDamageType == ETargetDamageType::Tracking)
	{
		if (BSConfig->AIConfig.bEnableReinforcementLearning)
		{
			UpdateArray.Emplace("InvalidAI_Tracking", ETooltipImageType::Warning);
		}
	}
	bRequestComponentUpdate = UpdateWarningTooltips(ComboBoxOption_DamageType, UpdateArray) || bRequestComponentUpdate;
	UpdateArray.Empty();
	
	if (bRequestComponentUpdate)
	{
		RequestComponentUpdate.Broadcast();
	}
	
	if (!ComboBoxOption_DamageType->GetTooltipWarningImageKeys().IsEmpty())
	{
		return false;
	}
	
	return true;
}

void UCustomGameModesWidget_Target::UpdateOptionsFromConfig()
{
	UpdateValueIfDifferent(SliderTextBoxOption_TargetMaxLifeSpan, BSConfig->TargetConfig.TargetMaxLifeSpan);
	UpdateValueIfDifferent(SliderTextBoxOption_MaxHealth, BSConfig->TargetConfig.MaxHealth);
	UpdateValueIfDifferent(SliderTextBoxOption_ExpirationHealthPenalty, BSConfig->TargetConfig.ExpirationHealthPenalty);
	
	UpdateValueIfDifferent(ComboBoxOption_DamageType, GetStringFromEnum(BSConfig->TargetConfig.TargetDamageType));
	UpdateValueIfDifferent(CheckBoxOption_UnlimitedTargetHealth, BSConfig->TargetConfig.MaxHealth == -1);
	
	UpdateDependentOptions_UnlimitedTargetHealth(BSConfig->TargetConfig.MaxHealth == -1);

	UpdateValueIfDifferent(SliderTextBoxOption_TargetScale, BSConfig->TargetConfig.MinSpawnedTargetScale);
	UpdateValueIfDifferent(SliderTextBoxOption_MinTargetScale, BSConfig->TargetConfig.MinSpawnedTargetScale);
	UpdateValueIfDifferent(SliderTextBoxOption_MaxTargetScale, BSConfig->TargetConfig.MaxSpawnedTargetScale);

	UpdateValueIfDifferent(ComboBoxOption_ConsecutiveTargetScalePolicy, GetStringFromEnum(BSConfig->TargetConfig.ConsecutiveTargetScalePolicy));
	
	UpdateDependentOptions_ConsecutiveTargetScalePolicy(BSConfig->TargetConfig.ConsecutiveTargetScalePolicy);
	
	UpdateBrushColors();
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_Target::UpdateDependentOptions_UnlimitedTargetHealth(const bool bInUnlimitedTargetHealth)
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

void UCustomGameModesWidget_Target::UpdateDependentOptions_ConsecutiveTargetScalePolicy(const EConsecutiveTargetScalePolicy InConsecutiveTargetScalePolicy)
{
	if (InConsecutiveTargetScalePolicy == EConsecutiveTargetScalePolicy::Static)
	{
		SliderTextBoxOption_TargetScale->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_MinTargetScale->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MaxTargetScale->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		SliderTextBoxOption_TargetScale->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MinTargetScale->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_MaxTargetScale->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UCustomGameModesWidget_Target::OnSelectionChanged_ConsecutiveTargetScalePolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || Selected.Num() != 1)
	{
		SetAllOptionsValid(UpdateAllOptionsValid());
		return;
	}
	
	BSConfig->TargetConfig.ConsecutiveTargetScalePolicy = GetEnumFromString<EConsecutiveTargetScalePolicy>(Selected[0]);
	UpdateDependentOptions_ConsecutiveTargetScalePolicy(BSConfig->TargetConfig.ConsecutiveTargetScalePolicy);

	if (BSConfig->TargetConfig.ConsecutiveTargetScalePolicy == EConsecutiveTargetScalePolicy::Static)
	{
		BSConfig->TargetConfig.MinSpawnedTargetScale = SliderTextBoxOption_TargetScale->GetSliderValue();
		BSConfig->TargetConfig.MaxSpawnedTargetScale = SliderTextBoxOption_TargetScale->GetSliderValue();
	}
	else
	{
		BSConfig->TargetConfig.MinSpawnedTargetScale = SliderTextBoxOption_MinTargetScale->GetSliderValue();
		BSConfig->TargetConfig.MaxSpawnedTargetScale = SliderTextBoxOption_MaxTargetScale->GetSliderValue();
	}
	UpdateBrushColors();
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_Target::OnCheckStateChanged_UnlimitedTargetHealth(const bool bChecked)
{
	BSConfig->TargetConfig.MaxHealth = bChecked ? -1 : SliderTextBoxOption_MaxHealth->GetSliderValue();
	UpdateDependentOptions_UnlimitedTargetHealth(bChecked);
	
	UpdateBrushColors();
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_Target::OnSliderTextBoxValueChanged(USliderTextBoxWidget* Widget, const float Value)
{
	if (Widget == SliderTextBoxOption_TargetMaxLifeSpan)
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
	else if (Widget == SliderTextBoxOption_TargetScale && SliderTextBoxOption_TargetScale->GetVisibility() != ESlateVisibility::Collapsed)
	{
		BSConfig->TargetConfig.MinSpawnedTargetScale = Value;
		BSConfig->TargetConfig.MaxSpawnedTargetScale = Value;
	}
	else if (Widget == SliderTextBoxOption_MinTargetScale)
	{
		BSConfig->TargetConfig.MinSpawnedTargetScale = Value;
	}
	else if (Widget == SliderTextBoxOption_MaxTargetScale)
	{
		BSConfig->TargetConfig.MaxSpawnedTargetScale = Value;
	}
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_Target::OnSelectionChanged_DamageType(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || Selected.Num() != 1)
	{
		SetAllOptionsValid(UpdateAllOptionsValid());
		return;
	}
	
	BSConfig->TargetConfig.TargetDamageType = GetEnumFromString<ETargetDamageType>(Selected[0]);
	SetAllOptionsValid(UpdateAllOptionsValid());
}

FString UCustomGameModesWidget_Target::GetComboBoxEntryTooltipStringTableKey_DamageType(const FString& EnumString)
{
	const ETargetDamageType EnumValue = GetEnumFromString<ETargetDamageType>(EnumString);
	return GetStringTableKeyNameFromEnum(EnumValue);
}

FString UCustomGameModesWidget_Target::GetComboBoxEntryTooltipStringTableKey_ConsecutiveTargetScalePolicy(const FString& EnumString)
{
	const EConsecutiveTargetScalePolicy EnumValue = GetEnumFromString<EConsecutiveTargetScalePolicy>(EnumString);
	return GetStringTableKeyNameFromEnum(EnumValue);
}