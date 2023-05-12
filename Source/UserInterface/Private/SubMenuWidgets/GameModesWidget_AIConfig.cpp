// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

// ReSharper disable CppMemberFunctionMayBeConst
#include "SubMenuWidgets/GameModesWidget_AIConfig.h"
#include "GlobalConstants.h"
#include "BSWidgetInterface.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/Slider.h"
#include "WidgetComponents/BSHorizontalBox.h"

using namespace Constants;

void UGameModesWidget_AIConfig::NativeConstruct()
{
	Super::NativeConstruct();

	AddToTooltipData(QMark_EnableAI, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "EnableAI"));
	AddToTooltipData(QMark_Alpha, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "Alpha"));
	AddToTooltipData(QMark_Epsilon, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "Epsilon"));
	AddToTooltipData(QMark_Gamma, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "Gamma"));

	CheckBox_EnableAI->OnCheckStateChanged.AddDynamic(this, &UGameModesWidget_AIConfig::OnCheckStateChanged_EnableAI);
	Slider_AIAlpha->OnValueChanged.AddDynamic(this, &UGameModesWidget_AIConfig::OnSliderChanged_AIAlpha);
	Value_AIAlpha->OnTextCommitted.AddDynamic(this, &UGameModesWidget_AIConfig::OnTextCommitted_AIAlpha);
	Slider_AIEpsilon->OnValueChanged.AddDynamic(this, &UGameModesWidget_AIConfig::OnSliderChanged_AIEpsilon);
	Value_AIEpsilon->OnTextCommitted.AddDynamic(this, &UGameModesWidget_AIConfig::OnTextCommitted_AIEpsilon);
	Slider_AIGamma->OnValueChanged.AddDynamic(this, &UGameModesWidget_AIConfig::OnSliderChanged_AIGamma);
	Value_AIGamma->OnTextCommitted.AddDynamic(this, &UGameModesWidget_AIConfig::OnTextCommitted_AIGamma);
}

void UGameModesWidget_AIConfig::InitSettingCategoryWidget()
{
	Super::InitSettingCategoryWidget();
}

void UGameModesWidget_AIConfig::InitializeAIConfig(const FBS_AIConfig& InAIConfig, const EDefaultMode& BaseGameMode)
{
	switch(BaseGameMode)
	{
	case EDefaultMode::Custom:
		break;
	case EDefaultMode::SingleBeat:
		CheckBox_EnableAI->SetIsChecked(InAIConfig.bEnableRLAgent);
		Slider_AIAlpha->SetValue(InAIConfig.Alpha);
		Value_AIAlpha->SetText(FText::AsNumber(InAIConfig.Alpha));
		Slider_AIGamma->SetValue(InAIConfig.Gamma);
		Value_AIGamma->SetText(FText::AsNumber(InAIConfig.Gamma));
		Slider_AIEpsilon->SetValue(InAIConfig.Epsilon);
		Value_AIEpsilon->SetText(FText::AsNumber(InAIConfig.Epsilon));
		if (InAIConfig.bEnableRLAgent)
		{
			BSBox_AIAlpha->SetVisibility(ESlateVisibility::Visible);
			BSBox_AIGamma->SetVisibility(ESlateVisibility::Visible);
			BSBox_AIEpsilon->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			BSBox_AIAlpha->SetVisibility(ESlateVisibility::Collapsed);
			BSBox_AIGamma->SetVisibility(ESlateVisibility::Collapsed);
			BSBox_AIEpsilon->SetVisibility(ESlateVisibility::Collapsed);
		}
		break;
	case EDefaultMode::MultiBeat:
		CheckBox_EnableAI->SetIsChecked(InAIConfig.bEnableRLAgent);
		Slider_AIAlpha->SetValue(InAIConfig.Alpha);
		Value_AIAlpha->SetText(FText::AsNumber(InAIConfig.Alpha));
		Slider_AIGamma->SetValue(InAIConfig.Gamma);
		Value_AIGamma->SetText(FText::AsNumber(InAIConfig.Gamma));
		Slider_AIEpsilon->SetValue(InAIConfig.Epsilon);
		Value_AIEpsilon->SetText(FText::AsNumber(InAIConfig.Epsilon));
		if (InAIConfig.bEnableRLAgent)
		{
			BSBox_AIAlpha->SetVisibility(ESlateVisibility::Visible);
			BSBox_AIGamma->SetVisibility(ESlateVisibility::Visible);
			BSBox_AIEpsilon->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			BSBox_AIAlpha->SetVisibility(ESlateVisibility::Collapsed);
			BSBox_AIGamma->SetVisibility(ESlateVisibility::Collapsed);
			BSBox_AIEpsilon->SetVisibility(ESlateVisibility::Collapsed);
		}
		break;
	case EDefaultMode::BeatGrid:
		break;
	case EDefaultMode::BeatTrack:
		break;
	default:
		break;
	}

	UpdateBrushColors();
}

FBS_AIConfig UGameModesWidget_AIConfig::GetAIConfig() const
{
	FBS_AIConfig ReturnConfig;
	ReturnConfig.bEnableRLAgent = CheckBox_EnableAI->IsChecked();
	ReturnConfig.Alpha = FMath::GridSnap(FMath::Clamp(Slider_AIAlpha->GetValue(), MinValue_Alpha, MaxValue_Alpha), SnapSize_Alpha);
	ReturnConfig.Epsilon = FMath::GridSnap(FMath::Clamp(Slider_AIEpsilon->GetValue(), MinValue_Epsilon, MaxValue_Epsilon), SnapSize_Epsilon);
	ReturnConfig.Gamma = FMath::GridSnap(FMath::Clamp(Slider_AIGamma->GetValue(), MinValue_Gamma, MaxValue_Gamma), SnapSize_Gamma);
	return ReturnConfig;
}

void UGameModesWidget_AIConfig::OnCheckStateChanged_EnableAI(const bool bEnableAI)
{
	if (bEnableAI)
	{
		BSBox_AIAlpha->SetVisibility(ESlateVisibility::Visible);
		BSBox_AIEpsilon->SetVisibility(ESlateVisibility::Visible);
		BSBox_AIGamma->SetVisibility(ESlateVisibility::Visible);
		return;
	}
	BSBox_AIAlpha->SetVisibility(ESlateVisibility::Collapsed);
	BSBox_AIEpsilon->SetVisibility(ESlateVisibility::Collapsed);
	BSBox_AIGamma->SetVisibility(ESlateVisibility::Collapsed);
}

void UGameModesWidget_AIConfig::OnSliderChanged_AIAlpha(const float NewAlpha)
{
	OnSliderChanged(NewAlpha, Value_AIAlpha, SnapSize_Alpha);
}

void UGameModesWidget_AIConfig::OnTextCommitted_AIAlpha(const FText& NewAlpha, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewAlpha, Value_AIAlpha, Slider_AIAlpha, SnapSize_Alpha,MinValue_Alpha, MaxValue_Alpha);
}

void UGameModesWidget_AIConfig::OnSliderChanged_AIEpsilon(const float NewEpsilon)
{
	OnSliderChanged(NewEpsilon, Value_AIEpsilon, SnapSize_Epsilon);
}

void UGameModesWidget_AIConfig::OnTextCommitted_AIEpsilon(const FText& NewEpsilon, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewEpsilon, Value_AIEpsilon, Slider_AIEpsilon, SnapSize_Epsilon,MinValue_Epsilon, MaxValue_Epsilon);
}

void UGameModesWidget_AIConfig::OnSliderChanged_AIGamma(const float NewGamma)
{
	OnSliderChanged(NewGamma, Value_AIGamma, SnapSize_Gamma);
}

void UGameModesWidget_AIConfig::OnTextCommitted_AIGamma(const FText& NewGamma, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewGamma, Value_AIGamma, Slider_AIGamma, SnapSize_Gamma,MinValue_Gamma, MaxValue_Gamma);
}
