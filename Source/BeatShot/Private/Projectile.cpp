// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Projectile.h"

#include "BSCharacter.h"
#include "BSGameInstance.h"
#include "BSPlayerController.h"
#include "SphereTarget.h"
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
	ProjectileMovement->InitialSpeed = 100.f;
	ProjectileMovement->MaxSpeed = 100.f;
	InitialLifeSpan = 1.0f;
	DamageTypeClass = UDamageType::StaticClass();
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);

	if (Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(this))->GameModeActorStruct.IsBeatTrackMode == true)
	{
		Damage = 0.f;
	}
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	// Get the owning actor of the projectile
	if (!GetOwner() || !HasAuthority() || !OtherActor->CanBeDamaged() || !OtherActor || OtherActor == GetInstigator())
	{
		Destroy();
		return;
	}

	if (OtherActor->Implements<IAbilitySystemInterface::UClassType>())
	{
		if (Cast<ASphereTarget>(OtherActor))
		{
			UBSAbilitySystemComponent* ASC = Cast<UBSAbilitySystemComponent>(Cast<ASphereTarget>(OtherActor)->GetAbilitySystemComponent());
			DamageEffectSpecHandle.Data.Get()->GetContext().AddHitResult(Hit);
			UE_LOG(LogTemp, Display, TEXT("%s"), *DamageEffectSpecHandle.Data->GetEffectContext().ToString());
			ASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		}
	}

	//ABSPlayerController* MyOwnerInstigator = MyOwner->GetInstigatorController<ABSPlayerController>();
	//UGameplayStatics::ApplyDamage(OtherActor, Damage, MyOwnerInstigator, this, DamageTypeClass);
	Destroy();
}

void AProjectile::FireInDirection(const FVector& ShootDirection) const
{
	ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
}
