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
	
	SetupTooltip(CheckBoxOption_ConstantNumTargetsToActivateAtOnce->GetTooltipImage(), CheckBoxOption_ConstantNumTargetsToActivateAtOnce->GetTooltipImageText());
	SetupTooltip(CheckBoxOption_ConstantTargetSpeed->GetTooltipImage(), CheckBoxOption_ConstantTargetSpeed->GetTooltipImageText());
	
	SetupTooltip(SliderTextBoxOption_NumTargetsToActivateAtOnce->GetTooltipImage(), SliderTextBoxOption_NumTargetsToActivateAtOnce->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_MinNumTargetsToActivateAtOnce->GetTooltipImage(), SliderTextBoxOption_MinNumTargetsToActivateAtOnce->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->GetTooltipImage(), SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_TargetSpeed->GetTooltipImage(), SliderTextBoxOption_TargetSpeed->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_MinTargetSpeed->GetTooltipImage(), SliderTextBoxOption_MinTargetSpeed->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_MaxTargetSpeed->GetTooltipImage(), SliderTextBoxOption_MaxTargetSpeed->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_LifetimeTargetScaleMultiplier->GetTooltipImage(), SliderTextBoxOption_LifetimeTargetScaleMultiplier->GetTooltipImageText());
	
	SetupTooltip(ComboBoxOption_TargetActivationSelectionPolicy->GetTooltipImage(), ComboBoxOption_TargetActivationSelectionPolicy->GetTooltipImageText());
	SetupTooltip(ComboBoxOption_TargetActivationResponses->GetTooltipImage(), ComboBoxOption_TargetActivationResponses->GetTooltipImageText());
	SetupTooltip(ComboBoxOption_MovingTargetDirectionMode->GetTooltipImage(), ComboBoxOption_MovingTargetDirectionMode->GetTooltipImageText());
	
	SliderTextBoxOption_NumTargetsToActivateAtOnce->SetValues(MinValue_MaxNumActivatedTargetsAtOnce, MaxValue_MaxNumActivatedTargetsAtOnce, SnapSize_MaxNumActivatedTargetsAtOnce);
	SliderTextBoxOption_MinNumTargetsToActivateAtOnce->SetValues(MinValue_MaxNumActivatedTargetsAtOnce, MaxValue_MaxNumActivatedTargetsAtOnce, SnapSize_MaxNumActivatedTargetsAtOnce);
	SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->SetValues(MinValue_MaxNumActivatedTargetsAtOnce, MaxValue_MaxNumActivatedTargetsAtOnce, SnapSize_MaxNumActivatedTargetsAtOnce);
	SliderTextBoxOption_TargetSpeed->SetValues(MinValue_TargetSpeed, MaxValue_TargetSpeed, SnapSize_TargetSpeed);
	SliderTextBoxOption_MinTargetSpeed->SetValues(MinValue_TargetSpeed, MaxValue_TargetSpeed, SnapSize_TargetSpeed);
	SliderTextBoxOption_MaxTargetSpeed->SetValues(MinValue_TargetSpeed, MaxValue_TargetSpeed, SnapSize_TargetSpeed);
	SliderTextBoxOption_LifetimeTargetScaleMultiplier->SetValues(MinValue_TargetScale, MaxValue_TargetScale, SnapSize_TargetScale);
	
	SliderTextBoxOption_NumTargetsToActivateAtOnce->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MinNumTargetsToActivateAtOnce->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_TargetSpeed->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MinTargetSpeed->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MaxTargetSpeed->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_LifetimeTargetScaleMultiplier->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	
	CheckBoxOption_ConstantNumTargetsToActivateAtOnce->CheckBox->OnCheckStateChanged.AddUniqueDynamic(this, &ThisClass::OnCheckStateChanged_ConstantNumTargetsToActivateAtOnce);
	CheckBoxOption_ConstantTargetSpeed->CheckBox->OnCheckStateChanged.AddUniqueDynamic(this, &ThisClass::OnCheckStateChanged_ConstantTargetSpeed);
	
	ComboBoxOption_TargetActivationSelectionPolicy->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_TargetActivationSelectionPolicy);
	ComboBoxOption_TargetActivationResponses->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_TargetActivationResponses);
	ComboBoxOption_MovingTargetDirectionMode->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_MovingTargetDirectionMode);

	ComboBoxOption_TargetActivationSelectionPolicy->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetActivationSelectionPolicy);
	ComboBoxOption_TargetActivationResponses->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetActivationResponses);
	ComboBoxOption_MovingTargetDirectionMode->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_MovingTargetDirectionMode);

	ComboBoxOption_TargetActivationSelectionPolicy->ComboBox->ClearOptions();
	ComboBoxOption_TargetActivationResponses->ComboBox->ClearOptions();
	ComboBoxOption_MovingTargetDirectionMode->ComboBox->ClearOptions();

	for (const ETargetActivationSelectionPolicy& Method : TEnumRange<ETargetActivationSelectionPolicy>())
	{
		ComboBoxOption_TargetActivationSelectionPolicy->ComboBox->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}
	for (const ETargetActivationResponse& Method : TEnumRange<ETargetActivationResponse>())
	{
		// Deprecated
		if (Method != ETargetActivationResponse::ChangeScale)
		{
			ComboBoxOption_TargetActivationResponses->ComboBox->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
		}
	}
	for (const EMovingTargetDirectionMode& Method : TEnumRange<EMovingTargetDirectionMode>())
	{
		ComboBoxOption_MovingTargetDirectionMode->ComboBox->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}

	SliderTextBoxOption_MinNumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::Collapsed);
	CheckBoxOption_ConstantTargetSpeed->SetVisibility(ESlateVisibility::Collapsed);
	ComboBoxOption_MovingTargetDirectionMode->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_LifetimeTargetScaleMultiplier->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_TargetSpeed->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MinTargetSpeed->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MaxTargetSpeed->SetVisibility(ESlateVisibility::Collapsed);

	UpdateBrushColors();
}

bool UCustomGameModesWidget_Activation::UpdateAllOptionsValid()
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
	return true;
}

void UCustomGameModesWidget_Activation::UpdateOptionsFromConfig()
{
	const bool bConstantNumTargetsToActivateAtOnce = BSConfig->TargetConfig.MinNumTargetsToActivateAtOnce == BSConfig->TargetConfig.MaxNumTargetsToActivateAtOnce;
	const bool bConstantTargetSpeed = BSConfig->TargetConfig.MinTargetSpeed == BSConfig->TargetConfig.MaxTargetSpeed;
	
	UpdateValueIfDifferent(CheckBoxOption_ConstantNumTargetsToActivateAtOnce, bConstantNumTargetsToActivateAtOnce);
	UpdateValueIfDifferent(CheckBoxOption_ConstantTargetSpeed, bConstantTargetSpeed);
	
	UpdateValueIfDifferent(SliderTextBoxOption_NumTargetsToActivateAtOnce, BSConfig->TargetConfig.MinNumTargetsToActivateAtOnce);
	UpdateValueIfDifferent(SliderTextBoxOption_MinNumTargetsToActivateAtOnce, BSConfig->TargetConfig.MinNumTargetsToActivateAtOnce);
	UpdateValueIfDifferent(SliderTextBoxOption_MaxNumTargetsToActivateAtOnce, BSConfig->TargetConfig.MaxNumTargetsToActivateAtOnce);

	UpdateValueIfDifferent(SliderTextBoxOption_TargetSpeed, BSConfig->TargetConfig.MinTargetSpeed);
	UpdateValueIfDifferent(SliderTextBoxOption_MinTargetSpeed, BSConfig->TargetConfig.MinTargetSpeed);
	UpdateValueIfDifferent(SliderTextBoxOption_MaxTargetSpeed, BSConfig->TargetConfig.MaxTargetSpeed);
	
	UpdateValueIfDifferent(SliderTextBoxOption_LifetimeTargetScaleMultiplier, BSConfig->TargetConfig.LifetimeTargetScaleMultiplier);
	
	UpdateValueIfDifferent(ComboBoxOption_TargetActivationResponses, GetStringArrayFromEnumArray(BSConfig->TargetConfig.TargetActivationResponses));
	UpdateValueIfDifferent(ComboBoxOption_TargetActivationSelectionPolicy, GetStringFromEnum(BSConfig->TargetConfig.TargetActivationSelectionPolicy));
	UpdateValueIfDifferent(ComboBoxOption_MovingTargetDirectionMode, GetStringFromEnum(BSConfig->TargetConfig.MovingTargetDirectionMode));
	
	UpdateDependentOptions_ConstantNumTargetsToActivateAtOnce(bConstantNumTargetsToActivateAtOnce);
	UpdateDependentOptions_TargetActivationResponses(BSConfig->TargetConfig.TargetActivationResponses, bConstantTargetSpeed);
	
	UpdateBrushColors();
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_Activation::UpdateDependentOptions_ConstantNumTargetsToActivateAtOnce(const bool bInConstant)
{
	if (bInConstant)
	{
		SliderTextBoxOption_NumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_MinNumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		SliderTextBoxOption_NumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MinNumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UCustomGameModesWidget_Activation::UpdateDependentOptions_TargetActivationResponses(const TArray<ETargetActivationResponse>& InResponses, const bool bUseConstantTargetSpeed)
{
	if (InResponses.Contains(ETargetActivationResponse::ChangeDirection))
	{
		ComboBoxOption_MovingTargetDirectionMode->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		ComboBoxOption_MovingTargetDirectionMode->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (InResponses.Contains(ETargetActivationResponse::ApplyLifetimeTargetScaling))
	{
		SliderTextBoxOption_LifetimeTargetScaleMultiplier->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		SliderTextBoxOption_LifetimeTargetScaleMultiplier->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (InResponses.Contains(ETargetActivationResponse::ChangeVelocity))
	{
		CheckBoxOption_ConstantTargetSpeed->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		UpdateDependentOptions_ConstantTargetSpeed(InResponses, bUseConstantTargetSpeed);
	}
	else
	{
		CheckBoxOption_ConstantTargetSpeed->SetVisibility(ESlateVisibility::Collapsed);
		UpdateDependentOptions_ConstantTargetSpeed(InResponses, bUseConstantTargetSpeed);
	}
}

void UCustomGameModesWidget_Activation::UpdateDependentOptions_ConstantTargetSpeed(const TArray<ETargetActivationResponse>& InResponses, const bool bUseConstantTargetSpeed)
{
	// Hide all speed options if Change Velocity not selected as a TargetActivationResponse
	if (!InResponses.Contains(ETargetActivationResponse::ChangeVelocity))
	{
		SliderTextBoxOption_TargetSpeed->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MinTargetSpeed->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MaxTargetSpeed->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	
	// Change Velocity is selected as a TargetActivationResponse
	if (bUseConstantTargetSpeed)
	{
		SliderTextBoxOption_TargetSpeed->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_MinTargetSpeed->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MaxTargetSpeed->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		SliderTextBoxOption_TargetSpeed->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MinTargetSpeed->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_MaxTargetSpeed->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UCustomGameModesWidget_Activation::OnCheckStateChanged_ConstantNumTargetsToActivateAtOnce(const bool bChecked)
{
	if (bChecked)
	{
		BSConfig->TargetConfig.MinNumTargetsToActivateAtOnce = SliderTextBoxOption_NumTargetsToActivateAtOnce->GetSliderValue();
		BSConfig->TargetConfig.MaxNumTargetsToActivateAtOnce = SliderTextBoxOption_NumTargetsToActivateAtOnce->GetSliderValue();
	}
	else
	{
		BSConfig->TargetConfig.MinNumTargetsToActivateAtOnce = SliderTextBoxOption_MinNumTargetsToActivateAtOnce->GetSliderValue();
		BSConfig->TargetConfig.MaxNumTargetsToActivateAtOnce = SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->GetSliderValue();
	}
	
	UpdateDependentOptions_ConstantNumTargetsToActivateAtOnce(bChecked);
	
	UpdateBrushColors();
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_Activation::OnCheckStateChanged_ConstantTargetSpeed(const bool bChecked)
{
	if (bChecked)
	{
		BSConfig->TargetConfig.MinTargetSpeed = SliderTextBoxOption_TargetSpeed->GetSliderValue();
		BSConfig->TargetConfig.MaxTargetSpeed = SliderTextBoxOption_TargetSpeed->GetSliderValue();
	}
	else
	{
		BSConfig->TargetConfig.MinTargetSpeed = SliderTextBoxOption_MinTargetSpeed->GetSliderValue();
		BSConfig->TargetConfig.MaxTargetSpeed = SliderTextBoxOption_MaxTargetSpeed->GetSliderValue();
	}
	
	UpdateDependentOptions_ConstantTargetSpeed(BSConfig->TargetConfig.TargetActivationResponses, bChecked);
	
	UpdateBrushColors();
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_Activation::OnSliderTextBoxValueChanged(USliderTextBoxWidget* Widget, const float Value)
{
	if (Widget == SliderTextBoxOption_NumTargetsToActivateAtOnce && SliderTextBoxOption_NumTargetsToActivateAtOnce->GetVisibility() != ESlateVisibility::Collapsed)
	{
		BSConfig->TargetConfig.MaxNumTargetsToActivateAtOnce = Value;
		BSConfig->TargetConfig.MinNumTargetsToActivateAtOnce = Value;
	}
	else if (Widget == SliderTextBoxOption_MinNumTargetsToActivateAtOnce)
	{
		BSConfig->TargetConfig.MinNumTargetsToActivateAtOnce = Value;
	}
	else if (Widget == SliderTextBoxOption_MaxNumTargetsToActivateAtOnce)
	{
		BSConfig->TargetConfig.MaxNumTargetsToActivateAtOnce = Value;
	}
	else if (Widget == SliderTextBoxOption_TargetSpeed && SliderTextBoxOption_TargetSpeed->GetVisibility() != ESlateVisibility::Collapsed)
	{
		BSConfig->TargetConfig.MinTargetSpeed = Value;
		BSConfig->TargetConfig.MaxTargetSpeed = Value;
	}
	else if (Widget == SliderTextBoxOption_MinTargetSpeed)
	{
		BSConfig->TargetConfig.MinTargetSpeed = Value;
	}
	else if (Widget == SliderTextBoxOption_MaxTargetSpeed)
	{
		BSConfig->TargetConfig.MaxTargetSpeed = Value;
	}
	else if (Widget == SliderTextBoxOption_LifetimeTargetScaleMultiplier)
	{
		BSConfig->TargetConfig.LifetimeTargetScaleMultiplier = Value;
	}
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_Activation::OnSelectionChanged_TargetActivationSelectionPolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || Selected.Num() != 1)
	{
		SetAllOptionsValid(UpdateAllOptionsValid());
		return;
	}

	BSConfig->TargetConfig.TargetActivationSelectionPolicy = GetEnumFromString<ETargetActivationSelectionPolicy>(Selected[0]);
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_Activation::OnSelectionChanged_TargetActivationResponses(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct)
	{
		return;
	}
	
	BSConfig->TargetConfig.TargetActivationResponses = GetEnumArrayFromStringArray<ETargetActivationResponse>(Selected);
	UpdateDependentOptions_TargetActivationResponses(BSConfig->TargetConfig.TargetActivationResponses, BSConfig->TargetConfig.MinTargetSpeed == BSConfig->TargetConfig.MaxTargetSpeed);

	UpdateBrushColors();
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_Activation::OnSelectionChanged_MovingTargetDirectionMode(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || Selected.Num() != 1)
	{
		SetAllOptionsValid(UpdateAllOptionsValid());
		return;
	}

	BSConfig->TargetConfig.MovingTargetDirectionMode = GetEnumFromString<EMovingTargetDirectionMode>(Selected[0]);
	SetAllOptionsValid(UpdateAllOptionsValid());
}

FString UCustomGameModesWidget_Activation::GetComboBoxEntryTooltipStringTableKey_TargetActivationSelectionPolicy(const FString& EnumString)
{
	const ETargetActivationSelectionPolicy EnumValue = GetEnumFromString<ETargetActivationSelectionPolicy>(EnumString);
	return GetStringTableKeyNameFromEnum(EnumValue);
}

FString UCustomGameModesWidget_Activation::GetComboBoxEntryTooltipStringTableKey_TargetActivationResponses(const FString& EnumString)
{
	const ETargetActivationResponse EnumValue = GetEnumFromString<ETargetActivationResponse>(EnumString);
	return GetStringTableKeyNameFromEnum(EnumValue);
}

FString UCustomGameModesWidget_Activation::GetComboBoxEntryTooltipStringTableKey_MovingTargetDirectionMode(const FString& EnumString)
{
	const EMovingTargetDirectionMode EnumValue = GetEnumFromString<EMovingTargetDirectionMode>(EnumString);
	return GetStringTableKeyNameFromEnum(EnumValue);
}