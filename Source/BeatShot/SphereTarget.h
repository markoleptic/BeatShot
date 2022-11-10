// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SphereTarget.generated.h"

class UDefaultHealthComponent;
class UCapsuleComponent;
class UNiagaraSystem;
class UCurveFloat;
class UCurveLinearColor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLifeSpanExpired, bool, DidExpire, FVector, Location);

UCLASS()
class BEATSHOT_API ASphereTarget : public AActor
{
	GENERATED_BODY()

public:
	ASphereTarget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void LifeSpanExpired() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = "Target Properties", BlueprintReadOnly)
		FOnLifeSpanExpired OnLifeSpanExpired;

	UPROPERTY(VisibleAnywhere, Category = "Target Properties", BlueprintReadOnly)
		UCapsuleComponent* CapsuleComp;

	UPROPERTY(VisibleAnywhere, Category = "Target Properties", BlueprintReadOnly)
		UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, Category = "Target Properties", BlueprintReadOnly)
		FTimerHandle TimeSinceSpawn;

	UPROPERTY(EditAnywhere, Category = "Effects", BlueprintReadWrite)
		UNiagaraSystem* NS_Standard_Explosion;

	// Called from DefaultHealthComponent when a SphereTarget receives damage.
	UFUNCTION(BlueprintCallable, Category = "Target Handling")
		void HandleDestruction();

	UFUNCTION(BlueprintCallable, Category = "Target Handling")
		void StartBeatGridTimer(float Lifespan);

	UFUNCTION(BlueprintCallable, Category = "Target Handling")
		void OnBeatGridTimerTimeOut();

	UPROPERTY(VisibleAnywhere, Category = "References", BlueprintReadOnly)
		class UDefaultGameInstance* GI;

	UPROPERTY(EditAnywhere, Category = "Materials", BlueprintReadWrite)
		UMaterialInterface* Material;

	UPROPERTY(EditAnywhere, Category = "Materials", BlueprintReadWrite)
		UMaterialInstanceDynamic* MID_TargetColorChanger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UDefaultHealthComponent* HealthComp;

	UFUNCTION(BlueprintCallable, Category = "Target Properties")
		void SetMaxHealth(float NewMaxHealth);

	// Base radius for sphere target.
	const float BaseSphereRadius = 50.f;

	// Color for BeatGrid targets that aren't active.
	const FLinearColor BeatGridPurple = { 83.f/255.f , 0.f, 245.f/255.f, 1.f };

	// Play the white to green to red color transition for sphere.
	UFUNCTION(BlueprintImplementableEvent, Category = "Target Properties")
		void PlayColorGradient();

	UFUNCTION(BlueprintImplementableEvent, Category = "Target Properties")
		void ShowTargetExplosion();

	// Briefly makes the target higher opacity. Only used for BeatGrid
	UFUNCTION(BlueprintImplementableEvent, Category = "Target Properties")
		void RemoveAndReappear();
};
