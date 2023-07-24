// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/SettingsMenuWidget_Input.h"
#include "WidgetComponents/InputMappingWidget.h"
#include "EnhancedActionKeyMapping.h"
#include "EnhancedInputSubsystems.h"
#include "Components/EditableTextBox.h"
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
	for (UInputMappingWidget* InputMappingWidget : InputMappingWidgets)
	{
		InputMappingWidget->RemoveFromParent();
	}
	InputMappingWidgets.Empty();
	
	Sensitivity = PlayerSettings_User.Sensitivity;
	TempKeybindings = PlayerSettings_User.Keybindings;
	Slider_Sensitivity->SetValue(Sensitivity);
	Value_CurrentSensitivity->SetText(FText::AsNumber(Sensitivity));

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetOwningPlayer()->GetLocalPlayer());
	check(Subsystem);
	
	for (const TPair<FName, FKey>& Keybinding : TempKeybindings)
	{
		Subsystem->AddPlayerMappedKeyInSlot(Keybinding.Key, Keybinding.Value);
	}
	
	TArray<FEnhancedActionKeyMapping> Mappings = Subsystem->GetAllPlayerMappableActionKeyMappings();
	TArray<TObjectPtr<const UInputAction>> Actions;
	
	for (FEnhancedActionKeyMapping& Mapping : Mappings)
	{
		Actions.AddUnique(Mapping.Action);
	}
	
	for (const TObjectPtr<const UInputAction> Action : Actions)
	{
		TArray<FEnhancedActionKeyMapping> ActionKeyMappings = Mappings.FilterByPredicate([&] (const FEnhancedActionKeyMapping& Mapping)
		{
			return Mapping.Action == Action;
		});
		
		FPlayerMappableKeyOptions Options = ActionKeyMappings[0].PlayerMappableOptions;
		UInputMappingWidget* InputMappingWidget = CreateWidget<UInputMappingWidget>(this, InputMappingWidgetClass);
		
		if (Options.DisplayCategory.EqualTo(FText::FromString("Combat")))
		{
			BSBox_Combat->AddChildToVerticalBox(InputMappingWidget);
		}
		else if (Options.DisplayCategory.EqualTo(FText::FromString("Movement")))
		{
			BSBox_Movement->AddChildToVerticalBox(InputMappingWidget);
		}
		
		InputMappingWidget->Init(ActionKeyMappings);
		InputMappingWidget->OnKeySelected.AddUObject(this, &ThisClass::OnKeySelected);
		
		InputMappingWidgets.Add(InputMappingWidget);
	}
	
	/*for (const FEnhancedActionKeyMapping& Mapping : Mappings)
	{
		FPlayerMappableKeyOptions Options = Mapping.PlayerMappableOptions;
		
		UInputMappingWidget* InputMappingWidget = CreateWidget<UInputMappingWidget>(this, InputMappingWidgetClass);
		
		if (Options.DisplayCategory.EqualTo(FText::FromString("Combat")))
		{
			BSBox_Combat->AddChildToVerticalBox(InputMappingWidget);
		}
		else if (Options.DisplayCategory.EqualTo(FText::FromString("Movement")))
		{
			BSBox_Movement->AddChildToVerticalBox(InputMappingWidget);
		}
		
		TArray<FEnhancedActionKeyMapping> ActionKeyMappings = PlayerMappableInputConfig->GetKeysBoundToAction(Mapping.Action.Get());
		InputMappingWidget->Init(ActionKeyMappings);
		InputMappingWidget->OnKeySelected.AddUObject(this, &ThisClass::OnKeySelected);

		UE_LOG(LogTemp, Display, TEXT("MappingName: %s"), *Mapping.GetMappingName().ToString());
	}*/
	UpdateBrushColors();
}

void USettingsMenuWidget_Input::OnKeySelected(const FName MappingName, const FInputChord SelectedKey)
{
	UE_LOG(LogTemp, Display, TEXT("NewKeySelected for %s: %s"), *MappingName.ToString(), *SelectedKey.GetInputText().ToString());
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetOwningPlayer()->GetLocalPlayer());
	TArray<FEnhancedActionKeyMapping> Mappings = Subsystem->GetAllPlayerMappableActionKeyMappings();
	
	TArray<UInputMappingWidget*> Matching = FindInputMappingWidgetsByKey(SelectedKey.Key);
	if (!Matching.IsEmpty())
	{
		for (UInputMappingWidget* InputMappingWidget : Matching)
		{
			FName FoundMappingName = InputMappingWidget->GetMappingNameForKey(SelectedKey.Key);
			if (FoundMappingName != NAME_None && FoundMappingName != MappingName)
			{
				const FName OldMappingName = InputMappingWidget->SetKey(SelectedKey.Key, FKey());
				Subsystem->RemovePlayerMappedKeyInSlot(OldMappingName);
			}
		}
	}

	Subsystem->AddPlayerMappedKeyInSlot(MappingName, SelectedKey.Key);
	
	TempKeybindings.FindOrAdd(MappingName) = SelectedKey.Key;
	
	for (TPair<FName, FKey> Pair : TempKeybindings)
	{
		UE_LOG(LogTemp, Display, TEXT("TempKeybindings:"));
		UE_LOG(LogTemp, Display, TEXT("%s %s"), *Pair.Key.ToString(), *Pair.Value.ToString());
	}
}

TArray<UInputMappingWidget*> USettingsMenuWidget_Input::FindInputMappingWidgetsByKey(const FKey InKey) const
{
	return InputMappingWidgets.FilterByPredicate([&] (const UInputMappingWidget* Widget)
	{
		return Widget->GetActionKeyMappings().ContainsByPredicate([&] (const FEnhancedActionKeyMapping& Mapping)
		{
			return Mapping.Key == InKey;
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
	Settings_User.Keybindings = TempKeybindings;
	Value_CurrentSensitivity->SetText(FText::AsNumber(Slider_Sensitivity->GetValue()));
	SavePlayerSettings(Settings_User);
	SavedTextWidget->SetSavedText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "SM_Saved_Sensitivity"));
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
