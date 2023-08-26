// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_General.h"
#include "Components/CheckBox.h"
#include "WidgetComponents/BSComboBoxString.h"
#include "WidgetComponents/MenuOptionWidgets/CheckBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/ComboBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/SliderTextBoxWidget.h"

void UCustomGameModesWidget_General::InitComponent(FBSConfig* InConfigPtr, TObjectPtr<UCustomGameModesWidgetComponent> InNext)
{
	Super::InitComponent(InConfigPtr, InNext);
}

void UCustomGameModesWidget_General::NativeConstruct()
{
	Super::NativeConstruct();

	SetupTooltip(SliderTextBoxOption_SpawnBeatDelay->GetTooltipImage(), SliderTextBoxOption_SpawnBeatDelay->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_TargetSpawnCD->GetTooltipImage(), SliderTextBoxOption_TargetSpawnCD->GetTooltipImageText());
	SetupTooltip(ComboBoxOption_RecentTargetMemoryPolicy->GetTooltipImage(), ComboBoxOption_RecentTargetMemoryPolicy->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_MaxNumRecentTargets->GetTooltipImage(), SliderTextBoxOption_MaxNumRecentTargets->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_RecentTargetTimeLength->GetTooltipImage(), SliderTextBoxOption_RecentTargetTimeLength->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_MaxNumTargetsAtOnce->GetTooltipImage(), SliderTextBoxOption_MaxNumTargetsAtOnce->GetTooltipImageText());
	SetupTooltip(CheckBoxOption_EnableAI->GetTooltipImage(), CheckBoxOption_EnableAI->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_Alpha->GetTooltipImage(), SliderTextBoxOption_Alpha->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_Epsilon->GetTooltipImage(), SliderTextBoxOption_Epsilon->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_Gamma->GetTooltipImage(), SliderTextBoxOption_Gamma->GetTooltipImageText());

	SliderTextBoxOption_SpawnBeatDelay->SetValues(MinValue_PlayerDelay, MaxValue_PlayerDelay, SnapSize_PlayerDelay);
	SliderTextBoxOption_TargetSpawnCD->SetValues(MinValue_TargetSpawnCD, MaxValue_TargetSpawnCD, SnapSize_TargetSpawnCD);
	SliderTextBoxOption_MaxNumRecentTargets->SetValues(MinValue_MaxNumRecentTargets, MaxValue_MaxNumRecentTargets, SnapSize_MaxNumRecentTargets);
	SliderTextBoxOption_RecentTargetTimeLength->SetValues(MinValue_RecentTargetTimeLength, MaxValue_RecentTargetTimeLength, SnapSize_RecentTargetTimeLength);
	SliderTextBoxOption_MaxNumTargetsAtOnce->SetValues(MinValue_MaxNumTargetsAtOnce, MaxValue_MaxNumTargetsAtOnce, SnapSize_MaxNumTargetsAtOnce);
	SliderTextBoxOption_Alpha->SetValues(MinValue_Alpha, MaxValue_Alpha, SnapSize_Alpha);
	SliderTextBoxOption_Epsilon->SetValues(MinValue_Epsilon, MaxValue_Epsilon, SnapSize_Epsilon);
	SliderTextBoxOption_Gamma->SetValues(MinValue_Gamma, MaxValue_Gamma, SnapSize_Gamma);

	SliderTextBoxOption_SpawnBeatDelay->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_TargetSpawnCD->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MaxNumRecentTargets->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_RecentTargetTimeLength->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MaxNumTargetsAtOnce->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_Alpha->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_Epsilon->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_Gamma->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);

	CheckBoxOption_EnableAI->CheckBox->OnCheckStateChanged.AddUniqueDynamic(this, &ThisClass::OnCheckStateChanged_EnableAI);
	ComboBoxOption_RecentTargetMemoryPolicy->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_RecentTargetMemoryPolicy);
	ComboBoxOption_RecentTargetMemoryPolicy->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetActivationSelectionPolicy);

	ComboBoxOption_RecentTargetMemoryPolicy->ComboBox->ClearOptions();

	for (const ERecentTargetMemoryPolicy& Method : TEnumRange<ERecentTargetMemoryPolicy>())
	{
		ComboBoxOption_RecentTargetMemoryPolicy->ComboBox->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}

	SliderTextBoxOption_RecentTargetTimeLength->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MaxNumRecentTargets->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_Alpha->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_Epsilon->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_Gamma->SetVisibility(ESlateVisibility::Collapsed);

	UpdateBrushColors();
}

bool UCustomGameModesWidget_General::UpdateAllOptionsValid()
{
	const bool bUpdateNeeded = UpdateTooltipWarningImages(CheckBoxOption_EnableAI, GetWarningTooltipKeys());
	
	if (bUpdateNeeded)
	{
		if (CheckBoxOption_EnableAI->GetTooltipWarningImageKeys().IsEmpty())
		{
			CheckBoxOption_EnableAI->CheckBox->SetIsEnabled(true);
			SliderTextBoxOption_Alpha->SetIsEnabled(true);
			SliderTextBoxOption_Epsilon->SetIsEnabled(true);
			SliderTextBoxOption_Gamma->SetIsEnabled(true);
		}
		else
		{
			CheckBoxOption_EnableAI->CheckBox->SetIsChecked(false);
			CheckBoxOption_EnableAI->CheckBox->SetIsEnabled(false);
			SliderTextBoxOption_Alpha->SetIsEnabled(false);
			SliderTextBoxOption_Epsilon->SetIsEnabled(false);
			SliderTextBoxOption_Gamma->SetIsEnabled(false);
		}
		RequestComponentUpdate.Broadcast();
	}
	
	if (!CheckBoxOption_EnableAI->GetTooltipWarningImageKeys().IsEmpty())
	{
		return false;
	}
	
	return true;
}

void UCustomGameModesWidget_General::UpdateOptionsFromConfig()
{
	UpdateValueIfDifferent(SliderTextBoxOption_SpawnBeatDelay, BSConfig->TargetConfig.SpawnBeatDelay);
	UpdateValueIfDifferent(SliderTextBoxOption_TargetSpawnCD, BSConfig->TargetConfig.TargetSpawnCD);
	UpdateValueIfDifferent(SliderTextBoxOption_MaxNumTargetsAtOnce, BSConfig->TargetConfig.MaxNumTargetsAtOnce);
	UpdateValueIfDifferent(SliderTextBoxOption_MaxNumRecentTargets, BSConfig->TargetConfig.MaxNumRecentTargets);
	UpdateValueIfDifferent(SliderTextBoxOption_RecentTargetTimeLength, BSConfig->TargetConfig.RecentTargetTimeLength);
	
	UpdateValueIfDifferent(SliderTextBoxOption_Alpha, BSConfig->AIConfig.Alpha);
	UpdateValueIfDifferent(SliderTextBoxOption_Epsilon, BSConfig->AIConfig.Epsilon);
	UpdateValueIfDifferent(SliderTextBoxOption_Gamma, BSConfig->AIConfig.Gamma);

	UpdateValueIfDifferent(CheckBoxOption_EnableAI, BSConfig->AIConfig.bEnableReinforcementLearning);
	UpdateValueIfDifferent(ComboBoxOption_RecentTargetMemoryPolicy, GetStringFromEnum(BSConfig->TargetConfig.RecentTargetMemoryPolicy));

	UpdateDependentOptions_RecentTargetMemoryPolicy(BSConfig->TargetConfig.RecentTargetMemoryPolicy);
	UpdateDependentOptions_EnableAI(BSConfig->AIConfig.bEnableReinforcementLearning);
	
	UpdateBrushColors();
	SetAllOptionsValid(UpdateAllOptionsValid());
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

void UCustomGameModesWidget_General::UpdateDependentOptions_EnableAI(const bool bInEnableReinforcementLearning)
{
	if (bInEnableReinforcementLearning)
	{
		SliderTextBoxOption_Alpha->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_Epsilon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_Gamma->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		SliderTextBoxOption_Alpha->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_Epsilon->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_Gamma->SetVisibility(ESlateVisibility::Collapsed);
	}
}

TArray<FString> UCustomGameModesWidget_General::GetWarningTooltipKeys()
{
	TArray<FString> ReturnArray;
	if (BSConfig->AIConfig.bEnableReinforcementLearning)
	{
		if (BSConfig->TargetConfig.TargetDistributionPolicy == ETargetDistributionPolicy::Grid)
		{
			if (BSConfig->GridConfig.NumHorizontalGridTargets % 5 != 0 || BSConfig->GridConfig.NumVerticalGridTargets % 5 != 0)
			{
				ReturnArray.Emplace("InvalidAI_GridSpacing");
			}
		}
		if (BSConfig->TargetConfig.TargetDamageType == ETargetDamageType::Tracking)
		{
			ReturnArray.Emplace("InvalidAI_Tracking");
		}
	}
	return ReturnArray;
}

void UCustomGameModesWidget_General::OnCheckStateChanged_EnableAI(const bool bChecked)
{
	BSConfig->AIConfig.bEnableReinforcementLearning = bChecked;
	UpdateDependentOptions_EnableAI(BSConfig->AIConfig.bEnableReinforcementLearning);
	
	UpdateBrushColors();
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_General::OnSliderTextBoxValueChanged(USliderTextBoxWidget* Widget, const float Value)
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
	else if (Widget == SliderTextBoxOption_MaxNumTargetsAtOnce)
	{
		BSConfig->TargetConfig.MaxNumTargetsAtOnce = Value;
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
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_General::OnSelectionChanged_RecentTargetMemoryPolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
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
	
	BSConfig->TargetConfig.RecentTargetMemoryPolicy = GetEnumFromString<ERecentTargetMemoryPolicy>(Selected[0]);
	UpdateDependentOptions_RecentTargetMemoryPolicy(BSConfig->TargetConfig.RecentTargetMemoryPolicy);
	UpdateBrushColors();
	
	SetAllOptionsValid(UpdateAllOptionsValid());
}

FString UCustomGameModesWidget_General::GetComboBoxEntryTooltipStringTableKey_TargetActivationSelectionPolicy(const FString& EnumString)
{
	return GetStringTableKeyNameFromEnum(GetEnumFromString<ERecentTargetMemoryPolicy>(EnumString));
}
