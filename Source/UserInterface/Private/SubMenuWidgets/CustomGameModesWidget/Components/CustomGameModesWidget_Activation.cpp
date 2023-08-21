// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_Activation.h"
#include "Components/CheckBox.h"
#include "Components/Slider.h"
#include "WidgetComponents/BSComboBoxString.h"
#include "WidgetComponents/MenuOptionWidgets/CheckBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/ComboBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/SliderTextBoxWidget.h"

void UCustomGameModesWidget_Activation::InitComponent(FBSConfig* InConfigPtr, TObjectPtr<UCustomGameModesWidgetComponent> InNext)
{
	Super::InitComponent(InConfigPtr, InNext);
}

void UCustomGameModesWidget_Activation::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetupTooltip(CheckBoxOption_ConstantNumTargetsToActivateAtOnce->GetTooltipImage(), CheckBoxOption_ConstantNumTargetsToActivateAtOnce->GetTooltipRegularText());
	SetupTooltip(SliderTextBoxOption_NumTargetsToActivateAtOnce->GetTooltipImage(), SliderTextBoxOption_NumTargetsToActivateAtOnce->GetTooltipRegularText());
	SetupTooltip(SliderTextBoxOption_MinNumTargetsToActivateAtOnce->GetTooltipImage(), SliderTextBoxOption_MinNumTargetsToActivateAtOnce->GetTooltipRegularText());
	SetupTooltip(SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->GetTooltipImage(), SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->GetTooltipRegularText());
	SetupTooltip(ComboBoxOption_TargetActivationSelectionPolicy->GetTooltipImage(), ComboBoxOption_TargetActivationSelectionPolicy->GetTooltipRegularText());
	SetupTooltip(ComboBoxOption_TargetActivationResponses->GetTooltipImage(), ComboBoxOption_TargetActivationResponses->GetTooltipRegularText());
	SetupTooltip(ComboBoxOption_MovingTargetDirectionMode->GetTooltipImage(), ComboBoxOption_MovingTargetDirectionMode->GetTooltipRegularText());
	SetupTooltip(ComboBoxOption_LifetimeTargetScalePolicy->GetTooltipImage(), ComboBoxOption_LifetimeTargetScalePolicy->GetTooltipRegularText());
	SetupTooltip(CheckBoxOption_ConstantTargetSpeed->GetTooltipImage(), CheckBoxOption_ConstantTargetSpeed->GetTooltipRegularText());
	SetupTooltip(SliderTextBoxOption_TargetSpeed->GetTooltipImage(), SliderTextBoxOption_TargetSpeed->GetTooltipRegularText());
	SetupTooltip(SliderTextBoxOption_MinTargetSpeed->GetTooltipImage(), SliderTextBoxOption_MinTargetSpeed->GetTooltipRegularText());
	SetupTooltip(SliderTextBoxOption_MaxTargetSpeed->GetTooltipImage(), SliderTextBoxOption_MaxTargetSpeed->GetTooltipRegularText());
	
	SliderTextBoxOption_NumTargetsToActivateAtOnce->SetValues(MinValue_MaxNumActivatedTargetsAtOnce, MaxValue_MaxNumActivatedTargetsAtOnce, SnapSize_MaxNumActivatedTargetsAtOnce);
	SliderTextBoxOption_MinNumTargetsToActivateAtOnce->SetValues(MinValue_MaxNumActivatedTargetsAtOnce, MaxValue_MaxNumActivatedTargetsAtOnce, SnapSize_MaxNumActivatedTargetsAtOnce);
	SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->SetValues(MinValue_MaxNumActivatedTargetsAtOnce, MaxValue_MaxNumActivatedTargetsAtOnce, SnapSize_MaxNumActivatedTargetsAtOnce);
	SliderTextBoxOption_TargetSpeed->SetValues(MinValue_TargetSpeed, MaxValue_TargetSpeed, SnapSize_TargetSpeed);
	SliderTextBoxOption_MinTargetSpeed->SetValues(MinValue_TargetSpeed, MaxValue_TargetSpeed, SnapSize_TargetSpeed);
	SliderTextBoxOption_MaxTargetSpeed->SetValues(MinValue_TargetSpeed, MaxValue_TargetSpeed, SnapSize_TargetSpeed);

	SliderTextBoxOption_NumTargetsToActivateAtOnce->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MinNumTargetsToActivateAtOnce->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_TargetSpeed->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MinTargetSpeed->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MaxTargetSpeed->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	
	CheckBoxOption_ConstantNumTargetsToActivateAtOnce->CheckBox->OnCheckStateChanged.AddUniqueDynamic(this, &ThisClass::OnCheckStateChanged_ConstantNumTargetsToActivateAtOnce);
	CheckBoxOption_ConstantTargetSpeed->CheckBox->OnCheckStateChanged.AddUniqueDynamic(this, &ThisClass::OnCheckStateChanged_ConstantTargetSpeed);

	ComboBoxOption_TargetActivationSelectionPolicy->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_TargetActivationSelectionPolicy);
	ComboBoxOption_TargetActivationResponses->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_TargetActivationResponses);
	ComboBoxOption_MovingTargetDirectionMode->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_MovingTargetDirectionMode);
	ComboBoxOption_LifetimeTargetScalePolicy->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_LifetimeTargetScalePolicy);

	ComboBoxOption_TargetActivationSelectionPolicy->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetActivationSelectionPolicy);
	ComboBoxOption_TargetActivationResponses->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetActivationResponses);
	ComboBoxOption_MovingTargetDirectionMode->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_MovingTargetDirectionMode);
	ComboBoxOption_LifetimeTargetScalePolicy->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_LifetimeTargetScalePolicy);

	ComboBoxOption_TargetActivationSelectionPolicy->ComboBox->ClearOptions();
	ComboBoxOption_TargetActivationResponses->ComboBox->ClearOptions();
	ComboBoxOption_MovingTargetDirectionMode->ComboBox->ClearOptions();
	ComboBoxOption_LifetimeTargetScalePolicy->ComboBox->ClearOptions();

	for (const ETargetActivationSelectionPolicy& Method : TEnumRange<ETargetActivationSelectionPolicy>())
	{
		ComboBoxOption_TargetActivationSelectionPolicy->ComboBox->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}
	for (const ETargetActivationResponse& Method : TEnumRange<ETargetActivationResponse>())
	{
		ComboBoxOption_TargetActivationResponses->ComboBox->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}
	for (const EMovingTargetDirectionMode& Method : TEnumRange<EMovingTargetDirectionMode>())
	{
		ComboBoxOption_MovingTargetDirectionMode->ComboBox->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}
	for (const ELifetimeTargetScalePolicy& Method : TEnumRange<ELifetimeTargetScalePolicy>())
	{
		ComboBoxOption_LifetimeTargetScalePolicy->ComboBox->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}

	SliderTextBoxOption_MinNumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::Collapsed);
	CheckBoxOption_ConstantTargetSpeed->SetVisibility(ESlateVisibility::Collapsed);
	ComboBoxOption_MovingTargetDirectionMode->SetVisibility(ESlateVisibility::Collapsed);
	ComboBoxOption_LifetimeTargetScalePolicy->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_TargetSpeed->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MinTargetSpeed->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MaxTargetSpeed->SetVisibility(ESlateVisibility::Collapsed);

	UpdateBrushColors();
}

bool UCustomGameModesWidget_Activation::UpdateCanTransitionForward()
{
	// TODO: Handle more complex activation that could trip ppl up
	if (ComboBoxOption_TargetActivationSelectionPolicy->ComboBox->GetSelectedOptionCount() != 1)
	{
		return false;
	}
	if (ComboBoxOption_TargetActivationResponses->ComboBox->GetSelectedOptionCount() < 1)
	{
		return false;
	}
	if (ComboBoxOption_MovingTargetDirectionMode->GetVisibility() != ESlateVisibility::Collapsed && ComboBoxOption_MovingTargetDirectionMode->ComboBox->GetSelectedOptionCount() != 1)
	{
		return false;
	}
	if (ComboBoxOption_LifetimeTargetScalePolicy->GetVisibility() != ESlateVisibility::Collapsed && ComboBoxOption_LifetimeTargetScalePolicy->ComboBox->GetSelectedOptionCount() != 1)
	{
		return false;
	}
	return true;
}

void UCustomGameModesWidget_Activation::UpdateOptions()
{
	SliderTextBoxOption_NumTargetsToActivateAtOnce->SetValue(ConfigPtr->TargetConfig.MinNumTargetsToActivateAtOnce);
	SliderTextBoxOption_MinNumTargetsToActivateAtOnce->SetValue(ConfigPtr->TargetConfig.MinNumTargetsToActivateAtOnce);
	SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->SetValue(ConfigPtr->TargetConfig.MinNumTargetsToActivateAtOnce);

	SliderTextBoxOption_TargetSpeed->SetValue(ConfigPtr->TargetConfig.MinTargetSpeed);
	SliderTextBoxOption_MinTargetSpeed->SetValue(ConfigPtr->TargetConfig.MinTargetSpeed);
	SliderTextBoxOption_MinTargetSpeed->SetValue(ConfigPtr->TargetConfig.MaxTargetSpeed);

	ComboBoxOption_TargetActivationResponses->ComboBox->SetSelectedOptions(GetStringArrayFromEnumArray(ConfigPtr->TargetConfig.TargetActivationResponses));
	ComboBoxOption_TargetActivationSelectionPolicy->ComboBox->SetSelectedOption(UEnum::GetDisplayValueAsText(ConfigPtr->TargetConfig.TargetActivationSelectionPolicy).ToString());
	ComboBoxOption_MovingTargetDirectionMode->ComboBox->SetSelectedOption(UEnum::GetDisplayValueAsText(ConfigPtr->TargetConfig.MovingTargetDirectionMode).ToString());
	ComboBoxOption_LifetimeTargetScalePolicy->ComboBox->SetSelectedOption(UEnum::GetDisplayValueAsText(ConfigPtr->TargetConfig.LifetimeTargetScalePolicy).ToString());

	if (ConfigPtr->TargetConfig.MinNumTargetsToActivateAtOnce == ConfigPtr->TargetConfig.MaxNumTargetsToActivateAtOnce)
	{
		CheckBoxOption_ConstantNumTargetsToActivateAtOnce->CheckBox->SetIsChecked(true);
		SliderTextBoxOption_NumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_MinNumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		CheckBoxOption_ConstantNumTargetsToActivateAtOnce->CheckBox->SetIsChecked(false);
		SliderTextBoxOption_NumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MinNumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	const TArray<FString> Options = ComboBoxOption_TargetActivationResponses->ComboBox->GetSelectedOptions();
	if (Options.Contains(UEnum::GetDisplayValueAsText(ETargetActivationResponse::ChangeDirection).ToString()))
	{
		ComboBoxOption_MovingTargetDirectionMode->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		ComboBoxOption_MovingTargetDirectionMode->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (Options.Contains(UEnum::GetDisplayValueAsText(ETargetActivationResponse::ChangeScale).ToString()))
	{
		ComboBoxOption_LifetimeTargetScalePolicy->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		ComboBoxOption_LifetimeTargetScalePolicy->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (Options.Contains(UEnum::GetDisplayValueAsText(ETargetActivationResponse::ChangeVelocity).ToString()))
	{
		CheckBoxOption_ConstantTargetSpeed->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		if (ConfigPtr->TargetConfig.MinTargetSpeed == ConfigPtr->TargetConfig.MaxTargetSpeed)
		{
			CheckBoxOption_ConstantTargetSpeed->CheckBox->SetIsChecked(true);
			SliderTextBoxOption_TargetSpeed->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			SliderTextBoxOption_MinTargetSpeed->SetVisibility(ESlateVisibility::Collapsed);
			SliderTextBoxOption_MaxTargetSpeed->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			CheckBoxOption_ConstantTargetSpeed->CheckBox->SetIsChecked(false);
			SliderTextBoxOption_TargetSpeed->SetVisibility(ESlateVisibility::Collapsed);
			SliderTextBoxOption_MinTargetSpeed->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			SliderTextBoxOption_MaxTargetSpeed->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
	else
	{
		CheckBoxOption_ConstantTargetSpeed->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_TargetSpeed->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MinTargetSpeed->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MaxTargetSpeed->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UCustomGameModesWidget_Activation::OnCheckStateChanged_ConstantNumTargetsToActivateAtOnce(const bool bChecked)
{
	if (bChecked)
	{
		CheckBoxOption_ConstantNumTargetsToActivateAtOnce->CheckBox->SetIsChecked(true);
		SliderTextBoxOption_NumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_MinNumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::Collapsed);
		ConfigPtr->TargetConfig.MinNumTargetsToActivateAtOnce = SliderTextBoxOption_NumTargetsToActivateAtOnce->GetValue();
		ConfigPtr->TargetConfig.MaxNumTargetsToActivateAtOnce = SliderTextBoxOption_NumTargetsToActivateAtOnce->GetValue();
	}
	else
	{
		CheckBoxOption_ConstantNumTargetsToActivateAtOnce->CheckBox->SetIsChecked(false);
		SliderTextBoxOption_NumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MinNumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ConfigPtr->TargetConfig.MinNumTargetsToActivateAtOnce = SliderTextBoxOption_MinNumTargetsToActivateAtOnce->GetValue();
		ConfigPtr->TargetConfig.MaxNumTargetsToActivateAtOnce = SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->GetValue();
	}
	
	UpdateBrushColors();
	SetCanTransitionForward(UpdateCanTransitionForward());
}

void UCustomGameModesWidget_Activation::OnCheckStateChanged_ConstantTargetSpeed(const bool bChecked)
{
	if (bChecked)
	{
		SliderTextBoxOption_TargetSpeed->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_MinTargetSpeed->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MaxTargetSpeed->SetVisibility(ESlateVisibility::Collapsed);
		ConfigPtr->TargetConfig.MinTargetSpeed = SliderTextBoxOption_TargetSpeed->GetValue();
		ConfigPtr->TargetConfig.MaxTargetSpeed = SliderTextBoxOption_TargetSpeed->GetValue();
	}
	else
	{
		SliderTextBoxOption_TargetSpeed->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MinTargetSpeed->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_MaxTargetSpeed->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ConfigPtr->TargetConfig.MinTargetSpeed = SliderTextBoxOption_MinTargetSpeed->GetValue();
		ConfigPtr->TargetConfig.MaxTargetSpeed = SliderTextBoxOption_MaxTargetSpeed->GetValue();
	}

	UpdateBrushColors();
	SetCanTransitionForward(UpdateCanTransitionForward());
}

void UCustomGameModesWidget_Activation::OnSelectionChanged_TargetActivationSelectionPolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	SetCanTransitionForward(UpdateCanTransitionForward());

	if (Selected.Num() != 1)
	{
		return;
	}

	ConfigPtr->TargetConfig.TargetActivationSelectionPolicy = GetEnumFromString<ETargetActivationSelectionPolicy>(Selected[0], ETargetActivationSelectionPolicy::None);
}

void UCustomGameModesWidget_Activation::OnSelectionChanged_TargetActivationResponses(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (Selected.Contains(UEnum::GetDisplayValueAsText(ETargetActivationResponse::ChangeDirection).ToString()))
	{
		ComboBoxOption_MovingTargetDirectionMode->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		ComboBoxOption_MovingTargetDirectionMode->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (Selected.Contains(UEnum::GetDisplayValueAsText(ETargetActivationResponse::ChangeScale).ToString()))
	{
		ComboBoxOption_LifetimeTargetScalePolicy->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		ComboBoxOption_LifetimeTargetScalePolicy->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (Selected.Contains(UEnum::GetDisplayValueAsText(ETargetActivationResponse::ChangeVelocity).ToString()))
	{
		CheckBoxOption_ConstantTargetSpeed->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		if (SliderTextBoxOption_MinTargetSpeed->Slider->GetValue() != SliderTextBoxOption_MaxTargetSpeed->Slider->GetValue())
		{
			CheckBoxOption_ConstantTargetSpeed->CheckBox->SetIsChecked(true);
			SliderTextBoxOption_TargetSpeed->SetVisibility(ESlateVisibility::Collapsed);
			SliderTextBoxOption_MinTargetSpeed->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			SliderTextBoxOption_MaxTargetSpeed->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			CheckBoxOption_ConstantTargetSpeed->CheckBox->SetIsChecked(false);
			SliderTextBoxOption_TargetSpeed->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			SliderTextBoxOption_MinTargetSpeed->SetVisibility(ESlateVisibility::Collapsed);
			SliderTextBoxOption_MaxTargetSpeed->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	else
	{
		CheckBoxOption_ConstantTargetSpeed->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_TargetSpeed->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MinTargetSpeed->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MaxTargetSpeed->SetVisibility(ESlateVisibility::Collapsed);
	}

	ConfigPtr->TargetConfig.TargetActivationResponses.Empty();
	for (const FString& String : Selected)
	{
		ConfigPtr->TargetConfig.TargetActivationResponses.AddUnique(GetEnumFromString<ETargetActivationResponse>(String, ETargetActivationResponse::None));
	}

	UpdateBrushColors();
	SetCanTransitionForward(UpdateCanTransitionForward());
}

void UCustomGameModesWidget_Activation::OnSelectionChanged_MovingTargetDirectionMode(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	SetCanTransitionForward(UpdateCanTransitionForward());

	if (Selected.Num() != 1)
	{
		return;
	}

	ConfigPtr->TargetConfig.MovingTargetDirectionMode = GetEnumFromString<EMovingTargetDirectionMode>(Selected[0], EMovingTargetDirectionMode::None);
}

void UCustomGameModesWidget_Activation::OnSelectionChanged_LifetimeTargetScalePolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	SetCanTransitionForward(UpdateCanTransitionForward());

	if (Selected.Num() != 1)
	{
		return;
	}

	ConfigPtr->TargetConfig.LifetimeTargetScalePolicy = GetEnumFromString<ELifetimeTargetScalePolicy>(Selected[0], ELifetimeTargetScalePolicy::None);
}

void UCustomGameModesWidget_Activation::OnSliderTextBoxValueChanged(USliderTextBoxWidget* Widget, const float Value)
{
	if (Widget == SliderTextBoxOption_NumTargetsToActivateAtOnce && SliderTextBoxOption_NumTargetsToActivateAtOnce->GetVisibility() != ESlateVisibility::Collapsed)
	{
		ConfigPtr->TargetConfig.MaxNumTargetsToActivateAtOnce = Value;
		ConfigPtr->TargetConfig.MinNumTargetsToActivateAtOnce = Value;
	}
	else if (Widget == SliderTextBoxOption_MinNumTargetsToActivateAtOnce)
	{
		ConfigPtr->TargetConfig.MinNumTargetsToActivateAtOnce = Value;
	}
	else if (Widget == SliderTextBoxOption_MaxNumTargetsToActivateAtOnce)
	{
		ConfigPtr->TargetConfig.MaxNumTargetsToActivateAtOnce = Value;
	}
	else if (Widget == SliderTextBoxOption_TargetSpeed && SliderTextBoxOption_TargetSpeed->GetVisibility() != ESlateVisibility::Collapsed)
	{
		ConfigPtr->TargetConfig.MinTargetSpeed = Value;
		ConfigPtr->TargetConfig.MaxTargetSpeed = Value;
	}
	else if (Widget == SliderTextBoxOption_MinTargetSpeed)
	{
		ConfigPtr->TargetConfig.MinTargetSpeed = Value;
	}
	else if (Widget == SliderTextBoxOption_MaxTargetSpeed)
	{
		ConfigPtr->TargetConfig.MaxTargetSpeed = Value;
	}
}

FString UCustomGameModesWidget_Activation::GetComboBoxEntryTooltipStringTableKey_TargetActivationSelectionPolicy(const FString& EnumString)
{
	const ETargetActivationSelectionPolicy EnumValue = GetEnumFromString<ETargetActivationSelectionPolicy>(EnumString, ETargetActivationSelectionPolicy::None);
	return GetStringTableKeyNameFromEnum(EnumValue);
}

FString UCustomGameModesWidget_Activation::GetComboBoxEntryTooltipStringTableKey_TargetActivationResponses(const FString& EnumString)
{
	const ETargetActivationResponse EnumValue = GetEnumFromString<ETargetActivationResponse>(EnumString, ETargetActivationResponse::None);
	return GetStringTableKeyNameFromEnum(EnumValue);
}

FString UCustomGameModesWidget_Activation::GetComboBoxEntryTooltipStringTableKey_MovingTargetDirectionMode(const FString& EnumString)
{
	const EMovingTargetDirectionMode EnumValue = GetEnumFromString<EMovingTargetDirectionMode>(EnumString, EMovingTargetDirectionMode::None);
	return GetStringTableKeyNameFromEnum(EnumValue);
}

FString UCustomGameModesWidget_Activation::GetComboBoxEntryTooltipStringTableKey_LifetimeTargetScalePolicy(const FString& EnumString)
{
	const ELifetimeTargetScalePolicy EnumValue = GetEnumFromString<ELifetimeTargetScalePolicy>(EnumString, ELifetimeTargetScalePolicy::None);
	return GetStringTableKeyNameFromEnum(EnumValue);
}
