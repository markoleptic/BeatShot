// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "SubMenuWidgets/GameModesWidget_BeatGridConfig.h"
#include "WidgetComponents/DoubleSyncedSliderAndTextBox.h"
#include "GlobalConstants.h"
#include "BSWidgetInterface.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Slider.h"
#include "WidgetComponents/BSVerticalBox.h"

using namespace Constants;

void UGameModesWidget_BeatGridConfig::InitSettingCategoryWidget()
{
	Super::InitSettingCategoryWidget();
}

void UGameModesWidget_BeatGridConfig::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetupTooltip(BeatGridSpreadConstrained->GetCheckBoxQMark(), GetTooltipTextFromKey("BeatGridEvenSpacing"));
	SetupTooltip(QMark_BeatGridAdjacentOnly, GetTooltipTextFromKey("BeatGridAdjacentOnly"));
	
	/* BeatGrid Spacing TextBox and Slider */
	FSyncedSlidersParams BeatGridSliderStruct;
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
	BeatGridSliderStruct.bStartMinLocked = true;
	BeatGridSliderStruct.bStartMaxLocked = true;
	BeatGridSpreadConstrained->InitConstrainedSlider(BeatGridSliderStruct);

	TooltipWarningSizing.SizeRule = ESlateSizeRule::Fill;
	
	BeatGridSpreadConstrained->OnValueChanged_Min.AddUObject(this, &UGameModesWidget_BeatGridConfig::OnConstrainedChanged_HorizontalSpacing);
	BeatGridSpreadConstrained->OnValueChanged_Max.AddUObject(this, &UGameModesWidget_BeatGridConfig::OnConstrainedChanged_VerticalSpacing);
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
}

TSharedRef<SWidget> UGameModesWidget_BeatGridConfig::RebuildWidget()
{
	/* Create MainContainer before calling Super NativeConstruct since the parent calls InitSettingCategoryWidget in NativeConstruct */
	if (!IsDesignTime())
	{
		BeatGridSpreadConstrained = CreateWidget<UDoubleSyncedSliderAndTextBox>(this, BeatGridSpreadConstrainedClass);
		MainContainer->AddChildToVerticalBox(BeatGridSpreadConstrained.Get());
	}
	
	return Super::RebuildWidget();
}

void UGameModesWidget_BeatGridConfig::InitializeBeatGrid(const FBS_GridConfig& InBeatGridConfig, const UHorizontalBox* InTargetScaleBox)
{
	CurrentValues = InBeatGridConfig;
	TargetScaleBox = InTargetScaleBox;
	CheckBox_RandomizeNextTarget->SetIsChecked(InBeatGridConfig.bRandomizeGridTargetActivation);
	Slider_NumHorizontalTargets->SetValue(InBeatGridConfig.NumHorizontalGridTargets);
	Slider_NumHorizontalTargets->SetMinValue(MinValue_NumBeatGridHorizontalTargets);
	Slider_NumHorizontalTargets->SetMaxValue(MaxValue_NumBeatGridHorizontalTargets);
	Value_NumHorizontalTargets->SetText(FText::AsNumber(InBeatGridConfig.NumHorizontalGridTargets));
	Slider_NumVerticalTargets->SetValue(InBeatGridConfig.NumVerticalGridTargets);
	Slider_NumVerticalTargets->SetMinValue(MinValue_NumBeatGridVerticalTargets);
	Slider_NumVerticalTargets->SetMaxValue(MaxValue_NumBeatGridVerticalTargets);
	Value_NumVerticalTargets->SetText(FText::AsNumber(InBeatGridConfig.NumVerticalGridTargets));
	BeatGridSpreadConstrained->UpdateDefaultValues(InBeatGridConfig.GridSpacing.X, InBeatGridConfig.GridSpacing.Y, InBeatGridConfig.GridSpacing.X == InBeatGridConfig.GridSpacing.Y);
	UpdateBrushColors();
}

FBS_GridConfig UGameModesWidget_BeatGridConfig::GetBeatGridConfig() const
{
	FBS_GridConfig ReturnConfig;
	ReturnConfig.GridSpacing = FVector2D(
	FMath::GridSnap(FMath::Clamp(BeatGridSpreadConstrained->GetMinValue(), MinValue_BeatGridHorizontalSpacing, MaxValue_BeatGridHorizontalSpacing), SnapSize_BeatGridHorizontalSpacing),
	FMath::GridSnap(FMath::Clamp(BeatGridSpreadConstrained->GetMaxValue(), MinValue_BeatGridVerticalSpacing, MaxValue_BeatGridVerticalSpacing), SnapSize_BeatGridVerticalSpacing));
	ReturnConfig.NumHorizontalGridTargets = FMath::GridSnap(FMath::Clamp(Slider_NumHorizontalTargets->GetValue(), MinValue_NumBeatGridHorizontalTargets, MaxValue_NumBeatGridHorizontalTargets), SnapSize_NumBeatGridHorizontalTargets);
	ReturnConfig.NumVerticalGridTargets = FMath::GridSnap(FMath::Clamp(Slider_NumVerticalTargets->GetValue(), MinValue_NumBeatGridVerticalTargets, MaxValue_NumBeatGridVerticalTargets), SnapSize_NumBeatGridVerticalTargets);
	ReturnConfig.bRandomizeGridTargetActivation = CheckBox_RandomizeNextTarget->IsChecked();
	ReturnConfig.NumGridTargetsVisibleAtOnce = -1;
	return ReturnConfig;
}

void UGameModesWidget_BeatGridConfig::OnConstrainedChanged_HorizontalSpacing(const float NewHorizontalSpacing)
{
	CurrentValues.GridSpacing.X = NewHorizontalSpacing;
	CheckBeatGridConstraints(EBeatGridConstraintType::HorizontalSpacing);
}

void UGameModesWidget_BeatGridConfig::OnConstrainedChanged_VerticalSpacing(const float NewVerticalSpacing)
{
	CurrentValues.GridSpacing.Y = NewVerticalSpacing;
	CheckBeatGridConstraints(EBeatGridConstraintType::VerticalSpacing);
}

void UGameModesWidget_BeatGridConfig::OnSliderChanged_BeatGridNumHorizontalTargets(const float NewNumHorizontalTargets)
{
	CurrentValues.NumHorizontalGridTargets = OnSliderChanged(NewNumHorizontalTargets, Value_NumHorizontalTargets, SnapSize_NumBeatGridHorizontalTargets);
	CheckBeatGridConstraints(EBeatGridConstraintType::NumHorizontalTargets);
}

void UGameModesWidget_BeatGridConfig::OnSliderChanged_BeatGridNumVerticalTargets(const float NewNumVerticalTargets)
{
	CurrentValues.NumVerticalGridTargets = OnSliderChanged(NewNumVerticalTargets, Value_NumVerticalTargets, SnapSize_NumBeatGridVerticalTargets);
	CheckBeatGridConstraints(EBeatGridConstraintType::NumVerticalTargets);
}

void UGameModesWidget_BeatGridConfig::OnTextCommitted_BeatGridNumHorizontalTargets(const FText& NewNumHorizontalTargets, ETextCommit::Type CommitType)
{
	CurrentValues.NumHorizontalGridTargets = OnEditableTextBoxChanged(NewNumHorizontalTargets, Value_NumHorizontalTargets, Slider_NumHorizontalTargets,
		SnapSize_NumBeatGridHorizontalTargets, MinValue_BeatGridHorizontalSpacing, MaxValue_BeatGridHorizontalSpacing);
	CheckBeatGridConstraints(EBeatGridConstraintType::NumHorizontalTargets);
}

void UGameModesWidget_BeatGridConfig::OnTextCommitted_BeatGridNumVerticalTargets(const FText& NewNumVerticalTargets, ETextCommit::Type CommitType)
{
	CurrentValues.NumVerticalGridTargets = OnEditableTextBoxChanged(NewNumVerticalTargets, Value_NumVerticalTargets, Slider_NumVerticalTargets,
		SnapSize_NumBeatGridVerticalTargets, MinValue_BeatGridVerticalSpacing, MaxValue_BeatGridVerticalSpacing);
	CheckBeatGridConstraints(EBeatGridConstraintType::NumVerticalTargets);
}

void UGameModesWidget_BeatGridConfig::OnCheckStateChanged_BeatGridNumHorizontalTargetsLock(const bool bIsLocked)
{
	bNumHorizontalTargetsLocked = bIsLocked;
	CurrentValues.NumHorizontalGridTargets = Slider_NumHorizontalTargets->GetValue();
	CheckBeatGridConstraints(EBeatGridConstraintType::NumHorizontalTargets);
}

void UGameModesWidget_BeatGridConfig::OnCheckStateChanged_BeatGridNumVerticalTargetsLock(const bool bIsLocked)
{
	bNumVerticalTargetsLocked = bIsLocked;
	CurrentValues.NumVerticalGridTargets = Slider_NumVerticalTargets->GetValue();
	CheckBeatGridConstraints(EBeatGridConstraintType::NumVerticalTargets);
}

void UGameModesWidget_BeatGridConfig::OnCheckStateChanged_RandomizeNextTarget(const bool bIsChecked)
{
	CurrentValues.bRandomizeGridTargetActivation = bIsChecked;
}

void UGameModesWidget_BeatGridConfig::OnCheckStateChanged_MinLock(const bool bIsLocked)
{
	bHorizontalSpacingLocked = bIsLocked;
	if (!bIsLocked)
	{
		BeatGridSpreadConstrained->OverrideMaxValue(true, MaxValue_BeatGridHorizontalSpacing);
	}
	CurrentValues.GridSpacing.X = BeatGridSpreadConstrained->GetMinValue();
	CheckBeatGridConstraints(EBeatGridConstraintType::HorizontalSpacing);
}

void UGameModesWidget_BeatGridConfig::OnCheckStateChanged_MaxLock(const bool bIsLocked)
{
	bVerticalSpacingLocked = bIsLocked;
	if (!bIsLocked)
	{
		BeatGridSpreadConstrained->OverrideMaxValue(false, MaxValue_BeatGridVerticalSpacing);
	}
	CurrentValues.GridSpacing.Y = BeatGridSpreadConstrained->GetMaxValue();
	CheckBeatGridConstraints(EBeatGridConstraintType::VerticalSpacing);
}

void UGameModesWidget_BeatGridConfig::OnBeatGridUpdate_MaxTargetScale(const float NewMaxTargetScale)
{
	MaxTargetSize = NewMaxTargetScale * SphereTargetDiameter;
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
			CurrentValues.NumHorizontalGridTargets = Value;
		}
	}
	if (bNumVerticalTargetsLocked && LastChangedConstraint != EBeatGridConstraintType::NumVerticalTargets)
	{
		if (const int32 Value = GetMaxAllowedNumVerticalTargets(); Value >= MinValue_NumBeatGridVerticalTargets && Value <= MaxValue_NumBeatGridVerticalTargets && Value < Slider_NumVerticalTargets->GetValue())
		{
			Slider_NumVerticalTargets->SetValue(Value);
			Value_NumVerticalTargets->SetText(FText::AsNumber(Value));
			CurrentValues.NumVerticalGridTargets = Value;
		}
	}
	
	if (CurrentValues.NumHorizontalGridTargets > GetMaxAllowedNumHorizontalTargets())
	{
		BeatGridConstraints.ConstraintTypes.Add(EBeatGridConstraintType::NumHorizontalTargets);
		if (const int32 Value = GetMaxAllowedNumHorizontalTargets(); Value >= MinValue_NumBeatGridHorizontalTargets && Value <= MaxValue_NumBeatGridHorizontalTargets)
		{
			SuggestionValueArray[0] = FText::Join(SpaceDelimit, HorizontalSuggest, FText::AsNumber(Value));
		}
	}
	if (CurrentValues.NumVerticalGridTargets > GetMaxAllowedNumVerticalTargets())
	{
		BeatGridConstraints.ConstraintTypes.Add(EBeatGridConstraintType::NumVerticalTargets);
		if (const int32 Value = GetMaxAllowedNumVerticalTargets(); Value >= MinValue_NumBeatGridVerticalTargets && Value <= MaxValue_NumBeatGridVerticalTargets)
		{
			SuggestionValueArray[1] = FText::Join(SpaceDelimit, VerticalSuggest,  FText::AsNumber(Value));
		}
	}
	if (MaxTargetSize / SphereTargetDiameter > GetMaxAllowedTargetScale())
	{
		BeatGridConstraints.ConstraintTypes.Add(EBeatGridConstraintType::TargetScale);
		if (const float Value = floorf(GetMaxAllowedTargetScale() * 100.f) / 100.f; Value >= MinValue_TargetScale && Value <= MaxValue_TargetScale)
		{
			SuggestionValueArray[2] = FText::Join(SpaceDelimit, ScaleSuggest, FText::AsNumber(Value));
		}
	}
	if (CurrentValues.GridSpacing.X > GetMaxAllowedHorizontalSpacing())
	{
		BeatGridConstraints.ConstraintTypes.Add(EBeatGridConstraintType::HorizontalSpacing);
		if (const float Value = floorf(GetMaxAllowedHorizontalSpacing() / 10.f) * 10.f; Value >= MinValue_BeatGridHorizontalSpacing && Value <= MaxValue_BeatGridHorizontalSpacing)
		{
			SuggestionValueArray[3] = FText::Join(SpaceDelimit, HorizontalSpacingSuggest, FText::AsNumber(Value));
		}
	}
	if (CurrentValues.GridSpacing.Y > GetMaxAllowedVerticalSpacing())
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
			JoinedValues = FText::Join(NewLineDelimit, FilterTooltipText(SuggestionValueArray, 0, true));
			BeatGridConstraints.Tooltip_NumHorizontalTargets = FText::Join(NewLineDelimit, SuggestStart, JoinedValues);
			break;
		case EBeatGridConstraintType::NumVerticalTargets:
			JoinedValues = FText::Join(NewLineDelimit, FilterTooltipText(SuggestionValueArray, 1, true));
			BeatGridConstraints.Tooltip_NumVerticalTargets = FText::Join(NewLineDelimit, SuggestStart, JoinedValues);
			break;
		case EBeatGridConstraintType::TargetScale:
			JoinedValues = FText::Join(NewLineDelimit, FilterTooltipText(SuggestionValueArray, 2, true));
			BeatGridConstraints.Tooltip_TargetScale = FText::Join(NewLineDelimit, SuggestStart, JoinedValues);
			break;
		case EBeatGridConstraintType::HorizontalSpacing:
			JoinedValues = FText::Join(NewLineDelimit, FilterTooltipText(SuggestionValueArray, 3, true));
			BeatGridConstraints.Tooltip_HorizontalSpacing = FText::Join(NewLineDelimit, SuggestStart, JoinedValues);
			break;
		case EBeatGridConstraintType::VerticalSpacing:
			JoinedValues = FText::Join(NewLineDelimit, FilterTooltipText(SuggestionValueArray, 4, true));
			BeatGridConstraints.Tooltip_VerticalSpacing = FText::Join(NewLineDelimit, SuggestStart, JoinedValues);
			break;
		case EBeatGridConstraintType::None:
			break;
		}
	}
	ActiveConstraints = BeatGridConstraints.ConstraintTypes;
	OnBeatGridUpdate_SaveStartButtonStates.Broadcast();
	UpdateAllWarningTooltips(BeatGridConstraints);
}

UTooltipImage* UGameModesWidget_BeatGridConfig::ConstructOrEditBeatGridWarningEMarkWidget(UTooltipImage* TooltipImage, const EBeatGridConstraintType ConstraintType)
{
	if (!TooltipImage)
	{
		TooltipImage = WidgetTree->ConstructWidget<UTooltipImage>(BeatGridWarningEMarkClass);
	}

	UHorizontalBox* BoxToPlaceIn;
	switch (ConstraintType)
	{
	case EBeatGridConstraintType::NumHorizontalTargets:
		BoxToPlaceIn = Box_NumHorizontalTargetsTextTooltip;
		WarningEMark_NumHorizontalTargets = TooltipImage;
		break;
	case EBeatGridConstraintType::NumVerticalTargets:
		BoxToPlaceIn = Box_NumVerticalTargetsTextTooltip;
		WarningEMark_NumVerticalTargets = TooltipImage;
		break;
	case EBeatGridConstraintType::TargetScale:
		BoxToPlaceIn = TargetScaleBox.Get();
		WarningEMark_MaxTargetScale = TooltipImage;
		break;
	case EBeatGridConstraintType::HorizontalSpacing:
		BoxToPlaceIn = BeatGridSpreadConstrained->GetTextTooltipBox_Min();
		WarningEMark_HorizontalSpacing = TooltipImage;
		break;
	case EBeatGridConstraintType::VerticalSpacing:
		BoxToPlaceIn = BeatGridSpreadConstrained->GetTextTooltipBox_Max();
		WarningEMark_VerticalSpacing = TooltipImage;
		break;
	default:
		BoxToPlaceIn = nullptr;
	}
	UHorizontalBoxSlot* HorizontalBoxSlot = BoxToPlaceIn->AddChildToHorizontalBox(TooltipImage);
	HorizontalBoxSlot->SetHorizontalAlignment(HAlign_Right);
	HorizontalBoxSlot->SetSize(TooltipWarningSizing);
	return TooltipImage;
}

void UGameModesWidget_BeatGridConfig::HandleBeatGridWarningDisplay(UTooltipImage* TooltipImage, const EBeatGridConstraintType ConstraintType, const FText& TooltipText, const bool bDisplay)
{
	if (bDisplay)
	{
		if (!TooltipImage)
		{
			SetupTooltip(ConstructOrEditBeatGridWarningEMarkWidget(TooltipImage, ConstraintType), TooltipText, true);
		}
		else
		{
			UpdateTooltip(TooltipImage, TooltipText);
			TooltipImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
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
		case EBeatGridConstraintType::None:
			break;
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
	int32 Value = (HorizontalSpread + CurrentValues.GridSpacing.X) / (MaxTargetSize + CurrentValues.GridSpacing.X);
	return (HorizontalSpread + CurrentValues.GridSpacing.X) / (MaxTargetSize + CurrentValues.GridSpacing.X);
}

int32 UGameModesWidget_BeatGridConfig::GetMaxAllowedNumVerticalTargets() const
{
	return (VerticalSpread + CurrentValues.GridSpacing.Y) / (MaxTargetSize + CurrentValues.GridSpacing.Y);
}

float UGameModesWidget_BeatGridConfig::GetMaxAllowedTargetScale() const
{
	const float MaxAllowedHorizontal = (HorizontalSpread - CurrentValues.GridSpacing.X * CurrentValues.NumHorizontalGridTargets + CurrentValues.GridSpacing.X) / (SphereTargetDiameter * CurrentValues.NumHorizontalGridTargets);
	const float MaxAllowedVertical = (VerticalSpread - CurrentValues.GridSpacing.Y * CurrentValues.NumVerticalGridTargets + CurrentValues.GridSpacing.Y) / (SphereTargetDiameter * CurrentValues.NumVerticalGridTargets);
	if (MaxAllowedVertical < MaxAllowedHorizontal)
	{
		return MaxAllowedVertical;
	}
	return MaxAllowedHorizontal;
}

float UGameModesWidget_BeatGridConfig::GetMaxAllowedHorizontalSpacing() const
{
	const float TotalTargetWidth = MaxTargetSize * CurrentValues.NumHorizontalGridTargets;
	return (HorizontalSpread - TotalTargetWidth) / (CurrentValues.NumHorizontalGridTargets - 1);
}

float UGameModesWidget_BeatGridConfig::GetMaxAllowedVerticalSpacing() const
{
	const float TotalTargetHeight = MaxTargetSize * CurrentValues.NumVerticalGridTargets;
	return (VerticalSpread - TotalTargetHeight) / (CurrentValues.NumVerticalGridTargets - 1);
}

float UGameModesWidget_BeatGridConfig::GetMinRequiredHorizontalSpread() const
{
	const float TotalTargetWidth = MaxTargetSize * CurrentValues.NumHorizontalGridTargets;
	return CurrentValues.GridSpacing.X * (CurrentValues.NumHorizontalGridTargets - 1) + TotalTargetWidth;
}

float UGameModesWidget_BeatGridConfig::GetMinRequiredVerticalSpread() const
{
	const float TotalTargetHeight = MaxTargetSize * CurrentValues.NumVerticalGridTargets;
	return CurrentValues.GridSpacing.Y * (CurrentValues.NumVerticalGridTargets - 1) + TotalTargetHeight;
}