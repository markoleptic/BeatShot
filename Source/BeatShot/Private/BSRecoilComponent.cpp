// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "BSRecoilComponent.h"
#include "Kismet/KismetMathLibrary.h"


UBSRecoilComponent::UBSRecoilComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bIsFiring = false;
	KickbackAlpha = 0.f;
	KickbackAngle = 0.f;
	bShouldKickback = false;
}

void UBSRecoilComponent::BeginPlay()
{
	Super::BeginPlay();

	/* Bind UpdateRecoil to the Recoil vector curve and timeline */
	FOnTimelineVector RecoilProgressFunction;
	RecoilProgressFunction.BindUFunction(this, FName("UpdateRecoil"));
	RecoilTimeline.AddInterpVector(RecoilCurve, RecoilProgressFunction);
}

void UBSRecoilComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	RecoilTimeline.TickTimeline(DeltaTime);
	UpdateKickbackAndRecoil(DeltaTime);
}

FRotator UBSRecoilComponent::GetCurrentRecoilRotation() const
{
	return FRotator(-CurrentShotRecoilRotation.Pitch, CurrentShotRecoilRotation.Yaw, CurrentShotRecoilRotation.Roll);
}

void UBSRecoilComponent::Recoil()
{
	bIsFiring = true;
	RecoilTimeline.SetPlayRate(1.f);
	/* Resume timeline from current position if it hasn't fully recovered */
	if (RecoilTimeline.IsReversing())
	{
		/* Increment shots fired since the timeline will always be reversing if the player is mid-spray */
		ShotsFired++;
		RecoilTimeline.Play();
	}
	else
	{
		/* Since recoil timeline isn't reversing, the player isn't mid-spray, so we can reset the current shots fired */
		ShotsFired = 0;
		RecoilTimeline.PlayFromStart();
	}
	bShouldKickback = true;
	KickbackAlpha = 0.f;
}

void UBSRecoilComponent::UpdateKickbackAndRecoil(float DeltaTime)
{
	UpdateKickback(DeltaTime);
	const FRotator Current = GetRelativeRotation();
	const FRotator UpdatedRotation = UKismetMathLibrary::RInterpTo(Current, CurrentShotCameraRecoilRotation, DeltaTime, CameraRecoilInterpSpeed);
	SetRelativeRotation(UpdatedRotation + FRotator(KickbackAngle, 0, 0));
}

void UBSRecoilComponent::UpdateKickback(float DeltaTime)
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

void UBSRecoilComponent::UpdateRecoil(FVector Output)
{
	if (!bIsFiring)
	{
		return;
	}

	/* Apply a lighter recoil penalty if its the first bullet */
	if (ShotsFired < 1)
	{
		CurrentShotCameraRecoilRotation.Yaw = Output.X * 0.35;
		CurrentShotCameraRecoilRotation.Pitch = Output.Y * 0.35;
		CurrentShotRecoilRotation.Yaw = Output.X;
		CurrentShotRecoilRotation.Pitch = Output.Y;
		return;
	}
	/* Apply a heavier recoil penalty if in the middle of a spray */
	CurrentShotCameraRecoilRotation.Yaw = Output.X * 0.5;
	CurrentShotCameraRecoilRotation.Pitch = Output.Y * 0.5;
	CurrentShotRecoilRotation.Yaw = Output.X;
	CurrentShotRecoilRotation.Pitch = Output.Y;
}

void UBSRecoilComponent::StopRecoil()
{
	bIsFiring = false;
	CurrentShotRecoilRotation = FRotator(0, 0, 0);
	CurrentShotCameraRecoilRotation = FRotator(0, 0, 0);
	
	/* Reverse the timeline so that it takes time to recover to the beginning */
	RecoilTimeline.SetPlayRate(5.454545f);
	RecoilTimeline.Reverse();
}

