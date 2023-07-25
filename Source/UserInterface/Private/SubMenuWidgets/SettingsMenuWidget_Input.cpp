// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/SettingsMenuWidget_Input.h"
#include "WidgetComponents/InputMappingWidget.h"
#include "EnhancedActionKeyMapping.h"
#include "EnhancedInputSubsystems.h"
#include "Components/EditableTextBox.h"
#include "Components/InputKeySelector.h"
#include "Components/ScrollBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetMathLibrary.h"
#include "WidgetComponents/BSButton.h"
#include "WidgetComponents/SavedTextWidget.h"

void USettingsMenuWidget_Input::NativeConstruct()
{
	Super::NativeConstruct();
	
	Value_NewSensitivity->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget_Input::OnValueChanged_NewSensitivity);
	Value_NewSensitivityCsgo->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget_Input::OnValueChanged_NewSensitivityCsgo);
	Slider_Sensitivity->OnValueChanged.AddDynamic(this, &USettingsMenuWidget_Input::OnSliderChanged_Sensitivity);

	Button_Reset->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	Button_Revert->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	Button_Save->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	
	Button_Reset->SetDefaults(static_cast<uint8>(ESettingButtonType::Reset));
	Button_Revert->SetDefaults(static_cast<uint8>(ESettingButtonType::Revert));
	Button_Save->SetDefaults(static_cast<uint8>(ESettingButtonType::Save));

	InitialPlayerSettings = LoadPlayerSettings().User;
	InitializeInputSettings(InitialPlayerSettings);
}

void USettingsMenuWidget_Input::InitializeInputSettings(const FPlayerSettings_User& PlayerSettings_User)
{
	// Remove any existing InputMappingWidgets
	for (UInputMappingWidget* InputMappingWidget : InputMappingWidgets)
	{
		InputMappingWidget->RemoveFromParent();
	}
	InputMappingWidgets.Empty();
	
	Sensitivity = PlayerSettings_User.Sensitivity;
	TempKeybindings = PlayerSettings_User.Keybindings;
	Slider_Sensitivity->SetValue(Sensitivity);
	Value_CurrentSensitivity->SetText(FText::AsNumber(Sensitivity));
	
	TArray<FEnhancedActionKeyMapping> Mappings = PlayerMappableInputConfig->GetPlayerMappableKeys();
	TArray<TObjectPtr<const UInputAction>> Actions;

	// Extract all unique actions from PlayerMappable
	for (FEnhancedActionKeyMapping& Mapping : Mappings)
	{
		Actions.AddUnique(Mapping.Action);
	}
	
	// Extract all unique actions from PlayerMappable
	for (const TObjectPtr<const UInputAction> Action : Actions)
	{
		// Find all Action Key Mappings for this Action
		TArray<FEnhancedActionKeyMapping> ActionKeyMappings = Mappings.FilterByPredicate([&] (const FEnhancedActionKeyMapping& Mapping)
		{
			return Mapping.Action == Action;
		});

		// Replace user bound keybindings
		for (FEnhancedActionKeyMapping& Mapping : ActionKeyMappings)
		{
			if (const FKey* Found = TempKeybindings.Find(Mapping.GetMappingName()))
			{
				Mapping.Key = *Found;
			}
		}
		
		FText DisplayCategory = ActionKeyMappings[0].PlayerMappableOptions.DisplayCategory;
		UInputMappingWidget* InputMappingWidget = CreateWidget<UInputMappingWidget>(this, InputMappingWidgetClass);

		// Place new widget in vertical box for category
		if (DisplayCategory.EqualTo(FText::FromString("Combat")))
		{
			BSBox_Combat->AddChildToVerticalBox(InputMappingWidget);
		}
		else if (DisplayCategory.EqualTo(FText::FromString("Movement")))
		{
			BSBox_Movement->AddChildToVerticalBox(InputMappingWidget);
		}
		
		InputMappingWidget->Init(ActionKeyMappings);
		InputMappingWidget->OnKeySelected.AddUObject(this, &ThisClass::OnKeySelected);
		InputMappingWidget->OnIsSelectingKey.AddUObject(this, &ThisClass::OnIsSelectingKey);
		
		InputMappingWidgets.Add(InputMappingWidget);
	}

	UpdateBrushColors();
}

void USettingsMenuWidget_Input::OnKeySelected(const FName MappingName, const FInputChord SelectedKey)
{
	//UE_LOG(LogTemp, Display, TEXT("NewKeySelected for %s: %s"), *MappingName.ToString(), *SelectedKey.GetInputText().ToString());

	// Key is Empty
	if (SelectedKey.Key == FKey())
	{
		TempKeybindings.Remove(MappingName);
		return;
	}

	// Check if any other keys are bound to this key if they aren't Empty
	TArray<UInputMappingWidget*> Matching = FindInputMappingWidgetsByKey(SelectedKey.Key);

	if (!Matching.IsEmpty())
	{
		for (UInputMappingWidget* InputMappingWidget : Matching)
		{
			FName FoundMappingName = InputMappingWidget->GetMappingNameForKey(SelectedKey.Key);
			if (FoundMappingName != NAME_None && FoundMappingName != MappingName)
			{
				InputMappingWidget->SetKey(FoundMappingName, FKey());
			}
		}
	}
	
	TempKeybindings.FindOrAdd(MappingName) = SelectedKey.Key;
}

void USettingsMenuWidget_Input::OnIsSelectingKey(UInputKeySelector* KeySelector)
{
	// Lame workaround to allow Mouse Wheel Up and Mouse Wheel Down for InputKeySelectors
	if (KeySelector == CurrentKeySelector)
	{
		bIsSelectingKey = false;
		CurrentKeySelector = nullptr;
		ScrollBox->SetConsumeMouseWheel(EConsumeMouseWheel::WhenScrollingPossible);
		return;
	}

	ScrollBox->SetConsumeMouseWheel(EConsumeMouseWheel::Never);
	bIsSelectingKey = true;
	CurrentKeySelector = KeySelector;
}

FReply USettingsMenuWidget_Input::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// Lame workaround to allow Mouse Wheel Up and Mouse Wheel Down for InputKeySelectors
	if (bIsSelectingKey && CurrentKeySelector)
	{
		FInputChord Chord;
		if (InMouseEvent.GetWheelDelta() > 0)
		{
			Chord.Key = FKey(EKeys::MouseScrollUp);
		}
		else
		{
			Chord.Key = FKey(EKeys::MouseScrollDown);
		}
		CurrentKeySelector->SetSelectedKey(Chord);
		SetFocus();
		bIsSelectingKey = false;
		ScrollBox->SetConsumeMouseWheel(EConsumeMouseWheel::WhenScrollingPossible);
	}
	return Super::NativeOnMouseWheel(InGeometry, InMouseEvent);
}

TArray<UInputMappingWidget*> USettingsMenuWidget_Input::FindInputMappingWidgetsByKey(const FKey InKey) const
{
	return InputMappingWidgets.FilterByPredicate([&] (const UInputMappingWidget* Widget)
	{
		return Widget->GetActionKeyMappings().ContainsByPredicate([&] (const FEnhancedActionKeyMapping& Mapping)
		{
			return Mapping.Key == InKey && Mapping.Key != FKey();
		});
	});
}

void USettingsMenuWidget_Input::OnValueChanged_NewSensitivity(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float NewSensValue = FCString::Atof(*NewValue.ToString());
	Slider_Sensitivity->SetValue(NewSensValue);
	Value_NewSensitivityCsgo->SetText(FText::FromString(FString::SanitizeFloat(NewSensValue * CsgoMultiplier)));
	Sensitivity = NewSensValue;
}

void USettingsMenuWidget_Input::OnValueChanged_NewSensitivityCsgo(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float NewCsgoSensValue = FCString::Atof(*NewValue.ToString());
	Slider_Sensitivity->SetValue(NewCsgoSensValue / CsgoMultiplier);
	Value_NewSensitivity->SetText(FText::FromString(FString::SanitizeFloat(NewCsgoSensValue / CsgoMultiplier)));
	Sensitivity = NewCsgoSensValue / CsgoMultiplier;
}

void USettingsMenuWidget_Input::OnSliderChanged_Sensitivity(const float NewValue)
{
	const float NewSensValue = UKismetMathLibrary::GridSnap_Float(NewValue, 0.1);
	Slider_Sensitivity->SetValue(NewSensValue);
	Value_NewSensitivity->SetText(FText::FromString(FString::SanitizeFloat(NewSensValue)));
	Value_NewSensitivityCsgo->SetText(FText::FromString(FString::SanitizeFloat(NewSensValue * CsgoMultiplier)));
	Sensitivity = NewSensValue;
}

void USettingsMenuWidget_Input::OnButtonClicked_BSButton(const UBSButton* Button)
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

void USettingsMenuWidget_Input::OnButtonClicked_Save()
{
	FPlayerSettings_User Settings_User = LoadPlayerSettings().User;
	Settings_User.Sensitivity = Slider_Sensitivity->GetValue();
	Value_CurrentSensitivity->SetText(FText::AsNumber(Slider_Sensitivity->GetValue()));
	Settings_User.Keybindings = TempKeybindings;
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetOwningPlayer()->GetLocalPlayer());
	Subsystem->RemoveAllPlayerMappedKeys();
	for (const TPair<FName, FKey>& Keybinding : TempKeybindings)
	{
		Subsystem->AddPlayerMappedKeyInSlot(Keybinding.Key, Keybinding.Value);
	}
	
	SavePlayerSettings(Settings_User);
	SavedTextWidget->SetSavedText(GetWidgetTextFromKey("SM_Saved_Input"));
	SavedTextWidget->PlayFadeInFadeOut();
}

void USettingsMenuWidget_Input::OnButtonClicked_Reset()
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetOwningPlayer()->GetLocalPlayer());
	Subsystem->RemoveAllPlayerMappedKeys();
	TempKeybindings.Empty();
	FPlayerSettings_User Settings_User = LoadPlayerSettings().User;
	Settings_User.Keybindings = TempKeybindings;
	InitializeInputSettings(Settings_User);
}

void USettingsMenuWidget_Input::OnButtonClicked_Revert()
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetOwningPlayer()->GetLocalPlayer());
	Subsystem->RemoveAllPlayerMappedKeys();
	InitializeInputSettings(InitialPlayerSettings);
}
