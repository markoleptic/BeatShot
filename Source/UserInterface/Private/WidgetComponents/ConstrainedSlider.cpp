// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


// ReSharper disable CppMemberFunctionMayBeConst

#include "WidgetComponents/ConstrainedSlider.h"
#include "Components/CheckBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Kismet/KismetStringLibrary.h"

void UConstrainedSlider::NativeConstruct()
{
	Super::NativeConstruct();
}

void UConstrainedSlider::InitConstrainedSlider(const FConstrainedSliderStruct& InStruct)
{
	Checkbox->OnCheckStateChanged.AddDynamic(this, &UConstrainedSlider::OnCheckStateChanged);
	MinSlider->OnValueChanged.AddDynamic(this, &UConstrainedSlider::OnMinSliderChanged);
	MaxSlider->OnValueChanged.AddDynamic(this, &UConstrainedSlider::OnMaxSliderChanged);
	MinValue->OnTextCommitted.AddDynamic(this, &UConstrainedSlider::OnMinValueCommitted);
	MaxValue->OnTextCommitted.AddDynamic(this, &UConstrainedSlider::OnMaxValueCommitted);
	
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
	MinLock->OnCheckStateChanged.AddDynamic(this, &UConstrainedSlider::OnCheckStateChanged_MinLock);
	MaxLock->OnCheckStateChanged.AddDynamic(this, &UConstrainedSlider::OnCheckStateChanged_MaxLock);
	
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

void UConstrainedSlider::UpdateDefaultValues(const float NewMinValue, const float NewMaxValue)
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

void UConstrainedSlider::OnCheckStateChanged(const bool bIsChecked)
{
	if (bIsChecked)
	{
		SliderStruct.bSyncSlidersAndValues = true;
		if (SliderStruct.DefaultMinValue == SliderStruct.DefaultMaxValue)
		{
			MinSlider->SetValue(SliderStruct.DefaultMinValue);
			MaxSlider->SetValue(SliderStruct.DefaultMaxValue);
			OnMinSliderChanged(SliderStruct.DefaultMinValue);
			OnMaxSliderChanged(SliderStruct.DefaultMaxValue);
		}
		else
		{
			const float MinMidValue = RoundValue((MinSlider->GetMaxValue() + MinSlider->GetMinValue()) / 2);
			const float MaxMidValue = RoundValue((MaxSlider->GetMaxValue() + MaxSlider->GetMinValue()) / 2);
			MinSlider->SetValue(MinMidValue);
			MaxSlider->SetValue(MaxMidValue);
			OnMinSliderChanged(MinMidValue);
			OnMaxSliderChanged(MaxMidValue);
		}
		return;
	}
	/** Return the sliders to their original default values if the Contrain Checkbox is unchecked */
	SliderStruct.bSyncSlidersAndValues = false;
	MinSlider->SetValue(SliderStruct.DefaultMinValue);
	MaxSlider->SetValue(SliderStruct.DefaultMaxValue);
	OnMinSliderChanged(SliderStruct.DefaultMinValue);
	OnMaxSliderChanged(SliderStruct.DefaultMaxValue);
}

void UConstrainedSlider::OnCheckStateChanged_MinLock(const bool bIsLocked)
{
	MinSlider->SetLocked(bIsLocked);
	MinValue->SetIsReadOnly(bIsLocked);
	OnMinLockStateChanged.Broadcast(bIsLocked, RoundValue(MinSlider->GetValue()));
}

void UConstrainedSlider::OnCheckStateChanged_MaxLock(const bool bIsLocked)
{
	MaxSlider->SetLocked(bIsLocked);
	MaxValue->SetIsReadOnly(bIsLocked);
	OnMaxLockStateChanged.Broadcast(bIsLocked, RoundValue(MaxSlider->GetValue()));
}

void UConstrainedSlider::OnMinSliderChanged(const float NewMin)
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

void UConstrainedSlider::OnMaxSliderChanged(const float NewMax)
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

void UConstrainedSlider::OnMinValueCommitted(const FText& NewMin, ETextCommit::Type CommitType)
{
	const FString NewMinString = UKismetStringLibrary::Replace(NewMin.ToString(), ",", "");
	const float NewMinValue = CheckConstraints(FCString::Atof(*NewMinString), true);
	MinSlider->SetValue(NewMinValue);
	OnMinSliderChanged(NewMinValue);
}

void UConstrainedSlider::OnMaxValueCommitted(const FText& NewMax, ETextCommit::Type CommitType)
{
	const FString NewMaxString = UKismetStringLibrary::Replace(NewMax.ToString(), ",", "");
	const float NewMaxValue = CheckConstraints(FCString::Atof(*NewMaxString), false);
	MaxSlider->SetValue(NewMaxValue);
	OnMaxSliderChanged(NewMaxValue);
}

float UConstrainedSlider::CheckConstraints(const float NewValue, const bool bIsMin)
{
	/** Checking constraints for a MinSlider or MinValue */
	if (bIsMin)
	{
		return RoundValue(FMath::Clamp(NewValue, MinSlider->GetMinValue(), MinSlider->GetMaxValue()));
	}
	/** Checking constraints for a MaxSlider or MaxValue */
	return RoundValue(FMath::Clamp(NewValue, MaxSlider->GetMinValue(), MaxSlider->GetMaxValue()));
}

float UConstrainedSlider::RoundValue(const float ValueToRound) const
{
	return FMath::GridSnap(ValueToRound, SliderStruct.GridSnapSize);
}
