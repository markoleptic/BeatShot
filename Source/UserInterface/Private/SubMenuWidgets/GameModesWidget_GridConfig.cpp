// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "SubMenuWidgets/GameModesWidget_GridConfig.h"
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

void UGameModesWidget_GridConfig::InitSettingCategoryWidget()
{
	Super::InitSettingCategoryWidget();
}

void UGameModesWidget_GridConfig::NativeConstruct()
{
	Super::NativeConstruct();
	
	/* BeatGrid Spacing TextBox and Slider */
	FSyncedSlidersParams GridSliderStruct;
	GridSliderStruct.MinConstraintLower = MinValue_HorizontalGridSpacing;
	GridSliderStruct.MinConstraintUpper = MaxValue_HorizontalGridSpacing;
	GridSliderStruct.MaxConstraintLower = MinValue_VerticalGridSpacing;
	GridSliderStruct.MaxConstraintUpper = MaxValue_VerticalGridSpacing;
	GridSliderStruct.DefaultMinValue = MaxValue_HorizontalGridSpacing;
	GridSliderStruct.DefaultMaxValue = MaxValue_VerticalGridSpacing;
	GridSliderStruct.MinText = FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_HorizontalGridSpacing");
	GridSliderStruct.MaxText = FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_VerticalGridSpacing");
	GridSliderStruct.GridSnapSize = SnapSize_VerticalGridSpacing;
	GridSliderStruct.bShowCheckBox = false;
	GridSliderStruct.bShowMaxLock = true;
	GridSliderStruct.bShowMinLock = true;
	GridSliderStruct.bStartMinLocked = true;
	GridSliderStruct.bStartMaxLocked = true;
	GridSliderStruct.bShowMinQMark = true;
	GridSliderStruct.bShowMaxQMark = true;
	GridSliderStruct.bIndentLeftOneLevel = true;
	BeatGridSpreadConstrained->InitConstrainedSlider(GridSliderStruct);

	SetupTooltip(BeatGridSpreadConstrained->GetQMark_Min(), GetTooltipTextFromKey("HorizontalGridSpacing"));
	SetupTooltip(BeatGridSpreadConstrained->GetQMark_Max(), GetTooltipTextFromKey("VerticalGridSpacing"));
	
	BeatGridSpreadConstrained->OnValueChanged_Min.AddUObject(this, &UGameModesWidget_GridConfig::OnConstrainedChanged_HorizontalSpacing);
	BeatGridSpreadConstrained->OnValueChanged_Max.AddUObject(this, &UGameModesWidget_GridConfig::OnConstrainedChanged_VerticalSpacing);
	BeatGridSpreadConstrained->OnCheckStateChanged_Min.AddUObject(this, &UGameModesWidget_GridConfig::OnCheckStateChanged_MinLock);
	BeatGridSpreadConstrained->OnCheckStateChanged_Max.AddUObject(this, &UGameModesWidget_GridConfig::OnCheckStateChanged_MaxLock);

	Slider_NumHorizontalTargets->OnValueChanged.AddDynamic(this, &UGameModesWidget_GridConfig::OnSliderChanged_BeatGridNumHorizontalTargets);
	Slider_NumVerticalTargets->OnValueChanged.AddDynamic(this, &UGameModesWidget_GridConfig::OnSliderChanged_BeatGridNumVerticalTargets);

	Value_NumHorizontalTargets->OnTextCommitted.AddDynamic(this, &UGameModesWidget_GridConfig::OnTextCommitted_BeatGridNumHorizontalTargets);
	Value_NumVerticalTargets->OnTextCommitted.AddDynamic(this, &UGameModesWidget_GridConfig::OnTextCommitted_BeatGridNumVerticalTargets);
	
	Slider_NumHorizontalTargets->SetMinValue(MinValue_NumHorizontalGridTargets);
	Slider_NumHorizontalTargets->SetMaxValue(MaxValue_NumHorizontalGridTargets);
	Slider_NumHorizontalTargets->SetStepSize(SnapSize_NumHorizontalGridTargets);
	
	Slider_NumVerticalTargets->SetMinValue(MinValue_NumVerticalGridTargets);
	Slider_NumVerticalTargets->SetMaxValue(MaxValue_NumVerticalGridTargets);
	Slider_NumVerticalTargets->SetStepSize(SnapSize_NumVerticalGridTargets);
	
	CheckBox_NumHorizontalTargetsLock->OnCheckStateChanged.AddDynamic(this, &UGameModesWidget_GridConfig::OnCheckStateChanged_BeatGridNumHorizontalTargetsLock);
	CheckBox_NumVerticalTargetsLock->OnCheckStateChanged.AddDynamic(this, &UGameModesWidget_GridConfig::OnCheckStateChanged_BeatGridNumVerticalTargetsLock);
	
	bNumHorizontalTargetsLocked = true;
	bNumVerticalTargetsLocked = true;
	bHorizontalSpacingLocked = true;
	bVerticalSpacingLocked = true;
	CheckBox_NumHorizontalTargetsLock->SetIsChecked(true);
	CheckBox_NumVerticalTargetsLock->SetIsChecked(true);
}

TSharedRef<SWidget> UGameModesWidget_GridConfig::RebuildWidget()
{
	/* Create MainContainer before calling Super NativeConstruct since the parent calls InitSettingCategoryWidget in NativeConstruct */
	if (!IsDesignTime())
	{
		BeatGridSpreadConstrained = CreateWidget<UDoubleSyncedSliderAndTextBox>(this, BeatGridSpreadConstrainedClass);
		MainContainer->AddChildToVerticalBox(BeatGridSpreadConstrained.Get());
	}
	
	return Super::RebuildWidget();
}

void UGameModesWidget_GridConfig::InitGridConfig(const FBS_GridConfig& InBeatGridConfig, const UHorizontalBox* InTargetScaleBox)
{
	CurrentValues = InBeatGridConfig;
	TargetScaleBox = InTargetScaleBox;
	Slider_NumHorizontalTargets->SetValue(InBeatGridConfig.NumHorizontalGridTargets);
	Value_NumHorizontalTargets->SetText(FText::AsNumber(InBeatGridConfig.NumHorizontalGridTargets));
	Slider_NumVerticalTargets->SetValue(InBeatGridConfig.NumVerticalGridTargets);
	Value_NumVerticalTargets->SetText(FText::AsNumber(InBeatGridConfig.NumVerticalGridTargets));
	BeatGridSpreadConstrained->UpdateDefaultValues(InBeatGridConfig.GridSpacing.X, InBeatGridConfig.GridSpacing.Y, InBeatGridConfig.GridSpacing.X == InBeatGridConfig.GridSpacing.Y);
	UpdateBrushColors();
}

FBS_GridConfig UGameModesWidget_GridConfig::GetGridConfig() const
{
	FBS_GridConfig ReturnConfig;
	ReturnConfig.GridSpacing = FVector2D(
	FMath::GridSnap(FMath::Clamp(BeatGridSpreadConstrained->GetMinValue(), MinValue_HorizontalGridSpacing, MaxValue_HorizontalGridSpacing), SnapSize_HorizontalGridSpacing),
	FMath::GridSnap(FMath::Clamp(BeatGridSpreadConstrained->GetMaxValue(), MinValue_VerticalGridSpacing, MaxValue_VerticalGridSpacing), SnapSize_VerticalGridSpacing));
	ReturnConfig.NumHorizontalGridTargets = FMath::GridSnap(FMath::Clamp(Slider_NumHorizontalTargets->GetValue(), MinValue_NumHorizontalGridTargets, MaxValue_NumHorizontalGridTargets), SnapSize_NumHorizontalGridTargets);
	ReturnConfig.NumVerticalGridTargets = FMath::GridSnap(FMath::Clamp(Slider_NumVerticalTargets->GetValue(), MinValue_NumVerticalGridTargets, MaxValue_NumVerticalGridTargets), SnapSize_NumVerticalGridTargets);
	ReturnConfig.NumGridTargetsVisibleAtOnce = -1;
	return ReturnConfig;
}

void UGameModesWidget_GridConfig::OnConstrainedChanged_HorizontalSpacing(const float NewHorizontalSpacing)
{
	CurrentValues.GridSpacing.X = NewHorizontalSpacing;
	CheckBeatGridConstraints(EGridConstraintType::HorizontalSpacing);
}

void UGameModesWidget_GridConfig::OnConstrainedChanged_VerticalSpacing(const float NewVerticalSpacing)
{
	CurrentValues.GridSpacing.Y = NewVerticalSpacing;
	CheckBeatGridConstraints(EGridConstraintType::VerticalSpacing);
}

void UGameModesWidget_GridConfig::OnSliderChanged_BeatGridNumHorizontalTargets(const float NewNumHorizontalTargets)
{
	CurrentValues.NumHorizontalGridTargets = OnSliderChanged(NewNumHorizontalTargets, Value_NumHorizontalTargets, SnapSize_NumHorizontalGridTargets);
	CheckBeatGridConstraints(EGridConstraintType::NumHorizontalTargets);
}

void UGameModesWidget_GridConfig::OnSliderChanged_BeatGridNumVerticalTargets(const float NewNumVerticalTargets)
{
	CurrentValues.NumVerticalGridTargets = OnSliderChanged(NewNumVerticalTargets, Value_NumVerticalTargets, SnapSize_NumVerticalGridTargets);
	CheckBeatGridConstraints(EGridConstraintType::NumVerticalTargets);
}

void UGameModesWidget_GridConfig::OnTextCommitted_BeatGridNumHorizontalTargets(const FText& NewNumHorizontalTargets, ETextCommit::Type CommitType)
{
	CurrentValues.NumHorizontalGridTargets = OnEditableTextBoxChanged(NewNumHorizontalTargets, Value_NumHorizontalTargets, Slider_NumHorizontalTargets,
		SnapSize_NumHorizontalGridTargets, MinValue_HorizontalGridSpacing, MaxValue_HorizontalGridSpacing);
	CheckBeatGridConstraints(EGridConstraintType::NumHorizontalTargets);
}

void UGameModesWidget_GridConfig::OnTextCommitted_BeatGridNumVerticalTargets(const FText& NewNumVerticalTargets, ETextCommit::Type CommitType)
{
	CurrentValues.NumVerticalGridTargets = OnEditableTextBoxChanged(NewNumVerticalTargets, Value_NumVerticalTargets, Slider_NumVerticalTargets,
		SnapSize_NumVerticalGridTargets, MinValue_VerticalGridSpacing, MaxValue_VerticalGridSpacing);
	CheckBeatGridConstraints(EGridConstraintType::NumVerticalTargets);
}

void UGameModesWidget_GridConfig::OnCheckStateChanged_BeatGridNumHorizontalTargetsLock(const bool bIsLocked)
{
	bNumHorizontalTargetsLocked = bIsLocked;
	CurrentValues.NumHorizontalGridTargets = Slider_NumHorizontalTargets->GetValue();
	CheckBeatGridConstraints(EGridConstraintType::NumHorizontalTargets);
}

void UGameModesWidget_GridConfig::OnCheckStateChanged_BeatGridNumVerticalTargetsLock(const bool bIsLocked)
{
	bNumVerticalTargetsLocked = bIsLocked;
	CurrentValues.NumVerticalGridTargets = Slider_NumVerticalTargets->GetValue();
	CheckBeatGridConstraints(EGridConstraintType::NumVerticalTargets);
}

void UGameModesWidget_GridConfig::OnCheckStateChanged_MinLock(const bool bIsLocked)
{
	bHorizontalSpacingLocked = bIsLocked;
	if (!bIsLocked)
	{
		BeatGridSpreadConstrained->OverrideMaxValue(true, MaxValue_HorizontalGridSpacing);
	}
	CurrentValues.GridSpacing.X = BeatGridSpreadConstrained->GetMinValue();
	CheckBeatGridConstraints(EGridConstraintType::HorizontalSpacing);
}

void UGameModesWidget_GridConfig::OnCheckStateChanged_MaxLock(const bool bIsLocked)
{
	bVerticalSpacingLocked = bIsLocked;
	if (!bIsLocked)
	{
		BeatGridSpreadConstrained->OverrideMaxValue(false, MaxValue_VerticalGridSpacing);
	}
	CurrentValues.GridSpacing.Y = BeatGridSpreadConstrained->GetMaxValue();
	CheckBeatGridConstraints(EGridConstraintType::VerticalSpacing);
}

void UGameModesWidget_GridConfig::OnBeatGridUpdate_MaxTargetScale(const float NewMaxTargetScale)
{
	MaxTargetSize = NewMaxTargetScale * SphereTargetDiameter;
	HorizontalSpread = MaxValue_HorizontalGridSpacing + MaxTargetSize;
	VerticalSpread = MaxValue_VerticalGridSpacing + MaxTargetSize;
	CheckBeatGridConstraints(EGridConstraintType::TargetScale);
}

FIntPoint UGameModesWidget_GridConfig::GetGridSpacing() const
{
	return FIntPoint(FMath::GridSnap(FMath::Clamp(Slider_NumHorizontalTargets->GetValue(), MinValue_NumHorizontalGridTargets, MaxValue_NumHorizontalGridTargets), SnapSize_NumHorizontalGridTargets),
		FMath::GridSnap(FMath::Clamp(Slider_NumVerticalTargets->GetValue(), MinValue_NumVerticalGridTargets, MaxValue_NumVerticalGridTargets), SnapSize_NumVerticalGridTargets));
}

void UGameModesWidget_GridConfig::CheckBeatGridConstraints(const EGridConstraintType LastChangedConstraint)
{
	FGridConstraints BeatGridConstraints;
	TArray<FText> SuggestionValueArray;
	SuggestionValueArray.Init(FText(), 5);

	if (bHorizontalSpacingLocked && LastChangedConstraint != EGridConstraintType::HorizontalSpacing)
	{
		if (const float Value = floorf(GetMaxAllowedHorizontalSpacing() / 10.f) * 10.f; Value >= MinValue_HorizontalGridSpacing && Value <= MaxValue_HorizontalGridSpacing)
		{
			BeatGridSpreadConstrained->OverrideMaxValue(true, Value);
		}
	}
	if (bVerticalSpacingLocked && LastChangedConstraint != EGridConstraintType::VerticalSpacing)
	{
		if (const float Value = floorf(GetMaxAllowedVerticalSpacing() / 10.f) * 10.f; Value >= MinValue_VerticalGridSpacing && Value <= MaxValue_VerticalGridSpacing)
		{
			BeatGridSpreadConstrained->OverrideMaxValue(false, Value);
		}
	}
	if (bNumHorizontalTargetsLocked && LastChangedConstraint != EGridConstraintType::NumHorizontalTargets)
	{
		if (const int32 Value = GetMaxAllowedNumHorizontalTargets(); Value >= MinValue_NumHorizontalGridTargets && Value <= MaxValue_NumHorizontalGridTargets && Value < Slider_NumHorizontalTargets->GetValue())
		{
			Slider_NumHorizontalTargets->SetValue(Value);
			Value_NumHorizontalTargets->SetText(FText::AsNumber(Value));
			CurrentValues.NumHorizontalGridTargets = Value;
		}
	}
	if (bNumVerticalTargetsLocked && LastChangedConstraint != EGridConstraintType::NumVerticalTargets)
	{
		if (const int32 Value = GetMaxAllowedNumVerticalTargets(); Value >= MinValue_NumVerticalGridTargets && Value <= MaxValue_NumVerticalGridTargets && Value < Slider_NumVerticalTargets->GetValue())
		{
			Slider_NumVerticalTargets->SetValue(Value);
			Value_NumVerticalTargets->SetText(FText::AsNumber(Value));
			CurrentValues.NumVerticalGridTargets = Value;
		}
	}
	
	if (CurrentValues.NumHorizontalGridTargets > GetMaxAllowedNumHorizontalTargets())
	{
		BeatGridConstraints.ConstraintTypes.Add(EGridConstraintType::NumHorizontalTargets);
		if (const int32 Value = GetMaxAllowedNumHorizontalTargets(); Value >= MinValue_NumHorizontalGridTargets && Value <= MaxValue_NumHorizontalGridTargets)
		{
			SuggestionValueArray[0] = FText::Join(SpaceDelimit, HorizontalSuggest, FText::AsNumber(Value));
		}
	}
	if (CurrentValues.NumVerticalGridTargets > GetMaxAllowedNumVerticalTargets())
	{
		BeatGridConstraints.ConstraintTypes.Add(EGridConstraintType::NumVerticalTargets);
		if (const int32 Value = GetMaxAllowedNumVerticalTargets(); Value >= MinValue_NumVerticalGridTargets && Value <= MaxValue_NumVerticalGridTargets)
		{
			SuggestionValueArray[1] = FText::Join(SpaceDelimit, VerticalSuggest,  FText::AsNumber(Value));
		}
	}
	if (MaxTargetSize / SphereTargetDiameter > GetMaxAllowedTargetScale())
	{
		BeatGridConstraints.ConstraintTypes.Add(EGridConstraintType::TargetScale);
		if (const float Value = floorf(GetMaxAllowedTargetScale() * 100.f) / 100.f; Value >= MinValue_TargetScale && Value <= MaxValue_TargetScale)
		{
			SuggestionValueArray[2] = FText::Join(SpaceDelimit, ScaleSuggest, FText::AsNumber(Value));
		}
	}
	if (CurrentValues.GridSpacing.X > GetMaxAllowedHorizontalSpacing())
	{
		BeatGridConstraints.ConstraintTypes.Add(EGridConstraintType::HorizontalSpacing);
		if (const float Value = floorf(GetMaxAllowedHorizontalSpacing() / 10.f) * 10.f; Value >= MinValue_HorizontalGridSpacing && Value <= MaxValue_HorizontalGridSpacing)
		{
			SuggestionValueArray[3] = FText::Join(SpaceDelimit, HorizontalSpacingSuggest, FText::AsNumber(Value));
		}
	}
	if (CurrentValues.GridSpacing.Y > GetMaxAllowedVerticalSpacing())
	{
		BeatGridConstraints.ConstraintTypes.Add(EGridConstraintType::VerticalSpacing);
		if (const float Value = floorf(GetMaxAllowedVerticalSpacing() / 10.f) * 10.f; Value >= MinValue_VerticalGridSpacing && Value <= MaxValue_VerticalGridSpacing)
		{
			SuggestionValueArray[4] = FText::Join(SpaceDelimit, VerticalSpacingSuggest, FText::AsNumber(Value));
		}
	}
	
	for (const EGridConstraintType ConstraintType : BeatGridConstraints.ConstraintTypes)
	{
		FText JoinedValues;
		switch (ConstraintType) {
		case EGridConstraintType::NumHorizontalTargets:
			JoinedValues = FText::Join(NewLineDelimit, FilterTooltipText(SuggestionValueArray, 0, true));
			BeatGridConstraints.Tooltip_NumHorizontalTargets = FText::Join(NewLineDelimit, SuggestStart, JoinedValues);
			break;
		case EGridConstraintType::NumVerticalTargets:
			JoinedValues = FText::Join(NewLineDelimit, FilterTooltipText(SuggestionValueArray, 1, true));
			BeatGridConstraints.Tooltip_NumVerticalTargets = FText::Join(NewLineDelimit, SuggestStart, JoinedValues);
			break;
		case EGridConstraintType::TargetScale:
			JoinedValues = FText::Join(NewLineDelimit, FilterTooltipText(SuggestionValueArray, 2, true));
			BeatGridConstraints.Tooltip_TargetScale = FText::Join(NewLineDelimit, SuggestStart, JoinedValues);
			break;
		case EGridConstraintType::HorizontalSpacing:
			JoinedValues = FText::Join(NewLineDelimit, FilterTooltipText(SuggestionValueArray, 3, true));
			BeatGridConstraints.Tooltip_HorizontalSpacing = FText::Join(NewLineDelimit, SuggestStart, JoinedValues);
			break;
		case EGridConstraintType::VerticalSpacing:
			JoinedValues = FText::Join(NewLineDelimit, FilterTooltipText(SuggestionValueArray, 4, true));
			BeatGridConstraints.Tooltip_VerticalSpacing = FText::Join(NewLineDelimit, SuggestStart, JoinedValues);
			break;
		case EGridConstraintType::None:
			break;
		}
	}
	ActiveConstraints = BeatGridConstraints.ConstraintTypes;
	OnBeatGridUpdate_SaveStartButtonStates.Broadcast();
	UpdateAllWarningTooltips(BeatGridConstraints);
}

UTooltipImage* UGameModesWidget_GridConfig::ConstructOrEditBeatGridWarningEMarkWidget(UTooltipImage* TooltipImage, const EGridConstraintType ConstraintType)
{
	if (!TooltipImage)
	{
		TooltipImage = WidgetTree->ConstructWidget<UTooltipImage>(BeatGridWarningEMarkClass);
	}

	UHorizontalBox* BoxToPlaceIn;
	switch (ConstraintType)
	{
	case EGridConstraintType::NumHorizontalTargets:
		BoxToPlaceIn = Box_NumHorizontalTargetsTextTooltip;
		WarningEMark_NumHorizontalTargets = TooltipImage;
		break;
	case EGridConstraintType::NumVerticalTargets:
		BoxToPlaceIn = Box_NumVerticalTargetsTextTooltip;
		WarningEMark_NumVerticalTargets = TooltipImage;
		break;
	case EGridConstraintType::TargetScale:
		BoxToPlaceIn = TargetScaleBox.Get();
		WarningEMark_MaxTargetScale = TooltipImage;
		break;
	case EGridConstraintType::HorizontalSpacing:
		BoxToPlaceIn = BeatGridSpreadConstrained->GetTextTooltipBox_Min();
		WarningEMark_HorizontalSpacing = TooltipImage;
		break;
	case EGridConstraintType::VerticalSpacing:
		BoxToPlaceIn = BeatGridSpreadConstrained->GetTextTooltipBox_Max();
		WarningEMark_VerticalSpacing = TooltipImage;
		break;
	default:
		BoxToPlaceIn = nullptr;
	}
	UHorizontalBoxSlot* HorizontalBoxSlot = BoxToPlaceIn->AddChildToHorizontalBox(TooltipImage);
	HorizontalBoxSlot->SetHorizontalAlignment(HAlign_Right);
	HorizontalBoxSlot->SetPadding(FMargin(10.f, 0.f, 0.f, 0.f));
	HorizontalBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	return TooltipImage;
}

void UGameModesWidget_GridConfig::HandleBeatGridWarningDisplay(UTooltipImage* TooltipImage, const EGridConstraintType ConstraintType, const FText& TooltipText, const bool bDisplay)
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

void UGameModesWidget_GridConfig::UpdateAllWarningTooltips(const FGridConstraints& BeatGridConstraints)
{
	for (EGridConstraintType ConstraintType : TEnumRange<EGridConstraintType>())
	{
		/* So that widgets will be hidden if not contained inside ConstraintTypes */
		const bool ShouldDisplay = BeatGridConstraints.ConstraintTypes.ContainsByPredicate([&ConstraintType] (const EGridConstraintType& Type)
		{
			if (Type == ConstraintType)
			{
				return true;
			}
			return false;
		});
		
		switch (ConstraintType)
		{
		case EGridConstraintType::NumHorizontalTargets:
			HandleBeatGridWarningDisplay(WarningEMark_NumHorizontalTargets, ConstraintType, BeatGridConstraints.Tooltip_NumHorizontalTargets, ShouldDisplay);
			break;
		case EGridConstraintType::NumVerticalTargets:
			HandleBeatGridWarningDisplay(WarningEMark_NumVerticalTargets, ConstraintType, BeatGridConstraints.Tooltip_NumVerticalTargets, ShouldDisplay);
			break;
		case EGridConstraintType::TargetScale:
			HandleBeatGridWarningDisplay(WarningEMark_MaxTargetScale, ConstraintType, BeatGridConstraints.Tooltip_TargetScale, ShouldDisplay);
			break;
		case EGridConstraintType::HorizontalSpacing:
			HandleBeatGridWarningDisplay(WarningEMark_HorizontalSpacing, ConstraintType, BeatGridConstraints.Tooltip_HorizontalSpacing, ShouldDisplay);
			break;
		case EGridConstraintType::VerticalSpacing:
			HandleBeatGridWarningDisplay(WarningEMark_VerticalSpacing, ConstraintType, BeatGridConstraints.Tooltip_VerticalSpacing, ShouldDisplay);
			break;
		case EGridConstraintType::None:
			break;
		}
	}
}

TArray<FText> UGameModesWidget_GridConfig::FilterTooltipText(const TArray<FText>& TooltipTextArray, const int32 Index, const bool bShowSelfOnTooltip)
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

// Max Allowed Getters

int32 UGameModesWidget_GridConfig::GetMaxAllowedNumHorizontalTargets() const
{
	// HorizontalSpread = MaxTargetSize * NumHorizontalTargets + HorizontalSpacing * (NumHorizontalTargets - 1)
	int32 Value = (HorizontalSpread + CurrentValues.GridSpacing.X) / (MaxTargetSize + CurrentValues.GridSpacing.X);
	return (HorizontalSpread + CurrentValues.GridSpacing.X) / (MaxTargetSize + CurrentValues.GridSpacing.X);
}

int32 UGameModesWidget_GridConfig::GetMaxAllowedNumVerticalTargets() const
{
	return (VerticalSpread + CurrentValues.GridSpacing.Y) / (MaxTargetSize + CurrentValues.GridSpacing.Y);
}

float UGameModesWidget_GridConfig::GetMaxAllowedTargetScale() const
{
	const float MaxAllowedHorizontal = (HorizontalSpread - CurrentValues.GridSpacing.X * CurrentValues.NumHorizontalGridTargets + CurrentValues.GridSpacing.X) / (SphereTargetDiameter * CurrentValues.NumHorizontalGridTargets);
	const float MaxAllowedVertical = (VerticalSpread - CurrentValues.GridSpacing.Y * CurrentValues.NumVerticalGridTargets + CurrentValues.GridSpacing.Y) / (SphereTargetDiameter * CurrentValues.NumVerticalGridTargets);
	if (MaxAllowedVertical < MaxAllowedHorizontal)
	{
		return MaxAllowedVertical;
	}
	return MaxAllowedHorizontal;
}

float UGameModesWidget_GridConfig::GetMaxAllowedHorizontalSpacing() const
{
	const float TotalTargetWidth = MaxTargetSize * CurrentValues.NumHorizontalGridTargets;
	return (HorizontalSpread - TotalTargetWidth) / (CurrentValues.NumHorizontalGridTargets - 1);
}

float UGameModesWidget_GridConfig::GetMaxAllowedVerticalSpacing() const
{
	const float TotalTargetHeight = MaxTargetSize * CurrentValues.NumVerticalGridTargets;
	return (VerticalSpread - TotalTargetHeight) / (CurrentValues.NumVerticalGridTargets - 1);
}

float UGameModesWidget_GridConfig::GetMinRequiredHorizontalSpread() const
{
	const float TotalTargetWidth = MaxTargetSize * CurrentValues.NumHorizontalGridTargets;
	return CurrentValues.GridSpacing.X * (CurrentValues.NumHorizontalGridTargets - 1) + TotalTargetWidth;
}

float UGameModesWidget_GridConfig::GetMinRequiredVerticalSpread() const
{
	const float TotalTargetHeight = MaxTargetSize * CurrentValues.NumVerticalGridTargets;
	return CurrentValues.GridSpacing.Y * (CurrentValues.NumVerticalGridTargets - 1) + TotalTargetHeight;
}