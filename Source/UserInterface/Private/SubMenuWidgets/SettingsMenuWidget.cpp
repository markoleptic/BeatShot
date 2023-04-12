// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/SettingsMenuWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "SubMenuWidgets/AASettingsWidget.h"
#include "WidgetComponents/SlideRightButton.h"

void USettingsMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	/* Menu Widgets */

	MenuWidgets.Add(Game_Button, Game);
	MenuWidgets.Add(VideoAndSound_Button, VideoAndSound);
	MenuWidgets.Add(AudioAnalyzer_Button, AudioAnalyzer);
	MenuWidgets.Add(Sensitivity_Button, Sensitivity);
	MenuWidgets.Add(CrossHair_Button, CrossHair);
	Game_Button->Button->OnClicked.AddDynamic(this, &USettingsMenuWidget::On_Game_ButtonClicked);
	VideoAndSound_Button->Button->OnClicked.AddDynamic(this, &USettingsMenuWidget::On_VideoAndSound_ButtonClicked);
	AudioAnalyzer_Button->Button->OnClicked.AddDynamic(this, &USettingsMenuWidget::On_AudioAnalyzer_ButtonClicked);
	Sensitivity_Button->Button->OnClicked.AddDynamic(this, &USettingsMenuWidget::On_Sensitivity_ButtonClicked);
	CrossHair_Button->Button->OnClicked.AddDynamic(this, &USettingsMenuWidget::On_CrossHair_ButtonClicked);
	
	AudioAnalyzer_Widget->OnRestartButtonClicked.BindUFunction(this, "OnRestartButtonClicked_AudioAnalyzer");

	On_Game_ButtonClicked();

	if (bIsMainMenuChild)
	{
		AudioAnalyzer_Widget->InitMainMenuChild();
	}
}

void USettingsMenuWidget::OnRestartButtonClicked_AudioAnalyzer() const
{
	if (!OnRestartButtonClicked.ExecuteIfBound())
	{
		UE_LOG(LogTemp, Display, TEXT("OnRestartButtonClicked not bound."));
	}
}

void USettingsMenuWidget::SlideButtons(const USlideRightButton* ActiveButton)
{
	for (TTuple<USlideRightButton*, UVerticalBox*>& Elem : MenuWidgets)
	{
		if (Elem.Key != ActiveButton)
		{
			Elem.Key->SlideButton(false);
			continue;
		}
		Elem.Key->SlideButton(true);
		MenuSwitcher->SetActiveWidget(Elem.Value);
	}
}
