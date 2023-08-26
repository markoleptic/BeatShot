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
	SetupTooltip(CheckBoxOption_ConstantActivatedTargetVelocity->GetTooltipImage(), CheckBoxOption_ConstantActivatedTargetVelocity->GetTooltipImageText());
	SetupTooltip(CheckBoxOption_MoveTargetsForward->GetTooltipImage(), CheckBoxOption_MoveTargetsForward->GetTooltipImageText());
	
	SetupTooltip(SliderTextBoxOption_NumTargetsToActivateAtOnce->GetTooltipImage(), SliderTextBoxOption_NumTargetsToActivateAtOnce->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_MinNumTargetsToActivateAtOnce->GetTooltipImage(), SliderTextBoxOption_MinNumTargetsToActivateAtOnce->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->GetTooltipImage(), SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_ActivatedTargetVelocity->GetTooltipImage(), SliderTextBoxOption_ActivatedTargetVelocity->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_MinActivatedTargetVelocity->GetTooltipImage(), SliderTextBoxOption_MinActivatedTargetVelocity->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_MaxActivatedTargetVelocity->GetTooltipImage(), SliderTextBoxOption_MaxActivatedTargetVelocity->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_LifetimeTargetScaleMultiplier->GetTooltipImage(), SliderTextBoxOption_LifetimeTargetScaleMultiplier->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_MoveForwardDistance->GetTooltipImage(), SliderTextBoxOption_MoveForwardDistance->GetTooltipImageText());
	
	SetupTooltip(ComboBoxOption_TargetActivationSelectionPolicy->GetTooltipImage(), ComboBoxOption_TargetActivationSelectionPolicy->GetTooltipImageText());
	SetupTooltip(ComboBoxOption_TargetActivationResponses->GetTooltipImage(), ComboBoxOption_TargetActivationResponses->GetTooltipImageText());
	SetupTooltip(ComboBoxOption_MovingTargetDirectionMode->GetTooltipImage(), ComboBoxOption_MovingTargetDirectionMode->GetTooltipImageText());
	
	SliderTextBoxOption_NumTargetsToActivateAtOnce->SetValues(MinValue_MaxNumActivatedTargetsAtOnce, MaxValue_MaxNumActivatedTargetsAtOnce, SnapSize_MaxNumActivatedTargetsAtOnce);
	SliderTextBoxOption_MinNumTargetsToActivateAtOnce->SetValues(MinValue_MaxNumActivatedTargetsAtOnce, MaxValue_MaxNumActivatedTargetsAtOnce, SnapSize_MaxNumActivatedTargetsAtOnce);
	SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->SetValues(MinValue_MaxNumActivatedTargetsAtOnce, MaxValue_MaxNumActivatedTargetsAtOnce, SnapSize_MaxNumActivatedTargetsAtOnce);
	SliderTextBoxOption_ActivatedTargetVelocity->SetValues(MinValue_TargetSpeed, MaxValue_TargetSpeed, SnapSize_TargetSpeed);
	SliderTextBoxOption_MinActivatedTargetVelocity->SetValues(MinValue_TargetSpeed, MaxValue_TargetSpeed, SnapSize_TargetSpeed);
	SliderTextBoxOption_MaxActivatedTargetVelocity->SetValues(MinValue_TargetSpeed, MaxValue_TargetSpeed, SnapSize_TargetSpeed);
	SliderTextBoxOption_LifetimeTargetScaleMultiplier->SetValues(MinValue_TargetScale, MaxValue_TargetScale, SnapSize_TargetScale);
	SliderTextBoxOption_MoveForwardDistance->SetValues(MinValue_ForwardSpread, MaxValue_ForwardSpread, SnapSize_HorizontalSpread);
	
	SliderTextBoxOption_NumTargetsToActivateAtOnce->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MinNumTargetsToActivateAtOnce->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_ActivatedTargetVelocity->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MinActivatedTargetVelocity->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MaxActivatedTargetVelocity->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_LifetimeTargetScaleMultiplier->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	
	CheckBoxOption_ConstantNumTargetsToActivateAtOnce->CheckBox->OnCheckStateChanged.AddUniqueDynamic(this, &ThisClass::OnCheckStateChanged_ConstantNumTargetsToActivateAtOnce);
	CheckBoxOption_ConstantActivatedTargetVelocity->CheckBox->OnCheckStateChanged.AddUniqueDynamic(this, &ThisClass::OnCheckStateChanged_ConstantActivatedTargetVelocity);
	CheckBoxOption_MoveTargetsForward->CheckBox->OnCheckStateChanged.AddUniqueDynamic(this, &ThisClass::OnCheckStateChanged_MoveTargetsForward);
	
	ComboBoxOption_TargetActivationSelectionPolicy->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_TargetActivationSelectionPolicy);
	ComboBoxOption_TargetActivationResponses->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_TargetActivationResponses);
	ComboBoxOption_MovingTargetDirectionMode->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_MovingTargetDirectionMode);

	ComboBoxOption_TargetActivationSelectionPolicy->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetActivationSelectionPolicy);
	ComboBoxOption_TargetActivationResponses->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetActivationResponses);
	ComboBoxOption_MovingTargetDirectionMode->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_MovingTargetDirectionMode);

	ComboBoxOption_TargetActivationSelectionPolicy->ComboBox->ClearOptions();
	ComboBoxOption_TargetActivationResponses->ComboBox->ClearOptions();
	ComboBoxOption_MovingTargetDirectionMode->ComboBox->ClearOptions();

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
	
	for (const EMovingTargetDirectionMode& Method : TEnumRange<EMovingTargetDirectionMode>())
	{
		Options.Add(GetStringFromEnum(Method));
	}
	ComboBoxOption_MovingTargetDirectionMode->SortAndAddOptions(Options);
	Options.Empty();

	SliderTextBoxOption_MinNumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MaxNumTargetsToActivateAtOnce->SetVisibility(ESlateVisibility::Collapsed);
	CheckBoxOption_ConstantActivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
	ComboBoxOption_MovingTargetDirectionMode->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_LifetimeTargetScaleMultiplier->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_ActivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MinActivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MaxActivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MoveForwardDistance->SetVisibility(ESlateVisibility::Collapsed);

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
	const bool bConstantTargetSpeed = BSConfig->TargetConfig.MinActivatedTargetSpeed == BSConfig->TargetConfig.MaxActivatedTargetSpeed;
	
	UpdateValueIfDifferent(CheckBoxOption_ConstantNumTargetsToActivateAtOnce, bConstantNumTargetsToActivateAtOnce);
	UpdateValueIfDifferent(CheckBoxOption_ConstantActivatedTargetVelocity, bConstantTargetSpeed);
	UpdateValueIfDifferent(CheckBoxOption_MoveTargetsForward, BSConfig->TargetConfig.bMoveTargetsForward);
	
	UpdateValueIfDifferent(SliderTextBoxOption_NumTargetsToActivateAtOnce, BSConfig->TargetConfig.MinNumTargetsToActivateAtOnce);
	UpdateValueIfDifferent(SliderTextBoxOption_MinNumTargetsToActivateAtOnce, BSConfig->TargetConfig.MinNumTargetsToActivateAtOnce);
	UpdateValueIfDifferent(SliderTextBoxOption_MaxNumTargetsToActivateAtOnce, BSConfig->TargetConfig.MaxNumTargetsToActivateAtOnce);

	UpdateValueIfDifferent(SliderTextBoxOption_ActivatedTargetVelocity, BSConfig->TargetConfig.MinActivatedTargetSpeed);
	UpdateValueIfDifferent(SliderTextBoxOption_MinActivatedTargetVelocity, BSConfig->TargetConfig.MinActivatedTargetSpeed);
	UpdateValueIfDifferent(SliderTextBoxOption_MaxActivatedTargetVelocity, BSConfig->TargetConfig.MaxActivatedTargetSpeed);
	
	UpdateValueIfDifferent(SliderTextBoxOption_LifetimeTargetScaleMultiplier, BSConfig->TargetConfig.LifetimeTargetScaleMultiplier);
	UpdateValueIfDifferent(SliderTextBoxOption_MoveForwardDistance, BSConfig->TargetConfig.MoveForwardDistance);
	
	UpdateValueIfDifferent(ComboBoxOption_TargetActivationResponses, GetStringArrayFromEnumArray(BSConfig->TargetConfig.TargetActivationResponses));
	UpdateValueIfDifferent(ComboBoxOption_TargetActivationSelectionPolicy, GetStringFromEnum(BSConfig->TargetConfig.TargetActivationSelectionPolicy));
	UpdateValueIfDifferent(ComboBoxOption_MovingTargetDirectionMode, GetStringFromEnum(BSConfig->TargetConfig.MovingTargetDirectionMode));
	
	UpdateDependentOptions_ConstantNumTargetsToActivateAtOnce(bConstantNumTargetsToActivateAtOnce);
	UpdateDependentOptions_TargetActivationResponses(BSConfig->TargetConfig.TargetActivationResponses, bConstantTargetSpeed);
	UpdateDependentOptions_MoveTargetsForward(BSConfig->TargetConfig.bMoveTargetsForward);
	
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

void UCustomGameModesWidget_Activation::UpdateDependentOptions_MoveTargetsForward(const bool bMoveTargetsForward)
{
	if (bMoveTargetsForward)
	{
		SliderTextBoxOption_MoveForwardDistance->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		SliderTextBoxOption_MoveForwardDistance->SetVisibility(ESlateVisibility::Collapsed);
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
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_Activation::OnCheckStateChanged_MoveTargetsForward(const bool bChecked)
{
	BSConfig->TargetConfig.bMoveTargetsForward = bChecked;
	UpdateDependentOptions_MoveTargetsForward(BSConfig->TargetConfig.bMoveTargetsForward);
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
	else if (Widget == SliderTextBoxOption_MoveForwardDistance)
	{
		BSConfig->TargetConfig.MoveForwardDistance = Value;
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
	UpdateDependentOptions_TargetActivationResponses(BSConfig->TargetConfig.TargetActivationResponses, BSConfig->TargetConfig.MinActivatedTargetSpeed == BSConfig->TargetConfig.MaxActivatedTargetSpeed);

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