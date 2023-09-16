// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/MenuOptionWidgets/MenuOptionWidget.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "WidgetComponents/TooltipImage.h"


void UMenuOptionWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	SetIndentLevel(IndentLevel);
	SetShowTooltipImage(bShowTooltipImage);
	SetShowCheckBoxLock(bShowCheckBoxLock);
	SetDescriptionText(DescriptionText);
	SetTooltipText(DescriptionTooltipText);
}

void UMenuOptionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CheckBox_Lock)
	{
		CheckBox_Lock->OnCheckStateChanged.AddUniqueDynamic(this, &ThisClass::OnCheckBox_LockStateChanged);
	}
	SetIndentLevel(IndentLevel);
	SetShowTooltipImage(bShowTooltipImage);
	SetShowCheckBoxLock(bShowCheckBoxLock);
	SetDescriptionText(DescriptionText);
	SetTooltipText(DescriptionTooltipText);
}

void UMenuOptionWidget::SetIndentLevel(const int32 Value)
{
	IndentLevel = Value;
	if (Indent_Left)
	{
		Indent_Left->SetSize(FVector2d(Value * 50.f, 0.f));
	}
}

void UMenuOptionWidget::SetShowTooltipImage(const bool bShow)
{
	bShowTooltipImage = bShow;

	if (!DescriptionTooltip)
	{
		return;
	}
	
	if (bShow)
	{
		DescriptionTooltip->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		DescriptionTooltip->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMenuOptionWidget::SetShowCheckBoxLock(const bool bShow)
{
	bShowCheckBoxLock = bShow;
	if (!CheckBox_Lock)
	{
		return;
	}
	if (bShow)
	{
		CheckBox_Lock->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		CheckBox_Lock->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMenuOptionWidget::SetDescriptionText(const FText& InText)
{
	DescriptionText = InText;

	if (!TextBlock_Description)
	{
		return;
	}
	
	TextBlock_Description->SetText(InText);
}

void UMenuOptionWidget::SetTooltipText(const FText& InText)
{
	DescriptionTooltipText = InText;
}

UTooltipImage* UMenuOptionWidget::GetTooltipImage() const
{
	return DescriptionTooltip;
}

bool UMenuOptionWidget::GetIsLocked() const
{
	if (CheckBox_Lock)
	{
		return CheckBox_Lock->IsChecked();
	}
	UE_LOG(LogTemp, Warning, TEXT("Tried to access a CheckBox_Lock that wasn't found in a MenuOptionWidget."));
	return false;
}

void UMenuOptionWidget::SetIsLocked(const bool bLocked) const
{
	if (CheckBox_Lock)
	{
		CheckBox_Lock->SetIsChecked(bLocked);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Tried to access a CheckBox_Lock that wasn't found in a MenuOptionWidget."));
	}
}

void UMenuOptionWidget::OnCheckBox_LockStateChanged(const bool bChecked)
{
	if (OnLockStateChanged.IsBound())
	{
		OnLockStateChanged.Broadcast(this, bChecked);
	}
}

FUpdateTooltipState& UMenuOptionWidget::AddWarningTooltipData(const FTooltipData& InTooltipData)
{
	const int32 Index = WarningTooltipData.Add(InTooltipData);
	return WarningTooltipData[Index].UpdateTooltipState;
}

FUpdateDynamicTooltipState& UMenuOptionWidget::AddDynamicWarningTooltipData(const FTooltipData& InTooltipData, const FString& FallbackStringTableKey, const float InMin, const int32 InPrecision)
{
	const int32 Index = WarningTooltipData.Add(InTooltipData);
	WarningTooltipData[Index].SetDynamicData(InMin, FallbackStringTableKey, InPrecision);
	return WarningTooltipData[Index].UpdateDynamicTooltipState;
}

void UMenuOptionWidget::ConstructTooltipWarningImageIfNeeded(FTooltipData& InTooltipData)
{
	if (InTooltipData.TooltipImage.IsValid())
	{
		return;
	}

	UTooltipImage* NewTooltipImage;
	switch (InTooltipData.TooltipType) {
	case ETooltipImageType::Caution:
		NewTooltipImage = CreateWidget<UTooltipImage>(this, TooltipCautionImageClass);
		break;
	case ETooltipImageType::Warning:
		NewTooltipImage = CreateWidget<UTooltipImage>(this, TooltipWarningImageClass);
		break;
	case ETooltipImageType::Default:
	default:
		NewTooltipImage = CreateWidget<UTooltipImage>(this, TooltipWarningImageClass);
		break;
	}
	UHorizontalBoxSlot* HorizontalBoxSlot = TooltipBox->AddChildToHorizontalBox(NewTooltipImage);
	HorizontalBoxSlot->SetHorizontalAlignment(HAlign_Right);
	HorizontalBoxSlot->SetPadding(FMargin(10.f, 0.f, 0.f, 0.f));
	HorizontalBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	
	InTooltipData.TooltipImage = NewTooltipImage;
}

void UMenuOptionWidget::UpdateWarningTooltips()
{
	for (FTooltipData& Data : GetTooltipWarningData())
	{
		if (Data.UpdateTooltipState.IsBound())
		{
			Data.SetShouldShowTooltipImage(Data.UpdateTooltipState.Execute());
		}
		else
		{
			Data.SetShouldShowTooltipImage(false);
		}
	}
}

void UMenuOptionWidget::UpdateDynamicWarningTooltips()
{
	for (FTooltipData& Data : GetTooltipWarningData())
	{
		if (Data.IsDynamic() && Data.UpdateDynamicTooltipState.IsBound())
		{
			const FDynamicTooltipState State = Data.UpdateDynamicTooltipState.Execute();
			Data.UpdateDynamicTooltipText(State.Actual, State.MaxAllowed);
		}
	}
}

void UMenuOptionWidget::UpdateAllWarningTooltips()
{
	UpdateWarningTooltips();
	UpdateDynamicWarningTooltips();
}

int32 UMenuOptionWidget::GetNumberOfWarnings()
{
	int32 Num = 0;
	for (FTooltipData& Data : WarningTooltipData)
	{
		if (Data.TooltipType == ETooltipImageType::Warning && Data.TooltipImage.IsValid() && Data.TooltipImage->IsVisible())
		{
			Num++;
		}
	}
	return Num;
}

int32 UMenuOptionWidget::GetNumberOfCautions()
{
	int32 Num = 0;
	for (FTooltipData& Data : WarningTooltipData)
	{
		if (Data.TooltipType == ETooltipImageType::Caution && Data.TooltipImage.IsValid() && Data.TooltipImage->IsVisible())
		{
			Num++;
		}
	}
	return Num;
}