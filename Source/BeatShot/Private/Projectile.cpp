// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Projectile.h"
#include "BSGameInstance.h"
#include "SphereTarget.h"
#include "Components/DecalComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "GameplayAbility/BSAbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"

AProjectile::AProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	
	// Use a sphere as a simple collision representation.
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>("Projectile Mesh");
	ProjectileMesh->CastShadow = false;
	RootComponent = ProjectileMesh;
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("Projectile Movement");
	ProjectileMovement->SetUpdatedComponent(ProjectileMesh);
	ProjectileMovement->InitialSpeed = 10000.f;
	ProjectileMovement->MaxSpeed = 10000.f;
	InitialLifeSpan = 1.0f;
	DamageTypeClass = UDamageType::StaticClass();
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);

	// TODO: Disable damage ability of projectile during BeatTrack mode
	// Or remove shoot ability during  beattrack, but would have to give it back at start of other modes
	if (Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(this))->GameModeActorStruct.IsBeatTrackMode == true)
	{
		Damage = 0.f;
	}
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	// Get the owning actor of the projectile
	if (!GetOwner() || !HasAuthority() || !OtherActor || OtherActor == GetInstigator())
	{
		Destroy();
		return;
	}
	if (bSpawnDecalOnHit)
	{
		SpawnDecal(Hit, 1, 2);
	}
	if (!OtherActor->CanBeDamaged())
	{
		Destroy();
		return;
	}

	if (OtherActor->Implements<IAbilitySystemInterface::UClassType>() && Cast<ASphereTarget>(OtherActor))
	{
		// Pass the HitResult to the GameplayEffectSpec
		UBSAbilitySystemComponent* ASC = Cast<UBSAbilitySystemComponent>(Cast<ASphereTarget>(OtherActor)->GetAbilitySystemComponent());
		DamageEffectSpecHandle.Data.Get()->GetContext().AddHitResult(Hit);
		ASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
	}
	Destroy();
}

void AProjectile::FireInDirection(const FVector& ShootDirection) const
{
	ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
}

void AProjectile::SpawnDecal(const FHitResult& Hit, const float StartDelay, const float FadeDuration) const
{
	/** Try to create decal at hit location */
	if (BulletDecalInstance && BulletDecalMaterial && bSpawnDecalOnHit)
	{
		UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BulletDecalInstance, FVector(4, 4, 4), Hit.ImpactPoint, Hit.Normal.Rotation(), 0.f);
		if (!Decal)
		{
			return;
		}
		Decal->SetFadeScreenSize(0.f);
		Decal->SetFadeOut(StartDelay, FadeDuration, true);
	}
}
