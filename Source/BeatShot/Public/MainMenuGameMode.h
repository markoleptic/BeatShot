// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "GameFramework/GameModeBase.h"
#include "MainMenuGameMode.generated.h"

/** The game mode for the main menu */
UCLASS()
class BEATSHOT_API AMainMenuGameMode : public AGameModeBase, public ISaveLoadInterface
{
	GENERATED_BODY()

	AMainMenuGameMode();

	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundClass* GlobalSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundClass* MenuSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundMix* GlobalSoundMix;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UAudioComponent* MainMenuMusicComp;
};
