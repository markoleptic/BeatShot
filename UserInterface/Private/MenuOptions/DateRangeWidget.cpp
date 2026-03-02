// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.


#include "MenuOptions/DateRangeWidget.h"
#include "CommonTextBlock.h"
#include "Components/BorderSlot.h"
#include "Components/EditableTextBox.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Slider.h"
#include "Components/Spacer.h"
#include "Styles/MenuOptionStyle.h"
#include "Utilities/BSWidgetInterface.h"

namespace
{
FText CreateEditableTextBoxText(const FDateTime& Value)
{
	return FText::FromString(Value.ToFormattedString(TEXT("%Y-%m-%d")));
}

FString SimplifyWhitespace(const FString& Input)
{
	FString Trimmed = Input;
	Trimmed.TrimStartAndEndInline();

	FString Output;
	Output.Reserve(Trimmed.Len());

	bool bPreviousWasWhitespace = false;

	for (TCHAR Char : Trimmed)
	{
		if (FChar::IsWhitespace(Char))
		{
			if (!bPreviousWasWhitespace)
			{
				Output.AppendChar(TEXT(' '));
				bPreviousWasWhitespace = true;
			}
		}
		else
		{
			Output.AppendChar(Char);
			bPreviousWasWhitespace = false;
		}
	}

	return Output;
}

bool ParseTime(const FString& DateTimeString, FDateTime& OutDateTime)
{
	FString FixedString = DateTimeString.Replace(TEXT("-"), TEXT(" "));
	FixedString.ReplaceInline(TEXT("/"), TEXT(" "), ESearchCase::CaseSensitive);
	FixedString.ReplaceInline(TEXT(":"), TEXT(" "), ESearchCase::CaseSensitive);
	FixedString.ReplaceInline(TEXT("."), TEXT(" "), ESearchCase::CaseSensitive);
	FixedString = SimplifyWhitespace(FixedString);
	TArray<FString> Tokens;
	FixedString.ParseIntoArray(Tokens, TEXT(" "), true);
	if (Tokens.Num() < 3)
	{
		return false;
	}

	const int32 Year = FCString::Atoi(*Tokens[0]);
	const int32 Month = FCString::Atoi(*Tokens[1]);
	const int32 Day = FCString::Atoi(*Tokens[2]);

	if (!FDateTime::Validate(Year, Month, Day, 0, 0, 0, 0))
	{
		return false;
	}

	OutDateTime = FDateTime(Year, Month, Day, 0, 0, 0, 0);

	return true;
}
}

void UDateRangeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MinText = IBSWidgetInterface::GetWidgetTextFromKey("G_Min");
	MaxText = IBSWidgetInterface::GetWidgetTextFromKey("G_Max");

	TextBlock_Min->SetText(MinText);
	TextBlock_Max->SetText(MaxText);

	Slider_Min->OnValueChanged.AddUniqueDynamic(this, &ThisClass::OnSliderChanged_Min);
	Slider_Max->OnValueChanged.AddUniqueDynamic(this, &ThisClass::OnSliderChanged_Max);
	EditableTextBox_Min->OnTextCommitted.AddUniqueDynamic(this, &ThisClass::OnTextCommitted_Min);
	EditableTextBox_Max->OnTextCommitted.AddUniqueDynamic(this, &ThisClass::OnTextCommitted_Max);
}

void UDateRangeWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	MinText = IBSWidgetInterface::GetWidgetTextFromKey("G_Min");
	MaxText = IBSWidgetInterface::GetWidgetTextFromKey("G_Max");

	if (TextBlock_Min)
	{
		TextBlock_Min->SetText(MinText);
	}
	if (TextBlock_Max)
	{
		TextBlock_Max->SetText(MaxText);
	}
}

void UDateRangeWidget::SetStyling()
{
	Super::SetStyling();
	if (MenuOptionStyle)
	{
		if (EditableTextBox_Min)
		{
			EditableTextBox_Min->WidgetStyle.SetFont(MenuOptionStyle->Font_EditableTextBesideSlider);
		}
		if (EditableTextBox_Max)
		{
			EditableTextBox_Max->WidgetStyle.SetFont(MenuOptionStyle->Font_EditableTextBesideSlider);
		}
		if (TextBlock_Min)
		{
			TextBlock_Min->SetFont(MenuOptionStyle->Font_DescriptionText);
		}
		if (TextBlock_Max)
		{
			TextBlock_Max->SetFont(MenuOptionStyle->Font_DescriptionText);
		}
		if (TextBlock_Description_Max)
		{
			TextBlock_Description_Max->SetFont(MenuOptionStyle->Font_DescriptionText);
			UHorizontalBoxSlot* HorizontalBoxSlot = Cast<UHorizontalBoxSlot>(TextBlock_Description_Max->Slot);
			if (HorizontalBoxSlot)
			{
				HorizontalBoxSlot->SetPadding(MenuOptionStyle->Padding_DescriptionText);
			}
		}
		if (Indent_Left_Max)
		{
			Indent_Left_Max->SetSize(FVector2d(IndentLevel * MenuOptionStyle->IndentAmount, 0.f));
		}
		if (Box_Left_Max)
		{
			if (UBorderSlot* BorderSlot = Cast<UBorderSlot>(Box_Left_Max->Slot))
			{
				BorderSlot->SetPadding(MenuOptionStyle->Padding_LeftBox);
			}
		}
		if (Box_Right_Max)
		{
			if (UBorderSlot* BorderSlot = Cast<UBorderSlot>(Box_Right_Max->Slot))
			{
				BorderSlot->SetPadding(MenuOptionStyle->Padding_RightBox);
			}
		}
	}
}

void
UDateRangeWidget::SetMenuOptionEnabledState(const EMenuOptionEnabledState State, const FText& TooltipText)
{
	Super::SetMenuOptionEnabledState(State, TooltipText);
}

FDateTime UDateRangeWidget::GetMinSliderValue(const bool bClamped) const
{
	if (!bClamped)
	{
		return GetDateTimeFromSliderValue(Slider_Min->GetValue());
	}

	const float ClampedValue = FMath::Clamp(Slider_Min->GetValue(), Slider_Min->GetMinValue(),
	                                        Slider_Min->GetMaxValue());
	const float SnappedValue = FMath::GridSnap(ClampedValue, GridSnapSize);
	return GetDateTimeFromSliderValue(SnappedValue);
}

FDateTime UDateRangeWidget::GetMaxSliderValue(const bool bClamped) const
{
	if (!bClamped)
	{
		return GetDateTimeFromSliderValue(Slider_Max->GetValue());
	}

	const float ClampedValue = FMath::Clamp(Slider_Max->GetValue(), Slider_Max->GetMinValue(),
	                                        Slider_Max->GetMaxValue());
	const float SnappedValue = FMath::GridSnap(ClampedValue, GridSnapSize);
	return GetDateTimeFromSliderValue(SnappedValue);
}

TTuple<FDateTime, FDateTime> UDateRangeWidget::GetValues(const bool bClamped) const
{
	return {GetMinSliderValue(bClamped), GetMaxSliderValue(bClamped)};
}

void UDateRangeWidget::SetValues(const FDateTime& Min, const FDateTime& Max, const float SnapSize)
{
	DatesToSliderValues.Empty();
	FDateTime StartDate = Min.GetDate();
	const FDateTime EndDate = Max.GetDate();
	float Value = 0.f;
	while (StartDate <= EndDate)
	{
		DatesToSliderValues.Add(StartDate, Value++);
		StartDate += FTimespan::FromDays(1.f);
	}
	if (!DatesToSliderValues.IsEmpty())
	{
		Value--;
	}

	Slider_Min->SetMinValue(0.f);
	Slider_Max->SetMinValue(0.f);
	Slider_Min->SetMaxValue(Value);
	Slider_Max->SetMaxValue(Value);
	Slider_Min->SetStepSize(SnapSize);
	Slider_Max->SetStepSize(SnapSize);
	GridSnapSize = SnapSize;
	LastValidMinText = FText();
	LastValidMaxText = FText();
}

void UDateRangeWidget::SetValue_Min(const FDateTime& Value) const
{
	Slider_Min->OnValueChanged.RemoveDynamic(this, &ThisClass::OnSliderChanged_Min);

	const float SliderValue = GetSliderValueFromDateTime(Value);
	const float ClampedValue = FMath::Clamp(SliderValue, Slider_Min->GetMinValue(), Slider_Min->GetMaxValue());
	const auto Date = GetDateTimeFromSliderValue(ClampedValue);
	LastValidMinText = CreateEditableTextBoxText(Date);
	EditableTextBox_Min->SetText(LastValidMinText);
	Slider_Min->SetValue(ClampedValue);

	Slider_Min->OnValueChanged.AddUniqueDynamic(this, &ThisClass::OnSliderChanged_Min);
}

void UDateRangeWidget::SetValue_Max(const FDateTime& Value) const
{
	Slider_Max->OnValueChanged.RemoveDynamic(this, &ThisClass::OnSliderChanged_Max);

	const float SliderValue = GetSliderValueFromDateTime(Value);
	const float ClampedValue = FMath::Clamp(SliderValue, Slider_Max->GetMinValue(), Slider_Max->GetMaxValue());
	const auto Date = GetDateTimeFromSliderValue(ClampedValue);
	LastValidMaxText = CreateEditableTextBoxText(Date);
	EditableTextBox_Max->SetText(LastValidMaxText);
	Slider_Max->SetValue(ClampedValue);

	Slider_Max->OnValueChanged.AddUniqueDynamic(this, &ThisClass::OnSliderChanged_Max);
}

void UDateRangeWidget::SetSliderAndTextBoxEnabledStates(const bool bEnabled) const
{
	Slider_Min->SetLocked(!bEnabled);
	Slider_Max->SetLocked(!bEnabled);
	EditableTextBox_Min->SetIsReadOnly(!bEnabled);
	EditableTextBox_Max->SetIsReadOnly(!bEnabled);
}

float UDateRangeWidget::GetSliderValueFromDateTime(const FDateTime& DateTime) const
{
	const FDateTime DateTimeDate = DateTime.GetDate();
	if (DatesToSliderValues.Contains(DateTimeDate))
	{
		return DatesToSliderValues[DateTimeDate];
	}
	return 0.f;
}

FDateTime UDateRangeWidget::GetDateTimeFromSliderValue(const float Value) const
{
	for (const auto& [Date, CurrentValue] : DatesToSliderValues)
	{
		if (FMath::IsNearlyEqual(Value, CurrentValue))
		{
			return Date;
		}
	}
	return FDateTime();
}

void UDateRangeWidget::OnSliderChanged_Min(const float Value)
{
	const float ClampedValue = FMath::GridSnap(Value, GridSnapSize);
	const auto Date = GetDateTimeFromSliderValue(ClampedValue);
	const FText DateText = CreateEditableTextBoxText(Date);
	EditableTextBox_Min->SetText(DateText);

	if (ClampedValue > Slider_Max->GetValue())
	{
		SetValue_Max(ClampedValue);
	}

	OnMinMaxMenuOptionChanged.Execute(Date, GetMaxSliderValue(true));
}

void UDateRangeWidget::OnSliderChanged_Max(const float Value)
{
	const float ClampedValue = FMath::GridSnap(Value, GridSnapSize);
	const auto Date = GetDateTimeFromSliderValue(ClampedValue);
	const FText DateText = CreateEditableTextBoxText(Date);
	EditableTextBox_Max->SetText(DateText);

	if (ClampedValue < Slider_Min->GetValue())
	{
		SetValue_Min(ClampedValue);
	}

	OnMinMaxMenuOptionChanged.Execute(GetMinSliderValue(true), Date);
}

void UDateRangeWidget::OnTextCommitted_Min(const FText& Text, ETextCommit::Type CommitType)
{
	FDateTime DateTime;
	if (ParseTime(Text.ToString(), DateTime) && DatesToSliderValues.Contains(DateTime))
	{
		LastValidMinText = CreateEditableTextBoxText(DateTime);
		EditableTextBox_Min->SetText(LastValidMinText);
		Slider_Min->SetValue(DatesToSliderValues[DateTime]);
		OnMinMaxMenuOptionChanged.Execute(DateTime, GetMaxSliderValue(true));
	}
	else
	{
		EditableTextBox_Min->SetText(LastValidMinText);
	}
}

void UDateRangeWidget::OnTextCommitted_Max(const FText& Text, ETextCommit::Type CommitType)
{
	FDateTime DateTime;
	if (ParseTime(Text.ToString(), DateTime) && DatesToSliderValues.Contains(DateTime))
	{
		LastValidMaxText = CreateEditableTextBoxText(DateTime);
		EditableTextBox_Max->SetText(LastValidMaxText);
		Slider_Max->SetValue(DatesToSliderValues[DateTime]);
		OnMinMaxMenuOptionChanged.Execute(GetMinSliderValue(true), DateTime);
	}
	else
	{
		EditableTextBox_Max->SetText(LastValidMaxText);
	}
}
