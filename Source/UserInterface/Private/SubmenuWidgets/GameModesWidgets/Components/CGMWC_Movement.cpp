// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubmenuWidgets/GameModesWidgets/Components/CGMWC_Movement.h"

#include "Components/CheckBox.h"
#include "WidgetComponents/Boxes/BSComboBoxString.h"
#include "WidgetComponents/MenuOptionWidgets/CheckBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/SliderTextBoxOptionWidget.h"

void UCGMWC_Movement::NativeConstruct()
{
	Super::NativeConstruct();

	SliderTextBoxOption_SpawnedTargetVelocity->SetValues(MinValue_TargetSpeed, MaxValue_TargetSpeed,
		SnapSize_TargetSpeed);
	SliderTextBoxOption_MinSpawnedTargetVelocity->SetValues(MinValue_TargetSpeed, MaxValue_TargetSpeed,
		SnapSize_TargetSpeed);
	SliderTextBoxOption_MaxSpawnedTargetVelocity->SetValues(MinValue_TargetSpeed, MaxValue_TargetSpeed,
		SnapSize_TargetSpeed);
	SliderTextBoxOption_ActivatedTargetVelocity->SetValues(MinValue_TargetSpeed, MaxValue_TargetSpeed,
		SnapSize_TargetSpeed);
	SliderTextBoxOption_MinActivatedTargetVelocity->SetValues(MinValue_TargetSpeed, MaxValue_TargetSpeed,
		SnapSize_TargetSpeed);
	SliderTextBoxOption_MaxActivatedTargetVelocity->SetValues(MinValue_TargetSpeed, MaxValue_TargetSpeed,
		SnapSize_TargetSpeed);
	SliderTextBoxOption_DeactivatedTargetVelocity->SetValues(MinValue_TargetSpeed, MaxValue_TargetSpeed,
		SnapSize_TargetSpeed);
	SliderTextBoxOption_MinDeactivatedTargetVelocity->SetValues(MinValue_TargetSpeed, MaxValue_TargetSpeed,
		SnapSize_TargetSpeed);
	SliderTextBoxOption_MaxDeactivatedTargetVelocity->SetValues(MinValue_TargetSpeed, MaxValue_TargetSpeed,
		SnapSize_TargetSpeed);

	SliderTextBoxOption_SpawnedTargetVelocity->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MinSpawnedTargetVelocity->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MaxSpawnedTargetVelocity->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_ActivatedTargetVelocity->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MinActivatedTargetVelocity->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MaxActivatedTargetVelocity->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_DeactivatedTargetVelocity->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MinDeactivatedTargetVelocity->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MaxDeactivatedTargetVelocity->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);

	ComboBoxOption_MovingTargetDirectionMode->ComboBox->OnSelectionChanged.AddUniqueDynamic(this,
		&ThisClass::OnSelectionChanged_MovingTargetDirectionMode);
	ComboBoxOption_MovingTargetDirectionMode->GetComboBoxEntryTooltipStringTableKey.BindUObject(this,
		&ThisClass::GetComboBoxEntryTooltipStringTableKey_MovingTargetDirectionMode);

	CheckBoxOption_ConstantSpawnedTargetVelocity->CheckBox->OnCheckStateChanged.AddDynamic(this,
		&ThisClass::OnCheckStateChanged_ConstantSpawnedTargetVelocity);
	CheckBoxOption_ConstantActivatedTargetVelocity->CheckBox->OnCheckStateChanged.AddUniqueDynamic(this,
		&ThisClass::OnCheckStateChanged_ConstantActivatedTargetVelocity);
	CheckBoxOption_ConstantDeactivatedTargetVelocity->CheckBox->OnCheckStateChanged.AddDynamic(this,
		&ThisClass::OnCheckStateChanged_ConstantDeactivatedTargetVelocity);

	ComboBoxOption_MovingTargetDirectionMode->ComboBox->ClearOptions();

	TArray<FString> Options;
	for (const EMovingTargetDirectionMode& Method : TEnumRange<EMovingTargetDirectionMode>())
	{
		Options.Add(GetStringFromEnum_FromTagMap(Method));
	}
	ComboBoxOption_MovingTargetDirectionMode->SortAddOptionsAndSetEnumType<EMovingTargetDirectionMode>(Options);
	Options.Empty();

	CheckBoxOption_ConstantSpawnedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_SpawnedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MinSpawnedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MaxSpawnedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);

	CheckBoxOption_ConstantActivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_ActivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MinActivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MaxActivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);

	CheckBoxOption_ConstantDeactivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_DeactivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MinDeactivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MaxDeactivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);

	SetupWarningTooltipCallbacks();
	UpdateBrushColors();
}

void UCGMWC_Movement::UpdateAllOptionsValid()
{
	Super::UpdateAllOptionsValid();
}

void UCGMWC_Movement::UpdateOptionsFromConfig()
{
	const bool bConstantSpawnedSpeed = BSConfig->TargetConfig.MinSpawnedTargetSpeed == BSConfig->TargetConfig.
		MaxSpawnedTargetSpeed;
	const bool bConstantActivatedSpeed = BSConfig->TargetConfig.MinActivatedTargetSpeed == BSConfig->TargetConfig.
		MaxActivatedTargetSpeed;
	const bool bConstantDeactivatedSpeed = BSConfig->TargetConfig.MinDeactivatedTargetSpeed == BSConfig->TargetConfig.
		MaxDeactivatedTargetSpeed;

	UpdateValueIfDifferent(CheckBoxOption_ConstantSpawnedTargetVelocity, bConstantSpawnedSpeed);
	UpdateValueIfDifferent(CheckBoxOption_ConstantActivatedTargetVelocity, bConstantActivatedSpeed);
	UpdateValueIfDifferent(CheckBoxOption_ConstantDeactivatedTargetVelocity, bConstantDeactivatedSpeed);

	UpdateValueIfDifferent(SliderTextBoxOption_SpawnedTargetVelocity, BSConfig->TargetConfig.MinSpawnedTargetSpeed);
	UpdateValueIfDifferent(SliderTextBoxOption_MinSpawnedTargetVelocity, BSConfig->TargetConfig.MinSpawnedTargetSpeed);
	UpdateValueIfDifferent(SliderTextBoxOption_MaxSpawnedTargetVelocity, BSConfig->TargetConfig.MaxSpawnedTargetSpeed);

	UpdateValueIfDifferent(SliderTextBoxOption_ActivatedTargetVelocity, BSConfig->TargetConfig.MinActivatedTargetSpeed);
	UpdateValueIfDifferent(SliderTextBoxOption_MinActivatedTargetVelocity,
		BSConfig->TargetConfig.MinActivatedTargetSpeed);
	UpdateValueIfDifferent(SliderTextBoxOption_MaxActivatedTargetVelocity,
		BSConfig->TargetConfig.MaxActivatedTargetSpeed);

	UpdateValueIfDifferent(SliderTextBoxOption_DeactivatedTargetVelocity,
		BSConfig->TargetConfig.MinDeactivatedTargetSpeed);
	UpdateValueIfDifferent(SliderTextBoxOption_MinDeactivatedTargetVelocity,
		BSConfig->TargetConfig.MinDeactivatedTargetSpeed);
	UpdateValueIfDifferent(SliderTextBoxOption_MaxDeactivatedTargetVelocity,
		BSConfig->TargetConfig.MaxDeactivatedTargetSpeed);

	UpdateValueIfDifferent(ComboBoxOption_MovingTargetDirectionMode,
		GetStringFromEnum_FromTagMap(BSConfig->TargetConfig.MovingTargetDirectionMode));

	UpdateDependentOptions_SpawnResponses(BSConfig->TargetConfig.TargetSpawnResponses, bConstantSpawnedSpeed);
	UpdateDependentOptions_ActivationResponses(BSConfig->TargetConfig.TargetActivationResponses,
		bConstantActivatedSpeed);
	UpdateDependentOptions_DeactivationResponses(BSConfig->TargetConfig.TargetDeactivationResponses,
		bConstantDeactivatedSpeed);

	UpdateBrushColors();
}

void UCGMWC_Movement::SetupWarningTooltipCallbacks()
{
	ComboBoxOption_MovingTargetDirectionMode->AddWarningTooltipData(FTooltipData("Invalid_Velocity_MTDM_None_2",
		ETooltipImageType::Caution)).BindLambda([this]()
	{
		return BSConfig->TargetConfig.MovingTargetDirectionMode == EMovingTargetDirectionMode::None && (BSConfig->
			TargetConfig.TargetSpawnResponses.Contains(ETargetSpawnResponse::ChangeVelocity) || BSConfig->TargetConfig.
			TargetActivationResponses.Contains(ETargetActivationResponse::ChangeVelocity) || BSConfig->TargetConfig.
			TargetDeactivationResponses.Contains(ETargetDeactivationResponse::ChangeVelocity));
	});
	ComboBoxOption_MovingTargetDirectionMode->AddWarningTooltipData(FTooltipData("Invalid_Direction_MTDM_None_2",
		ETooltipImageType::Caution)).BindLambda([this]()
	{
		return BSConfig->TargetConfig.MovingTargetDirectionMode == EMovingTargetDirectionMode::None && (BSConfig->
			TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::ChangeDirection) || BSConfig->
			TargetConfig.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::ChangeDirection) || BSConfig
			->TargetConfig.TargetSpawnResponses.Contains(ETargetSpawnResponse::ChangeVelocity));
	});
	ComboBoxOption_MovingTargetDirectionMode->AddWarningTooltipData(
		FTooltipData("Caution_ZeroForwardDistance_MTDM_ForwardOnly", ETooltipImageType::Caution)).BindLambda([this]()
	{
		return BSConfig->TargetConfig.MovingTargetDirectionMode == EMovingTargetDirectionMode::ForwardOnly && BSConfig->
			TargetConfig.BoxBounds.X <= 0.f;
	});
}

void UCGMWC_Movement::UpdateDependentOptions_SpawnResponses(const TArray<ETargetSpawnResponse>& Responses,
	const bool bConstant)
{
	if (Responses.Contains(ETargetSpawnResponse::ChangeVelocity))
	{
		CheckBoxOption_ConstantSpawnedTargetVelocity->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		if (bConstant)
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
		CheckBoxOption_ConstantSpawnedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_SpawnedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MinSpawnedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MaxSpawnedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UCGMWC_Movement::UpdateDependentOptions_ActivationResponses(const TArray<ETargetActivationResponse>& Responses,
	const bool bConstant)
{
	if (Responses.Contains(ETargetActivationResponse::ChangeVelocity))
	{
		CheckBoxOption_ConstantActivatedTargetVelocity->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		if (bConstant)
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
	else
	{
		CheckBoxOption_ConstantActivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_ActivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MinActivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MaxActivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UCGMWC_Movement::UpdateDependentOptions_DeactivationResponses(const TArray<ETargetDeactivationResponse>& Responses,
	const bool bConstant)
{
	if (Responses.Contains(ETargetDeactivationResponse::ChangeVelocity))
	{
		CheckBoxOption_ConstantDeactivatedTargetVelocity->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		if (bConstant)
		{
			SliderTextBoxOption_DeactivatedTargetVelocity->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			SliderTextBoxOption_MinDeactivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
			SliderTextBoxOption_MaxDeactivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			SliderTextBoxOption_DeactivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
			SliderTextBoxOption_MinDeactivatedTargetVelocity->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			SliderTextBoxOption_MaxDeactivatedTargetVelocity->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
	else
	{
		CheckBoxOption_ConstantDeactivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_DeactivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MinDeactivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MaxDeactivatedTargetVelocity->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UCGMWC_Movement::OnSelectionChanged_MovingTargetDirectionMode(const TArray<FString>& Selected,
	const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || Selected.Num() != 1)
	{
		return;
	}

	BSConfig->TargetConfig.MovingTargetDirectionMode = GetEnumFromString<EMovingTargetDirectionMode>(Selected[0]);
	UpdateAllOptionsValid();
}

void UCGMWC_Movement::OnCheckStateChanged_ConstantSpawnedTargetVelocity(const bool bChecked)
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

	UpdateDependentOptions_SpawnResponses(BSConfig->TargetConfig.TargetSpawnResponses, bChecked);
	UpdateBrushColors();
	UpdateAllOptionsValid();
}

void UCGMWC_Movement::OnCheckStateChanged_ConstantActivatedTargetVelocity(const bool bChecked)
{
	if (bChecked)
	{
		BSConfig->TargetConfig.MinActivatedTargetSpeed = SliderTextBoxOption_ActivatedTargetVelocity->GetSliderValue();
		BSConfig->TargetConfig.MaxActivatedTargetSpeed = SliderTextBoxOption_ActivatedTargetVelocity->GetSliderValue();
	}
	else
	{
		BSConfig->TargetConfig.MinActivatedTargetSpeed = SliderTextBoxOption_MinActivatedTargetVelocity->
			GetSliderValue();
		BSConfig->TargetConfig.MaxActivatedTargetSpeed = SliderTextBoxOption_MaxActivatedTargetVelocity->
			GetSliderValue();
	}
	UpdateDependentOptions_ActivationResponses(BSConfig->TargetConfig.TargetActivationResponses, bChecked);
	UpdateBrushColors();
	UpdateAllOptionsValid();
}

void UCGMWC_Movement::OnCheckStateChanged_ConstantDeactivatedTargetVelocity(const bool bChecked)
{
	if (bChecked)
	{
		BSConfig->TargetConfig.MinDeactivatedTargetSpeed = SliderTextBoxOption_DeactivatedTargetVelocity->
			GetSliderValue();
		BSConfig->TargetConfig.MaxDeactivatedTargetSpeed = SliderTextBoxOption_DeactivatedTargetVelocity->
			GetSliderValue();
	}
	else
	{
		BSConfig->TargetConfig.MinDeactivatedTargetSpeed = SliderTextBoxOption_MinDeactivatedTargetVelocity->
			GetSliderValue();
		BSConfig->TargetConfig.MaxDeactivatedTargetSpeed = SliderTextBoxOption_MaxDeactivatedTargetVelocity->
			GetSliderValue();
	}

	UpdateDependentOptions_DeactivationResponses(BSConfig->TargetConfig.TargetDeactivationResponses, bChecked);
	UpdateBrushColors();
	UpdateAllOptionsValid();
}

void UCGMWC_Movement::OnSliderTextBoxValueChanged(USliderTextBoxOptionWidget* Widget, const float Value)
{
	if (Widget == SliderTextBoxOption_SpawnedTargetVelocity)
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
	else if (Widget == SliderTextBoxOption_ActivatedTargetVelocity)
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
	else if (Widget == SliderTextBoxOption_DeactivatedTargetVelocity)
	{
		BSConfig->TargetConfig.MinDeactivatedTargetSpeed = Value;
		BSConfig->TargetConfig.MaxDeactivatedTargetSpeed = Value;
	}
	else if (Widget == SliderTextBoxOption_MinDeactivatedTargetVelocity)
	{
		BSConfig->TargetConfig.MinDeactivatedTargetSpeed = Value;
	}
	else if (Widget == SliderTextBoxOption_MaxDeactivatedTargetVelocity)
	{
		BSConfig->TargetConfig.MaxDeactivatedTargetSpeed = Value;
	}
	UpdateAllOptionsValid();
}

FString UCGMWC_Movement::GetComboBoxEntryTooltipStringTableKey_MovingTargetDirectionMode(const FString& EnumString)
{
	const EMovingTargetDirectionMode EnumValue = GetEnumFromString<EMovingTargetDirectionMode>(EnumString);
	return GetStringTableKeyNameFromEnum(EnumValue);
}
