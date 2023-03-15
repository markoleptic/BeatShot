// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Gun_AK47.h"
#include "BSCharacter.h"
#include "BSPlayerController.h"
#include "Projectile.h"
#include "SphereTarget.h"
#include "NiagaraFunctionLibrary.h"
#include "Camera/CameraComponent.h"
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

ABSCharacter* AGun_AK47::GetBSCharacter() const
{
	return Cast<ABSCharacter>(GetParentActor());
}

void AGun_AK47::BeginPlay()
{
	Super::BeginPlay();

	BulletDecalInstance = UMaterialInstanceDynamic::Create(BulletDecalMaterial, GetInstigatorController<ABSPlayerController>());

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
	ShotsFired++;
	if (bShouldRecoil)
	{
		bShouldKickback = true;
		KickbackAlpha = 0.f;
	}
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
	CurrentShotRecoilRotation = FRotator(0, 0, 0);
	CurrentShotCameraRecoilRotation = FRotator(0, 0, 0);
	ShotsFired = 0;
	/* Reverse the timeline so that it takes time to recover to the beginning */
	RecoilTimeline.SetPlayRate(5.454545f);
	RecoilTimeline.Reverse();
}

void AGun_AK47::EnableFire()
{
	bCanFire = true;
}

FTransform AGun_AK47::GetTraceTransform() const
{
	UCameraComponent* Camera = GetBSCharacter()->GetCamera();
	FVector MuzzleLoc = MeshComp->GetSocketTransform("Muzzle").GetLocation();
	float AngleDeg = -CurrentShotRecoilRotation.Pitch;
	float AngleDeg2 = CurrentShotRecoilRotation.Yaw;
	FVector RotatedVector1 = UKismetMathLibrary::RotateAngleAxis(Camera->GetForwardVector(), AngleDeg, Camera->GetRightVector());
	FVector RotatedVector2 = UKismetMathLibrary::RotateAngleAxis(RotatedVector1, AngleDeg2, Camera->GetUpVector());
	FVector EndTrace = Camera->GetComponentLocation() + RotatedVector2 * FVector(TraceDistance);
		
	FTransform Transform;
	if (FHitResult Hit; GetWorld()->LineTraceSingleByChannel(Hit, Camera->GetComponentLocation(), EndTrace, ECC_Visibility, FCollisionQueryParams::DefaultQueryParam))
	{
		Transform = {UKismetMathLibrary::FindLookAtRotation(MuzzleLoc, Hit.ImpactPoint), MuzzleLoc, FVector(1.f)};
		//DrawDebugLine(GetWorld(),  Camera->GetComponentLocation(), Hit.ImpactPoint, FColor::Blue, false, 10.f);
		//DrawDebugLine(GetWorld(), MuzzleLoc, Hit.ImpactPoint, FColor::Red, false, 10.f);
	}
	else
	{
		Transform = {UKismetMathLibrary::FindLookAtRotation(MuzzleLoc, EndTrace), MuzzleLoc, FVector(1.f)};
	}
	return Transform;
}

void AGun_AK47::TraceForward() const
{
	const FVector StartTrace = GetBSCharacter()->GetArrowComponent()->GetComponentLocation();
	const FVector EndTrace = GetBSCharacter()->GetArrowComponent()->GetForwardVector() * FVector(TraceDistance, TraceDistance, TraceDistance);
	if (FHitResult Hit; GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_GameTraceChannel1, FCollisionQueryParams::DefaultQueryParam))
	{
		if (ASphereTarget* HitTarget = Cast<ASphereTarget>(Hit.GetActor()))
		{
			UGameplayStatics::ApplyDamage(HitTarget, 1.f, GetBSCharacter()->GetBSPlayerController(), GetBSCharacter(), UDamageType::StaticClass());
			HitTarget->SetSphereColor(PlayerSettings.Game.PeakTargetColor);
			if (!PlayerSettings.Game.bUseSeparateOutlineColor)
			{
				HitTarget->SetOutlineColor(PlayerSettings.Game.PeakTargetColor);
			}
			return;
		}
	}
	if (TrackingTarget)
	{
		TrackingTarget->SetSphereColor(PlayerSettings.Game.EndTargetColor);
		if (!PlayerSettings.Game.bUseSeparateOutlineColor)
		{
			TrackingTarget->SetOutlineColor(PlayerSettings.Game.EndTargetColor);
		}
	}
}

void AGun_AK47::UpdateRecoilAndKickback(const float DeltaTime)
{
	RecoilTimeline.TickTimeline(DeltaTime);
	UpdateCameraKickback(DeltaTime);
	const FRotator Current = GetBSCharacter()->GetCamera()->GetRelativeRotation();
	const FRotator UpdatedRotation = UKismetMathLibrary::RInterpTo(Current, CurrentShotCameraRecoilRotation, DeltaTime, 4);
	GetBSCharacter()->GetCamera()->SetRelativeRotation(UpdatedRotation);
	GetBSCharacter()->GetCameraRecoilComponent()->SetRelativeRotation(FRotator(KickbackAngle, 0, 0));
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
	/** Don't continue playing animation if semi-automatic */
	if (bAutomaticFire || ShotsFired < 1)
	{
		CurrentShotCameraRecoilRotation.Yaw = Output.X * 0.5;
		CurrentShotCameraRecoilRotation.Pitch = Output.Y * 0.5;
		CurrentShotRecoilRotation.Yaw = Output.X;
		CurrentShotRecoilRotation.Pitch = Output.Y;
	}
	else
	{
		CurrentShotCameraRecoilRotation = FRotator(0, 0, 0);
		CurrentShotRecoilRotation = FRotator(0, 0, 0);
	}
}