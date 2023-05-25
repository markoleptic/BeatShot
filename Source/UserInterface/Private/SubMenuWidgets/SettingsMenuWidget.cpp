// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/SettingsMenuWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Components/VerticalBox.h"
#include "Components/Slider.h"
#include "SubMenuWidgets/SettingsMenuWidget_AudioAnalyzer.h"
#include "WidgetComponents/MenuButton.h"

void USettingsMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MenuButton_Game->SetDefaults(Game, MenuButton_VideoAndSound);
	MenuButton_VideoAndSound->SetDefaults(VideoAndSound, MenuButton_AudioAnalyzer);
	MenuButton_AudioAnalyzer->SetDefaults(AudioAnalyzer, MenuButton_Sensitivity);
	MenuButton_Sensitivity->SetDefaults(Sensitivity, MenuButton_CrossHair);
	MenuButton_CrossHair->SetDefaults(CrossHair, MenuButton_Game);
	
	MenuButton_Game->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	MenuButton_VideoAndSound->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	MenuButton_AudioAnalyzer->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	MenuButton_Sensitivity->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
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
