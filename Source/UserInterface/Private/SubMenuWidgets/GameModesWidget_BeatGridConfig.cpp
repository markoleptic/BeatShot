// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "SubMenuWidgets/GameModesWidget_BeatGridConfig.h"
#include "WidgetComponents/DoubleSyncedSliderAndTextBox.h"
#include "GlobalConstants.h"
#include "UserInterface.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Slider.h"
#include "WidgetComponents/BSVerticalBox.h"
#include "WidgetComponents/TooltipWidget.h"

using namespace Constants;

UTooltipWidget* UGameModesWidget_BeatGridConfig::ConstructTooltipWidget()
{
	return CreateWidget<UTooltipWidget>(this, TooltipWidgetClass);
}

void UGameModesWidget_BeatGridConfig::InitSettingCategoryWidget()
{
	if (BeatGridSpreadConstrained)
	{
		AddWidgetBoxPair(BeatGridSpreadConstrained.Get(), BeatGridSpreadConstrained->MainContainer);
	}
	Super::InitSettingCategoryWidget();
}

void UGameModesWidget_BeatGridConfig::NativeConstruct()
{
	/* Create MainContainer before calling Super NativeConstruct since the parent calls InitSettingCategoryWidget in NativeConstruct */
	BeatGridSpreadConstrained = CreateWidget<UDoubleSyncedSliderAndTextBox>(this, BeatGridSpreadConstrainedClass);
	MainContainer->AddChildToVerticalBox(BeatGridSpreadConstrained.Get());
	
	Super::NativeConstruct();
	
	SetTooltipWidget(ConstructTooltipWidget());
	AddToTooltipData(BeatGridSpreadConstrained->CheckboxQMark, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "BeatGridEvenSpacing"));
	AddToTooltipData(BeatGridAdjacentOnlyQMark, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "BeatGridAdjacentOnly"));
	
	/* BeatGrid Spacing TextBox and Slider */
	FConstrainedSliderStruct BeatGridSliderStruct;
	BeatGridSliderStruct.MinConstraintLower = MinValue_BeatGridHorizontalSpacing;
	BeatGridSliderStruct.MinConstraintUpper = MaxValue_BeatGridHorizontalSpacing;
	BeatGridSliderStruct.MaxConstraintLower = MinValue_BeatGridVerticalSpacing;
	BeatGridSliderStruct.MaxConstraintUpper = MaxValue_BeatGridVerticalSpacing;
	BeatGridSliderStruct.DefaultMinValue = MaxValue_BeatGridHorizontalSpacing;
	BeatGridSliderStruct.DefaultMaxValue = MaxValue_BeatGridVerticalSpacing;
	BeatGridSliderStruct.MinText = FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_BeatGrid_HorizontalSpacing");
	BeatGridSliderStruct.MaxText = FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_BeatGrid_VerticalSpacing");
	BeatGridSliderStruct.CheckboxText = FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_EvenSpacing");
	BeatGridSliderStruct.GridSnapSize = SnapSize_BeatGridVerticalSpacing;
	BeatGridSliderStruct.bShowMaxLock = true;
	BeatGridSliderStruct.bShowMinLock = true;
	BeatGridSpreadConstrained->InitConstrainedSlider(BeatGridSliderStruct);

	TooltipWarningSizing.SizeRule = ESlateSizeRule::Fill;
	
	BeatGridSpreadConstrained->OnMinValueChanged.AddUObject(this, &UGameModesWidget_BeatGridConfig::OnConstrainedChanged_HorizontalSpacing);
	BeatGridSpreadConstrained->OnMaxValueChanged.AddUObject(this, &UGameModesWidget_BeatGridConfig::OnConstrainedChanged_VerticalSpacing);
	BeatGridSpreadConstrained->OnCheckStateChanged_Min.AddUObject(this, &UGameModesWidget_BeatGridConfig::OnCheckStateChanged_MinLock);
	BeatGridSpreadConstrained->OnCheckStateChanged_Max.AddUObject(this, &UGameModesWidget_BeatGridConfig::OnCheckStateChanged_MaxLock);
	
	Slider_NumHorizontalTargets->OnValueChanged.AddDynamic(this, &UGameModesWidget_BeatGridConfig::OnSliderChanged_BeatGridNumHorizontalTargets);
	Value_NumHorizontalTargets->OnTextCommitted.AddDynamic(this, &UGameModesWidget_BeatGridConfig::OnTextCommitted_BeatGridNumHorizontalTargets);
	
	Slider_NumVerticalTargets->OnValueChanged.AddDynamic(this, &UGameModesWidget_BeatGridConfig::OnSliderChanged_BeatGridNumVerticalTargets);
	Value_NumVerticalTargets->OnTextCommitted.AddDynamic(this, &UGameModesWidget_BeatGridConfig::OnTextCommitted_BeatGridNumVerticalTargets);
	
	CheckBox_NumHorizontalTargetsLock->OnCheckStateChanged.AddDynamic(this, &UGameModesWidget_BeatGridConfig::OnCheckStateChanged_BeatGridNumHorizontalTargetsLock);
	CheckBox_NumVerticalTargetsLock->OnCheckStateChanged.AddDynamic(this, &UGameModesWidget_BeatGridConfig::OnCheckStateChanged_BeatGridNumVerticalTargetsLock);
	CheckBox_RandomizeNextTarget->OnCheckStateChanged.AddDynamic(this, &UGameModesWidget_BeatGridConfig::OnCheckStateChanged_RandomizeNextTarget);
	
	bNumHorizontalTargetsLocked = true;
	bNumVerticalTargetsLocked = true;
	bHorizontalSpacingLocked = true;
	bVerticalSpacingLocked = true;
	CheckBox_NumHorizontalTargetsLock->SetIsChecked(true);
	CheckBox_NumVerticalTargetsLock->SetIsChecked(true);
	BeatGridSpreadConstrained->MinLock->SetIsChecked(true);
	BeatGridSpreadConstrained->MaxLock->SetIsChecked(true);
}

void UGameModesWidget_BeatGridConfig::InitializeBeatGrid(const FBS_BeatGridConfig& InBeatGridConfig, const UHorizontalBox* InTargetScaleBox)
{
	CurrentValues = InBeatGridConfig;
	TargetScaleBox = InTargetScaleBox;
	CheckBox_RandomizeNextTarget->SetIsChecked(InBeatGridConfig.RandomizeBeatGrid);
	Slider_NumHorizontalTargets->SetValue(InBeatGridConfig.NumHorizontalBeatGridTargets);
	Slider_NumHorizontalTargets->SetMinValue(MinValue_NumBeatGridHorizontalTargets);
	Slider_NumHorizontalTargets->SetMaxValue(MaxValue_NumBeatGridHorizontalTargets);
	Value_NumHorizontalTargets->SetText(FText::AsNumber(InBeatGridConfig.NumHorizontalBeatGridTargets));
	Slider_NumVerticalTargets->SetValue(InBeatGridConfig.NumVerticalBeatGridTargets);
	Slider_NumVerticalTargets->SetMinValue(MinValue_NumBeatGridVerticalTargets);
	Slider_NumVerticalTargets->SetMaxValue(MaxValue_NumBeatGridVerticalTargets);
	Value_NumVerticalTargets->SetText(FText::AsNumber(InBeatGridConfig.NumVerticalBeatGridTargets));
	BeatGridSpreadConstrained->UpdateDefaultValues(InBeatGridConfig.BeatGridSpacing.X, InBeatGridConfig.BeatGridSpacing.Y);
	// MinSlider->SetValue(InBeatGridConfig.BeatGridSpacing.X);
	// MaxSlider->SetValue(InBeatGridConfig.BeatGridSpacing.Y);
	// MinValue->SetText(FText::AsNumber(InBeatGridConfig.BeatGridSpacing.X));
	// MaxValue->SetText(FText::AsNumber(InBeatGridConfig.BeatGridSpacing.Y));
}

FBS_BeatGridConfig UGameModesWidget_BeatGridConfig::GetBeatGridConfig() const
{
	FBS_BeatGridConfig ReturnConfig;
	ReturnConfig.BeatGridSpacing = FVector2D(
	FMath::GridSnap(FMath::Clamp(BeatGridSpreadConstrained->MinSlider->GetValue(), MinValue_BeatGridHorizontalSpacing, MaxValue_BeatGridHorizontalSpacing), SnapSize_BeatGridHorizontalSpacing),
	FMath::GridSnap(FMath::Clamp(BeatGridSpreadConstrained->MaxSlider->GetValue(), MinValue_BeatGridVerticalSpacing, MaxValue_BeatGridVerticalSpacing), SnapSize_BeatGridVerticalSpacing));
	ReturnConfig.NumHorizontalBeatGridTargets = FMath::GridSnap(FMath::Clamp(Slider_NumHorizontalTargets->GetValue(), MinValue_NumBeatGridHorizontalTargets, MaxValue_NumBeatGridHorizontalTargets), SnapSize_NumBeatGridHorizontalTargets);
	ReturnConfig.NumVerticalBeatGridTargets = FMath::GridSnap(FMath::Clamp(Slider_NumVerticalTargets->GetValue(), MinValue_NumBeatGridVerticalTargets, MaxValue_NumBeatGridVerticalTargets), SnapSize_NumBeatGridVerticalTargets);
	ReturnConfig.RandomizeBeatGrid = CheckBox_RandomizeNextTarget->IsChecked();
	ReturnConfig.NumTargetsAtOnceBeatGrid = -1;
	return ReturnConfig;
}

void UGameModesWidget_BeatGridConfig::OnConstrainedChanged_HorizontalSpacing(const float NewHorizontalSpacing)
{
	CurrentValues.BeatGridSpacing.X = NewHorizontalSpacing;
	CheckBeatGridConstraints(EBeatGridConstraintType::HorizontalSpacing);
}

void UGameModesWidget_BeatGridConfig::OnConstrainedChanged_VerticalSpacing(const float NewVerticalSpacing)
{
	CurrentValues.BeatGridSpacing.Y = NewVerticalSpacing;
	CheckBeatGridConstraints(EBeatGridConstraintType::VerticalSpacing);
}

void UGameModesWidget_BeatGridConfig::OnSliderChanged_BeatGridNumHorizontalTargets(const float NewNumHorizontalTargets)
{
	CurrentValues.NumHorizontalBeatGridTargets = UUserInterface::OnSliderChanged(NewNumHorizontalTargets, Value_NumHorizontalTargets, SnapSize_NumBeatGridHorizontalTargets);
	CheckBeatGridConstraints(EBeatGridConstraintType::NumHorizontalTargets);
}

void UGameModesWidget_BeatGridConfig::OnSliderChanged_BeatGridNumVerticalTargets(const float NewNumVerticalTargets)
{
	CurrentValues.NumVerticalBeatGridTargets = UUserInterface::OnSliderChanged(NewNumVerticalTargets, Value_NumVerticalTargets, SnapSize_NumBeatGridVerticalTargets);
	CheckBeatGridConstraints(EBeatGridConstraintType::NumVerticalTargets);
}

void UGameModesWidget_BeatGridConfig::OnTextCommitted_BeatGridNumHorizontalTargets(const FText& NewNumHorizontalTargets, ETextCommit::Type CommitType)
{
	CurrentValues.NumHorizontalBeatGridTargets = UUserInterface::OnEditableTextBoxChanged(NewNumHorizontalTargets, Value_NumHorizontalTargets, Slider_NumHorizontalTargets,
		SnapSize_NumBeatGridHorizontalTargets, MinValue_BeatGridHorizontalSpacing, MaxValue_BeatGridHorizontalSpacing);
	CheckBeatGridConstraints(EBeatGridConstraintType::NumHorizontalTargets);
}

void UGameModesWidget_BeatGridConfig::OnTextCommitted_BeatGridNumVerticalTargets(const FText& NewNumVerticalTargets, ETextCommit::Type CommitType)
{
	CurrentValues.NumVerticalBeatGridTargets = UUserInterface::OnEditableTextBoxChanged(NewNumVerticalTargets, Value_NumVerticalTargets, Slider_NumVerticalTargets,
		SnapSize_NumBeatGridVerticalTargets, MinValue_BeatGridVerticalSpacing, MaxValue_BeatGridVerticalSpacing);
	CheckBeatGridConstraints(EBeatGridConstraintType::NumVerticalTargets);
}

void UGameModesWidget_BeatGridConfig::OnCheckStateChanged_BeatGridNumHorizontalTargetsLock(const bool bIsLocked)
{
	bNumHorizontalTargetsLocked = bIsLocked;
	CurrentValues.NumHorizontalBeatGridTargets = Slider_NumHorizontalTargets->GetValue();
	CheckBeatGridConstraints(EBeatGridConstraintType::NumHorizontalTargets);
}

void UGameModesWidget_BeatGridConfig::OnCheckStateChanged_BeatGridNumVerticalTargetsLock(const bool bIsLocked)
{
	bNumVerticalTargetsLocked = bIsLocked;
	CurrentValues.NumVerticalBeatGridTargets = Slider_NumVerticalTargets->GetValue();
	CheckBeatGridConstraints(EBeatGridConstraintType::NumVerticalTargets);
}

void UGameModesWidget_BeatGridConfig::OnCheckStateChanged_RandomizeNextTarget(const bool bIsChecked)
{
	CurrentValues.RandomizeBeatGrid = bIsChecked;
}

void UGameModesWidget_BeatGridConfig::OnCheckStateChanged_MinLock(const bool bIsLocked)
{
	bHorizontalSpacingLocked = bIsLocked;
	if (!bIsLocked)
	{
		BeatGridSpreadConstrained->OverrideMaxValue(true, MaxValue_BeatGridHorizontalSpacing);
	}
	CurrentValues.BeatGridSpacing.X = BeatGridSpreadConstrained->MinSlider->GetValue();
	CheckBeatGridConstraints(EBeatGridConstraintType::HorizontalSpacing);
}

void UGameModesWidget_BeatGridConfig::OnCheckStateChanged_MaxLock(const bool bIsLocked)
{
	bVerticalSpacingLocked = bIsLocked;
	if (!bIsLocked)
	{
		BeatGridSpreadConstrained->OverrideMaxValue(false, MaxValue_BeatGridVerticalSpacing);
	}
	CurrentValues.BeatGridSpacing.Y = BeatGridSpreadConstrained->MaxSlider->GetValue();
	CheckBeatGridConstraints(EBeatGridConstraintType::VerticalSpacing);
}

void UGameModesWidget_BeatGridConfig::OnBeatGridUpdate_MaxTargetScale(const float NewMaxTargetScale)
{
	MaxTargetSize = NewMaxTargetScale * SphereDiameter;
	HorizontalSpread = MaxValue_BeatGridHorizontalSpacing + MaxTargetSize;
	VerticalSpread = MaxValue_BeatGridVerticalSpacing + MaxTargetSize;
	CheckBeatGridConstraints(EBeatGridConstraintType::TargetScale);
}

void UGameModesWidget_BeatGridConfig::CheckBeatGridConstraints(const EBeatGridConstraintType LastChangedConstraint)
{
	FBeatGridConstraints BeatGridConstraints;
	TArray<FText> SuggestionValueArray;
	SuggestionValueArray.Init(FText(), 5);

	if (bHorizontalSpacingLocked && LastChangedConstraint != EBeatGridConstraintType::HorizontalSpacing)
	{
		if (const float Value = floorf(GetMaxAllowedHorizontalSpacing() / 10.f) * 10.f; Value >= MinValue_BeatGridHorizontalSpacing && Value <= MaxValue_BeatGridHorizontalSpacing)
		{
			BeatGridSpreadConstrained->OverrideMaxValue(true, Value);
		}
	}
	if (bVerticalSpacingLocked && LastChangedConstraint != EBeatGridConstraintType::VerticalSpacing)
	{
		if (const float Value = floorf(GetMaxAllowedVerticalSpacing() / 10.f) * 10.f; Value >= MinValue_BeatGridVerticalSpacing && Value <= MaxValue_BeatGridVerticalSpacing)
		{
			BeatGridSpreadConstrained->OverrideMaxValue(false, Value);
		}
	}
	if (bNumHorizontalTargetsLocked && LastChangedConstraint != EBeatGridConstraintType::NumHorizontalTargets)
	{
		if (const int32 Value = GetMaxAllowedNumHorizontalTargets(); Value >= MinValue_NumBeatGridHorizontalTargets && Value <= MaxValue_NumBeatGridHorizontalTargets && Value < Slider_NumHorizontalTargets->GetValue())
		{
			Slider_NumHorizontalTargets->SetValue(Value);
			Value_NumHorizontalTargets->SetText(FText::AsNumber(Value));
			CurrentValues.NumHorizontalBeatGridTargets = Value;
		}
	}
	if (bNumVerticalTargetsLocked && LastChangedConstraint != EBeatGridConstraintType::NumVerticalTargets)
	{
		if (const int32 Value = GetMaxAllowedNumVerticalTargets(); Value >= MinValue_NumBeatGridVerticalTargets && Value <= MaxValue_NumBeatGridVerticalTargets && Value < Slider_NumVerticalTargets->GetValue())
		{
			Slider_NumVerticalTargets->SetValue(Value);
			Value_NumVerticalTargets->SetText(FText::AsNumber(Value));
			CurrentValues.NumVerticalBeatGridTargets = Value;
		}
	}
	
	if (CurrentValues.NumHorizontalBeatGridTargets > GetMaxAllowedNumHorizontalTargets())
	{
		BeatGridConstraints.ConstraintTypes.Add(EBeatGridConstraintType::NumHorizontalTargets);
		if (const int32 Value = GetMaxAllowedNumHorizontalTargets(); Value >= MinValue_NumBeatGridHorizontalTargets && Value <= MaxValue_NumBeatGridHorizontalTargets)
		{
			SuggestionValueArray[0] = FText::Join(SpaceDelimit, HorizontalSuggest, FText::AsNumber(Value));
		}
	}
	if (CurrentValues.NumVerticalBeatGridTargets > GetMaxAllowedNumVerticalTargets())
	{
		BeatGridConstraints.ConstraintTypes.Add(EBeatGridConstraintType::NumVerticalTargets);
		if (const int32 Value = GetMaxAllowedNumVerticalTargets(); Value >= MinValue_NumBeatGridVerticalTargets && Value <= MaxValue_NumBeatGridVerticalTargets)
		{
			SuggestionValueArray[1] = FText::Join(SpaceDelimit, VerticalSuggest,  FText::AsNumber(Value));
		}
	}
	if (MaxTargetSize / SphereDiameter > GetMaxAllowedTargetScale())
	{
		BeatGridConstraints.ConstraintTypes.Add(EBeatGridConstraintType::TargetScale);
		if (const float Value = floorf(GetMaxAllowedTargetScale() * 100.f) / 100.f; Value >= MinValue_TargetScale && Value <= MaxValue_TargetScale)
		{
			SuggestionValueArray[2] = FText::Join(SpaceDelimit, ScaleSuggest, FText::AsNumber(Value));
		}
	}
	if (CurrentValues.BeatGridSpacing.X > GetMaxAllowedHorizontalSpacing())
	{
		BeatGridConstraints.ConstraintTypes.Add(EBeatGridConstraintType::HorizontalSpacing);
		if (const float Value = floorf(GetMaxAllowedHorizontalSpacing() / 10.f) * 10.f; Value >= MinValue_BeatGridHorizontalSpacing && Value <= MaxValue_BeatGridHorizontalSpacing)
		{
			SuggestionValueArray[3] = FText::Join(SpaceDelimit, HorizontalSpacingSuggest, FText::AsNumber(Value));
		}
	}
	if (CurrentValues.BeatGridSpacing.Y > GetMaxAllowedVerticalSpacing())
	{
		BeatGridConstraints.ConstraintTypes.Add(EBeatGridConstraintType::VerticalSpacing);
		if (const float Value = floorf(GetMaxAllowedVerticalSpacing() / 10.f) * 10.f; Value >= MinValue_BeatGridVerticalSpacing && Value <= MaxValue_BeatGridVerticalSpacing)
		{
			SuggestionValueArray[4] = FText::Join(SpaceDelimit, VerticalSpacingSuggest, FText::AsNumber(Value));
		}
	}
	
	for (const EBeatGridConstraintType ConstraintType : BeatGridConstraints.ConstraintTypes)
	{
		FText JoinedValues;
		switch (ConstraintType) {
		case EBeatGridConstraintType::NumHorizontalTargets:
			JoinedValues = FText::Join(NewLineDelimit, FilterTooltipText(SuggestionValueArray, 0, false));
			BeatGridConstraints.Tooltip_NumHorizontalTargets = FText::Join(NewLineDelimit, SuggestStart, JoinedValues);
			break;
		case EBeatGridConstraintType::NumVerticalTargets:
			JoinedValues = FText::Join(NewLineDelimit, FilterTooltipText(SuggestionValueArray, 1, false));
			BeatGridConstraints.Tooltip_NumVerticalTargets = FText::Join(NewLineDelimit, SuggestStart, JoinedValues);
			break;
		case EBeatGridConstraintType::TargetScale:
			JoinedValues = FText::Join(NewLineDelimit, FilterTooltipText(SuggestionValueArray, 2, false));
			BeatGridConstraints.Tooltip_TargetScale = FText::Join(NewLineDelimit, SuggestStart, JoinedValues);
			break;
		case EBeatGridConstraintType::HorizontalSpacing:
			JoinedValues = FText::Join(NewLineDelimit, FilterTooltipText(SuggestionValueArray, 3, false));
			BeatGridConstraints.Tooltip_HorizontalSpacing = FText::Join(NewLineDelimit, SuggestStart, JoinedValues);
			break;
		case EBeatGridConstraintType::VerticalSpacing:
			JoinedValues = FText::Join(NewLineDelimit, FilterTooltipText(SuggestionValueArray, 4, false));
			BeatGridConstraints.Tooltip_VerticalSpacing = FText::Join(NewLineDelimit, SuggestStart, JoinedValues);
			break;
		case EBeatGridConstraintType::None:
			break;
		}
	}
	ActiveConstraints = BeatGridConstraints.ConstraintTypes;
	OnBeatGridUpdate_SaveStartButtonStates.Execute();
	UpdateAllWarningTooltips(BeatGridConstraints);
}

UTooltipImage* UGameModesWidget_BeatGridConfig::ConstructBeatGridWarningEMarkWidget(const EBeatGridConstraintType ConstraintType)
{
	UTooltipImage* BeatGridWarningEMarkWidget = WidgetTree->ConstructWidget<UTooltipImage>(BeatGridWarningEMarkClass);

	UHorizontalBox* BoxToPlaceIn;
	switch (ConstraintType)
	{
	case EBeatGridConstraintType::NumHorizontalTargets:
		BoxToPlaceIn = Box_NumHorizontalTargetsTextTooltip;
		WarningEMark_NumHorizontalTargets = BeatGridWarningEMarkWidget;
		break;
	case EBeatGridConstraintType::NumVerticalTargets:
		BoxToPlaceIn = Box_NumVerticalTargetsTextTooltip;
		WarningEMark_NumVerticalTargets = BeatGridWarningEMarkWidget;
		break;
	case EBeatGridConstraintType::TargetScale:
		BoxToPlaceIn = TargetScaleBox.Get();
		WarningEMark_MaxTargetScale = BeatGridWarningEMarkWidget;
		break;
	case EBeatGridConstraintType::HorizontalSpacing:
		BoxToPlaceIn = BeatGridSpreadConstrained->TextTooltipBox_Min;
		WarningEMark_HorizontalSpacing = BeatGridWarningEMarkWidget;
		break;
	case EBeatGridConstraintType::VerticalSpacing:
		BoxToPlaceIn = BeatGridSpreadConstrained->TextTooltipBox_Max;
		WarningEMark_VerticalSpacing = BeatGridWarningEMarkWidget;
		break;
	default:
		BoxToPlaceIn = nullptr;
	}
	UHorizontalBoxSlot* HorizontalBoxSlot = BoxToPlaceIn->AddChildToHorizontalBox(BeatGridWarningEMarkWidget);
	HorizontalBoxSlot->SetHorizontalAlignment(HAlign_Right);
	HorizontalBoxSlot->SetSize(TooltipWarningSizing);
	return BeatGridWarningEMarkWidget;
}

void UGameModesWidget_BeatGridConfig::HandleBeatGridWarningDisplay(UTooltipImage* TooltipImage, const EBeatGridConstraintType ConstraintType, const FText& TooltipText, const bool bDisplay)
{
	if (bDisplay)
	{
		if (TooltipImage != nullptr)
		{
			TooltipImage->SetVisibility(ESlateVisibility::Visible);
			EditTooltipText(TooltipImage, TooltipText);
		}
		else
		{
			AddToTooltipData(ConstructBeatGridWarningEMarkWidget(ConstraintType), TooltipText, true);
		}
	}
	else
	{
		if (TooltipImage != nullptr)
		{
			TooltipImage->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UGameModesWidget_BeatGridConfig::UpdateAllWarningTooltips(const FBeatGridConstraints& BeatGridConstraints)
{
	for (EBeatGridConstraintType ConstraintType : TEnumRange<EBeatGridConstraintType>())
	{
		/* So that widgets will be hidden if not contained inside ConstraintTypes */
		const bool ShouldDisplay = BeatGridConstraints.ConstraintTypes.ContainsByPredicate([&ConstraintType] (const EBeatGridConstraintType& Type)
		{
			if (Type == ConstraintType)
			{
				return true;
			}
			return false;
		});
		
		switch (ConstraintType)
		{
		case EBeatGridConstraintType::NumHorizontalTargets:
			HandleBeatGridWarningDisplay(WarningEMark_NumHorizontalTargets, ConstraintType, BeatGridConstraints.Tooltip_NumHorizontalTargets, ShouldDisplay);
			break;
		case EBeatGridConstraintType::NumVerticalTargets:
			HandleBeatGridWarningDisplay(WarningEMark_NumVerticalTargets, ConstraintType, BeatGridConstraints.Tooltip_NumVerticalTargets, ShouldDisplay);
			break;
		case EBeatGridConstraintType::TargetScale:
			HandleBeatGridWarningDisplay(WarningEMark_MaxTargetScale, ConstraintType, BeatGridConstraints.Tooltip_TargetScale, ShouldDisplay);
			break;
		case EBeatGridConstraintType::HorizontalSpacing:
			HandleBeatGridWarningDisplay(WarningEMark_HorizontalSpacing, ConstraintType, BeatGridConstraints.Tooltip_HorizontalSpacing, ShouldDisplay);
			break;
		case EBeatGridConstraintType::VerticalSpacing:
			HandleBeatGridWarningDisplay(WarningEMark_VerticalSpacing, ConstraintType, BeatGridConstraints.Tooltip_VerticalSpacing, ShouldDisplay);
			break;
		case EBeatGridConstraintType::None: break;
		}
	}
}

TArray<FText> UGameModesWidget_BeatGridConfig::FilterTooltipText(const TArray<FText>& TooltipTextArray, const int32 Index, const bool bShowSelfOnTooltip)
{
	TArray<FText> LastResortArray = TArray<FText>();
	LastResortArray.Add(FText::FromString("- Any parameters with a warning beside them"));
	TArray<FText> NonEmptyTextEntries = TooltipTextArray.FilterByPredicate([] (const FText& Text)
	{
		if (Text.IsEmptyOrWhitespace())
		{
			return false;
		}
		return true;
	});
	if (bShowSelfOnTooltip)
	{
		if (!NonEmptyTextEntries.IsEmpty())
		{
			return NonEmptyTextEntries;
		}
		return LastResortArray;
	}

	const TArray<FText> ExcludingSelfEntries = NonEmptyTextEntries.FilterByPredicate([&TooltipTextArray, &Index] (const FText& Text)
	{
		if (Text.EqualTo(TooltipTextArray[Index]))
		{
			return false;
		}
		return true;
	});
	if (!ExcludingSelfEntries.IsEmpty())
	{
		return ExcludingSelfEntries;
	}
	if (!NonEmptyTextEntries.IsEmpty())
	{
		return NonEmptyTextEntries;
	}
	return LastResortArray;
}

int32 UGameModesWidget_BeatGridConfig::GetMaxAllowedNumHorizontalTargets() const
{
	// HorizontalSpread = MaxTargetSize * NumHorizontalTargets + HorizontalSpacing * (NumHorizontalTargets - 1)
	return (HorizontalSpread + CurrentValues.BeatGridSpacing.X) / (MaxTargetSize + CurrentValues.BeatGridSpacing.X);
}

int32 UGameModesWidget_BeatGridConfig::GetMaxAllowedNumVerticalTargets() const
{
	return (VerticalSpread + CurrentValues.BeatGridSpacing.Y) / (MaxTargetSize + CurrentValues.BeatGridSpacing.Y);
}

float UGameModesWidget_BeatGridConfig::GetMaxAllowedTargetScale() const
{
	const float MaxAllowedHorizontal = (HorizontalSpread - CurrentValues.BeatGridSpacing.X * CurrentValues.NumHorizontalBeatGridTargets + CurrentValues.BeatGridSpacing.X) / (SphereDiameter * CurrentValues.NumHorizontalBeatGridTargets);
	const float MaxAllowedVertical = (VerticalSpread - CurrentValues.BeatGridSpacing.Y * CurrentValues.NumVerticalBeatGridTargets + CurrentValues.BeatGridSpacing.Y) / (SphereDiameter * CurrentValues.NumVerticalBeatGridTargets);
	if (MaxAllowedVertical < MaxAllowedHorizontal)
	{
		return MaxAllowedVertical;
	}
	return MaxAllowedHorizontal;
}

float UGameModesWidget_BeatGridConfig::GetMaxAllowedHorizontalSpacing() const
{
	const float TotalTargetWidth = MaxTargetSize * CurrentValues.NumHorizontalBeatGridTargets;
	return (HorizontalSpread - TotalTargetWidth) / (CurrentValues.NumHorizontalBeatGridTargets - 1);
}

float UGameModesWidget_BeatGridConfig::GetMaxAllowedVerticalSpacing() const
{
	const float TotalTargetHeight = MaxTargetSize * CurrentValues.NumVerticalBeatGridTargets;
	return (VerticalSpread - TotalTargetHeight) / (CurrentValues.NumVerticalBeatGridTargets - 1);
}

float UGameModesWidget_BeatGridConfig::GetMinRequiredHorizontalSpread() const
{
	const float TotalTargetWidth = MaxTargetSize * CurrentValues.NumHorizontalBeatGridTargets;
	return CurrentValues.BeatGridSpacing.X * (CurrentValues.NumHorizontalBeatGridTargets - 1) + TotalTargetWidth;
}

float UGameModesWidget_BeatGridConfig::GetMinRequiredVerticalSpread() const
{
	const float TotalTargetHeight = MaxTargetSize * CurrentValues.NumVerticalBeatGridTargets;
	return CurrentValues.BeatGridSpacing.Y * (CurrentValues.NumVerticalBeatGridTargets - 1) + TotalTargetHeight;
}