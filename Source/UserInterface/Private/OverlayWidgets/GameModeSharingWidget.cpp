// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "OverlayWidgets/GameModeSharingWidget.h"
#include "Components/MultiLineEditableTextBox.h"
#include "WidgetComponents/Buttons/BSButton.h"

FString UGameModeSharingWidget::GetImportString() const
{
	return MultilineTextBox->GetText().ToString();
}

void UGameModeSharingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	MultilineTextBox->SetHintText(FText::FromString("Paste an exported game mode here."));
	MultilineTextBox->OnTextChanged.AddDynamic(this, &ThisClass::OnTextChanged_MultilineTextBox);
	Button_1->SetIsEnabled(false);
}

void UGameModeSharingWidget::OnTextChanged_MultilineTextBox(const FText& NewText)
{
	if (!NewText.IsEmpty())
	{
		Button_1->SetIsEnabled(true);
	}
	else
	{
		Button_1->SetIsEnabled(false);
	}
}
