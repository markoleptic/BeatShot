// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/SettingsMenuWidget_Input.h"
#include "WidgetComponents/InputMappingWidget.h"
#include "EnhancedActionKeyMapping.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

void USettingsMenuWidget_Input::NativeConstruct()
{
	Super::NativeConstruct();
	InitializeInputSettings();
}

void USettingsMenuWidget_Input::InitializeInputSettings()
{
	FPlayerSettings_User Settings_User = LoadPlayerSettings().User;

	/*UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetOwningPlayer()->GetLocalPlayer());
	check(Subsystem);
	for (TPair<FName, FKey> Keybinding : Settings_User.Keybindings)
	{
		Subsystem->AddPlayerMappedKeyInSlot(Keybinding.Key, Keybinding.Value);
	}*/
	//TArray<FKey> Keys = Subsystem->QueryKeysMappedToAction(Mapping.Action.Get());
	
	TArray<FEnhancedActionKeyMapping> Mappings = PlayerMappableInputConfig->GetPlayerMappableKeys();
	for (const FEnhancedActionKeyMapping& Mapping : Mappings)
	{
		if (!Mapping.IsPlayerMappable())
		{
			continue;
		}
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
	}
	UpdateBrushColors();
}

void USettingsMenuWidget_Input::OnKeySelected(const FName KeyName, const FInputChord SelectedKey)
{
	UE_LOG(LogTemp, Display, TEXT("NewKeySelected for %s: %s"), *KeyName.ToString(), *SelectedKey.GetInputText().ToString());
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetOwningPlayer()->GetLocalPlayer());
	Subsystem->AddPlayerMappedKeyInSlot(KeyName, SelectedKey.Key);
}
