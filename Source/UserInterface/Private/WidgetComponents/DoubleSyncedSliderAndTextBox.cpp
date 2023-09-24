// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


// ReSharper disable CppMemberFunctionMayBeConst

#include "WidgetComponents/DoubleSyncedSliderAndTextBox.h"
#include "Components/CheckBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Spacer.h"
#include "Kismet/KismetStringLibrary.h"
#include "WidgetComponents/Boxes/BSHorizontalBox.h"

float UDoubleSyncedSliderAndTextBox::GetMinValue() const
{
	return Slider_Min->GetValue();
}

float UDoubleSyncedSliderAndTextBox::GetMaxValue() const
{
	return Slider_Max->GetValue();
}

bool UDoubleSyncedSliderAndTextBox::GetIsSynced() const
{
	if (SliderStruct.bLocksOnlySync)
	{
		return SliderStruct.bSyncSlidersAndValues;
	}
	return Checkbox_SyncSlidersAndValues->IsChecked();
}

void UDoubleSyncedSliderAndTextBox::SetLocksEnabled(const bool bEnableLocks)
{
	Checkbox_MinLock->SetIsEnabled(bEnableLocks);
	Checkbox_MaxLock->SetIsEnabled(bEnableLocks);
}

void UDoubleSyncedSliderAndTextBox::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDoubleSyncedSliderAndTextBox::InitConstrainedSlider(const FSyncedSlidersParams& InParams)
{
	Checkbox_SyncSlidersAndValues->OnCheckStateChanged.AddUniqueDynamic(this, &UDoubleSyncedSliderAndTextBox::OnCheckStateChanged_SyncSlidersAndValues);
	Slider_Min->OnValueChanged.AddUniqueDynamic(this, &UDoubleSyncedSliderAndTextBox::OnSliderChanged_Min);
	Slider_Max->OnValueChanged.AddUniqueDynamic(this, &UDoubleSyncedSliderAndTextBox::OnSliderChanged_Max);
	Value_Min->OnTextCommitted.AddUniqueDynamic(this, &UDoubleSyncedSliderAndTextBox::OnTextCommitted_Min);
	Value_Max->OnTextCommitted.AddUniqueDynamic(this, &UDoubleSyncedSliderAndTextBox::OnTextCommitted_Max);
	
	SliderStruct = InParams;
	SliderStruct.DefaultMinValue = RoundValue(SliderStruct.DefaultMinValue);
	SliderStruct.DefaultMaxValue = RoundValue(SliderStruct.DefaultMaxValue);
	SliderStruct.MinConstraintLower = RoundValue(SliderStruct.MinConstraintLower);
	SliderStruct.MinConstraintUpper = RoundValue(SliderStruct.MinConstraintUpper);
	SliderStruct.MaxConstraintLower = RoundValue(SliderStruct.MaxConstraintLower);
	SliderStruct.MaxConstraintUpper = RoundValue(SliderStruct.MaxConstraintUpper);

	if (!SliderStruct.bShowCheckBox)
	{
		BSBox_CheckBox->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		BSBox_CheckBox->SetVisibility(ESlateVisibility::Visible);
	}

	if (!InParams.bShowMinLock)
	{
		Checkbox_MinLock->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		Checkbox_MinLock->SetVisibility(ESlateVisibility::Visible);
		Checkbox_MinLock->SetIsChecked(InParams.bStartMinLocked);
	}
	if (!InParams.bShowMaxLock)
	{
		Checkbox_MaxLock->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		Checkbox_MaxLock->SetVisibility(ESlateVisibility::Visible);
		Checkbox_MaxLock->SetIsChecked(InParams.bStartMaxLocked);
	}
	Checkbox_MinLock->OnCheckStateChanged.AddUniqueDynamic(this, &UDoubleSyncedSliderAndTextBox::OnCheckStateChanged_MinLock);
	Checkbox_MaxLock->OnCheckStateChanged.AddUniqueDynamic(this, &UDoubleSyncedSliderAndTextBox::OnCheckStateChanged_MaxLock);
	
	Text_Checkbox_SyncSlidersAndValues->SetText(SliderStruct.CheckboxText);
	Checkbox_SyncSlidersAndValues->SetIsChecked(SliderStruct.bSyncSlidersAndValues);

	Text_Min->SetText(SliderStruct.MinText);
	Text_Max->SetText(SliderStruct.MaxText);
	
	Slider_Min->SetMinValue(SliderStruct.MinConstraintLower);
	Slider_Min->SetMaxValue(SliderStruct.MinConstraintUpper);
	Slider_Max->SetMinValue(SliderStruct.MaxConstraintLower);
	Slider_Max->SetMaxValue(SliderStruct.MaxConstraintUpper);

	Slider_Min->SetValue(SliderStruct.DefaultMinValue);
	Slider_Max->SetValue(SliderStruct.DefaultMaxValue);
	Value_Min->SetText(FText::AsNumber(SliderStruct.DefaultMinValue));
	Value_Max->SetText(FText::AsNumber(SliderStruct.DefaultMaxValue));

	if (SliderStruct.bIndentLeftOneLevel)
	{
		Spacer_Min->SetSize(FVector2D(50.f,0.f));
		Spacer_Max->SetSize(FVector2D(50.f,0.f));
		Spacer_CheckBox->SetSize(FVector2D(50.f,0.f));
	}

	if (SliderStruct.bShowMinQMark)
	{
		QMark_Min->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		QMark_Min->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (SliderStruct.bShowMaxQMark)
	{
		QMark_Max->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		QMark_Max->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UDoubleSyncedSliderAndTextBox::UpdateDefaultValues(const float NewMinValue, const float NewMaxValue, const bool bSync)
{
	SliderStruct.DefaultMinValue = RoundValue(NewMinValue);
	SliderStruct.DefaultMaxValue = RoundValue(NewMaxValue);
	SliderStruct.bSyncSlidersAndValues = bSync;
	if (SliderStruct.bLocksOnlySync)
	{
		Checkbox_MinLock->SetIsChecked(bSync);
		Checkbox_MaxLock->SetIsChecked(bSync);
	}
	Checkbox_SyncSlidersAndValues->SetIsChecked(bSync);
	SyncSlidersAndValues(bSync, true);
	OnCheckStateChanged_Sync.Broadcast(bSync);
}

void UDoubleSyncedSliderAndTextBox::UpdateDefaultValuesAbsolute(const float NewMinValue, const float NewMaxValue, const bool bSync)
{
	SliderStruct.DefaultMinValue = RoundValue(NewMinValue);
	SliderStruct.DefaultMaxValue = RoundValue(NewMaxValue);
	if (SliderStruct.bLocksOnlySync)
	{
		Checkbox_MinLock->SetIsChecked(bSync);
		Checkbox_MaxLock->SetIsChecked(bSync);
	}
	SyncSlidersAndValues(bSync, false);
	Checkbox_SyncSlidersAndValues->SetIsChecked(bSync);
	OnCheckStateChanged_Sync.Broadcast(bSync);
	Slider_Min->SetValue(NewMinValue);
	Slider_Max->SetValue(NewMaxValue);
	OnSliderChanged(true, NewMinValue);
	OnSliderChanged(false, NewMaxValue);
}

void UDoubleSyncedSliderAndTextBox::ResetValuesToDefault()
{
	/* Use pre-synced values if possible */
	float NewMinValue;
	float NewMaxValue;
	if (PreSyncedMaxValue != -1.f && PreSyncedMinValue != -1.f)
	{
		NewMinValue = CheckConstraints(PreSyncedMinValue, true);
		NewMaxValue = CheckConstraints(PreSyncedMaxValue, false);
	}
	else
	{
		NewMinValue = SliderStruct.DefaultMinValue;
		NewMaxValue = SliderStruct.DefaultMaxValue;
	}
	Slider_Min->SetValue(NewMinValue);
	Slider_Max->SetValue(NewMaxValue);
	OnSliderChanged(true, NewMinValue);
	OnSliderChanged(false, NewMaxValue);
}

void UDoubleSyncedSliderAndTextBox::OnCheckStateChanged_SyncSlidersAndValues(const bool bIsChecked)
{
	SyncSlidersAndValues(bIsChecked, true);
	OnCheckStateChanged_Sync.Broadcast(bIsChecked);
}

void UDoubleSyncedSliderAndTextBox::OnCheckStateChanged_MinLock(const bool bIsLocked)
{
	if (SliderStruct.bLocksOnlySync && Checkbox_MaxLock->IsChecked() != bIsLocked)
	{
		Checkbox_MaxLock->SetIsChecked(bIsLocked);
	}
	
	if (SliderStruct.bLocksOnlySync && GetIsSynced() != bIsLocked)
	{
		SyncSlidersAndValues(bIsLocked, true);
	}
	OnCheckStateChanged_Min.Broadcast(bIsLocked);
}

void UDoubleSyncedSliderAndTextBox::OnCheckStateChanged_MaxLock(const bool bIsLocked)
{
	if (SliderStruct.bLocksOnlySync && Checkbox_MinLock->IsChecked() != bIsLocked)
	{
		Checkbox_MinLock->SetIsChecked(bIsLocked);
	}

	if (SliderStruct.bLocksOnlySync && GetIsSynced() != bIsLocked)
	{
		SyncSlidersAndValues(bIsLocked, true);
	}
	OnCheckStateChanged_Max.Broadcast(bIsLocked);
}

void UDoubleSyncedSliderAndTextBox::OnSliderChanged(const bool bIsSlider_Min, const float NewValue)
{
	const float ConstrainedValue = CheckConstraints(NewValue, bIsSlider_Min);
	if (GetIsSynced())
	{
		if (bIsSlider_Min)
		{
			Slider_Max->SetValue(ConstrainedValue);
		}
		else
		{
			Slider_Min->SetValue(ConstrainedValue);
		}
		
		Value_Min->SetText(FText::AsNumber(ConstrainedValue));
		Value_Max->SetText(FText::AsNumber(ConstrainedValue));
		OnValueChanged_Min.Broadcast(ConstrainedValue);
		OnValueChanged_Max.Broadcast(ConstrainedValue);
		return;
	}
	if (bIsSlider_Min)
	{
		Value_Min->SetText(FText::AsNumber(ConstrainedValue));
		OnValueChanged_Min.Broadcast(ConstrainedValue);
		return;
	}
	Value_Max->SetText(FText::AsNumber(ConstrainedValue));
	OnValueChanged_Max.Broadcast(ConstrainedValue);
}

void UDoubleSyncedSliderAndTextBox::OnSliderChanged_Min(const float NewMin)
{
	OnSliderChanged(true, NewMin);
}

void UDoubleSyncedSliderAndTextBox::OnSliderChanged_Max(const float NewMax)
{
	OnSliderChanged(false, NewMax);
}

void UDoubleSyncedSliderAndTextBox::OnTextCommitted(const bool bIsValue_Min, const FText& NewValue)
{
	const float ConstrainedValue = CheckConstraints(FCString::Atof(*UKismetStringLibrary::Replace(NewValue.ToString(), ",", "")), true);
	if (GetIsSynced())
	{
		Value_Max->SetText(FText::AsNumber(ConstrainedValue));
		Value_Min->SetText(FText::AsNumber(ConstrainedValue));
		Slider_Min->SetValue(ConstrainedValue);
		Slider_Max->SetValue(ConstrainedValue);
		OnValueChanged_Min.Broadcast(ConstrainedValue);
		OnValueChanged_Max.Broadcast(ConstrainedValue);
		return;
	}
	if (bIsValue_Min)
	{
		Value_Min->SetText(FText::AsNumber(ConstrainedValue));
		Slider_Min->SetValue(ConstrainedValue);
		OnValueChanged_Min.Broadcast(ConstrainedValue);
		return;
	}
	Value_Max->SetText(FText::AsNumber(ConstrainedValue));
	Slider_Max->SetValue(ConstrainedValue);
	OnValueChanged_Max.Broadcast(ConstrainedValue);
}

void UDoubleSyncedSliderAndTextBox::OnTextCommitted_Min(const FText& NewMin, ETextCommit::Type CommitType)
{
	OnTextCommitted(true, NewMin);
}

void UDoubleSyncedSliderAndTextBox::OnTextCommitted_Max(const FText& NewMax, ETextCommit::Type CommitType)
{
	OnTextCommitted(false, NewMax);
}

void UDoubleSyncedSliderAndTextBox::SyncSlidersAndValues(const bool bSync, const bool bTryUsePreSyncedValues)
{
	SliderStruct.bSyncSlidersAndValues = bSync;
	
	if (bSync)
	{
		PreSyncedMinValue = Slider_Min->GetValue();
		PreSyncedMaxValue = Slider_Max->GetValue();
		
		/* Set max values for both sliders */
		if (Slider_Min->GetMaxValue() < Slider_Max->GetMaxValue())
		{
			Slider_Max->SetMaxValue(Slider_Min->GetMaxValue());
		}
		else
		{
			Slider_Min->SetMaxValue(Slider_Max->GetMaxValue());
		}
		
		/* Clamp slider values */
		if (PreSyncedMaxValue > CheckConstraints(PreSyncedMaxValue, false))
		{
			Slider_Max->SetValue(CheckConstraints(PreSyncedMaxValue, false));
		}
		if (PreSyncedMinValue > CheckConstraints(PreSyncedMinValue, true))
		{
			Slider_Min->SetValue(CheckConstraints(PreSyncedMinValue, true));
		}
		
		/* Default to choosing MaxValue for both */
		if (Slider_Max->GetValue() != Slider_Min->GetValue())
		{
			Slider_Min->SetValue(Slider_Max->GetValue());
		}
	
		/* Doesn't matter which slider we use since they are synced and will be the same */
		OnSliderChanged(true, Slider_Min->GetValue());
		return;
	}
	
	/* Return the sliders to their original min/maxes */
	Slider_Min->SetMaxValue(SliderStruct.MinConstraintUpper);
	Slider_Max->SetMaxValue(SliderStruct.MaxConstraintUpper);
	float NewMinValue;
	float NewMaxValue;
	
	/* Use pre-synced values if possible */
	if (bTryUsePreSyncedValues && PreSyncedMaxValue != -1.f && PreSyncedMinValue != -1.f)
	{
		NewMinValue = CheckConstraints(PreSyncedMinValue, true);
		NewMaxValue = CheckConstraints(PreSyncedMaxValue, false);
	}
	else
	{
		NewMinValue = SliderStruct.DefaultMinValue;
		NewMaxValue = SliderStruct.DefaultMaxValue;
	}
	Slider_Min->SetValue(NewMinValue);
	Slider_Max->SetValue(NewMaxValue);
	OnSliderChanged(true, NewMinValue);
	OnSliderChanged(false, NewMaxValue);
}

float UDoubleSyncedSliderAndTextBox::CheckConstraints(const float NewValue, const bool bIsMin)
{
	/** Checking constraints for a Slider_Min or Value_Min */
	if (bIsMin)
	{
		return RoundValue(FMath::Clamp(NewValue, Slider_Min->GetMinValue(), Slider_Min->GetMaxValue()));
	}
	/** Checking constraints for a Slider_Max or Value_Max */
	return RoundValue(FMath::Clamp(NewValue, Slider_Max->GetMinValue(), Slider_Max->GetMaxValue()));
}

void UDoubleSyncedSliderAndTextBox::OverrideMaxValue(const bool bIsMin, const float ValueToOverride)
{
	if (GetIsSynced())
	{
		if (Slider_Min->GetValue() > ValueToOverride)
		{
			Slider_Min->SetValue(ValueToOverride);
			Value_Min->SetText(FText::AsNumber(ValueToOverride));
			OnValueChanged_Min.Broadcast(ValueToOverride);
		}
		if (Slider_Max->GetValue() > ValueToOverride)
		{
			Slider_Max->SetValue(ValueToOverride);
			Value_Max->SetText(FText::AsNumber(ValueToOverride));
			OnValueChanged_Max.Broadcast(ValueToOverride);
		}
		return;
	}

	if (bIsMin)
	{
		if (Slider_Min->GetValue() > ValueToOverride)
		{
			Slider_Min->SetValue(ValueToOverride);
			Value_Min->SetText(FText::AsNumber(ValueToOverride));
			OnValueChanged_Min.Broadcast(ValueToOverride);
		}
		return;
	}
	
	if (Slider_Max->GetValue() > ValueToOverride)
	{
		Slider_Max->SetValue(ValueToOverride);
		Value_Max->SetText(FText::AsNumber(ValueToOverride));
		OnValueChanged_Max.Broadcast(ValueToOverride);
	}
}

float UDoubleSyncedSliderAndTextBox::RoundValue(const float ValueToRound) const
{
	return FMath::GridSnap(ValueToRound, SliderStruct.GridSnapSize);
}
