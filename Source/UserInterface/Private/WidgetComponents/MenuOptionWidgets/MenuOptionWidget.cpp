﻿// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/MenuOptionWidgets/MenuOptionWidget.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "WidgetComponents/TooltipImage.h"

void UMenuOptionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	CheckBox_Lock->OnCheckStateChanged.AddUniqueDynamic(this, &ThisClass::OnCheckBox_LockStateChanged);
	SetIndentLevel(IndentLevel);
	SetShowTooltipImage(bShowTooltipImage);
	SetShowCheckBoxLock(bShowCheckBoxLock);
	SetDescriptionText(DescriptionText);
	SetTooltipText(TooltipImageText);
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
	TooltipImageText = InText;
}

UTooltipImage* UMenuOptionWidget::GetTooltipImage() const
{
	return TooltipImage;
}

UTooltipImage* UMenuOptionWidget::FindOrAddTooltipWarningImage(const FString& InTooltipStringTableKey)
{
	UTooltipImage** Found = WarningTooltips.Find(InTooltipStringTableKey);
	if (!Found)
	{
		UTooltipImage* NewTooltipImage = CreateWidget<UTooltipImage>(this, TooltipWarningImageClass);
		UHorizontalBoxSlot* HorizontalBoxSlot = TooltipBox->AddChildToHorizontalBox(NewTooltipImage);
		HorizontalBoxSlot->SetHorizontalAlignment(HAlign_Right);
		HorizontalBoxSlot->SetPadding(FMargin(10.f, 0.f, 0.f, 0.f));
		HorizontalBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
		WarningTooltips.Add(InTooltipStringTableKey, NewTooltipImage);
		return NewTooltipImage;
	}
	return *Found;
}

TArray<FString> UMenuOptionWidget::GetTooltipWarningImageKeys() const
{
	TArray<FString> KeyArray;
	WarningTooltips.GenerateKeyArray(KeyArray);
	return KeyArray;
}

void UMenuOptionWidget::RemoveTooltipWarningImage(const FString& InTooltipStringTableKey)
{
	UTooltipImage* Found = nullptr;
	if (WarningTooltips.RemoveAndCopyValue(InTooltipStringTableKey, Found))
	{
		if (Found)
		{
			Found->RemoveFromParent();
		}
	}
}

void UMenuOptionWidget::RemoveAllTooltipWarningImages()
{
	TArray<UTooltipImage*> TooltipWarningImages;
	WarningTooltips.GenerateValueArray(TooltipWarningImages);
	WarningTooltips.Empty();
	for (UTooltipImage* TooltipImageToRemove : TooltipWarningImages)
	{
		TooltipImageToRemove->RemoveFromParent();
	}
}

void UMenuOptionWidget::OnCheckBox_LockStateChanged(const bool bChecked)
{
	if (OnLockStateChanged.IsBound())
	{
		OnLockStateChanged.Broadcast(bChecked);
	}
}
