// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubmenuWidgets/GameModesWidgets/Components/CGMWC_Movement.h"
#include "WidgetComponents/MenuOptionWidgets/ConstantMinMaxMenuOptionWidget.h"
#include "WidgetComponents/Boxes/BSComboBoxString.h"

void UCGMWC_Movement::NativeConstruct()
{
	Super::NativeConstruct();
	
	MenuOption_SpawnedTargetVelocity->SetValues(MinValue_TargetSpeed, MaxValue_TargetSpeed,
		SnapSize_TargetSpeed);
	MenuOption_ActivatedTargetVelocity->SetValues(MinValue_TargetSpeed, MaxValue_TargetSpeed,
		SnapSize_TargetSpeed);
	MenuOption_DeactivatedTargetVelocity->SetValues(MinValue_TargetSpeed, MaxValue_TargetSpeed,
		SnapSize_TargetSpeed);

	MenuOption_SpawnedTargetVelocity->OnMinMaxMenuOptionChanged.AddUObject(this,
		&ThisClass::OnMinMaxMenuOptionChanged);
	MenuOption_ActivatedTargetVelocity->OnMinMaxMenuOptionChanged.AddUObject(this,
		&ThisClass::OnMinMaxMenuOptionChanged);
	MenuOption_DeactivatedTargetVelocity->OnMinMaxMenuOptionChanged.AddUObject(this,
		&ThisClass::OnMinMaxMenuOptionChanged);

	ComboBoxOption_MovingTargetDirectionMode->ComboBox->OnSelectionChanged.AddUniqueDynamic(this,
		&ThisClass::OnSelectionChanged_MovingTargetDirectionMode);
	ComboBoxOption_MovingTargetDirectionMode->GetComboBoxEntryTooltipStringTableKey.BindUObject(this,
		&ThisClass::GetComboBoxEntryTooltipStringTableKey_MovingTargetDirectionMode);
	ComboBoxOption_MovingTargetDirectionMode->ComboBox->ClearOptions();

	TArray<FString> Options;
	for (const EMovingTargetDirectionMode& Method : TEnumRange<EMovingTargetDirectionMode>())
	{
		Options.Add(GetStringFromEnum_FromTagMap(Method));
	}
	ComboBoxOption_MovingTargetDirectionMode->SortAddOptionsAndSetEnumType<EMovingTargetDirectionMode>(Options);
	Options.Empty();

	SetMenuOptionEnabledStateAndAddTooltip(MenuOption_SpawnedTargetVelocity, EMenuOptionEnabledState::Enabled);
	SetMenuOptionEnabledStateAndAddTooltip(MenuOption_ActivatedTargetVelocity, EMenuOptionEnabledState::Enabled);
	SetMenuOptionEnabledStateAndAddTooltip(MenuOption_DeactivatedTargetVelocity, EMenuOptionEnabledState::Enabled);

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

	UpdateValuesIfDifferent(MenuOption_SpawnedTargetVelocity, bConstantSpawnedSpeed,
		BSConfig->TargetConfig.MinSpawnedTargetSpeed, BSConfig->TargetConfig.MaxSpawnedTargetSpeed);
	UpdateValuesIfDifferent(MenuOption_ActivatedTargetVelocity, bConstantActivatedSpeed,
		BSConfig->TargetConfig.MinActivatedTargetSpeed, BSConfig->TargetConfig.MaxActivatedTargetSpeed);
	UpdateValuesIfDifferent(MenuOption_DeactivatedTargetVelocity, bConstantDeactivatedSpeed,
		BSConfig->TargetConfig.MinDeactivatedTargetSpeed, BSConfig->TargetConfig.MaxDeactivatedTargetSpeed);

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
		SetMenuOptionEnabledStateAndAddTooltip(MenuOption_SpawnedTargetVelocity, EMenuOptionEnabledState::Enabled);
	}
	else
	{
		SetMenuOptionEnabledStateAndAddTooltip(MenuOption_SpawnedTargetVelocity,
			EMenuOptionEnabledState::DependentMissing, "DM_SpawnedTargetVelocity");
	}
}

void UCGMWC_Movement::UpdateDependentOptions_ActivationResponses(const TArray<ETargetActivationResponse>& Responses,
	const bool bConstant)
{
	if (Responses.Contains(ETargetActivationResponse::ChangeVelocity))
	{
		SetMenuOptionEnabledStateAndAddTooltip(MenuOption_ActivatedTargetVelocity, EMenuOptionEnabledState::Enabled);
	}
	else
	{
		SetMenuOptionEnabledStateAndAddTooltip(MenuOption_ActivatedTargetVelocity,
			EMenuOptionEnabledState::DependentMissing, "DM_ActivatedTargetVelocity");
	}
}

void UCGMWC_Movement::UpdateDependentOptions_DeactivationResponses(const TArray<ETargetDeactivationResponse>& Responses,
	const bool bConstant)
{
	if (Responses.Contains(ETargetDeactivationResponse::ChangeVelocity))
	{
		SetMenuOptionEnabledStateAndAddTooltip(MenuOption_DeactivatedTargetVelocity, EMenuOptionEnabledState::Enabled);
	}
	else
	{
		SetMenuOptionEnabledStateAndAddTooltip(MenuOption_DeactivatedTargetVelocity,
			EMenuOptionEnabledState::DependentMissing, "DM_DeactivatedTargetVelocity");
	}
}

void UCGMWC_Movement::OnSelectionChanged_MovingTargetDirectionMode(const TArray<FString>& Selected,
	const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || Selected.Num() != 1) return;
	
	BSConfig->TargetConfig.MovingTargetDirectionMode = GetEnumFromString_FromTagMap<EMovingTargetDirectionMode>(Selected[0]);
	UpdateAllOptionsValid();
}

void UCGMWC_Movement::OnMinMaxMenuOptionChanged(UConstantMinMaxMenuOptionWidget* Widget,
		const bool bChecked, const float MinOrConstant, const float Max)
{
	if (Widget == MenuOption_SpawnedTargetVelocity)
	{
		BSConfig->TargetConfig.MinSpawnedTargetSpeed = MinOrConstant;
		BSConfig->TargetConfig.MaxSpawnedTargetSpeed = bChecked ? MinOrConstant : Max;
	}
	else if (Widget == MenuOption_ActivatedTargetVelocity)
	{
		BSConfig->TargetConfig.MinActivatedTargetSpeed = MinOrConstant;
		BSConfig->TargetConfig.MaxActivatedTargetSpeed = bChecked ? MinOrConstant : Max;
	}
	else if (Widget == MenuOption_DeactivatedTargetVelocity)
	{
		BSConfig->TargetConfig.MinDeactivatedTargetSpeed = MinOrConstant;
		BSConfig->TargetConfig.MaxDeactivatedTargetSpeed = bChecked ? MinOrConstant : Max;
	}
	UpdateBrushColors();
	UpdateAllOptionsValid();
}

FString UCGMWC_Movement::GetComboBoxEntryTooltipStringTableKey_MovingTargetDirectionMode(const FString& EnumString)
{
	const EMovingTargetDirectionMode EnumValue = GetEnumFromString_FromTagMap<EMovingTargetDirectionMode>(EnumString);
	return GetStringTableKeyNameFromEnum(EnumValue);
}
