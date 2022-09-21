// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameModeActorStruct.h"
#include "GameFramework/GameModeBase.h"
#include "DefaultGameMode.generated.h"

class ABeatTrack;
class UDefaultGameInstance;
class AWideSpreadMultiBeat;
class AGameModeActorBase;

UCLASS()
class BEATSHOT_API ADefaultGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:

	// Reference Game Instance
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	UDefaultGameInstance* GI;

	// Blueprint event used to stop BPAAPlayer and BPAATracker during pause game
	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode Initialization")
	void StopAAPlayerAndTracker();

	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode Initialization")
	void InitializeAudioManagers();

	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode Initialization")
	void PauseAAManagers();

	UFUNCTION(BlueprintCallable, Category = "GameModeActor Spawning")
	void InitializeGameMode(EGameModeActorName GameModeActorName);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameModeActor Spawning")
	TSubclassOf<AGameModeActorBase> WideSpreadMultiBeatClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameModeActor Spawning")
	TSubclassOf<AGameModeActorBase> NarrowSpreadMultiBeatClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameModeActor Spawning")
	TSubclassOf<AGameModeActorBase> NarrowSpreadSingleBeatClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameModeActor Spawning")
	TSubclassOf<AGameModeActorBase> WideSpreadSingleBeatClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameModeActor Spawning")
	TSubclassOf<AGameModeActorBase> BeatTrackClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameModeActor Spawning")
	TSubclassOf<AGameModeActorBase> CustomBeatClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameModeActor Spawning")
	TSubclassOf<AActor> VisualizerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameModeActor Spawning")
	AActor* Visualizer;

	// the GameModeActor that was spawned
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameModeActor Spawning")
	AGameModeActorBase* GameModeActorBase;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
		FAASettingsStruct AASettings;

	UFUNCTION(BlueprintCallable, Category = "Bound functions")
		void RefreshAASettings();

	UFUNCTION(BlueprintCallable, Category = "GameModeActor Spawning")
		void SetGameModeActorDestroyed(AActor* DestroyedActor);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
	bool GameModeActorAlive;

};
