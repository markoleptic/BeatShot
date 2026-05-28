// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Menus/HelpWidget.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "Utilities/Buttons/MenuButton.h"

void UHelpWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MenuButton_GameModes->SetDefaults(Box_GameModes, MenuButton_Scoring);
	MenuButton_Scoring->SetDefaults(Box_Scoring, MenuButton_AudioAnalyzer);
	MenuButton_AudioAnalyzer->SetDefaults(Box_AudioAnalyzer, MenuButton_GameModes);

	MenuButton_GameModes->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_BSButton);
	MenuButton_Scoring->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_BSButton);
	MenuButton_AudioAnalyzer->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_BSButton);

	MenuButton_GameModes->SetActive();
	Switcher->SetActiveWidget(MenuButton_GameModes->GetAssociatedWidget());
}

void UHelpWidget::OnButtonClicked_BSButton(const UBSButton* Button)
{
	Switcher->SetActiveWidget(Cast<UMenuButton>(Button)->GetAssociatedWidget());
}
