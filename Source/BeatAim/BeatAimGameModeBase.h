// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameModeActorStruct.h"
#include "GameFramework/GameModeBase.h"
#include "BeatAimGameModeBase.generated.h"

class UDefaultGameInstance;
class AWideSpreadMultiBeat;
class AGameModeActorBase;
UCLASS()
class BEATAIM_API ABeatAimGameModeBase : public AGameModeBase
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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameModeActor Spawning")
	AGameModeActorBase* GameModeActorBase;

	UFUNCTION(BlueprintCallable, Category = "GameModeActor Spawning")
	AGameModeActorBase* SetGameModeActorBase(EGameModeActorName GameModeActorEnum);
};
