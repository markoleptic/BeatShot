// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TargetSpawner.generated.h"

class ABeatAimGameModeBase;
class UDefaultGameInstance;
class ADefaultCharacter;
class ASphereTarget;
class UBoxComponent;
UCLASS()
class BEATAIM_API ATargetSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATargetSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Reference to Game Instance
	UDefaultGameInstance* GI;

	UFUNCTION(BlueprintCallable)
	void SpawnActor();

	// Lets the spawner know it's safe to spawn a target
	void SetShouldSpawn(bool bShouldSpawn);

	UFUNCTION()
		float GetTimeBasedScore(float TimeElapsed, float ScoreMultiplier);

	UFUNCTION()
		float GetTimeSinceSpawn(FTimerHandle TimerHandle);

	FTimerHandle TimeSinceSpawn;

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Properties", meta = (AllowPrivateAccess = true))
		bool ShouldSpawn;

	UPROPERTY(EditDefaultsOnly, Category = "Spawn Properties")
		FBoxSphereBounds BoxBounds;

	UPROPERTY(VisibleAnywhere, Category = "Spawn Properties")
		bool LastTargetSpawnedCenter = false;

	UPROPERTY(EditDefaultsOnly, Category = "Spawn Properties")
		UBoxComponent* SpawnBox;

	UPROPERTY(EditAnywhere, Category = "Spawn Properties")
		TSubclassOf<ASphereTarget> ActorToSpawn;

	UFUNCTION()
	void OnTargetDestroyed(AActor* DestroyedActor);

	// Randomize scale of target
	void RandomizeScale(ASphereTarget* Target);

	// Randomize location of target
	void RandomizeLocation();

	// Location to spawn the target
	FVector SpawnLocation;
protected:
};
