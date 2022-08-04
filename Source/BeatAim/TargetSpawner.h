// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TargetSpawner.generated.h"

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

	//UPROPERTY(BlueprintCallable)
	//FTargetSpawned OnTargetSpawned;
	ADefaultCharacter* DefaultCharacter;

private:
	UFUNCTION(BlueprintCallable)
		void SpawnActor();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Properties", meta = (AllowPrivateAccess = true))
		bool ShouldSpawn = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Properties", meta = (AllowPrivateAccess = true))
		float TargetsSpawned = 0.f;
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

	void RandomizeScale(ASphereTarget* Target);
	void RandomizeLocation();
	FVector SpawnLocation;
};
