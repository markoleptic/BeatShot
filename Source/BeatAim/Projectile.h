// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UStaticMeshComponent;

UCLASS()
class BEATAIM_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = "Combat", meta = (AllowPrivateAccess = true))
		UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = true))
		float Damage = 1000.f;

	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
		UStaticMeshComponent* ProjectileMesh;

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, 
			AActor* OtherActor, 
			UPrimitiveComponent* OtherComponent, 
			FVector NormalImpulse, 
			const FHitResult& Hit);

	void FireInDirection(const FVector& ShootDirection);

	class AController* MyOwnerInstigator;
	class ADefaultCharacter* Shooter;
	class ASphereTarget* Target;
	class UDefaultGameInstance* GI;

};

