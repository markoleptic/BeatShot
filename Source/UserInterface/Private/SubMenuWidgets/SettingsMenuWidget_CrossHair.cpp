// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/SettingsMenuWidget_CrossHair.h"
#include "Components/Button.h"
#include "BSWidgetInterface.h"
#include "Components/EditableTextBox.h"
#include "Math/Color.h"
#include "Components/Slider.h"
#include "OverlayWidgets/CrossHairWidget.h"
#include "WidgetComponents/ColorSelectWidget.h"
#include "WidgetComponents/SavedTextWidget.h"

void USettingsMenuWidget_CrossHair::NativeConstruct()
{
	Super::NativeConstruct();

	Value_InnerOffset->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget_CrossHair::OnValueChanged_InnerOffset);
	Value_LineLength->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget_CrossHair::OnValueChanged_LineLength);
	Value_LineWidth->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget_CrossHair::OnValueChanged_LineWidth);
	Value_OutlineOpacity->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget_CrossHair::OnValueChanged_OutlineOpacity);
	Value_OutlineWidth->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget_CrossHair::OnValueChanged_OutlineWidth);

	Slider_InnerOffset->OnValueChanged.AddDynamic(this, &USettingsMenuWidget_CrossHair::OnSliderChanged_InnerOffset);
	Slider_LineLength->OnValueChanged.AddDynamic(this, &USettingsMenuWidget_CrossHair::OnSliderChanged_LineLength);
	Slider_LineWidth->OnValueChanged.AddDynamic(this, &USettingsMenuWidget_CrossHair::OnSliderChanged_LineWidth);
	Slider_OutlineOpacity->OnValueChanged.AddDynamic(this, &USettingsMenuWidget_CrossHair::OnSliderChanged_OutlineOpacity);
	Slider_OutlineWidth->OnValueChanged.AddDynamic(this, &USettingsMenuWidget_CrossHair::OnSliderChanged_OutlineWidth);

	Button_Reset->OnClicked.AddDynamic(this, &USettingsMenuWidget_CrossHair::OnButtonClicked_Reset);
	Button_Revert->OnClicked.AddDynamic(this, &USettingsMenuWidget_CrossHair::OnButtonClicked_Revert);
	Button_Save->OnClicked.AddDynamic(this, &USettingsMenuWidget_CrossHair::OnOnButtonClicked_Save);

	ColorSelectWidget->OnColorChanged.BindUFunction(this, "OnColorChanged");
	SavedTextWidget->SetSavedText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "SM_Save_CrossHair"));
	InitialCrossHairSettings = LoadPlayerSettings().CrossHair;
	NewCrossHairSettings = InitialCrossHairSettings;
	SetCrossHairOptions(InitialCrossHairSettings);
}

void USettingsMenuWidget_CrossHair::SetCrossHairOptions(const FPlayerSettings_CrossHair& CrossHairSettings)
{
	ColorSelectWidget->InitializeColor(CrossHairSettings.CrossHairColor);
	ColorSelectWidget->SetBorderColors(true, true);
	Value_InnerOffset->SetText(FText::AsNumber(CrossHairSettings.InnerOffset));
	Value_LineLength->SetText(FText::AsNumber(CrossHairSettings.LineLength));
	Value_LineWidth->SetText(FText::AsNumber(CrossHairSettings.LineWidth));
	Value_OutlineOpacity->SetText(FText::AsNumber(roundf(CrossHairSettings.OutlineOpacity * 100)));
	Value_OutlineWidth->SetText(FText::AsNumber(CrossHairSettings.OutlineWidth));

	Slider_InnerOffset->SetValue(CrossHairSettings.InnerOffset);
	Slider_LineLength->SetValue(CrossHairSettings.LineLength);
	Slider_LineWidth->SetValue(CrossHairSettings.LineWidth);
	Slider_OutlineOpacity->SetValue(roundf(CrossHairSettings.OutlineOpacity * 100));
	Slider_OutlineWidth->SetValue(CrossHairSettings.OutlineWidth);

	CrossHairWidget->InitializeCrossHair(CrossHairSettings);
}

void USettingsMenuWidget_CrossHair::OnColorChanged(const FLinearColor& NewColor)
{
	NewCrossHairSettings.CrossHairColor = NewColor;
	CrossHairWidget->SetImageColor(NewCrossHairSettings.CrossHairColor);
}

void USettingsMenuWidget_CrossHair::OnValueChanged_InnerOffset(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float SnappedValue = IBSWidgetInterface::OnEditableTextBoxChanged(NewValue, Value_InnerOffset, Slider_InnerOffset, 1, 0, 30);
	NewCrossHairSettings.InnerOffset = SnappedValue;
	CrossHairWidget->SetInnerOffset(SnappedValue);
}

void USettingsMenuWidget_CrossHair::OnValueChanged_LineLength(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float SnappedValue = IBSWidgetInterface::OnEditableTextBoxChanged(NewValue, Value_LineLength, Slider_LineLength, 1, 0, 100);
	NewCrossHairSettings.LineLength = SnappedValue;
	CrossHairWidget->SetLineLength(SnappedValue);
}

void USettingsMenuWidget_CrossHair::OnValueChanged_LineWidth(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float SnappedValue = IBSWidgetInterface::OnEditableTextBoxChanged(NewValue, Value_LineWidth, Slider_LineWidth, 1, 0, 100);
	NewCrossHairSettings.LineWidth = SnappedValue;
	CrossHairWidget->SetLineWidth(SnappedValue);
}

void USettingsMenuWidget_CrossHair::OnValueChanged_OutlineWidth(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float SnappedValue = IBSWidgetInterface::OnEditableTextBoxChanged(NewValue, Value_OutlineWidth, Slider_OutlineWidth, 1, 0, 50);
	NewCrossHairSettings.OutlineWidth = SnappedValue;
	CrossHairWidget->SetOutlineWidth(SnappedValue);
}

void USettingsMenuWidget_CrossHair::OnValueChanged_OutlineOpacity(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float SnappedValue = IBSWidgetInterface::OnEditableTextBoxChanged(NewValue, Value_OutlineOpacity, Slider_OutlineOpacity, 1, 0, 100);
	NewCrossHairSettings.OutlineOpacity = SnappedValue / 100;
	CrossHairWidget->SetOutlineOpacity(SnappedValue / 100);
}

void USettingsMenuWidget_CrossHair::OnSliderChanged_InnerOffset(const float NewValue)
{
	const float SnappedValue = IBSWidgetInterface::OnSliderChanged(NewValue, Value_InnerOffset, 1);
	NewCrossHairSettings.InnerOffset = SnappedValue;
	CrossHairWidget->SetInnerOffset(SnappedValue);
}

void USettingsMenuWidget_CrossHair::OnSliderChanged_LineLength(const float NewValue)
{
	const float SnappedValue = IBSWidgetInterface::OnSliderChanged(NewValue, Value_LineLength, 1);
	NewCrossHairSettings.LineLength = SnappedValue;
	CrossHairWidget->SetLineLength(SnappedValue);
}

void USettingsMenuWidget_CrossHair::OnSliderChanged_LineWidth(const float NewValue)
{
	const float SnappedValue = IBSWidgetInterface::OnSliderChanged(NewValue, Value_LineWidth, 1);
	NewCrossHairSettings.LineWidth = SnappedValue;
	CrossHairWidget->SetLineWidth(SnappedValue);
}

void USettingsMenuWidget_CrossHair::OnSliderChanged_OutlineOpacity(const float NewValue)
{
	const float SnappedValue = IBSWidgetInterface::OnSliderChanged(NewValue, Value_OutlineOpacity, 1);
	NewCrossHairSettings.OutlineOpacity = SnappedValue / 100;
	CrossHairWidget->SetOutlineOpacity(SnappedValue / 100);
}

void USettingsMenuWidget_CrossHair::OnSliderChanged_OutlineWidth(const float NewValue)
{
	const float SnappedValue = IBSWidgetInterface::OnSliderChanged(NewValue, Value_OutlineWidth, 1);
	NewCrossHairSettings.OutlineWidth = SnappedValue;
	CrossHairWidget->SetOutlineWidth(SnappedValue);
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

void USettingsMenuWidget_CrossHair::OnOnButtonClicked_Save()
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
