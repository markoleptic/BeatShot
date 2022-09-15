// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "DefaultCharacter.h"
#include "SphereTarget.h"
#include "DefaultGameInstance.h"
#include "GameModeActorBase.h"
#include "PlayerHUD.h"
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
	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));


	if (GI->GameModeActorStruct.IsBeatTrackMode == true)
	{
		Damage = 0.f;
	}
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	AActor* MyOwner = GetOwner();
	if (MyOwner == nullptr)
	{
		Destroy();
		return;
	}
	MyOwnerInstigator = MyOwner->GetInstigatorController();
	Shooter = GetInstigator<ADefaultCharacter>();
	if (Shooter)
	{
		Target = Cast<ASphereTarget>(OtherActor);
		if (Target && Shooter && GI->GameModeActorBaseRef && GI->GameModeActorStruct.IsBeatTrackMode == false)
		{
			//If reached this point, player has shot a target, and can get exact moment it was hit
			GI->GameModeActorBaseRef->UpdateTargetsHit();
		}
	}

	if (OtherActor != nullptr && OtherActor != this && OtherComponent != nullptr) //&& OtherComponent->IsSimulatingPhysics()
	{
		UGameplayStatics::ApplyDamage(OtherActor, Damage, MyOwnerInstigator, this, DamageTypeClass);
	}
	Destroy();
}

void AProjectile::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
}




