// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameModeActorStruct.h"
#include "TargetSpawner.generated.h"

class ABeatAimGameModeBase;
class UDefaultGameInstance;
class ADefaultCharacter;
class ASphereTarget;
class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTargetSpawnSignature);

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	UDefaultGameInstance* GI;

	UFUNCTION(BlueprintCallable)
	void SpawnActor();

	UFUNCTION(BlueprintCallable)
	void SpawnSingleActor();

	UFUNCTION(BlueprintCallable)
	void SpawnTracker();

	UFUNCTION(BlueprintCallable)
	bool GetShouldSpawn();

	// Lets the spawner know it's safe to spawn a target
	void SetShouldSpawn(bool bShouldSpawn);

	UPROPERTY(BlueprintAssignable)
	FOnTargetSpawnSignature OnTargetSpawn;

	UPROPERTY(VisibleAnywhere)
	FTimerHandle TimeSinceSpawn;

	UPROPERTY(VisibleAnywhere)
	float NumTargetsAddedToArray;

	UFUNCTION(BlueprintCallable)
	void InitializeGameModeActor(FGameModeActorStruct NewGameModeActor);

	UFUNCTION(BlueprintCallable)
	void SetTargetSpawnCD(float NewTargetSpawnCD);

	// Tracking Variables and Functions

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Properties", meta = (AllowPrivateAccess = true))
	float TotalDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Properties", meta = (AllowPrivateAccess = true))
	float CurrentDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Properties", meta = (AllowPrivateAccess = true))
	float TrackingSpeed = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Properties", meta = (AllowPrivateAccess = true))
	FVector TrackingDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Properties", meta = (AllowPrivateAccess = true))
	ASphereTarget* TrackingTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Properties", meta = (AllowPrivateAccess = true))
	FVector StartLocation;


private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Properties", meta = (AllowPrivateAccess = true))
	FGameModeActorStruct GameModeActorStruct;

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

	UPROPERTY(EditAnywhere, Category = "Spawn Properties")
	float CheckSpawnRadius;

	UPROPERTY(EditAnywhere, Category = "Spawn Properties")
	float MaxNumberOfTargetsAtOnce;

	UFUNCTION()
	void OnTargetDestroyed(AActor* DestroyedActor);

	// Randomize scale of target
	UFUNCTION()
	float RandomizeScale(ASphereTarget* Target);

	// Randomize location of target
	UFUNCTION()
	void RandomizeLocation(FVector FLastSpawnLocation, float LastTargetScaleValue);

	// Location to spawn the first target
	UPROPERTY(VisibleAnywhere, Category = "Spawn Properties", meta = (AllowPrivateAccess = true))
	FVector FirstSpawnLocation;

	// Location to spawn the target
	UPROPERTY(VisibleAnywhere, Category = "Spawn Properties", meta = (AllowPrivateAccess = true))
	FVector SpawnLocation;

	// Location to refer to the last target spawned
	UPROPERTY(VisibleAnywhere, Category = "Spawn Properties", meta = (AllowPrivateAccess = true))
	FVector LastSpawnLocation;

	// The scale applied to the most recently spawned target
	UPROPERTY(VisibleAnywhere, Category = "Spawn Properties", meta = (AllowPrivateAccess = true))
	float LastTargetScale;

	// The base size of the sphere target
	const float SphereTargetRadius = 50.f;

	TArray<FVector> RecentSpawnLocations;

	TArray<FSphere> RecentSpawnBounds;

	bool SingleBeat;
};
