// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/MenuOptionWidgets/ComboBoxOptionWidget.h"
#include "WidgetComponents/BSComboBoxEntry.h"
#include "WidgetComponents/BSComboBoxString.h"

void UComboBoxOptionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ComboBox->OnGenerateWidgetEventDelegate.BindDynamic(this, &ThisClass::OnGenerateWidgetEvent);
	ComboBox->OnSelectionChanged_GenerateWidgetForMultiSelection.BindDynamic(this, &ThisClass::OnSelectionChanged_GenerateMultiSelectionItem);
}

UBSComboBoxEntry* UComboBoxOptionWidget::ConstructComboBoxEntryWidget()
{
	return CreateWidget<UBSComboBoxEntry>(this, ComboBox->GetComboboxEntryWidget());
}

FString UComboBoxOptionWidget::GetStringTableKeyFromComboBox(const UBSComboBoxString* ComboBoxString, const FString& EnumString)
{
	if (GetComboBoxEntryTooltipStringTableKey.IsBound())
	{
		return GetComboBoxEntryTooltipStringTableKey.Execute(EnumString);
	}
	return IBSWidgetInterface::GetStringTableKeyFromComboBox(ComboBoxString, EnumString);
}

void UComboBoxOptionWidget::SortAndAddOptions(TArray<FString>& InOptions) const
{
	InOptions.Sort([] (const FString& FirstOption, const FString& SecondOption)
	{
		return FirstOption < SecondOption;
	});
	for (const FString Option : InOptions)
	{
		ComboBox->AddOption(Option);
	}
}
