// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/FAQWidget.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/WidgetSwitcher.h"
#include "WidgetComponents/SlideRightButton.h"

void UFAQWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	MenuWidgets.Add(GameModesButton, GameModesBox);
	MenuWidgets.Add(ScoringButton, ScoringBox);
	MenuWidgets.Add(AudioAnalyzerButton, AudioAnalyzerBox);
	
	GameModesButton->Button->OnClicked.AddDynamic(this, &UFAQWidget::OnGameModesButtonClicked);
	ScoringButton->Button->OnClicked.AddDynamic(this, &UFAQWidget::OnScoringButtonClicked);
	AudioAnalyzerButton->Button->OnClicked.AddDynamic(this, &UFAQWidget::OnAudioAnalyzerButtonClicked);
	
	SlideButtons(GameModesButton);
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

void UFAQWidget::OnGameModesButtonClicked()
{
	SlideButtons(GameModesButton);
}

void UFAQWidget::OnScoringButtonClicked()
{
	SlideButtons(ScoringButton);
}

void UFAQWidget::OnAudioAnalyzerButtonClicked()
{
	SlideButtons(AudioAnalyzerButton);
}
