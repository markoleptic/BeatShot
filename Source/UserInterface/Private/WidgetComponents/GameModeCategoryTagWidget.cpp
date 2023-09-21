// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/GameModeCategoryTagWidget.h"


void UGameModeCategoryTagWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	SetText(CategoryText);
}

void UGameModeCategoryTagWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetText(CategoryText);
}

void UGameModeCategoryTagWidget::SetText(const FText& InText)
{
	CategoryText = InText;
	if (TextBlock_Category)
	{
		TextBlock_Category->SetText(CategoryText);
	}
}