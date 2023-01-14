// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "DefaultGameInstance.h"
#include "DefaultPlayerController.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectile::AProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Use a sphere as a simple collision representation.
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>("Projectile Mesh");
	ProjectileMesh->CastShadow = false;
	RootComponent = ProjectileMesh;
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("Projectile Movement");
	ProjectileMovement->SetUpdatedComponent(ProjectileMesh);
	ProjectileMovement->InitialSpeed = 250000.f;
	ProjectileMovement->MaxSpeed = 250000.f;
	InitialLifeSpan = 1.0f;
	DamageTypeClass = UDamageType::StaticClass();
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);

	if (Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this))->GameModeActorStruct.IsBeatTrackMode == true)
	{
		Damage = 0.f;
	}
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// This is the entry point for any target hit by the Projectile
void AProjectile::OnHit(UPrimitiveComponent* HitComponent, 
	AActor* OtherActor, UPrimitiveComponent* OtherComponent, 
	FVector NormalImpulse, const FHitResult& Hit)
{
	// Get the owning actor of the projectile
	const AActor* MyOwner = GetOwner();
	if (MyOwner == nullptr || !OtherActor->CanBeDamaged())
	{
		Destroy();
		return;
	}
	if (OtherActor != nullptr && OtherActor != this && OtherComponent != nullptr) //&& OtherComponent->IsSimulatingPhysics()
	{
		ADefaultPlayerController* MyOwnerInstigator = MyOwner->GetInstigatorController<ADefaultPlayerController>();
		UGameplayStatics::ApplyDamage(OtherActor, Damage, MyOwnerInstigator, this, DamageTypeClass);
	}
	Destroy();
}

void AProjectile::FireInDirection(const FVector& ShootDirection) const
{
	ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
}




