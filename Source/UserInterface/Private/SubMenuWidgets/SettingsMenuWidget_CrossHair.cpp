// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/SettingsMenuWidget_CrossHair.h"
#include "BSWidgetInterface.h"
#include "Math/Color.h"
#include "OverlayWidgets/CrossHairWidget.h"
#include "WidgetComponents/Buttons/BSButton.h"
#include "WidgetComponents/SavedTextWidget.h"
#include "WidgetComponents/MenuOptionWidgets/ColorSelectOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/SliderTextBoxWidget.h"

void USettingsMenuWidget_CrossHair::NativeConstruct()
{
	Super::NativeConstruct();

	MenuOption_InnerOffset->SetValues(0, 30, 1);
	MenuOption_LineLength->SetValues(0, 100, 1);
	MenuOption_LineWidth->SetValues(0, 100, 1);
	MenuOption_OutlineOpacity->SetValues(0, 100, 1);
	MenuOption_OutlineWidth->SetValues(0, 50, 1);
	
	MenuOption_InnerOffset->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	MenuOption_LineLength->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	MenuOption_LineWidth->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	MenuOption_OutlineOpacity->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	MenuOption_OutlineWidth->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);

	Button_Reset->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	Button_Revert->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	Button_Save->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	
	Button_Reset->SetDefaults(static_cast<uint8>(ESettingButtonType::Reset));
	Button_Revert->SetDefaults(static_cast<uint8>(ESettingButtonType::Revert));
	Button_Save->SetDefaults(static_cast<uint8>(ESettingButtonType::Save));

	MenuOption_ColorSelect->OnColorChanged.BindUFunction(this, "OnColorChanged");
	SavedTextWidget->SetSavedText(FText::FromString("CrossHair Settings Saved"));
	InitialCrossHairSettings = LoadPlayerSettings().CrossHair;
	NewCrossHairSettings = InitialCrossHairSettings;
	SetCrossHairOptions(InitialCrossHairSettings);
	
	UpdateBrushColors();
}

void USettingsMenuWidget_CrossHair::SetCrossHairOptions(const FPlayerSettings_CrossHair& CrossHairSettings)
{
	MenuOption_ColorSelect->InitializeColor(CrossHairSettings.CrossHairColor);
	MenuOption_InnerOffset->SetValue(CrossHairSettings.InnerOffset);
	MenuOption_LineLength->SetValue(CrossHairSettings.LineLength);
	MenuOption_LineWidth->SetValue(CrossHairSettings.LineWidth);
	MenuOption_OutlineOpacity->SetValue(CrossHairSettings.OutlineOpacity * 100);
	MenuOption_OutlineWidth->SetValue(CrossHairSettings.OutlineWidth);
	
	CrossHairWidget->InitializeCrossHair(CrossHairSettings);
}

void USettingsMenuWidget_CrossHair::OnSliderTextBoxValueChanged(USliderTextBoxWidget* Widget, const float Value)
{
	if (Widget == MenuOption_InnerOffset)
	{
		NewCrossHairSettings.InnerOffset = Value;
		CrossHairWidget->SetInnerOffset(Value);
	}
	else if (Widget == MenuOption_LineLength)
	{
		NewCrossHairSettings.LineLength = Value;
		CrossHairWidget->SetLineLength(Value);
	}
	else if (Widget == MenuOption_LineWidth)
	{
		NewCrossHairSettings.LineWidth = Value;
		CrossHairWidget->SetLineWidth(Value);
	}
	else if (Widget == MenuOption_OutlineOpacity)
	{
		NewCrossHairSettings.OutlineOpacity = Value / 100.f;
		CrossHairWidget->SetOutlineOpacity(Value / 100.f);
	}
	else if (Widget == MenuOption_OutlineWidth)
	{
		NewCrossHairSettings.OutlineWidth = Value;
		CrossHairWidget->SetOutlineWidth(Value);
	}
}

void USettingsMenuWidget_CrossHair::OnColorChanged(const FLinearColor& NewColor)
{
	NewCrossHairSettings.CrossHairColor = NewColor;
	CrossHairWidget->SetImageColor(NewCrossHairSettings.CrossHairColor);
}

void USettingsMenuWidget_CrossHair::OnButtonClicked_Reset()
{
	NewCrossHairSettings = FPlayerSettings_CrossHair();
	SetCrossHairOptions(NewCrossHairSettings);
}

void USettingsMenuWidget_CrossHair::OnButtonClicked_Revert()
{
	NewCrossHairSettings = InitialCrossHairSettings;
	SetCrossHairOptions(NewCrossHairSettings);
}

void USettingsMenuWidget_CrossHair::OnButtonClicked_Save()
{
	/** Load settings again in case the user changed other settings before navigating to crosshair settings */
	FPlayerSettings_CrossHair Settings;
	Settings.LineWidth = NewCrossHairSettings.LineWidth;
	Settings.LineLength = NewCrossHairSettings.LineLength;
	Settings.InnerOffset = NewCrossHairSettings.InnerOffset;
	Settings.CrossHairColor = NewCrossHairSettings.CrossHairColor;
	Settings.OutlineOpacity = NewCrossHairSettings.OutlineOpacity;
	Settings.OutlineWidth = NewCrossHairSettings.OutlineWidth;
	SavePlayerSettings(Settings);
	SavedTextWidget->PlayFadeInFadeOut();
	InitialCrossHairSettings = NewCrossHairSettings;
}

void USettingsMenuWidget_CrossHair::OnButtonClicked_BSButton(const UBSButton* Button)
{
	switch (static_cast<ESettingButtonType>(Button->GetEnumValue())) {
	case ESettingButtonType::Save:
		OnButtonClicked_Save();
		break;
	case ESettingButtonType::Reset:
		OnButtonClicked_Reset();
		break;
	case ESettingButtonType::Revert:
		OnButtonClicked_Revert();
		break;
	default:
		break;
	}
}
