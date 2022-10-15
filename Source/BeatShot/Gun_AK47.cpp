// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun_AK47.h"
#include "DefaultCharacter.h"
#include "DefaultPlayerController.h"
#include "Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "DefaultGameInstance.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "SphereTarget.h"

// Sets default values
AGun_AK47::AGun_AK47()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Gun");
	RootComponent = GunMesh;
	GunMesh->SetOnlyOwnerSee(false);
	GunMesh->bCastDynamicShadow = false;
	GunMesh->CastShadow = false;
	MuzzleLocation = CreateDefaultSubobject<USceneComponent>("Muzzle Location");
	MuzzleLocation->SetupAttachment(GunMesh, "Muzzle");
	TraceDistance = 10000;
}

// Called when the game starts or when spawned
void AGun_AK47::BeginPlay()
{
	Super::BeginPlay();

	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	PlayerController = GI->DefaultPlayerControllerRef;
	Character = GI->DefaultCharacterRef;
}

// Called every frame
void AGun_AK47::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// only do tracing for Beat Track game modes
	if (GI->GameModeActorStruct.IsBeatTrackMode == true)
	{
		// Get the camera transform.
		FVector CameraLocation;
		FRotator CameraRotation;
		FHitResult Hit;
		GI->DefaultCharacterRef->GetActorEyesViewPoint(CameraLocation, CameraRotation);

		// Transform MuzzleOffset from camera space to world space.
		FVector Start = CameraLocation;
		FVector End = CameraLocation + CameraRotation.Vector() * TraceDistance;
		FCollisionQueryParams TraceParams;

		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility,
		                                                     FCollisionQueryParams::DefaultQueryParam))
		{
			if (ASphereTarget* HitTarget = Cast<ASphereTarget>(Hit.GetActor()))
			{
				UGameplayStatics::ApplyDamage(HitTarget, 1.f, PlayerController, Character, UDamageType::StaticClass());
				HitTarget->MID_TargetColorChanger->SetVectorParameterByIndex(0, FLinearColor::Green);
			}
			else
			{
				if (GI->SphereTargetRef)
				{
					GI->SphereTargetRef->MID_TargetColorChanger->SetVectorParameterByIndex(0, FLinearColor::Red);
				}
			}
		}
		else
		{
			if (GI->SphereTargetRef)
			{
				GI->SphereTargetRef->MID_TargetColorChanger->SetVectorParameterByIndex(0, FLinearColor::Red);
			}
		}
	}
}

void AGun_AK47::Fire()
{
	if (ProjectileClass)
	{
		if (UWorld* const World = GetWorld())
		{
			FVector CameraLocation;
			FRotator CameraRotation;
			GI->DefaultCharacterRef->GetActorEyesViewPoint(CameraLocation, CameraRotation);

			FVector SpawnLocation = CameraLocation + CameraRotation.Vector() * 100; //+ FTransform(CameraRotation).TransformVector(End);
			FRotator SpawnRotation = CameraRotation;

			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.Owner = GI->DefaultCharacterRef;
			ActorSpawnParams.Instigator = GI->DefaultCharacterRef;

			// Spawn the projectile at the muzzle
			AProjectile* Projectile = World->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

			if (Projectile)
			{
				//If reached this point, the player has fired
				if (GI->DefaultPlayerControllerRef->CountdownActive == false)
				{
					if (GI->GameModeActorStruct.IsBeatTrackMode == false)
					{
						//Only updating Shots Fired
						OnShotFired.Broadcast();
					}
				}
				FTransform MuzzleTransform = GunMesh->GetSocketTransform("Muzzle");
				// Set the projectile's initial trajectory.
				FVector LaunchDirection = SpawnRotation.Vector();
				Projectile->FireInDirection(LaunchDirection);
				UNiagaraFunctionLibrary::SpawnSystemAttached(NS_MuzzleFlash, GunMesh, TEXT("Muzzle"), FVector(5,0,0), MuzzleTransform.Rotator(), EAttachLocation::SnapToTarget, true);
			}
		}

		// Try and play the sound if specified
		if (FireSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GI->DefaultCharacterRef->GetActorLocation());
		}

		// Try and play a firing animation if specified
		if (FireAnimation != nullptr)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = GI->DefaultCharacterRef->GetHandsMesh()->GetAnimInstance();
			if (AnimInstance != nullptr)
			{
				AnimInstance->Montage_Play(FireAnimation, 1.f);
			}
		}
	}
}

