// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun_AK47.h"
#include "DefaultCharacter.h"
#include "DefaultPlayerController.h"
#include "DefaultGameInstance.h"
#include "Projectile.h"
#include "SphereTarget.h"
#include "NiagaraFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

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
	KickbackAlpha = 0.f;
	KickbackAngle = 0.f;
	bShouldKickback = false;
	CurrentShotRecoilRotation = FRotator();
	CurrentShotCameraRecoilRotation = FRotator();
}

void AGun_AK47::BeginPlay()
{
	Super::BeginPlay();

	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	Character = Cast<ADefaultCharacter>(GetParentActor());
	PlayerController = Character->GetController<ADefaultPlayerController>();
	BulletDecalInstance = UMaterialInstanceDynamic::Create(BulletDecalMaterial,
	                                                       GetInstigatorController<ADefaultPlayerController>());
	ProjectileSpawnParams.Owner = Character;
	ProjectileSpawnParams.Instigator = Character;
	bShouldTrace = GI->GameModeActorStruct.IsBeatTrackMode;

	/* Bind UpdateRecoilPattern to the Recoil vector curve and timeline */
	FOnTimelineVector RecoilProgressFunction;
	RecoilProgressFunction.BindUFunction(this, FName("UpdateRecoilPattern"));
	RecoilTimeline.AddInterpVector(RecoilVectorCurve, RecoilProgressFunction);
}

void AGun_AK47::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bShouldRecoil)
	{
		UpdateRecoilAndKickback(DeltaTime);
	}
	/* only do tracing for BeatTrack game modes */
	if (bShouldTrace)
	{
		TraceForward();
	}
}

void AGun_AK47::Fire()
{
	if (!bCanFire)
	{
		return;
	}
	FHitResult Hit;
	FRotator ShotDirectionRotation = Character->ShotDirection->GetComponentRotation();
	FVector MuzzleLocation = MeshComp->GetSocketTransform("Muzzle").GetLocation();
	FRotator MuzzleRotation = MeshComp->GetSocketTransform("Muzzle").Rotator();
	FVector StartTrace = Character->ShotDirection->GetComponentLocation();
	FVector EndTrace = StartTrace + UKismetMathLibrary::RotateAngleAxis(
			UKismetMathLibrary::RotateAngleAxis(Character->ShotDirection->GetForwardVector(),
			                                    -CurrentShotRecoilRotation.Pitch,
			                                    Character->ShotDirection->GetRightVector()),
			CurrentShotRecoilRotation.Yaw, Character->ShotDirection->GetUpVector())
		* FVector(TraceDistance, TraceDistance, TraceDistance);
	/* Trace the character's pov to get location to fire projectile */
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit,
	                                                 StartTrace, EndTrace, ECC_GameTraceChannel1,
	                                                 FCollisionQueryParams::DefaultQueryParam);
	/* Spawn the projectile at the muzzle */
	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass,
	                                                              MuzzleLocation, ShotDirectionRotation,
	                                                              ProjectileSpawnParams);
	if (!Projectile) { return; }
	/* Set the projectile's initial trajectory */
	if (bHit)
	{
		Projectile->FireInDirection(UKismetMathLibrary::FindLookAtRotation(
			MuzzleLocation,
			Hit.ImpactPoint).Vector());
	}
	else
	{
		Projectile->FireInDirection(UKismetMathLibrary::FindLookAtRotation(
			MuzzleLocation,
			EndTrace).Vector());
	}
	/* Update number of shots fired */
	if (!PlayerController->CountdownActive &&
		!GI->GameModeActorStruct.IsBeatTrackMode)
	{
		OnShotFired.Broadcast();
	}
	/* Update how many shots fired for recoil purposes */
	ShotsFired++;
	if (bShouldRecoil)
	{
		bShouldKickback = true;
		KickbackAlpha = 0.f;
	}
	ShowEffectsAndPlaySounds(MuzzleRotation, Hit);
}

void AGun_AK47::StartFire()
{
	if (!bCanFire || bIsFiring)
	{
		return;
	}
	if (bShouldRecoil)
	{
		RecoilTimeline.SetPlayRate(1.f);
		/* Resume timeline if it hasn't fully recovered */
		if (RecoilTimeline.IsReversing())
		{
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
		/* sets a timer for firing the weapon - if bAutomaticFire is true then this timer will repeat until cleared by StopFire(), leading to fully automatic fire */
		GetWorldTimerManager().SetTimer(ShotDelay, this, &AGun_AK47::Fire, 0.11f, true, 0.0f);
	}
	else
	{
		Fire();
	}
}

void AGun_AK47::StopFire()
{
	if (bAutomaticFire || GetWorldTimerManager().IsTimerActive(ShotDelay))
	{
		/* Stops the gun firing (for automatic fire) */
		GetWorldTimerManager().ClearTimer(ShotDelay);
	}
	bIsFiring = false;
	CurrentShotRecoilRotation = FRotator(0,0,0);
	ShotsFired = 0;
	/* Reverse the timeline so that it takes time to recover to the beginning */
	RecoilTimeline.SetPlayRate(5.454545f);
	RecoilTimeline.Reverse();
}

void AGun_AK47::EnableFire()
{
	bCanFire = true;
}

void AGun_AK47::TraceForward() const
{
	const FVector StartTrace = Character->ShotDirection->GetComponentLocation();
	const FVector EndTrace = Character->ShotDirection->GetForwardVector() * FVector(
		TraceDistance, TraceDistance, TraceDistance);
	if (FHitResult Hit; GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_GameTraceChannel1,
	                                                         FCollisionQueryParams::DefaultQueryParam))
	{
		if (ASphereTarget* HitTarget = Cast<ASphereTarget>(Hit.GetActor()))
		{
			UGameplayStatics::ApplyDamage(HitTarget, 1.f, PlayerController, Character, UDamageType::StaticClass());
			HitTarget->MID_TargetColorChanger->SetVectorParameterByIndex(0, FLinearColor::Green);
			return;
		}
	}
	if (!GI->SphereTargetRef)
	{
		return;
	}
	GI->SphereTargetRef->MID_TargetColorChanger->SetVectorParameterByIndex(0, FLinearColor::Red);
}

void AGun_AK47::UpdateRecoilAndKickback(const float DeltaTime)
{
	if (!Character->GetCamera() || !Character->CameraRecoilComp)
	{
		return;
	}
	RecoilTimeline.TickTimeline(DeltaTime);
	UpdateCameraKickback(DeltaTime);
	const FRotator Current = Character->GetCamera()->GetRelativeRotation();
	const FRotator UpdatedRotation = UKismetMathLibrary::RInterpTo(
		Current, CurrentShotCameraRecoilRotation,
		DeltaTime, 4);
	Character->Camera->SetRelativeRotation(UpdatedRotation);
	Character->CameraRecoilComp->SetRelativeRotation(FRotator(KickbackAngle, 0, 0));
}

void AGun_AK47::UpdateCameraKickback(const float DeltaTime)
{
	if (bShouldKickback)
	{
		if (KickbackAlpha + DeltaTime >= KickbackDuration)
		{
			KickbackAlpha = KickbackDuration;
			bShouldKickback = false;
		}
		else
		{
			KickbackAlpha += DeltaTime;
		}
	}
	else
	{
		KickbackAngle = 0.f;
	}
	KickbackAngle = KickbackCurve->GetFloatValue(KickbackAlpha / KickbackDuration);
}

void AGun_AK47::UpdateRecoilPattern(const FVector Output)
{
	if (!bIsFiring)
	{
		return;
	}
	/* Don't continue playing animation if semi-automatic */
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

void AGun_AK47::ShowEffectsAndPlaySounds(const FRotator& MuzzleRotation, const FHitResult& Hit) const
{
	/* Play Muzzle flash */
	if (NS_MuzzleFlash)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(NS_MuzzleFlash, MeshComp, TEXT("Muzzle"),
		                                             MuzzleFlashOffset, MuzzleRotation,
		                                             EAttachLocation::SnapToTarget, true);
	}
	/* Play the sound if specified */
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	/* Try and play a firing animation if specified */
	if (FireAnimation)
	{
		/* Get the animation object for the arms mesh */
		if (UAnimInstance* AnimInstance = Character->GetHandsMesh()->GetAnimInstance(); AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
	/* Try to create decal at hit location */
	if (BulletDecalInstance && BulletDecalMaterial && bShowBulletDecals)
	{
		UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(),
		                                                                BulletDecalInstance, FVector(4, 4, 4),
		                                                                Hit.ImpactPoint, Hit.Normal.Rotation(),
		                                                                0.f);
		if (!Decal)
		{
			return;
		}
		Decal->SetFadeScreenSize(0.f);
		Decal->SetFadeOut(1, 2, true);
	}
}

/**
 *FVector StartTrace = Character->ShotDirection->GetComponentLocation();
 *FVector EndTrace = Character->ShotDirection->GetForwardVector() * FVector(TraceDistance, TraceDistance, TraceDistance);
*/

/** Debug line for what actor is looking at
 * DrawDebugLine(GetWorld(), StartTrace,
 *	EndTrace, FColor::Red, false, 10.f);
*/

/** Debug line going from muzzle to where actor is looking at
 * DrawDebugLine(GetWorld(), MuzzleLocation,
 *	Hit.ImpactPoint, FColor::Emerald, false, 10.f);
*/
