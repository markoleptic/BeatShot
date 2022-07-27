// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
		// Use a sphere as a simple collision representation.
		CollisionSphere = CreateDefaultSubobject<USphereComponent>("CollisionSphere");
		// Set the sphere's collision radius.
		CollisionSphere->InitSphereRadius(2.0f);
		// Set the root component to be the collision component.
		RootComponent = CollisionSphere;
		ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("Projectile Movement");
		ProjectileMovement->SetUpdatedComponent(CollisionSphere);
		ProjectileMovement->InitialSpeed = 250000.f;
		ProjectileMovement->MaxSpeed = 250000.f;
		ProjectileMovement->bRotationFollowsVelocity = false;
		ProjectileMovement->bShouldBounce = false;
		ProjectileMovement->ProjectileGravityScale = 0.0f;
		InitialLifeSpan = 1.0f;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != nullptr && OtherActor != this && OtherComponent != nullptr && OtherComponent->IsSimulatingPhysics())
	{
		OtherComponent->AddImpulseAtLocation(ProjectileMovement->Velocity * 100.0f, Hit.ImpactPoint);
	}

	Destroy();
}

void AProjectile::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
}




