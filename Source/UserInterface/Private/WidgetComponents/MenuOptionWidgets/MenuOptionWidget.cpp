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

FTooltipData UMenuOptionWidget::FindOrAddTooltip(const FString& InTooltipStringTableKey,  const ETooltipImageType& TooltipType, const FText& OptionalAdditionalText)
{
	FTooltipData* Found = GetAllTooltipData().FindByPredicate([&InTooltipStringTableKey, &TooltipType] (const FTooltipData& TooltipData)
	{
		return TooltipData.TooltipStringTableKey.Equals(InTooltipStringTableKey) && TooltipData.TooltipType == TooltipType;
	});
	if (!Found)
	{
		UTooltipImage* NewTooltipImage;
		switch (TooltipType) {
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
		
		FTooltipData TooltipData;
		TooltipData.TooltipImage = NewTooltipImage;
		TooltipData.AdditionalTooltipText = OptionalAdditionalText;
		TooltipData.TooltipStringTableKey = InTooltipStringTableKey;
		TooltipData.TooltipType = TooltipType;
		return TooltipData;
	}
	return *Found;
}

TArray<FString> UMenuOptionWidget::GetTooltipWarningImageKeys() const
{
	TArray<FString> KeyArray;
	for (const UPanelSlot* PanelSlot: TooltipBox->GetSlots())
	{
		if (UTooltipImage* TooltipImage = Cast<UTooltipImage>(PanelSlot->Content))
		{
			if (TooltipImage->GetTooltipData()->TooltipType != ETooltipImageType::Default)
			{
				KeyArray.Add(TooltipImage->GetTooltipData()->TooltipStringTableKey);
			}
		}
	}
	return KeyArray;
}

TArray<FTooltipData> UMenuOptionWidget::GetAllTooltipData() const
{
	TArray<FTooltipData> TooltipWarningValues;
	for (const UPanelSlot* PanelSlot: TooltipBox->GetSlots())
	{
		if (UTooltipImage* TooltipImage = Cast<UTooltipImage>(PanelSlot->Content))
		{
			if (TooltipImage->GetTooltipData()->TooltipType != ETooltipImageType::Default)
			{
				TooltipWarningValues.Add(*TooltipImage->GetTooltipData());
			}
		}
	}
	return TooltipWarningValues;
}

void UMenuOptionWidget::RemoveTooltipWarningImage(const FString& InTooltipStringTableKey)
{
	for (const UPanelSlot* PanelSlot: TooltipBox->GetSlots())
	{
		if (UTooltipImage* TooltipImage = Cast<UTooltipImage>(PanelSlot->Content))
		{
			if (TooltipImage->GetTooltipData()->TooltipType != ETooltipImageType::Default)
			{
				if (TooltipImage->GetTooltipData()->TooltipStringTableKey.Equals(InTooltipStringTableKey))
				{
					TooltipImage->RemoveFromParent();
					return;
				}
			}
		}
	}
}

void UMenuOptionWidget::RemoveAllTooltipWarningImages()
{
	for (const FTooltipData& Value : GetAllTooltipData())
	{
		if (Value.TooltipImage.IsValid())
		{
			Value.TooltipImage->RemoveFromParent();
		}
	}
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
