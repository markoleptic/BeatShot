// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


// ReSharper disable CppMemberFunctionMayBeConst

#include "WidgetComponents/DoubleSyncedSliderAndTextBox.h"
#include "Components/CheckBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Kismet/KismetStringLibrary.h"

void UDoubleSyncedSliderAndTextBox::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDoubleSyncedSliderAndTextBox::InitConstrainedSlider(const FConstrainedSliderStruct& InStruct)
{
	Checkbox->OnCheckStateChanged.AddDynamic(this, &UDoubleSyncedSliderAndTextBox::OnCheckStateChanged);
	MinSlider->OnValueChanged.AddDynamic(this, &UDoubleSyncedSliderAndTextBox::OnSliderChanged_Min);
	MaxSlider->OnValueChanged.AddDynamic(this, &UDoubleSyncedSliderAndTextBox::OnSliderChanged_Max);
	MinValue->OnTextCommitted.AddDynamic(this, &UDoubleSyncedSliderAndTextBox::OnTextCommitted_Min);
	MaxValue->OnTextCommitted.AddDynamic(this, &UDoubleSyncedSliderAndTextBox::OnTextCommitted_Max);
	
	SliderStruct = InStruct;
	SliderStruct.DefaultMinValue = RoundValue(SliderStruct.DefaultMinValue);
	SliderStruct.DefaultMaxValue = RoundValue(SliderStruct.DefaultMaxValue);
	SliderStruct.MinConstraintLower = RoundValue(SliderStruct.MinConstraintLower);
	SliderStruct.MinConstraintUpper = RoundValue(SliderStruct.MinConstraintUpper);
	SliderStruct.MaxConstraintLower = RoundValue(SliderStruct.MaxConstraintLower);
	SliderStruct.MaxConstraintUpper = RoundValue(SliderStruct.MaxConstraintUpper);

	if (!InStruct.bShowMinLock)
	{
		MinLock->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		MinLock->SetVisibility(ESlateVisibility::Visible);
	}
	if (!InStruct.bShowMaxLock)
	{
		MaxLock->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		MinLock->SetVisibility(ESlateVisibility::Visible);
	}
	MinLock->OnCheckStateChanged.AddDynamic(this, &UDoubleSyncedSliderAndTextBox::OnCheckStateChanged_MinLock);
	MaxLock->OnCheckStateChanged.AddDynamic(this, &UDoubleSyncedSliderAndTextBox::OnCheckStateChanged_MaxLock);
	
	CheckboxText->SetText(SliderStruct.CheckboxText);
	Checkbox->SetIsChecked(SliderStruct.bSyncSlidersAndValues);

	MinText->SetText(SliderStruct.MinText);
	MaxText->SetText(SliderStruct.MaxText);
	MinSlider->SetMinValue(SliderStruct.MinConstraintLower);
	MinSlider->SetMaxValue(SliderStruct.MinConstraintUpper);
	MaxSlider->SetMinValue(SliderStruct.MaxConstraintLower);
	MaxSlider->SetMaxValue(SliderStruct.MaxConstraintUpper);

	MinSlider->SetValue(SliderStruct.DefaultMinValue);
	MaxSlider->SetValue(SliderStruct.DefaultMaxValue);
	MinValue->SetText(FText::AsNumber(SliderStruct.DefaultMinValue));
	MaxValue->SetText(FText::AsNumber(SliderStruct.DefaultMaxValue));
}

void UDoubleSyncedSliderAndTextBox::UpdateDefaultValues(const float NewMinValue, const float NewMaxValue)
{
	SliderStruct.DefaultMinValue = RoundValue(NewMinValue);
	SliderStruct.DefaultMaxValue = RoundValue(NewMaxValue);
	if (SliderStruct.DefaultMinValue == SliderStruct.DefaultMaxValue)
	{
		Checkbox->SetIsChecked(true);
		OnCheckStateChanged(true);
		return;
	}
	Checkbox->SetIsChecked(false);
	OnCheckStateChanged(false);
}

void UDoubleSyncedSliderAndTextBox::OnCheckStateChanged(const bool bIsChecked)
{
	if (bIsChecked)
	{
		SliderStruct.bSyncSlidersAndValues = true;
		
		MaxSlider->SetMaxValue(MinSlider->GetMaxValue());
		
		if (SliderStruct.DefaultMinValue == SliderStruct.DefaultMaxValue)
		{
			MinSlider->SetValue(SliderStruct.DefaultMinValue);
			MaxSlider->SetValue(SliderStruct.DefaultMaxValue);
			OnSliderChanged_Min(SliderStruct.DefaultMinValue);
			OnSliderChanged_Max(SliderStruct.DefaultMaxValue);
		}
		else
		{
			const float MinMidValue = RoundValue((MinSlider->GetMaxValue() + MinSlider->GetMinValue()) / 2);
			const float MaxMidValue = RoundValue((MaxSlider->GetMaxValue() + MaxSlider->GetMinValue()) / 2);
			MinSlider->SetValue(MinMidValue);
			MaxSlider->SetValue(MaxMidValue);
			OnSliderChanged_Min(MinMidValue);
			OnSliderChanged_Max(MaxMidValue);
		}
		return;
	}
	/** Return the sliders to their original default values if the Contrain Checkbox is unchecked */
	SliderStruct.bSyncSlidersAndValues = false;
	MinSlider->SetMaxValue(SliderStruct.MinConstraintUpper);
	MaxSlider->SetMaxValue(SliderStruct.MaxConstraintUpper);
	MinSlider->SetValue(SliderStruct.DefaultMinValue);
	MaxSlider->SetValue(SliderStruct.DefaultMaxValue);
	OnSliderChanged_Min(SliderStruct.DefaultMinValue);
	OnSliderChanged_Max(SliderStruct.DefaultMaxValue);
}

void UDoubleSyncedSliderAndTextBox::OnCheckStateChanged_MinLock(const bool bIsLocked)
{
	OnCheckStateChanged_Min.Broadcast(bIsLocked);
}

void UDoubleSyncedSliderAndTextBox::OnCheckStateChanged_MaxLock(const bool bIsLocked)
{
	OnCheckStateChanged_Max.Broadcast(bIsLocked);
}

void UDoubleSyncedSliderAndTextBox::OnSliderChanged_Min(const float NewMin)
{
	const float NewMinValue = CheckConstraints(NewMin, true);
	MinValue->SetText(FText::AsNumber(NewMinValue));
	
	if (SliderStruct.bSyncSlidersAndValues)
	{
		MaxSlider->SetValue(NewMinValue);
		MaxValue->SetText(FText::AsNumber(NewMinValue));
		OnMaxValueChanged.Broadcast(NewMinValue);
	}
	OnMinValueChanged.Broadcast(NewMinValue);
}

void UDoubleSyncedSliderAndTextBox::OnSliderChanged_Max(const float NewMax)
{
	const float NewMaxValue = CheckConstraints(NewMax, false);
	MaxValue->SetText(FText::AsNumber(NewMaxValue));
	
	if (SliderStruct.bSyncSlidersAndValues)
	{
		MinSlider->SetValue(NewMaxValue);
		MinValue->SetText(FText::AsNumber(NewMaxValue));
		OnMinValueChanged.Broadcast(NewMaxValue);
	}
	OnMaxValueChanged.Broadcast(NewMaxValue);
}

void UDoubleSyncedSliderAndTextBox::OnTextCommitted_Min(const FText& NewMin, ETextCommit::Type CommitType)
{
	const FString NewMinString = UKismetStringLibrary::Replace(NewMin.ToString(), ",", "");
	const float NewMinValue = CheckConstraints(FCString::Atof(*NewMinString), true);
	MinValue->SetText(FText::AsNumber(NewMinValue));
	MinSlider->SetValue(NewMinValue);
	
	if (SliderStruct.bSyncSlidersAndValues)
	{
		MaxSlider->SetValue(NewMinValue);
		MaxValue->SetText(FText::AsNumber(NewMinValue));
		OnMaxValueChanged.Broadcast(NewMinValue);
	}
	OnMinValueChanged.Broadcast(NewMinValue);
}

void UDoubleSyncedSliderAndTextBox::OnTextCommitted_Max(const FText& NewMax, ETextCommit::Type CommitType)
{
	/* Remove commas */
	const FString NewMaxString = UKismetStringLibrary::Replace(NewMax.ToString(), ",", "");
	const float NewMaxValue = CheckConstraints(FCString::Atof(*NewMaxString), false);
	MaxValue->SetText(FText::AsNumber(NewMaxValue));
	MaxSlider->SetValue(NewMaxValue);
	
	if (SliderStruct.bSyncSlidersAndValues)
	{
		MinSlider->SetValue(NewMaxValue);
		MinValue->SetText(FText::AsNumber(NewMaxValue));
		OnMinValueChanged.Broadcast(NewMaxValue);
	}
	OnMaxValueChanged.Broadcast(NewMaxValue);
}

float UDoubleSyncedSliderAndTextBox::CheckConstraints(const float NewValue, const bool bIsMin)
{
	/** Checking constraints for a MinSlider or MinValue */
	if (bIsMin)
	{
		return RoundValue(FMath::Clamp(NewValue, MinSlider->GetMinValue(), MinSlider->GetMaxValue()));
	}
	/** Checking constraints for a MaxSlider or MaxValue */
	return RoundValue(FMath::Clamp(NewValue, MaxSlider->GetMinValue(), MaxSlider->GetMaxValue()));
}

void UDoubleSyncedSliderAndTextBox::OverrideMaxValue(const bool bIsMin, const float ValueToOverride)
{
	if (SliderStruct.bSyncSlidersAndValues)
	{
		if (MinSlider->GetValue() > ValueToOverride)
		{
			MinSlider->SetValue(ValueToOverride);
			MinValue->SetText(FText::AsNumber(ValueToOverride));
			OnMinValueChanged.Broadcast(ValueToOverride);
		}
		if (MaxSlider->GetValue() > ValueToOverride)
		{
			MaxSlider->SetValue(ValueToOverride);
			MaxValue->SetText(FText::AsNumber(ValueToOverride));
			OnMaxValueChanged.Broadcast(ValueToOverride);
		}
		return;
	}

	if (bIsMin)
	{
		if (MinSlider->GetValue() > ValueToOverride)
		{
			MinSlider->SetValue(ValueToOverride);
			MinValue->SetText(FText::AsNumber(ValueToOverride));
			OnMinValueChanged.Broadcast(ValueToOverride);
		}
		return;
	}
	
	if (MaxSlider->GetValue() > ValueToOverride)
	{
		MaxSlider->SetValue(ValueToOverride);
		MaxValue->SetText(FText::AsNumber(ValueToOverride));
		OnMaxValueChanged.Broadcast(ValueToOverride);
	}
}

float UDoubleSyncedSliderAndTextBox::RoundValue(const float ValueToRound) const
{
	return FMath::GridSnap(ValueToRound, SliderStruct.GridSnapSize);
}
