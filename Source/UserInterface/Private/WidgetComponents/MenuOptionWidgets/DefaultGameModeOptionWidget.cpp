// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/MenuOptionWidgets/DefaultGameModeOptionWidget.h"

#include "CommonTextBlock.h"

void UDefaultGameModeOptionWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDefaultGameModeOptionWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UDefaultGameModeOptionWidget::SetStyling()
{
	Super::SetStyling();
}

void UDefaultGameModeOptionWidget::SetAltDescriptionText(const FText& Text)
{
	AltDescriptionText->SetText(Text);
}
