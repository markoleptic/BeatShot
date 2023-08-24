// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "GameFramework/GameModeBase.h"
#include "MainMenuGameMode.generated.h"

class UGameModesWidget;
class ATargetManagerPreview;
class ABSPlayerController;
class UTargetWidget;

/** The game mode for the main menu */
UCLASS()
class BEATSHOT_API AMainMenuGameMode : public AGameModeBase, public ISaveLoadInterface
{
	GENERATED_BODY()

public:
	
	AMainMenuGameMode();
	void BindControllerToTargetManager(ABSPlayerController* InController, UGameModesWidget* GameModesWidget);
	virtual void BeginPlay() override;

	/** Manually calls OnAudioAnalyzerTick in TargetManager at fixed intervals */
	void SimulateTargetManager();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundClass* GlobalSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundClass* MenuSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundMix* GlobalSoundMix;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UAudioComponent* MainMenuMusicComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Classes")
	TSubclassOf<ATargetManagerPreview> TargetManagerClass;
	
	/** The spawned TargetManager that provides preview logic for CustomGameModesWidget_Preview */
    UPROPERTY()
    TObjectPtr<ATargetManagerPreview> TargetManager;
	
	FTimerDelegate SimulateTargetManagerDelegate;
	FTimerHandle SimulateTargetManagerTimer;
	FTimerHandle TotalSimulationTimer;
};
