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
	
	SliderTextBoxOption_MaxNumActivatedTargetsAtOnce->SetValues(MinValue_MaxNumActivatedTargetsAtOnce, MaxValue_MaxNumActivatedTargetsAtOnce, SnapSize_MaxNumActivatedTargetsAtOnce);
	SliderTextBoxOption_NumTargetsToActivateAtOnce->SetValues(MinValue_MaxNumActivatedTargetsAtOnce, MaxValue_MaxNumActivatedTargetsAtOnce, SnapSize_MaxNumActivatedTargetsAtOnce);
	SliderTextBoxOption_MinNumTargetsToActivateAtOnce->SetValues(MinValue_MaxNumActivatedTargetsAtOnce, MaxValue_MaxNumActivatedTargetsAtOnce, SnapSize_MaxNumActivatedTargetsAtOnce);
	SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->SetValues(MinValue_MaxNumActivatedTargetsAtOnce, MaxValue_MaxNumActivatedTargetsAtOnce, SnapSize_MaxNumActivatedTargetsAtOnce);
	SliderTextBoxOption_ActivatedTargetVelocity->SetValues(MinValue_TargetSpeed, MaxValue_TargetSpeed, SnapSize_TargetSpeed);
	SliderTextBoxOption_MinActivatedTargetVelocity->SetValues(MinValue_TargetSpeed, MaxValue_TargetSpeed, SnapSize_TargetSpeed);
	SliderTextBoxOption_MaxActivatedTargetVelocity->SetValues(MinValue_TargetSpeed, MaxValue_TargetSpeed, SnapSize_TargetSpeed);
	SliderTextBoxOption_LifetimeTargetScaleMultiplier->SetValues(MinValue_TargetScale, MaxValue_TargetScale, SnapSize_TargetScale);
	SliderTextBoxOption_MaxNumActivatedTargetsAtOnce->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_NumTargetsToActivateAtOnce->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MinNumTargetsToActivateAtOnce->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_ActivatedTargetVelocity->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MinActivatedTargetVelocity->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MaxActivatedTargetVelocity->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_LifetimeTargetScaleMultiplier->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);

	CheckBoxOption_ConstantNumTargetsToActivateAtOnce->CheckBox->OnCheckStateChanged.AddUniqueDynamic(this, &ThisClass::OnCheckStateChanged_ConstantNumTargetsToActivateAtOnce);
	CheckBoxOption_ConstantActivatedTargetVelocity->CheckBox->OnCheckStateChanged.AddUniqueDynamic(this, &ThisClass::OnCheckStateChanged_ConstantActivatedTargetVelocity);
	
	ComboBoxOption_TargetActivationSelectionPolicy->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_TargetActivationSelectionPolicy);
	ComboBoxOption_TargetActivationResponses->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_TargetActivationResponses);

	ComboBoxOption_TargetActivationSelectionPolicy->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetActivationSelectionPolicy);
	ComboBoxOption_TargetActivationResponses->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetActivationResponses);

	ComboBoxOption_TargetActivationSelectionPolicy->ComboBox->ClearOptions();
	ComboBoxOption_TargetActivationResponses->ComboBox->ClearOptions();
	
	TArray<FString> Options;
	
	for (const ETargetActivationSelectionPolicy& Method : TEnumRange<ETargetActivationSelectionPolicy>())
	{
		Options.Add(GetStringFromEnum(Method));
	}
	ComboBoxOption_TargetActivationSelectionPolicy->SortAndAddOptions(Options);
	Options.Empty();
	
	for (const ETargetActivationResponse& Method : TEnumRange<ETargetActivationResponse>())
	{
		// Deprecated
		if (Method != ETargetActivationResponse::ChangeScale)
		{
			Options.Add(GetStringFromEnum(Method));
		}
	}
	ComboBoxOption_TargetActivationResponses->SortAndAddOptions(Options);
	Options.Empty();
	
	SliderTextBoxOption_MinNumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::Collapsed);
	CheckBoxOption_ConstantActivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_LifetimeTargetScaleMultiplier->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_ActivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MinActivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MaxActivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);

	UpdateBrushColors();
}

void UCustomGameModesWidget_Activation::UpdateAllOptionsValid()
{
	TArray<FTooltipData> UpdateArray;
	bool bRequestComponentUpdate = false;
	uint32 NumWarnings = 0;
	uint32 NumCautions = 0;

	// ComboBoxOption_TargetActivationResponses
	if (BSConfig->TargetConfig.MovingTargetDirectionMode == EMovingTargetDirectionMode::None)
	{
		if (BSConfig->TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::ChangeVelocity))
		{
			NumCautions++;
			UpdateArray.Emplace("Invalid_Velocity_MTDM_None", ETooltipImageType::Caution);
		}
		if (BSConfig->TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::ChangeDirection))
		{
			NumCautions++;
			UpdateArray.Emplace("Invalid_Direction_MTDM_None", ETooltipImageType::Caution);
		}
	}
	bRequestComponentUpdate = UpdateWarningTooltips(ComboBoxOption_TargetActivationResponses, UpdateArray) || bRequestComponentUpdate;
	UpdateArray.Empty();
	
	CustomGameModeCategoryInfo.Update(NumCautions, NumWarnings);
	
	if (bRequestComponentUpdate)
	{
		RequestComponentUpdate.Broadcast();
	}
}

void UCustomGameModesWidget_Activation::UpdateOptionsFromConfig()
{
	const bool bConstantNumTargetsToActivateAtOnce = BSConfig->TargetConfig.MinNumTargetsToActivateAtOnce == BSConfig->TargetConfig.MaxNumTargetsToActivateAtOnce;
	const bool bConstantTargetSpeed = BSConfig->TargetConfig.MinActivatedTargetSpeed == BSConfig->TargetConfig.MaxActivatedTargetSpeed;
	
	UpdateValueIfDifferent(CheckBoxOption_ConstantNumTargetsToActivateAtOnce, bConstantNumTargetsToActivateAtOnce);
	UpdateValueIfDifferent(CheckBoxOption_ConstantActivatedTargetVelocity, bConstantTargetSpeed);
	
	UpdateValueIfDifferent(SliderTextBoxOption_MaxNumActivatedTargetsAtOnce, BSConfig->TargetConfig.MaxNumActivatedTargetsAtOnce);
	UpdateValueIfDifferent(SliderTextBoxOption_NumTargetsToActivateAtOnce, BSConfig->TargetConfig.MinNumTargetsToActivateAtOnce);
	UpdateValueIfDifferent(SliderTextBoxOption_MinNumTargetsToActivateAtOnce, BSConfig->TargetConfig.MinNumTargetsToActivateAtOnce);
	UpdateValueIfDifferent(SliderTextBoxOption_MaxNumTargetsToActivateAtOnce, BSConfig->TargetConfig.MaxNumTargetsToActivateAtOnce);

	UpdateValueIfDifferent(SliderTextBoxOption_ActivatedTargetVelocity, BSConfig->TargetConfig.MinActivatedTargetSpeed);
	UpdateValueIfDifferent(SliderTextBoxOption_MinActivatedTargetVelocity, BSConfig->TargetConfig.MinActivatedTargetSpeed);
	UpdateValueIfDifferent(SliderTextBoxOption_MaxActivatedTargetVelocity, BSConfig->TargetConfig.MaxActivatedTargetSpeed);
	
	UpdateValueIfDifferent(SliderTextBoxOption_LifetimeTargetScaleMultiplier, BSConfig->TargetConfig.LifetimeTargetScaleMultiplier);
	
	UpdateValueIfDifferent(ComboBoxOption_TargetActivationResponses, GetStringArrayFromEnumArray(BSConfig->TargetConfig.TargetActivationResponses));
	UpdateValueIfDifferent(ComboBoxOption_TargetActivationSelectionPolicy, GetStringFromEnum(BSConfig->TargetConfig.TargetActivationSelectionPolicy));

	UpdateDependentOptions_ConstantNumTargetsToActivateAtOnce(bConstantNumTargetsToActivateAtOnce);
	UpdateDependentOptions_TargetActivationResponses(BSConfig->TargetConfig.TargetActivationResponses, bConstantTargetSpeed);

	UpdateDependentOptions_TargetDistributionPolicy(BSConfig->TargetConfig.TargetDistributionPolicy);
	
	UpdateBrushColors();
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
		CheckBoxOption_ConstantActivatedTargetVelocity->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		UpdateDependentOptions_ConstantTargetSpeed(InResponses, bUseConstantTargetSpeed);
	}
	else
	{
		CheckBoxOption_ConstantActivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
		UpdateDependentOptions_ConstantTargetSpeed(InResponses, bUseConstantTargetSpeed);
	}
}

void UCustomGameModesWidget_Activation::UpdateDependentOptions_ConstantTargetSpeed(const TArray<ETargetActivationResponse>& InResponses, const bool bUseConstantTargetSpeed)
{
	// Hide all speed options if Change Velocity not selected as a TargetActivationResponse
	if (!InResponses.Contains(ETargetActivationResponse::ChangeVelocity))
	{
		SliderTextBoxOption_ActivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MinActivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MaxActivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	
	// Change Velocity is selected as a TargetActivationResponse
	if (bUseConstantTargetSpeed)
	{
		SliderTextBoxOption_ActivatedTargetVelocity->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_MinActivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MaxActivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		SliderTextBoxOption_ActivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MinActivatedTargetVelocity->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_MaxActivatedTargetVelocity->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UCustomGameModesWidget_Activation::UpdateDependentOptions_TargetDistributionPolicy(const ETargetDistributionPolicy& Policy)
{
	switch(Policy) {
	case ETargetDistributionPolicy::Grid:
		ComboBoxOption_TargetActivationSelectionPolicy->ComboBox->SetIsEnabled(true);
		break;
	case ETargetDistributionPolicy::None:
	case ETargetDistributionPolicy::HeadshotHeightOnly:
	case ETargetDistributionPolicy::EdgeOnly:
	case ETargetDistributionPolicy::FullRange:
		BSConfig->TargetConfig.TargetActivationSelectionPolicy = ETargetActivationSelectionPolicy::Random;
		UpdateValueIfDifferent(ComboBoxOption_TargetActivationSelectionPolicy, GetStringFromEnum(BSConfig->TargetConfig.TargetActivationSelectionPolicy));
		ComboBoxOption_TargetActivationSelectionPolicy->ComboBox->SetIsEnabled(false);
		break;
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
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_Activation::OnCheckStateChanged_ConstantActivatedTargetVelocity(const bool bChecked)
{
	if (bChecked)
	{
		BSConfig->TargetConfig.MinActivatedTargetSpeed = SliderTextBoxOption_ActivatedTargetVelocity->GetSliderValue();
		BSConfig->TargetConfig.MaxActivatedTargetSpeed = SliderTextBoxOption_ActivatedTargetVelocity->GetSliderValue();
	}
	else
	{
		BSConfig->TargetConfig.MinActivatedTargetSpeed = SliderTextBoxOption_MinActivatedTargetVelocity->GetSliderValue();
		BSConfig->TargetConfig.MaxActivatedTargetSpeed = SliderTextBoxOption_MaxActivatedTargetVelocity->GetSliderValue();
	}
	
	UpdateDependentOptions_ConstantTargetSpeed(BSConfig->TargetConfig.TargetActivationResponses, bChecked);
	UpdateBrushColors();
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_Activation::OnSliderTextBoxValueChanged(USliderTextBoxWidget* Widget, const float Value)
{
	if (Widget == SliderTextBoxOption_MaxNumActivatedTargetsAtOnce)
	{
		BSConfig->TargetConfig.MaxNumActivatedTargetsAtOnce = Value;
	}
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
	else if (Widget == SliderTextBoxOption_ActivatedTargetVelocity && SliderTextBoxOption_ActivatedTargetVelocity->GetVisibility() != ESlateVisibility::Collapsed)
	{
		BSConfig->TargetConfig.MinActivatedTargetSpeed = Value;
		BSConfig->TargetConfig.MaxActivatedTargetSpeed = Value;
	}
	else if (Widget == SliderTextBoxOption_MinActivatedTargetVelocity)
	{
		BSConfig->TargetConfig.MinActivatedTargetSpeed = Value;
	}
	else if (Widget == SliderTextBoxOption_MaxActivatedTargetVelocity)
	{
		BSConfig->TargetConfig.MaxActivatedTargetSpeed = Value;
	}
	else if (Widget == SliderTextBoxOption_LifetimeTargetScaleMultiplier)
	{
		BSConfig->TargetConfig.LifetimeTargetScaleMultiplier = Value;
	}
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_Activation::OnSelectionChanged_TargetActivationSelectionPolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || Selected.Num() != 1)
	{
		
		return;
	}

	BSConfig->TargetConfig.TargetActivationSelectionPolicy = GetEnumFromString<ETargetActivationSelectionPolicy>(Selected[0]);
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_Activation::OnSelectionChanged_TargetActivationResponses(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct)
	{
		return;
	}
	
	BSConfig->TargetConfig.TargetActivationResponses = GetEnumArrayFromStringArray<ETargetActivationResponse>(Selected);
	UpdateDependentOptions_TargetActivationResponses(BSConfig->TargetConfig.TargetActivationResponses, BSConfig->TargetConfig.MinActivatedTargetSpeed == BSConfig->TargetConfig.MaxActivatedTargetSpeed);
	UpdateBrushColors();
	UpdateAllOptionsValid();
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