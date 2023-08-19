// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/MenuOptionWidgets/ComboBoxOptionWidget.h"
#include "WidgetComponents/BSComboBoxEntry.h"
#include "WidgetComponents/BSComboBoxString.h"

void UComboBoxOptionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_ComboBox);
	ComboBox->OnGenerateWidgetEventDelegate.BindDynamic(this, &ThisClass::OnGenerateWidgetEvent);
	ComboBox->OnSelectionChanged_GenerateWidgetForMultiSelection.BindDynamic(this, &ThisClass::OnSelectionChanged_GenerateMultiSelectionItem);
}

UBSComboBoxEntry* UComboBoxOptionWidget::ConstructComboBoxEntryWidget()
{
	return CreateWidget<UBSComboBoxEntry>(this, ComboBox->GetComboboxEntryWidget());
}

void UComboBoxOptionWidget::OnSelectionChanged_ComboBox(const TArray<FString>& Selected,
                                                        const ESelectInfo::Type SelectionType)
{
	
}
