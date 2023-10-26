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

void UDefaultGameModeOptionWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	AltDescriptionText->SetScrollingEnabled(true);
}

void UDefaultGameModeOptionWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	AltDescriptionText->SetScrollingEnabled(false);
}

void UDefaultGameModeOptionWidget::SetAltDescriptionText(const FText& Text)
{
	AltDescriptionText->SetText(Text);
}
