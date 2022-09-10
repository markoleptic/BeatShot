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
	UFUNCTION(BlueprintImplementableEvent)
	void StopAAPlayerAndTracker();

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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameModeActor Spawning")
	AGameModeActorBase* GameModeActorBase;

	UFUNCTION(BlueprintCallable, Category = "GameModeActor Spawning")
	AGameModeActorBase* SetGameModeActorBase(EGameModeActorName GameModeActorEnum);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
	TArray<float> BandLimitsThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
	TArray<FVector2D> BandLimits;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
	float TimeWindow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
	int HistorySize;
};
