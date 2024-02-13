// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Equipment/BSKnife.h"
#include "BSGameInstance.h"
#include "SaveGamePlayerSettings.h"


ABSKnife::ABSKnife()
{
	PrimaryActorTick.bCanEverTick = false;

	Knife = CreateDefaultSubobject<UStaticMeshComponent>("Knife");
	SetRootComponent(Knife);
}

void ABSKnife::BeginPlay()
{
	Super::BeginPlay();

	OnPlayerSettingsChanged_Game(LoadPlayerSettings().Game);

	UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance());
	GI->GetPublicGameSettingsChangedDelegate().AddUObject(this, &ThisClass::OnPlayerSettingsChanged_Game);
}

void ABSKnife::OnPlayerSettingsChanged_Game(const FPlayerSettings_Game& GameSettings)
{
	SetShowKnifeMesh(GameSettings.bShowWeaponMesh);
}

void ABSKnife::SetShowKnifeMesh(const bool bShow)
{
	SetActorHiddenInGame(!bShow);
}
