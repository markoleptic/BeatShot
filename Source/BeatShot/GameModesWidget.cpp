// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModesWidget.h"
#include "DefaultGameInstance.h"
#include "SaveGameCustomGameMode.h"
#include "Kismet/GameplayStatics.h"

void UGameModesWidget::NativeConstruct()
{
	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));

	if (UGameplayStatics::DoesSaveGameExist(TEXT("CustomGameModesSlot"), 3))
	{
		SaveGameCustomGameMode = Cast<USaveGameCustomGameMode>(UGameplayStatics::LoadGameFromSlot(TEXT("CustomGameModesSlot"), 3));
	}
	else
	{
		SaveGameCustomGameMode = Cast<USaveGameCustomGameMode>(UGameplayStatics::CreateSaveGameObject(USaveGameCustomGameMode::StaticClass()));
	}

	LoadCustomGameModes();
	PopulateGameModeSettings();
}

void UGameModesWidget::SaveCustomGameMode()
{
	if (USaveGameCustomGameMode* SaveCustomGameModeObject = Cast<USaveGameCustomGameMode>(UGameplayStatics::CreateSaveGameObject(USaveGameCustomGameMode::StaticClass())))
	{
		SaveCustomGameModeObject->CustomGameModesMap = CustomGameModesMap;
		if (UGameplayStatics::SaveGameToSlot(SaveCustomGameModeObject, TEXT("CustomGameModesSlot"), 3))
		{
			UE_LOG(LogTemp, Warning, TEXT("SaveCustomGameModes Succeeded"));
		}
	}
}

void UGameModesWidget::ResetCustomGameMode()
{
	CustomGameMode.ResetStruct();
	PopulateGameModeSettings();
}

TMap<FString, FGameModeActorStruct> UGameModesWidget::LoadCustomGameModes()
{
	if (USaveGameCustomGameMode* SaveGameCustomGameModeObject = Cast<USaveGameCustomGameMode>(UGameplayStatics::LoadGameFromSlot(TEXT("CustomGameModesSlot"), 3)))
	{
		CustomGameModesMap = SaveGameCustomGameModeObject->CustomGameModesMap;
		return SaveGameCustomGameModeObject->CustomGameModesMap;
	}
	return CustomGameModesMap;
}
