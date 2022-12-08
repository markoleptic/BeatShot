// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModeActorBase.h"
#include "GameFramework/Actor.h"
#include "SphereTarget.generated.h"

class UDefaultHealthComponent;
class UCapsuleComponent;
class UNiagaraSystem;
class UCurveFloat;
class UCurveLinearColor;

/* Target Spawner binds to this function to receive info about how target was destroyed */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnLifeSpanExpired, bool, DidExpire, float, TimeAlive, FVector, Location)
;

UCLASS()
class BEATSHOT_API ASphereTarget : public AActor
{
	GENERATED_BODY()

public:
	/* Sets default values for this actor's properties */
	ASphereTarget();

protected:
	/* Called when the game starts or when spawned */
	virtual void BeginPlay() override;
	/* Called when a non BeatGrid target lifespan has expired */
	virtual void LifeSpanExpired() override;

public:
	/* Called in TargetSpawner to activate a BeatGrid target */
	void StartBeatGridTimer(float Lifespan);

	/* Play the white to green to red color transition for sphere. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Target Properties")
	void PlayColorGradient();

	/* Briefly makes the target higher opacity. Only used for BeatGrid */
	UFUNCTION(BlueprintImplementableEvent, Category = "Target Properties")
	void RemoveAndReappear();

	/* Called from DefaultHealthComponent when a SphereTarget receives damage. */
	void HandleDestruction();

	UPROPERTY(VisibleAnywhere, Category = "Target Properties", BlueprintReadOnly)
	FOnLifeSpanExpired OnLifeSpanExpired;

	UPROPERTY(VisibleAnywhere, Category = "Target Properties", BlueprintReadOnly)
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(VisibleAnywhere, Category = "Target Properties", BlueprintReadOnly)
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, Category = "Target Properties", BlueprintReadOnly)
	FTimerHandle TimeSinceSpawn;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem* NS_Standard_Explosion;

	UPROPERTY(EditAnywhere, Category = "Materials", BlueprintReadWrite)
	UMaterialInterface* Material;

	UPROPERTY(EditAnywhere, Category = "Materials", BlueprintReadWrite)
	UMaterialInstanceDynamic* MID_TargetColorChanger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UDefaultHealthComponent* HealthComp;

	/* Color for BeatGrid targets that aren't active. */
	UPROPERTY(BlueprintReadOnly)
	FLinearColor BeatGridPurple = {83.f / 255.f, 0.f, 245.f / 255.f, 1.f};

private:
	/** Unlike other modes which use LifeSpanExpired to notify TargetSpawner of their expiration,
	 *  BeatGrid needs to use this function since the the targets aren't going to be destroyed,
	 *  but instead just deactivated */
	UFUNCTION()
	void OnBeatGridTimerTimeOut();

	/* Set the max health of the target */
	void SetMaxHealth(float NewMaxHealth) const;

	/* Play the explosion effect at the location of target, scaled to size with the color of the target when it was destroyed. */
	void PlayExplosionEffect(const FVector ExplosionLocation, const float SphereRadius,
	                         const FLinearColor ColorWhenDestroyed) const;

	/* Base radius for sphere target. */
	const float BaseSphereRadius = 50.f;

	/* Locally stored GameModeActorStruct to access GameMode properties without storing ref to game instance */
	FGameModeActorStruct GameModeActorStruct;
};
