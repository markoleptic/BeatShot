﻿// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


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
	
	SetupTooltip(SliderTextBoxOption_TargetMaxLifeSpan->GetTooltipImage(), SliderTextBoxOption_TargetMaxLifeSpan->GetTooltipImageText());
	SetupTooltip(CheckBoxOption_UnlimitedTargetHealth->GetTooltipImage(), CheckBoxOption_UnlimitedTargetHealth->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_MaxHealth->GetTooltipImage(), SliderTextBoxOption_MaxHealth->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_ExpirationHealthPenalty->GetTooltipImage(), SliderTextBoxOption_ExpirationHealthPenalty->GetTooltipImageText());
	SetupTooltip(ComboBoxOption_DamageType->GetTooltipImage(), ComboBoxOption_DamageType->GetTooltipImageText());
	SetupTooltip(ComboBoxOption_ConsecutiveTargetScalePolicy->GetTooltipImage(), ComboBoxOption_ConsecutiveTargetScalePolicy->GetTooltipImageText());
	SetupTooltip(CheckBoxOption_ConstantTargetScale->GetTooltipImage(), CheckBoxOption_ConstantTargetScale->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_TargetScale->GetTooltipImage(), SliderTextBoxOption_TargetScale->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_MinTargetScale->GetTooltipImage(), SliderTextBoxOption_MinTargetScale->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_MaxTargetScale->GetTooltipImage(), SliderTextBoxOption_MaxTargetScale->GetTooltipImageText());
	
	SliderTextBoxOption_TargetMaxLifeSpan->SetValues(MinValue_Lifespan, MaxValue_Lifespan, SnapSize_Lifespan);
	SliderTextBoxOption_MaxHealth->SetValues(MinValue_MaxHealth, MaxValue_MaxHealth, SnapSize_MaxHealth);
	SliderTextBoxOption_ExpirationHealthPenalty->SetValues(MinValue_ExpirationHealthPenalty, MaxValue_ExpirationHealthPenalty, SnapSize_ExpirationHealthPenalty);
	SliderTextBoxOption_TargetScale->SetValues(MinValue_TargetScale, MaxValue_TargetScale, SnapSize_TargetScale);
	SliderTextBoxOption_MinTargetScale->SetValues(MinValue_TargetScale, MaxValue_TargetScale, SnapSize_TargetScale);
	SliderTextBoxOption_MaxTargetScale->SetValues(MinValue_TargetScale, MaxValue_TargetScale, SnapSize_TargetScale);

	SliderTextBoxOption_TargetMaxLifeSpan->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MaxHealth->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_ExpirationHealthPenalty->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_TargetScale->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MinTargetScale->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MaxTargetScale->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	
	ComboBoxOption_DamageType->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_DamageType);
	ComboBoxOption_ConsecutiveTargetScalePolicy->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_ConsecutiveTargetScalePolicy);

	ComboBoxOption_DamageType->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_DamageType);
	ComboBoxOption_ConsecutiveTargetScalePolicy->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_ConsecutiveTargetScalePolicy);

	CheckBoxOption_UnlimitedTargetHealth->CheckBox->OnCheckStateChanged.AddUniqueDynamic(this, &ThisClass::OnCheckStateChanged_UnlimitedTargetHealth);
	CheckBoxOption_ConstantTargetScale->CheckBox->OnCheckStateChanged.AddUniqueDynamic(this, &ThisClass::OnCheckStateChanged_ConstantTargetScale);
	
	ComboBoxOption_DamageType->ComboBox->ClearOptions();
	ComboBoxOption_ConsecutiveTargetScalePolicy->ComboBox->ClearOptions();

	for (const ETargetDamageType& Method : TEnumRange<ETargetDamageType>())
	{
		ComboBoxOption_DamageType->ComboBox->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}
	for (const EConsecutiveTargetScalePolicy& Method : TEnumRange<EConsecutiveTargetScalePolicy>())
	{
		ComboBoxOption_ConsecutiveTargetScalePolicy->ComboBox->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}
	
	CheckBoxOption_ConstantTargetScale->CheckBox->SetIsChecked(true);
	SliderTextBoxOption_TargetScale->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	SliderTextBoxOption_MinTargetScale->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MaxTargetScale->SetVisibility(ESlateVisibility::Collapsed);
	
	UpdateBrushColors();
}

bool UCustomGameModesWidget_Target::UpdateAllOptionsValid()
{
	const bool bUpdateRequired = UpdateTooltipWarningImages(ComboBoxOption_DamageType, GetWarningTooltipKeys());
	if (bUpdateRequired)
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
	bool bUpdateBrushColors = false;
	UpdateValueIfDifferent(SliderTextBoxOption_TargetMaxLifeSpan, BSConfig->TargetConfig.TargetMaxLifeSpan);
	UpdateValueIfDifferent(SliderTextBoxOption_MaxHealth, BSConfig->TargetConfig.MaxHealth);
	UpdateValueIfDifferent(SliderTextBoxOption_ExpirationHealthPenalty, BSConfig->TargetConfig.ExpirationHealthPenalty);
	UpdateValueIfDifferent(SliderTextBoxOption_TargetScale, BSConfig->TargetConfig.MinTargetScale);
	UpdateValueIfDifferent(SliderTextBoxOption_MinTargetScale, BSConfig->TargetConfig.MinTargetScale);
	UpdateValueIfDifferent(SliderTextBoxOption_MaxTargetScale, BSConfig->TargetConfig.MaxTargetScale);
	
	UpdateValueIfDifferent(ComboBoxOption_DamageType, UEnum::GetDisplayValueAsText(BSConfig->TargetConfig.TargetDamageType).ToString());
	UpdateValueIfDifferent(ComboBoxOption_ConsecutiveTargetScalePolicy, UEnum::GetDisplayValueAsText(BSConfig->TargetConfig.ConsecutiveTargetScalePolicy).ToString());

	if (UpdateValueIfDifferent(CheckBoxOption_UnlimitedTargetHealth, BSConfig->TargetConfig.MaxHealth == -1))
	{
		if (BSConfig->TargetConfig.MaxHealth == -1)
		{
			SliderTextBoxOption_MaxHealth->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			SliderTextBoxOption_MaxHealth->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		bUpdateBrushColors = true;
	}
	
	if (UpdateValueIfDifferent(CheckBoxOption_ConstantTargetScale, BSConfig->TargetConfig.MinTargetScale == BSConfig->TargetConfig.MaxTargetScale))
	{
		if (BSConfig->TargetConfig.MinTargetScale == BSConfig->TargetConfig.MaxTargetScale)
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
		bUpdateBrushColors = true;
	}
	
	if (bUpdateBrushColors)
	{
		UpdateBrushColors();
	}
	SetAllOptionsValid(UpdateAllOptionsValid());
}

TArray<FString> UCustomGameModesWidget_Target::GetWarningTooltipKeys()
{
	TArray<FString> ReturnArray;
	if (BSConfig->TargetConfig.TargetDamageType == ETargetDamageType::Tracking)
	{
		if (BSConfig->AIConfig.bEnableReinforcementLearning)
		{
			ReturnArray.Emplace("InvalidAI_Tracking");
		}
	}
	return ReturnArray;
}

void UCustomGameModesWidget_Target::OnCheckStateChanged_UnlimitedTargetHealth(const bool bChecked)
{
	if (bChecked)
	{
		BSConfig->TargetConfig.MaxHealth = -1;
		SliderTextBoxOption_MaxHealth->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		BSConfig->TargetConfig.MaxHealth = SliderTextBoxOption_MaxHealth->GetSliderValue();
		SliderTextBoxOption_MaxHealth->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	UpdateBrushColors();
}

void UCustomGameModesWidget_Target::OnCheckStateChanged_ConstantTargetScale(const bool bChecked)
{
	if (bChecked)
	{
		SliderTextBoxOption_TargetScale->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_MinTargetScale->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MaxTargetScale->SetVisibility(ESlateVisibility::Collapsed);
		BSConfig->TargetConfig.MinTargetScale = SliderTextBoxOption_TargetScale->GetSliderValue();
		BSConfig->TargetConfig.MaxTargetScale = SliderTextBoxOption_TargetScale->GetSliderValue();
	}
	else
	{
		SliderTextBoxOption_TargetScale->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MinTargetScale->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_MaxTargetScale->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		BSConfig->TargetConfig.MinTargetScale = SliderTextBoxOption_MinTargetScale->GetSliderValue();
		BSConfig->TargetConfig.MaxTargetScale = SliderTextBoxOption_MaxTargetScale->GetSliderValue();
	}
	
	SetAllOptionsValid(UpdateAllOptionsValid());
	UpdateBrushColors();
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
		BSConfig->TargetConfig.MinTargetScale = Value;
		BSConfig->TargetConfig.MaxTargetScale = Value;
	}
	else if (Widget == SliderTextBoxOption_MinTargetScale)
	{
		BSConfig->TargetConfig.MinTargetScale = Value;
	}
	else if (Widget == SliderTextBoxOption_MaxTargetScale)
	{
		BSConfig->TargetConfig.MaxTargetScale = Value;
	}
	
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_Target::OnSelectionChanged_DamageType(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
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
	
	const ETargetDamageType TargetDamageType = GetEnumFromString<ETargetDamageType>(Selected[0], ETargetDamageType::None);
	BSConfig->TargetConfig.TargetDamageType = TargetDamageType;
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_Target::OnSelectionChanged_ConsecutiveTargetScalePolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
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
	
	const EConsecutiveTargetScalePolicy TargetDistributionPolicy = GetEnumFromString<EConsecutiveTargetScalePolicy>(Selected[0], EConsecutiveTargetScalePolicy::None);
	BSConfig->TargetConfig.ConsecutiveTargetScalePolicy = TargetDistributionPolicy;
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