// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/MenuOptionWidgets/EditableTextBoxOptionWidget.h"
#include "Components/EditableTextBox.h"

void UEditableTextBoxOptionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	EditableTextBox->OnTextCommitted.AddUniqueDynamic(this, &ThisClass::OnTextCommitted_EditableTextBox);
}

void UEditableTextBoxOptionWidget::OnTextCommitted_EditableTextBox(const FText& Text, ETextCommit::Type CommitType)
{
	
}
