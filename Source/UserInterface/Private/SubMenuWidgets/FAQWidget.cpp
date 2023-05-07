// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/FAQWidget.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/WidgetSwitcher.h"
#include "WidgetComponents/SlideRightButton.h"

void UFAQWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MenuWidgets.Add(Button_GameModes, Border_GameModes);
	MenuWidgets.Add(Button_Scoring, Border_Scoring);
	MenuWidgets.Add(Button_AudioAnalyzer, Border_AudioAnalyzer);

	Button_GameModes->Button->OnClicked.AddDynamic(this, &UFAQWidget::OnButtonClicked_GameModes);
	Button_Scoring->Button->OnClicked.AddDynamic(this, &UFAQWidget::OnButtonClicked_Scoring);
	Button_AudioAnalyzer->Button->OnClicked.AddDynamic(this, &UFAQWidget::OnButtonClicked_AudioAnalyzer);

	SlideButtons(Button_GameModes);
}

void UFAQWidget::SlideButtons(const USlideRightButton* ActiveButton)
{
	for (TTuple<USlideRightButton*, UBorder*>& Elem : MenuWidgets)
	{
		if (Elem.Key != ActiveButton)
		{
			Elem.Key->SlideButton(false);
			continue;
		}
		Elem.Key->SlideButton(true);
		FAQSwitcher->SetActiveWidget(Elem.Value);
	}
}

void UFAQWidget::OnButtonClicked_GameModes()
{
	SlideButtons(Button_GameModes);
}

void UFAQWidget::OnButtonClicked_Scoring()
{
	SlideButtons(Button_Scoring);
}

void UFAQWidget::OnButtonClicked_AudioAnalyzer()
{
	SlideButtons(Button_AudioAnalyzer);
}
