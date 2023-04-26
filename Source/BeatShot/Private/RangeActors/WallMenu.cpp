// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "RangeActors/WallMenu.h"
#include "BSCharacter.h"
#include "BSGameInstance.h"
#include "Kismet/GameplayStatics.h"

void AWallMenu::BeginPlay()
{
	Super::BeginPlay();
	UBSGameInstance* GI = Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GI->AddDelegateToOnPlayerSettingsChanged(OnPlayerSettingsChangedDelegate_Game);
	GI->GetPublicGameSettingsChangedDelegate().AddUniqueDynamic(this, &AWallMenu::OnPlayerSettingsChanged_Game);
	GI->GetPublicUserSettingsChangedDelegate().AddUniqueDynamic(this, &AWallMenu::OnPlayerSettingsChanged_User);
	InitializeSettings(LoadPlayerSettings().Game, LoadPlayerSettings().User);
}

FPlayerSettings AWallMenu::LoadPlayerSettings() const
{
	return ISaveLoadInterface::LoadPlayerSettings();
}

void AWallMenu::SavePlayerSettings(const FPlayerSettings_Game& InGameSettings)
{
	ISaveLoadInterface::SavePlayerSettings(InGameSettings);
}

void AWallMenu::OnPlayerSettingsChanged_Game(const FPlayerSettings_Game& GameSettings)
{
	InitializeSettings(GameSettings, LoadPlayerSettings().User);
}

void AWallMenu::OnPlayerSettingsChanged_User(const FPlayerSettings_User& UserSettings)
{
	InitializeSettings(LoadPlayerSettings().Game, UserSettings);
}
