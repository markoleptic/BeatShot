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
	
	MenuWidgets.Add(SM_Game_Button, SM_Game);
	MenuWidgets.Add(SM_VideoAndSound_Button, SM_VideoAndSound);
	MenuWidgets.Add(SM_AudioAnalyzer_Button, SM_AudioAnalyzer);
	MenuWidgets.Add(SM_Sensitivity_Button, SM_Sensitivity);
	MenuWidgets.Add(SM_CrossHair_Button, SM_CrossHair);
	SM_Game_Button->Button->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnSM_Game_ButtonClicked);
	SM_VideoAndSound_Button->Button->OnClicked.AddDynamic(
		this, &USettingsMenuWidget::OnSM_VideoAndSound_ButtonClicked);
	SM_AudioAnalyzer_Button->Button->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnSM_AudioAnalyzer_ButtonClicked);
	SM_Sensitivity_Button->Button->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnSM_Sensitivity_ButtonClicked);
	SM_CrossHair_Button->Button->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnSM_CrossHair_ButtonClicked);

	SM_Game_Widget->OnSettingsSaved_Game.AddUFunction(this, "OnPlayerSettingsSaved");
	SM_VideoAndSound_Widget->OnSettingsSaved_VideoAndSound.AddUFunction(this, "OnPlayerSettingsSaved");
	SM_AudioAnalyzer_Widget->OnSettingsSaved_AudioAnalyzer.BindUFunction(this, "OnAASettingsSaved");
	SM_AudioAnalyzer_Widget->OnRestartButtonClicked.BindUFunction(this, "OnRestartButtonClicked_AudioAnalyzer");
	SM_Sensitivity_Widget->OnSettingsSaved_Sensitivity.AddUFunction(this, "OnPlayerSettingsSaved");
	SM_CrossHair_Widget->OnSettingsSaved_CrossHair.AddUFunction(this, "OnPlayerSettingsSaved");

	RestartState.TransitionState = ETransitionState::Restart;
	RestartState.bSaveCurrentScores = false;
	
	OnSM_Game_ButtonClicked();

	if (bIsMainMenuChild)
	{
		SM_AudioAnalyzer_Widget->InitMainMenuChild();
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


