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
#include "Camera/CameraComponent.h"
#include "Curves/CurveVector.h"
#include "Kismet/KismetMathLibrary.h"
//#include "Animation/AnimationAsset.h"
//#include "Animation/AnimSequence.h"
//#include "Engine/Engine.h"
//#include "DrawDebugHelpers.h"
//#include "Math/UnrealMathUtility.h"

// Sets default values
AGun_AK47::AGun_AK47()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>("Gun");
	MeshComp->SetOnlyOwnerSee(false);
	MeshComp->CastShadow = false;
	RootComponent = MeshComp;

	MuzzleLocation = CreateDefaultSubobject<USceneComponent>("Muzzle Location");
	MuzzleLocation->SetupAttachment(MeshComp, "Muzzle");
	TraceDistance = 10000;
	RecoverRotation = FRotator::ZeroRotator;
	bShouldRecover = false;
	LastRecoilRotation = FRotator();
	bIsFiring = false;
	bIsRecovering = false;
	AutoFiring = true;
}

// Called when the game starts or when spawned
void AGun_AK47::BeginPlay()
{
	Super::BeginPlay();

	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	PlayerController = GI->DefaultPlayerControllerRef;
	Character = GI->DefaultCharacterRef;

	FOnTimelineVector RecoilProgressFunction;
	RecoilProgressFunction.BindUFunction(this, FName("UpdateKickback"));
	RecoilTimeline.AddInterpVector(RecoilVectorCurve, RecoilProgressFunction);
}

// Called every frame
void AGun_AK47::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RecoilTimeline.TickTimeline(DeltaTime);

	if (bIsRecovering && abs(TotalRecoilRecovered.Pitch) >= abs(RecoilToRecover.Pitch))
	{
		bIsRecovering = false;
		RecoilToRecover = FRotator();
		TotalRecoilRecovered = FRotator();
	}
	if (bIsFiring)
	{
		LastRecoilRotation = PlayerController->GetControlRotation();
		FRotator UpdatedRotation = UKismetMathLibrary::RInterpTo(
			LastRecoilRotation,
			LastRecoilRotation + CurrentShotRecoilRotation,
			DeltaTime, 10);
		PlayerController->SetControlRotation(UpdatedRotation);
		TotalRotationFromRecoil += UKismetMathLibrary::NormalizedDeltaRotator(UpdatedRotation, LastRecoilRotation);
	}
	if (bIsRecovering)
	{
		FRotator LastRecoilRecoverRotation = PlayerController->GetControlRotation();
		FRotator RecoveredRotation = UKismetMathLibrary::RInterpTo(
			LastRecoilRecoverRotation,
			LastRecoilRecoverRotation - RecoilToRecover,
			DeltaTime, 4);
		PlayerController->SetControlRotation(RecoveredRotation);
		// TotalRotationFromRecoil use this maybe
		TotalRecoilRecovered += UKismetMathLibrary::NormalizedDeltaRotator(LastRecoilRecoverRotation, RecoveredRotation);

		UE_LOG(LogTemp, Display, TEXT("TotalRecoilRecovered %s"), *TotalRecoilRecovered.ToCompactString());
	}

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
	if (ProjectileClass && bCanFire)
	{
		if (UWorld* const World = GetWorld())
		{
			FVector CameraLocation;
			FRotator CameraRotation;
			GI->DefaultCharacterRef->GetActorEyesViewPoint(CameraLocation, CameraRotation);

			const FVector SpawnLocation = CameraLocation + CameraRotation.Vector() * 100; //+ FTransform(CameraRotation).TransformVector(End);
			const FRotator SpawnRotation = CameraRotation;

			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.Owner = GI->DefaultCharacterRef;
			ActorSpawnParams.Instigator = GI->DefaultCharacterRef;

			// Spawn the projectile at the muzzle

			if (AProjectile* Projectile = World->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams))
			{
				// If reached this point, the player has fired
				if (GI->DefaultPlayerControllerRef->CountdownActive == false && GI->GameModeActorStruct.IsBeatTrackMode == false)
				{
					// Only updating Shots Fired
					OnShotFired.Broadcast();
				}
				const FTransform MuzzleTransform = MeshComp->GetSocketTransform("Muzzle");
				// Set the projectile's initial trajectory.
				const FVector LaunchDirection = SpawnRotation.Vector();
				Projectile->FireInDirection(LaunchDirection);
				UNiagaraFunctionLibrary::SpawnSystemAttached(NS_MuzzleFlash, MeshComp, TEXT("Muzzle"),
					FVector(5, 0, 0), MuzzleTransform.Rotator(),
					EAttachLocation::SnapToTarget, true);
				ShotsFired++;
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

void AGun_AK47::StartFire()
{
	if (bCanFire)
	{
		if (bIsFiring==false)
		{
			StartRotation = PlayerController->GetControlRotation();
			RecoilTimeline.PlayFromStart();
		}
		bIsFiring = true;
		// sets a timer for firing the weapon - if bAutomaticFire is true then this timer will repeat until cleared by StopFire(), leading to fully automatic fire
		GetWorldTimerManager().SetTimer(ShotDelay, this, &AGun_AK47::Fire, 0.11f, true, 0.0f);
	}
}

void AGun_AK47::StopFire()
{
	// Stops the gun firing (for automatic fire)
	GetWorldTimerManager().ClearTimer(ShotDelay);
	bIsFiring = false;

	// Get the recoil rotation to recover
	RecoilToRecover = TotalRotationFromRecoil;
	RecoilToRecover.Roll = 0.f;
	UE_LOG(LogTemp, Display, TEXT("TotalRotFromRecoil: %s"), *TotalRotationFromRecoil.ToString());
	TotalRotationFromRecoil = FRotator();

	// if player has countered recoil, subtract it
	EndRotation = PlayerController->GetControlRotation();
	FRotator DeltaRotationStartEnd = UKismetMathLibrary::NormalizedDeltaRotator(EndRotation, StartRotation);
	DeltaRotationStartEnd.Roll = 0.f;
	UE_LOG(LogTemp, Display, TEXT("PlayerControlledRecoil: %s"), *DeltaRotationStartEnd.ToString());

	if (DeltaRotationStartEnd.Pitch <= 0)
	{
		RecoilToRecover = FRotator();
		TotalRecoilRecovered = FRotator();
		bIsRecovering = false;
	}
	else if (DeltaRotationStartEnd.Pitch <= RecoilToRecover.Pitch)
	{
		RecoilToRecover.Pitch -= RecoilToRecover.Pitch - DeltaRotationStartEnd.Pitch;
		bIsRecovering = true;
	}
	else
	{
		bIsRecovering = true;
	}

	ShotsFired = 0;
	RecoilTimeline.Stop();
}

void AGun_AK47::EnableFire()
{
	bCanFire = true;
}

void AGun_AK47::UpdateKickback(FVector Output)
{
	if (bIsFiring == true)
	{
		UE_LOG(LogTemp, Display, TEXT("VECTOR: %s"), *Output.ToString());
		CurrentShotRecoilRotation.Yaw = Output.Y * 2;
		CurrentShotRecoilRotation.Pitch = Output.Z * 1.5;
	}
}