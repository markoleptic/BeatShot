// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/MenuOptionWidgets/MenuOptionWidget.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "WidgetComponents/TooltipImage.h"

void UMenuOptionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	CheckBox_Lock->OnCheckStateChanged.AddUniqueDynamic(this, &ThisClass::OnCheckBox_LockStateChanged);
	SetIndentLevel(IndentLevel);
	SetShowTooltipImage(bShowTooltipImage);
	SetShowTooltipWarningImage(bShowTooltipWarningImage);
	SetShowCheckBoxLock(bShowCheckBoxLock);
	SetDescriptionText(DescriptionText);
	SetTooltipText(TooltipRegularText);
	SetTooltipWarningText(TooltipWarningText);
}

void UMenuOptionWidget::SetIndentLevel(const int32 Value)
{
	IndentLevel = Value;
	Indent_Left->SetSize(FVector2d(Value * 50.f, 0.f));
}

void UMenuOptionWidget::SetShowTooltipImage(const bool bShow)
{
	bShowTooltipImage = bShow;
	if (bShow)
	{
		TooltipImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		TooltipImage->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMenuOptionWidget::SetShowTooltipWarningImage(const bool bShow)
{
	bShowTooltipWarningImage = bShow;
	if (bShow)
	{
		TooltipWarningImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		TooltipWarningImage->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMenuOptionWidget::SetShowCheckBoxLock(const bool bShow)
{
	bShowCheckBoxLock = bShow;
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
	TextBlock_Description->SetText(InText);
}

void UMenuOptionWidget::SetTooltipText(const FText& InText)
{
	TooltipRegularText = InText;
}

void UMenuOptionWidget::SetTooltipWarningText(const FText& InText)
{
	TooltipWarningText = InText;
}

void UMenuOptionWidget::OnCheckBox_LockStateChanged(const bool bChecked)
{
	if (OnLockStateChanged.IsBound())
	{
		OnLockStateChanged.Broadcast(bChecked);
	}
}
