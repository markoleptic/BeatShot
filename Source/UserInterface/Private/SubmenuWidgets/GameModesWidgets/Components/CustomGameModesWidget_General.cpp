// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/GameModesWidgets/Components/CustomGameModesWidget_General.h"
#include "Components/CheckBox.h"
#include "WidgetComponents/Boxes/BSComboBoxString.h"
#include "WidgetComponents/MenuOptionWidgets/CheckBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/ComboBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/SliderTextBoxOptionWidget.h"

void UCustomGameModesWidget_General::InitComponent(FBSConfig* InConfigPtr, TObjectPtr<UCustomGameModesWidgetComponent> InNext)
{
	Super::InitComponent(InConfigPtr, InNext);
}

void UCustomGameModesWidget_General::NativeConstruct()
{
	Super::NativeConstruct();
	
	SliderTextBoxOption_SpawnBeatDelay->SetValues(MinValue_PlayerDelay, MaxValue_PlayerDelay, SnapSize_PlayerDelay);
	SliderTextBoxOption_TargetSpawnCD->SetValues(MinValue_TargetSpawnCD, MaxValue_TargetSpawnCD, SnapSize_TargetSpawnCD);
	SliderTextBoxOption_MaxNumRecentTargets->SetValues(MinValue_MaxNumRecentTargets, MaxValue_MaxNumRecentTargets, SnapSize_MaxNumRecentTargets);
	SliderTextBoxOption_RecentTargetTimeLength->SetValues(MinValue_RecentTargetTimeLength, MaxValue_RecentTargetTimeLength, SnapSize_RecentTargetTimeLength);
	SliderTextBoxOption_TargetMaxLifeSpan->SetValues(MinValue_Lifespan, MaxValue_Lifespan, SnapSize_Lifespan);
	SliderTextBoxOption_MaxHealth->SetValues(MinValue_MaxHealth, MaxValue_MaxHealth, SnapSize_MaxHealth);
	SliderTextBoxOption_ExpirationHealthPenalty->SetValues(MinValue_ExpirationHealthPenalty, MaxValue_ExpirationHealthPenalty, SnapSize_ExpirationHealthPenalty);
	SliderTextBoxOption_TargetScale->SetValues(MinValue_TargetScale, MaxValue_TargetScale, SnapSize_TargetScale);
	SliderTextBoxOption_MinTargetScale->SetValues(MinValue_TargetScale, MaxValue_TargetScale, SnapSize_TargetScale);
	SliderTextBoxOption_MaxTargetScale->SetValues(MinValue_TargetScale, MaxValue_TargetScale, SnapSize_TargetScale);
	SliderTextBoxOption_Alpha->SetValues(MinValue_Alpha, MaxValue_Alpha, SnapSize_Alpha);
	SliderTextBoxOption_Epsilon->SetValues(MinValue_Epsilon, MaxValue_Epsilon, SnapSize_Epsilon);
	SliderTextBoxOption_Gamma->SetValues(MinValue_Gamma, MaxValue_Gamma, SnapSize_Gamma);
	SliderTextBoxOption_StartThreshold->SetValues(MinValue_DynamicStartThreshold, MaxValue_DynamicStartThreshold, SnapSize_DynamicStartThreshold);
	SliderTextBoxOption_EndThreshold->SetValues(MinValue_DynamicEndThreshold, MaxValue_DynamicEndThreshold, SnapSize_DynamicEndThreshold);
	SliderTextBoxOption_DecrementAmount->SetValues(MinValue_DynamicDecrementAmount, MaxValue_DynamicDecrementAmount, SnapSize_DynamicDecrementAmount);
	
	SliderTextBoxOption_SpawnBeatDelay->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_TargetSpawnCD->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MaxNumRecentTargets->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_RecentTargetTimeLength->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_Alpha->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_Epsilon->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_Gamma->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_TargetScale->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MinTargetScale->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MaxTargetScale->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_TargetMaxLifeSpan->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MaxHealth->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_ExpirationHealthPenalty->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_StartThreshold->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_EndThreshold->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_DecrementAmount->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);

	CheckBoxOption_EnableAI->CheckBox->OnCheckStateChanged.AddUniqueDynamic(this, &ThisClass::OnCheckStateChanged_EnableAI);
	
	ComboBoxOption_RecentTargetMemoryPolicy->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_RecentTargetMemoryPolicy);
	ComboBoxOption_RecentTargetMemoryPolicy->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetActivationSelectionPolicy);
	ComboBoxOption_ConsecutiveTargetScalePolicy->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_ConsecutiveTargetScalePolicy);
	ComboBoxOption_DamageType->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_DamageType);
	ComboBoxOption_DamageType->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_DamageType);
	CheckBoxOption_UnlimitedTargetHealth->CheckBox->OnCheckStateChanged.AddUniqueDynamic(this, &ThisClass::OnCheckStateChanged_UnlimitedTargetHealth);
	ComboBoxOption_ConsecutiveTargetScalePolicy->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_ConsecutiveTargetScalePolicy);
	ComboBoxOption_MovingTargetDirectionMode->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_MovingTargetDirectionMode);
	ComboBoxOption_MovingTargetDirectionMode->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_MovingTargetDirectionMode);
	ComboBoxOption_HyperParameterMode->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_HyperParameterMode);
	ComboBoxOption_HyperParameterMode->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_HyperParameterMode);
	
	ComboBoxOption_RecentTargetMemoryPolicy->ComboBox->ClearOptions();
	ComboBoxOption_DamageType->ComboBox->ClearOptions();
	ComboBoxOption_ConsecutiveTargetScalePolicy->ComboBox->ClearOptions();
	ComboBoxOption_MovingTargetDirectionMode->ComboBox->ClearOptions();
	ComboBoxOption_HyperParameterMode->ComboBox->ClearOptions();

	TArray<FString> Options;
	for (const ERecentTargetMemoryPolicy& Method : TEnumRange<ERecentTargetMemoryPolicy>())
	{
		Options.Add(GetStringFromEnum_FromTagMap(Method));
	}
	ComboBoxOption_RecentTargetMemoryPolicy->SortAddOptionsAndSetEnumType<ERecentTargetMemoryPolicy>(Options);
	Options.Empty();

	for (const ETargetDamageType& Method : TEnumRange<ETargetDamageType>())
	{
		Options.Add(GetStringFromEnum_FromTagMap(Method));
	}
	ComboBoxOption_DamageType->SortAddOptionsAndSetEnumType<ETargetDamageType>(Options);
	Options.Empty();
	
	for (const EConsecutiveTargetScalePolicy& Method : TEnumRange<EConsecutiveTargetScalePolicy>())
	{
		Options.Add(GetStringFromEnum_FromTagMap(Method));
	}
	ComboBoxOption_ConsecutiveTargetScalePolicy->SortAddOptionsAndSetEnumType<EConsecutiveTargetScalePolicy>(Options);
	Options.Empty();

		
	for (const EMovingTargetDirectionMode& Method : TEnumRange<EMovingTargetDirectionMode>())
	{
		Options.Add(GetStringFromEnum_FromTagMap(Method));
	}
	ComboBoxOption_MovingTargetDirectionMode->SortAddOptionsAndSetEnumType<EMovingTargetDirectionMode>(Options);
	Options.Empty();

	for (const EReinforcementLearningHyperParameterMode& Method : TEnumRange<EReinforcementLearningHyperParameterMode>())
	{
		Options.Add(GetStringFromEnum_FromTagMap(Method));
	}
	ComboBoxOption_HyperParameterMode->SortAddOptionsAndSetEnumType<EReinforcementLearningHyperParameterMode>(Options);
	Options.Empty();

	SliderTextBoxOption_RecentTargetTimeLength->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MaxNumRecentTargets->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_Alpha->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_Epsilon->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_Gamma->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_TargetScale->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MinTargetScale->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MaxTargetScale->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_StartThreshold->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_EndThreshold->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_DecrementAmount->SetVisibility(ESlateVisibility::Collapsed);
	ComboBoxOption_HyperParameterMode->SetVisibility(ESlateVisibility::Collapsed);
	
	SetupWarningTooltipCallbacks();
	UpdateBrushColors();
}

void UCustomGameModesWidget_General::UpdateAllOptionsValid()
{
	Super::UpdateAllOptionsValid();
}

void UCustomGameModesWidget_General::UpdateOptionsFromConfig()
{
	UpdateValueIfDifferent(SliderTextBoxOption_SpawnBeatDelay, BSConfig->TargetConfig.SpawnBeatDelay);
	UpdateValueIfDifferent(SliderTextBoxOption_TargetSpawnCD, BSConfig->TargetConfig.TargetSpawnCD);
	UpdateValueIfDifferent(SliderTextBoxOption_MaxNumRecentTargets, BSConfig->TargetConfig.MaxNumRecentTargets);
	UpdateValueIfDifferent(SliderTextBoxOption_RecentTargetTimeLength, BSConfig->TargetConfig.RecentTargetTimeLength);

	UpdateValueIfDifferent(CheckBoxOption_EnableAI, BSConfig->AIConfig.bEnableReinforcementLearning);
	UpdateValueIfDifferent(ComboBoxOption_HyperParameterMode, GetStringFromEnum_FromTagMap(BSConfig->AIConfig.HyperParameterMode));
	UpdateValueIfDifferent(SliderTextBoxOption_Alpha, BSConfig->AIConfig.Alpha);
	UpdateValueIfDifferent(SliderTextBoxOption_Epsilon, BSConfig->AIConfig.Epsilon);
	UpdateValueIfDifferent(SliderTextBoxOption_Gamma, BSConfig->AIConfig.Gamma);
	
	UpdateValueIfDifferent(ComboBoxOption_RecentTargetMemoryPolicy, GetStringFromEnum_FromTagMap(BSConfig->TargetConfig.RecentTargetMemoryPolicy));
	UpdateValueIfDifferent(ComboBoxOption_ConsecutiveTargetScalePolicy, GetStringFromEnum_FromTagMap(BSConfig->TargetConfig.ConsecutiveTargetScalePolicy));
	
	UpdateValueIfDifferent(SliderTextBoxOption_TargetMaxLifeSpan, BSConfig->TargetConfig.TargetMaxLifeSpan);
	UpdateValueIfDifferent(SliderTextBoxOption_MaxHealth, BSConfig->TargetConfig.MaxHealth);
	UpdateValueIfDifferent(SliderTextBoxOption_ExpirationHealthPenalty, BSConfig->TargetConfig.ExpirationHealthPenalty);
	
	UpdateValueIfDifferent(ComboBoxOption_DamageType, GetStringFromEnum_FromTagMap(BSConfig->TargetConfig.TargetDamageType));
	UpdateValueIfDifferent(CheckBoxOption_UnlimitedTargetHealth, BSConfig->TargetConfig.MaxHealth == -1);
	UpdateValueIfDifferent(ComboBoxOption_MovingTargetDirectionMode, GetStringFromEnum_FromTagMap(BSConfig->TargetConfig.MovingTargetDirectionMode));
	
	UpdateValueIfDifferent(SliderTextBoxOption_TargetScale, BSConfig->TargetConfig.MinSpawnedTargetScale);
	UpdateValueIfDifferent(SliderTextBoxOption_MinTargetScale, BSConfig->TargetConfig.MinSpawnedTargetScale);
	UpdateValueIfDifferent(SliderTextBoxOption_MaxTargetScale, BSConfig->TargetConfig.MaxSpawnedTargetScale);
	
	UpdateValueIfDifferent(SliderTextBoxOption_StartThreshold, BSConfig->DynamicTargetScaling.StartThreshold);
	UpdateValueIfDifferent(SliderTextBoxOption_EndThreshold, BSConfig->DynamicTargetScaling.EndThreshold);
	UpdateValueIfDifferent(SliderTextBoxOption_DecrementAmount, BSConfig->DynamicTargetScaling.DecrementAmount);
	
	UpdateDependentOptions_UnlimitedTargetHealth(BSConfig->TargetConfig.MaxHealth == -1);
	UpdateDependentOptions_RecentTargetMemoryPolicy(BSConfig->TargetConfig.RecentTargetMemoryPolicy);
	UpdateDependentOptions_EnableAI(BSConfig->AIConfig.bEnableReinforcementLearning, BSConfig->AIConfig.HyperParameterMode);
	UpdateDependentOptions_ConsecutiveTargetScalePolicy(BSConfig->TargetConfig.ConsecutiveTargetScalePolicy);
	
	UpdateBrushColors();
}

void UCustomGameModesWidget_General::SetupWarningTooltipCallbacks()
{
	CheckBoxOption_EnableAI->AddWarningTooltipData(FTooltipData("Invalid_HeadshotHeightOnly_AI", ETooltipImageType::Warning)).BindLambda([this]()
	{
		return BSConfig->TargetConfig.TargetDistributionPolicy == ETargetDistributionPolicy::Grid && BSConfig->AIConfig.bEnableReinforcementLearning && BSConfig->TargetConfig.TargetDistributionPolicy == ETargetDistributionPolicy::HeadshotHeightOnly;
	});
	CheckBoxOption_EnableAI->AddWarningTooltipData(FTooltipData("Invalid_Tracking_AI", ETooltipImageType::Warning)).BindLambda([this]()
	{
		return BSConfig->TargetConfig.TargetDistributionPolicy == ETargetDistributionPolicy::Grid && BSConfig->AIConfig.bEnableReinforcementLearning && BSConfig->TargetConfig.TargetDamageType == ETargetDamageType::Tracking;
	});
	ComboBoxOption_DamageType->AddWarningTooltipData(FTooltipData("Invalid_Tracking_AI", ETooltipImageType::Warning)).BindLambda([this]()
	{
		return BSConfig->AIConfig.bEnableReinforcementLearning && BSConfig->TargetConfig.TargetDamageType == ETargetDamageType::Tracking;
	});
	ComboBoxOption_MovingTargetDirectionMode->AddWarningTooltipData(FTooltipData("Invalid_Velocity_MTDM_None_2", ETooltipImageType::Caution)).BindLambda([this]()
	{
		return BSConfig->TargetConfig.MovingTargetDirectionMode == EMovingTargetDirectionMode::None && (BSConfig->TargetConfig.bApplyVelocityWhenSpawned ||
			BSConfig->TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::ChangeVelocity) ||
			BSConfig->TargetConfig.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::ChangeVelocity)
			);
	});
	ComboBoxOption_MovingTargetDirectionMode->AddWarningTooltipData(FTooltipData("Invalid_Direction_MTDM_None_2", ETooltipImageType::Caution)).BindLambda([this]()
	{
		return BSConfig->TargetConfig.MovingTargetDirectionMode == EMovingTargetDirectionMode::None && (BSConfig->TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::ChangeDirection) ||
			BSConfig->TargetConfig.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::ChangeDirection));
	});
	ComboBoxOption_MovingTargetDirectionMode->AddWarningTooltipData(FTooltipData("Caution_ZeroForwardDistance_MTDM_ForwardOnly", ETooltipImageType::Caution)).BindLambda([this]()
	{
		return BSConfig->TargetConfig.MovingTargetDirectionMode == EMovingTargetDirectionMode::ForwardOnly && BSConfig->TargetConfig.BoxBounds.X <= 0.f;
	});
	SliderTextBoxOption_TargetScale->AddDynamicWarningTooltipData(FTooltipData("Invalid_Grid_MaxSpawnedTargetScale", ETooltipImageType::Warning),
		"Invalid_Grid_MaxSpawnedTargetScale_Fallback", MinValue_TargetScale, 2).BindLambda([this]()
	{
		const float Max = FMath::Max(BSConfig->TargetConfig.MaxSpawnedTargetScale, BSConfig->TargetConfig.MinSpawnedTargetScale);
		return FDynamicTooltipState(Max, GetMaxAllowedTargetScale());
	});
	SliderTextBoxOption_MinTargetScale->AddDynamicWarningTooltipData(FTooltipData("Invalid_Grid_MaxSpawnedTargetScale", ETooltipImageType::Warning),
	"Invalid_Grid_MaxSpawnedTargetScale_Fallback", MinValue_TargetScale, 2).BindLambda([this]()
	{
		return FDynamicTooltipState(BSConfig->TargetConfig.MinSpawnedTargetScale, GetMaxAllowedTargetScale());
	});
	SliderTextBoxOption_MaxTargetScale->AddDynamicWarningTooltipData(FTooltipData("Invalid_Grid_MaxSpawnedTargetScale", ETooltipImageType::Warning),
	"Invalid_Grid_MaxSpawnedTargetScale_Fallback", MinValue_TargetScale, 2).BindLambda([this]()
	{
		return FDynamicTooltipState(BSConfig->TargetConfig.MaxSpawnedTargetScale, GetMaxAllowedTargetScale());
	});
}

void UCustomGameModesWidget_General::UpdateDependentOptions_RecentTargetMemoryPolicy(const ERecentTargetMemoryPolicy& InRecentTargetMemoryPolicy)
{
	switch (InRecentTargetMemoryPolicy)
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
}

void UCustomGameModesWidget_General::UpdateDependentOptions_EnableAI(const bool bInEnableReinforcementLearning, const EReinforcementLearningHyperParameterMode HyperParameterMode)
{
	if (bInEnableReinforcementLearning)
	{
		ComboBoxOption_HyperParameterMode->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		ComboBoxOption_HyperParameterMode->SetVisibility(ESlateVisibility::Collapsed);
	}
	UpdateDependentOptions_HyperParameterMode(bInEnableReinforcementLearning, HyperParameterMode);
}

void UCustomGameModesWidget_General::UpdateDependentOptions_HyperParameterMode(const bool bInEnableReinforcementLearning, const EReinforcementLearningHyperParameterMode HyperParameterMode)
{
	if (!bInEnableReinforcementLearning)
	{
		SliderTextBoxOption_Alpha->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_Epsilon->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_Gamma->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	switch (HyperParameterMode) {
	case EReinforcementLearningHyperParameterMode::None:
	case EReinforcementLearningHyperParameterMode::Auto:
		SliderTextBoxOption_Alpha->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_Epsilon->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_Gamma->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case EReinforcementLearningHyperParameterMode::Custom:
		SliderTextBoxOption_Alpha->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_Epsilon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_Gamma->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		break;
	}
}

void UCustomGameModesWidget_General::UpdateDependentOptions_UnlimitedTargetHealth(const bool bInUnlimitedTargetHealth)
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

void UCustomGameModesWidget_General::UpdateDependentOptions_ConsecutiveTargetScalePolicy(const EConsecutiveTargetScalePolicy InConsecutiveTargetScalePolicy)
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
	
	if (InConsecutiveTargetScalePolicy == EConsecutiveTargetScalePolicy::SkillBased)
	{
		SliderTextBoxOption_StartThreshold->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_EndThreshold->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_DecrementAmount->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		SliderTextBoxOption_StartThreshold->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_EndThreshold->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_DecrementAmount->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UCustomGameModesWidget_General::OnCheckStateChanged_EnableAI(const bool bChecked)
{
	BSConfig->AIConfig.bEnableReinforcementLearning = bChecked;
	UpdateDependentOptions_EnableAI(BSConfig->AIConfig.bEnableReinforcementLearning, BSConfig->AIConfig.HyperParameterMode);
	
	UpdateBrushColors();
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_General::OnCheckStateChanged_UnlimitedTargetHealth(const bool bChecked)
{
	BSConfig->TargetConfig.MaxHealth = bChecked ? -1 : SliderTextBoxOption_MaxHealth->GetSliderValue();
	UpdateDependentOptions_UnlimitedTargetHealth(bChecked);
	
	UpdateBrushColors();
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_General::OnSliderTextBoxValueChanged(USliderTextBoxOptionWidget* Widget, const float Value)
{
	if (Widget == SliderTextBoxOption_SpawnBeatDelay)
	{
		BSConfig->TargetConfig.SpawnBeatDelay = Value;
	}
	else if (Widget == SliderTextBoxOption_TargetSpawnCD)
	{
		BSConfig->TargetConfig.TargetSpawnCD = Value;
	}
	else if (Widget == SliderTextBoxOption_MaxNumRecentTargets)
	{
		BSConfig->TargetConfig.MaxNumRecentTargets = Value;
	}
	else if (Widget == SliderTextBoxOption_RecentTargetTimeLength)
	{
		BSConfig->TargetConfig.RecentTargetTimeLength = Value;
	}
	else if (Widget == SliderTextBoxOption_Alpha)
	{
		BSConfig->AIConfig.Alpha = Value;
	}
	else if (Widget == SliderTextBoxOption_Epsilon)
	{
		BSConfig->AIConfig.Epsilon = Value;
	}
	else if (Widget == SliderTextBoxOption_Gamma)
	{
		BSConfig->AIConfig.Gamma = Value;
	}
	else if (Widget == SliderTextBoxOption_TargetMaxLifeSpan)
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
	else if (Widget == SliderTextBoxOption_StartThreshold)
	{
		BSConfig->DynamicTargetScaling.StartThreshold = Value;
	}
	else if (Widget == SliderTextBoxOption_EndThreshold)
	{
		BSConfig->DynamicTargetScaling.EndThreshold = Value;
	}
	else if (Widget == SliderTextBoxOption_DecrementAmount)
	{
		BSConfig->DynamicTargetScaling.DecrementAmount = Value;
	}
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_General::OnSelectionChanged_RecentTargetMemoryPolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct)
	{
		return;
	}
	
	if (Selected.Num() != 1)
	{
		
		return;
	}
	
	BSConfig->TargetConfig.RecentTargetMemoryPolicy = GetEnumFromString<ERecentTargetMemoryPolicy>(Selected[0]);
	UpdateDependentOptions_RecentTargetMemoryPolicy(BSConfig->TargetConfig.RecentTargetMemoryPolicy);
	UpdateBrushColors();
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_General::OnSelectionChanged_DamageType(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || Selected.Num() != 1)
	{
		
		return;
	}
	
	BSConfig->TargetConfig.TargetDamageType = GetEnumFromString<ETargetDamageType>(Selected[0]);
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_General::OnSelectionChanged_ConsecutiveTargetScalePolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || Selected.Num() != 1)
	{
		
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
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_General::OnSelectionChanged_MovingTargetDirectionMode(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || Selected.Num() != 1)
	{
		
		return;
	}

	BSConfig->TargetConfig.MovingTargetDirectionMode = GetEnumFromString<EMovingTargetDirectionMode>(Selected[0]);
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_General::OnSelectionChanged_HyperParameterMode(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || Selected.Num() != 1)
	{
		return;
	}
	BSConfig->AIConfig.HyperParameterMode = GetEnumFromString<EReinforcementLearningHyperParameterMode>(Selected[0]);
	UpdateDependentOptions_EnableAI(BSConfig->AIConfig.bEnableReinforcementLearning, BSConfig->AIConfig.HyperParameterMode);
	
	UpdateBrushColors();
	UpdateAllOptionsValid();
}

FString UCustomGameModesWidget_General::GetComboBoxEntryTooltipStringTableKey_HyperParameterMode(const FString& EnumString)
{
	const EReinforcementLearningHyperParameterMode EnumValue = GetEnumFromString<EReinforcementLearningHyperParameterMode>(EnumString);
	return GetStringTableKeyNameFromEnum(EnumValue);
}

FString UCustomGameModesWidget_General::GetComboBoxEntryTooltipStringTableKey_MovingTargetDirectionMode(const FString& EnumString)
{
	const EMovingTargetDirectionMode EnumValue = GetEnumFromString<EMovingTargetDirectionMode>(EnumString);
	return GetStringTableKeyNameFromEnum(EnumValue);
}

FString UCustomGameModesWidget_General::GetComboBoxEntryTooltipStringTableKey_TargetActivationSelectionPolicy(const FString& EnumString)
{
	return GetStringTableKeyNameFromEnum(GetEnumFromString<ERecentTargetMemoryPolicy>(EnumString));
}

FString UCustomGameModesWidget_General::GetComboBoxEntryTooltipStringTableKey_DamageType(const FString& EnumString)
{
	const ETargetDamageType EnumValue = GetEnumFromString<ETargetDamageType>(EnumString);
	return GetStringTableKeyNameFromEnum(EnumValue);
}

FString UCustomGameModesWidget_General::GetComboBoxEntryTooltipStringTableKey_ConsecutiveTargetScalePolicy(const FString& EnumString)
{
	const EConsecutiveTargetScalePolicy EnumValue = GetEnumFromString<EConsecutiveTargetScalePolicy>(EnumString);
	return GetStringTableKeyNameFromEnum(EnumValue);
}