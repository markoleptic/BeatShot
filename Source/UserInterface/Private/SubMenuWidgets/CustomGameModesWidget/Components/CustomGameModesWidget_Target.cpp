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
	
	SetupTooltip(SliderTextBoxOption_TargetMaxLifeSpan->GetTooltipImage(), SliderTextBoxOption_TargetMaxLifeSpan->GetTooltipRegularText());
	SetupTooltip(SliderTextBoxOption_MaxHealth->GetTooltipImage(), SliderTextBoxOption_MaxHealth->GetTooltipRegularText());
	SetupTooltip(SliderTextBoxOption_ExpirationHealthPenalty->GetTooltipImage(), SliderTextBoxOption_ExpirationHealthPenalty->GetTooltipRegularText());
	SetupTooltip(ComboBoxOption_DamageType->GetTooltipImage(), ComboBoxOption_DamageType->GetTooltipRegularText());
	SetupTooltip(ComboBoxOption_ConsecutiveTargetScalePolicy->GetTooltipImage(), ComboBoxOption_ConsecutiveTargetScalePolicy->GetTooltipRegularText());
	SetupTooltip(CheckBoxOption_ConstantTargetScale->GetTooltipImage(), CheckBoxOption_ConstantTargetScale->GetTooltipRegularText());
	SetupTooltip(SliderTextBoxOption_TargetScale->GetTooltipImage(), SliderTextBoxOption_TargetScale->GetTooltipRegularText());
	SetupTooltip(SliderTextBoxOption_MinTargetScale->GetTooltipImage(), SliderTextBoxOption_MinTargetScale->GetTooltipRegularText());
	SetupTooltip(SliderTextBoxOption_MaxTargetScale->GetTooltipImage(), SliderTextBoxOption_MaxTargetScale->GetTooltipRegularText());
	
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

	SliderTextBoxOption_MinTargetScale->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MaxTargetScale->SetVisibility(ESlateVisibility::Collapsed);
	
	UpdateBrushColors();
}

bool UCustomGameModesWidget_Target::UpdateCanTransitionForward()
{
	return Super::UpdateCanTransitionForward();
}

void UCustomGameModesWidget_Target::UpdateOptions()
{
	SliderTextBoxOption_TargetMaxLifeSpan->SetValue(ConfigPtr->TargetConfig.TargetMaxLifeSpan);
	SliderTextBoxOption_MaxHealth->SetValue(ConfigPtr->TargetConfig.MaxHealth);
	SliderTextBoxOption_ExpirationHealthPenalty->SetValue(ConfigPtr->TargetConfig.ExpirationHealthPenalty);
	
	ComboBoxOption_DamageType->ComboBox->SetSelectedOption(UEnum::GetDisplayValueAsText(ConfigPtr->TargetConfig.TargetDamageType).ToString());
	ComboBoxOption_ConsecutiveTargetScalePolicy->ComboBox->SetSelectedOption(UEnum::GetDisplayValueAsText(ConfigPtr->TargetConfig.ConsecutiveTargetScalePolicy).ToString());

	SliderTextBoxOption_TargetScale->SetValue(ConfigPtr->TargetConfig.MinTargetScale);
	SliderTextBoxOption_MinTargetScale->SetValue(ConfigPtr->TargetConfig.MinTargetScale);
	SliderTextBoxOption_MaxTargetScale->SetValue(ConfigPtr->TargetConfig.MaxTargetScale);
	
	CheckBoxOption_ConstantTargetScale->CheckBox->SetIsChecked(ConfigPtr->TargetConfig.MinTargetScale == ConfigPtr->TargetConfig.MaxTargetScale);
	
	if (ConfigPtr->TargetConfig.MinTargetScale == ConfigPtr->TargetConfig.MaxTargetScale)
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
	
	SetCanTransitionForward(UpdateCanTransitionForward());
	UpdateBrushColors();
}

void UCustomGameModesWidget_Target::OnCheckStateChanged_ConstantTargetScale(const bool bChecked)
{
	if (bChecked)
	{
		SliderTextBoxOption_TargetScale->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_MinTargetScale->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MaxTargetScale->SetVisibility(ESlateVisibility::Collapsed);
		ConfigPtr->TargetConfig.MinTargetScale = SliderTextBoxOption_TargetScale->GetValue();
		ConfigPtr->TargetConfig.MaxTargetScale = SliderTextBoxOption_TargetScale->GetValue();
	}
	else
	{
		SliderTextBoxOption_TargetScale->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MinTargetScale->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_MaxTargetScale->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ConfigPtr->TargetConfig.MinTargetScale = SliderTextBoxOption_MinTargetScale->GetValue();
		ConfigPtr->TargetConfig.MaxTargetScale = SliderTextBoxOption_MaxTargetScale->GetValue();
	}
	
	SetCanTransitionForward(UpdateCanTransitionForward());
	UpdateBrushColors();
}

void UCustomGameModesWidget_Target::OnSelectionChanged_DamageType(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (Selected.Num() != 1)
	{
		SetCanTransitionForward(UpdateCanTransitionForward());
		return;
	}
	
	const ETargetDamageType TargetDamageType = GetEnumFromString<ETargetDamageType>(Selected[0], ETargetDamageType::None);
	ConfigPtr->TargetConfig.TargetDamageType = TargetDamageType;
	
	SetCanTransitionForward(UpdateCanTransitionForward());
}

void UCustomGameModesWidget_Target::OnSelectionChanged_ConsecutiveTargetScalePolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (Selected.Num() != 1)
	{
		SetCanTransitionForward(UpdateCanTransitionForward());
		return;
	}
	
	const EConsecutiveTargetScalePolicy TargetDistributionPolicy = GetEnumFromString<EConsecutiveTargetScalePolicy>(Selected[0], EConsecutiveTargetScalePolicy::None);
	ConfigPtr->TargetConfig.ConsecutiveTargetScalePolicy = TargetDistributionPolicy;
	
	SetCanTransitionForward(UpdateCanTransitionForward());
}

FString UCustomGameModesWidget_Target::GetComboBoxEntryTooltipStringTableKey_DamageType(const FString& EnumString)
{
	const ETargetDamageType EnumValue = GetEnumFromString<ETargetDamageType>(EnumString, ETargetDamageType::None);
	return GetStringTableKeyNameFromEnum(EnumValue);
}

FString UCustomGameModesWidget_Target::GetComboBoxEntryTooltipStringTableKey_ConsecutiveTargetScalePolicy(const FString& EnumString)
{
	const EConsecutiveTargetScalePolicy EnumValue = GetEnumFromString<EConsecutiveTargetScalePolicy>(EnumString, EConsecutiveTargetScalePolicy::None);
	return GetStringTableKeyNameFromEnum(EnumValue);
}

void UCustomGameModesWidget_Target::OnSliderTextBoxValueChanged(USliderTextBoxWidget* Widget, const float Value)
{
	if (Widget == SliderTextBoxOption_TargetMaxLifeSpan)
	{
		ConfigPtr->TargetConfig.TargetMaxLifeSpan = Value;
	}
	else if (Widget == SliderTextBoxOption_MaxHealth)
	{
		ConfigPtr->TargetConfig.MaxHealth = Value;
	}
	else if (Widget == SliderTextBoxOption_ExpirationHealthPenalty)
	{
		ConfigPtr->TargetConfig.ExpirationHealthPenalty = Value;
	}
	else if (Widget == SliderTextBoxOption_TargetScale && SliderTextBoxOption_TargetScale->GetVisibility() != ESlateVisibility::Collapsed)
	{
		ConfigPtr->TargetConfig.MinTargetScale = Value;
		ConfigPtr->TargetConfig.MaxTargetScale = Value;
	}
	else if (Widget == SliderTextBoxOption_MinTargetScale)
	{
		ConfigPtr->TargetConfig.MinTargetScale = Value;
	}
	else if (Widget == SliderTextBoxOption_MaxTargetScale)
	{
		ConfigPtr->TargetConfig.MaxTargetScale = Value;
	}
}
