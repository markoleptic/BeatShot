// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/FAQWidget.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "WidgetComponents/Buttons/MenuButton.h"

void UFAQWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MenuButton_GameModes->SetDefaults(Box_GameModes, MenuButton_Scoring);
	MenuButton_Scoring->SetDefaults(Box_Scoring, MenuButton_AudioAnalyzer);
	MenuButton_AudioAnalyzer->SetDefaults(Box_AudioAnalyzer, MenuButton_GameModes);

	MenuButton_GameModes->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	MenuButton_Scoring->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	MenuButton_AudioAnalyzer->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);

	MenuButton_GameModes->SetActive();
}

void UFAQWidget::OnButtonClicked_BSButton(const UBSButton* Button)
{
	FAQSwitcher->SetActiveWidget(Cast<UMenuButton>(Button)->GetBox());
}
