// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UStaticMeshComponent;
class ASphereTarget;

UCLASS()
class BEATSHOT_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	UFUNCTION()
		void FireInDirection(const FVector& ShootDirection) const;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditAnywhere, Category = "Materials", BlueprintReadWrite)
		UMaterialInterface* BulletDecalMaterial;

	UPROPERTY(EditAnywhere, Category = "Materials", BlueprintReadWrite)
		UMaterialInstanceDynamic* BulletDecalInstance;

private:

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComponent,
			FVector NormalImpulse,
			const FHitResult& Hit);

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
		float Damage = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
		UClass* DamageTypeClass;
};

