// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun_AK47.h"
#include "DefaultCharacter.h"
#include "DefaultPlayerController.h"
#include "DefaultGameInstance.h"
#include "Projectile.h"
#include "SphereTarget.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AGun_AK47::AGun_AK47()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>("Gun");
	MeshComp->SetOnlyOwnerSee(false);
	MeshComp->CastShadow = false;
	RootComponent = MeshComp;

	MuzzleLocationComp = CreateDefaultSubobject<USceneComponent>("Muzzle Location");
	MuzzleLocationComp->SetupAttachment(MeshComp, "Muzzle");
	TraceDistance = 999999;
	bCanFire = true;
	bIsFiring = false;
	RecoilAlpha = 0.f;
	RecoilAngle = 0.f;
	Recoil = false;
	CurrentShotRecoilRotation = FRotator();
	CurrentShotCameraRecoilRotation = FRotator();
	StartRotation = FRotator();
	EndRotation = FRotator();
}

// Called when the game starts or when spawned
void AGun_AK47::BeginPlay()
{
	Super::BeginPlay();

	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	Character = Cast<ADefaultCharacter>(GetParentActor());
	PlayerController = Character->GetController<ADefaultPlayerController>();
	BulletDecalInstance = UMaterialInstanceDynamic::Create(BulletDecalMaterial, GetInstigatorController<ADefaultPlayerController>());
	ProjectileSpawnParams.Owner = Character;
	ProjectileSpawnParams.Instigator = Character;
	bShouldTrace = GI->GameModeActorStruct.IsBeatTrackMode;

	FOnTimelineVector RecoilProgressFunction;
	RecoilProgressFunction.BindUFunction(this, FName("UpdateRecoilPattern"));
	RecoilTimeline.AddInterpVector(RecoilVectorCurve, RecoilProgressFunction);
}

// Called every frame
void AGun_AK47::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShouldRecoil)
	{
		RecoilTimeline.TickTimeline(DeltaTime);
		UpdateCameraKickback(DeltaTime);
		if (Character->GetCamera())
		{
			FRotator Current = Character->GetCamera()->GetRelativeRotation();
			FRotator UpdatedRotation = UKismetMathLibrary::RInterpTo(
				Current, CurrentShotCameraRecoilRotation,
				DeltaTime, 4);
			Character->Camera->SetRelativeRotation(UpdatedRotation);
		}
		if (Character->CameraRecoilComp)
		{
			Character->CameraRecoilComp->SetRelativeRotation(FRotator(RecoilAngle, 0, 0));
		}
	}

	// only do tracing for Beat Track game modes
	if (bShouldTrace)
	{
		// Transform MuzzleOffset from camera space to world space.
		FVector StartTrace = Character->ShotDirection->GetComponentLocation();
		FVector EndTrace = Character->ShotDirection->GetForwardVector() * FVector(TraceDistance, TraceDistance, TraceDistance);

		if (FHitResult Hit; GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_GameTraceChannel1,
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
	if (ProjectileClass && bCanFire)
	{
		FHitResult Hit;
		FRotator ShotDirectionRotation = Character->ShotDirection->GetComponentRotation();
		FVector MuzzleLocation = MeshComp->GetSocketTransform("Muzzle").GetLocation();
		FRotator MuzzleRotation = MeshComp->GetSocketTransform("Muzzle").Rotator();
		//FVector StartTrace = Character->ShotDirection->GetComponentLocation();
		//FVector EndTrace = Character->ShotDirection->GetForwardVector() * FVector(TraceDistance, TraceDistance, TraceDistance);
		FVector StartTrace = Character->ShotDirection->GetComponentLocation();
		FVector EndTrace = StartTrace + UKismetMathLibrary::RotateAngleAxis(
			UKismetMathLibrary::RotateAngleAxis(Character->ShotDirection->GetForwardVector(),
				-CurrentShotRecoilRotation.Pitch, Character->ShotDirection->GetRightVector()),
			CurrentShotRecoilRotation.Yaw, Character->ShotDirection->GetUpVector())
			* FVector(TraceDistance, TraceDistance, TraceDistance);

		// Trace the character's pov to get location to fire projectile
		bool bHit = GetWorld()->LineTraceSingleByChannel(Hit,
			StartTrace, EndTrace, ECC_GameTraceChannel1, FCollisionQueryParams::DefaultQueryParam);

		//Debug line for what actor is looking at
		//DrawDebugLine(GetWorld(), StartTrace,
		//	EndTrace, FColor::Red, false, 10.f);

		// Spawn the projectile at the muzzle
		if (AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass,
			MuzzleLocation, ShotDirectionRotation, ProjectileSpawnParams))
		{
			// Set the projectile's initial trajectory.
			if (bHit)
			{
				Projectile->FireInDirection(UKismetMathLibrary::FindLookAtRotation(
					MuzzleLocation,
					Hit.ImpactPoint).Vector());
				//Debug line going from muzzle to where actor is looking at
				//DrawDebugLine(GetWorld(), MuzzleLocation,
				//	Hit.ImpactPoint, FColor::Emerald, false, 10.f);
			}
			else
			{
				Projectile->FireInDirection(UKismetMathLibrary::FindLookAtRotation(
					MuzzleLocation,
					EndTrace).Vector());
			}

			// If reached this point, the player has fired
			if (PlayerController->CountdownActive == false &&
				GI->GameModeActorStruct.IsBeatTrackMode == false)
			{
				// Only updating Shots Fired if in live game mode
				OnShotFired.Broadcast();
			}

			// Update how many shots fired for recoil purposes
			ShotsFired++;
			if (bShouldRecoil)
			{
				Recoil = true;
				RecoilAlpha = 0.f;
			}

			// Play Muzzle flash
			if (NS_MuzzleFlash != nullptr)
			{
				UNiagaraFunctionLibrary::SpawnSystemAttached(NS_MuzzleFlash, MeshComp, TEXT("Muzzle"),
					MuzzleFlashOffset, MuzzleRotation,
					EAttachLocation::SnapToTarget, true);
			}
			// Try and play the sound if specified
			if (FireSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
			}
			// Try and play a firing animation if specified
			if (FireAnimation != nullptr)
			{
				// Get the animation object for the arms mesh
				if (UAnimInstance* AnimInstance = Character->GetHandsMesh()->GetAnimInstance(); AnimInstance != nullptr)
				{
					AnimInstance->Montage_Play(FireAnimation, 1.f);
				}
			}
			// Try to create decal at hit location
			if (BulletDecalInstance && BulletDecalMaterial)
			{
				if (UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(),
					BulletDecalInstance, FVector(4, 4, 4),
					Hit.ImpactPoint, Hit.Normal.Rotation(), 0.f))
				{
					Decal->SetFadeScreenSize(0.f);
					Decal->SetFadeOut(1, 2, true);
				}
			}
		}
	}
}

void AGun_AK47::StartFire()
{
	if (bCanFire && !bIsFiring)
	{
		if (bShouldRecoil)
		{
			StartRotation = PlayerController->GetControlRotation();
			RecoilTimeline.SetPlayRate(1.f);
			if (RecoilTimeline.IsReversing()) {
				RecoilTimeline.Play();
			}
			else
			{
				RecoilTimeline.PlayFromStart();
			}
		}
		bIsFiring = true;
		if (bAutomaticFire)
		{
			// sets a timer for firing the weapon - if bAutomaticFire is true then this timer will repeat until cleared by StopFire(), leading to fully automatic fire
			GetWorldTimerManager().SetTimer(ShotDelay, this, &AGun_AK47::Fire, 0.11f, true, 0.0f);
		}
		else
		{
			Fire();
		}
	}
}

void AGun_AK47::StopFire()
{
	if (bAutomaticFire || GetWorldTimerManager().IsTimerActive(ShotDelay))
	{
		// Stops the gun firing (for automatic fire)
		GetWorldTimerManager().ClearTimer(ShotDelay);
	}
	bIsFiring = false;
	CurrentShotRecoilRotation.Yaw = 0.f;
	CurrentShotRecoilRotation.Pitch = 0.f;
	CurrentShotCameraRecoilRotation.Yaw = 0.f;
	CurrentShotCameraRecoilRotation.Pitch = 0.f;
	ShotsFired = 0;
	RecoilTimeline.SetPlayRate(5.454545f);
	RecoilTimeline.Reverse();
}

void AGun_AK47::EnableFire()
{
	bCanFire = true;
}

void AGun_AK47::UpdateCameraKickback(float DeltaTime)
{
	if (Recoil)
	{
		if (RecoilAlpha + DeltaTime >= RecoilDuration)
		{
			RecoilAlpha = RecoilDuration;
			Recoil = false;
		}
		else
		{
			RecoilAlpha += DeltaTime;
		}
	}
	else
	{
		RecoilAngle = 0.f;
	}
	RecoilAngle = KickbackCurve->GetFloatValue(RecoilAlpha / RecoilDuration);
}

void AGun_AK47::UpdateRecoilPattern(FVector Output)
{
	if (bIsFiring == true)
	{
		// Don't continue playing animation if semi-automatic
		if (bAutomaticFire || ShotsFired < 1)
		{
			CurrentShotCameraRecoilRotation.Yaw = Output.X * 0.5;
			CurrentShotCameraRecoilRotation.Pitch = Output.Y * 0.5;
			CurrentShotRecoilRotation.Yaw = Output.X;
			CurrentShotRecoilRotation.Pitch = Output.Y;
		}
		else
		{
			CurrentShotCameraRecoilRotation.Yaw = 0;
			CurrentShotCameraRecoilRotation.Pitch = 0;
			CurrentShotRecoilRotation.Yaw = 0;
			CurrentShotRecoilRotation.Pitch = 0;
		}
	}
}
