// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


// ReSharper disable CppMemberFunctionMayBeConst

#include "SubMenuWidgets/SettingsMenuWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "SubMenuWidgets/AASettingsWidget.h"
#include "SubMenuWidgets/CrossHairSettingsWidget.h"
#include "SubMenuWidgets/GameSettingsWidget.h"
#include "SubMenuWidgets/SensitivitySettingsWidget.h"
#include "SubMenuWidgets/VideoAndSoundSettingsWidget.h"
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

	Game_Widget->OnSettingsSaved_Game.AddUFunction(this, "OnPlayerSettingsSaved");
	VideoAndSound_Widget->OnSettingsSaved_VideoAndSound.AddUFunction(this, "OnPlayerSettingsSaved");
	AudioAnalyzer_Widget->OnSettingsSaved_AudioAnalyzer.BindUFunction(this, "OnAASettingsSaved");
	AudioAnalyzer_Widget->OnRestartButtonClicked.BindUFunction(this, "OnRestartButtonClicked_AudioAnalyzer");
	Sensitivity_Widget->OnSettingsSaved_Sensitivity.AddUFunction(this, "OnPlayerSettingsSaved");
	CrossHair_Widget->OnSettingsSaved_CrossHair.AddUFunction(this, "OnPlayerSettingsSaved");

	RestartState.TransitionState = ETransitionState::Restart;
	RestartState.bSaveCurrentScores = false;

	On_Game_ButtonClicked();

	if (bIsMainMenuChild)
	{
		AudioAnalyzer_Widget->InitMainMenuChild();
	}
}

void USettingsMenuWidget::OnPlayerSettingsSaved()
{
	OnPlayerSettingsChanged.Broadcast(LoadPlayerSettings());
}

void USettingsMenuWidget::OnAASettingsSaved()
{
	OnAASettingsChanged.Broadcast(LoadAASettings());
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
