// Fill out your copyright notice in the Description page of Project Settings.


#include "WallMenu.h"
#include "DefaultCharacter.h"
#include "Kismet/GameplayStatics.h"

void AWallMenu::BeginPlay()
{
	Super::BeginPlay();
	if (ADefaultCharacter* DefaultCharacter = Cast<ADefaultCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
	{
		OnPlayerSettingsChange.AddUniqueDynamic(DefaultCharacter, &ADefaultCharacter::OnUserSettingsChange);
	}
}

void AWallMenu::Destroyed()
{
	if (ADefaultCharacter* DefaultCharacter = Cast<ADefaultCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
	{
		if (OnPlayerSettingsChange.IsAlreadyBound(DefaultCharacter, &ADefaultCharacter::OnUserSettingsChange))
		{
			OnPlayerSettingsChange.RemoveDynamic(DefaultCharacter, &ADefaultCharacter::OnUserSettingsChange);
		}
	}
	Super::Destroyed();
}

FPlayerSettings AWallMenu::LoadPlayerSettings() const
{
	return ISaveLoadInterface::LoadPlayerSettings();
}

void AWallMenu::SavePlayerSettings(const FPlayerSettings PlayerSettingsToSave)
{
	ISaveLoadInterface::SavePlayerSettings(PlayerSettingsToSave);
	OnPlayerSettingsChange.Broadcast(PlayerSettingsToSave);
}
