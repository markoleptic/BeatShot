// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/SettingsWidgets/SettingsMenuWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Components/VerticalBox.h"
#include "Components/Slider.h"
#include "SubMenuWidgets/SettingsWidgets/SettingsMenuWidget_AudioAnalyzer.h"
#include "WidgetComponents/Buttons/MenuButton.h"

void USettingsMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MenuButton_Game->SetDefaults(Game, MenuButton_VideoAndSound);
	MenuButton_VideoAndSound->SetDefaults(VideoAndSound, MenuButton_AudioAnalyzer);
	MenuButton_AudioAnalyzer->SetDefaults(AudioAnalyzer, MenuButton_Input);
	MenuButton_Input->SetDefaults(Input, MenuButton_CrossHair);
	MenuButton_CrossHair->SetDefaults(CrossHair, MenuButton_Game);
	
	MenuButton_Game->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	MenuButton_VideoAndSound->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	MenuButton_AudioAnalyzer->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	MenuButton_Input->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	MenuButton_CrossHair->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	
	AudioAnalyzer_Widget->OnRestartButtonClicked.BindUFunction(this, "OnRestartButtonClicked_AudioAnalyzer");

	MenuButton_Game->SetActive();

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

void USettingsMenuWidget::OnButtonClicked_BSButton(const UBSButton* Button)
{
	MenuSwitcher->SetActiveWidget(Cast<UMenuButton>(Button)->GetBox());
}
