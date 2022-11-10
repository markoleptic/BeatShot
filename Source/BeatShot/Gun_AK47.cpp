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
	PunchAngle = FRotator::ZeroRotator;
}

// Called when the game starts or when spawned
void AGun_AK47::BeginPlay()
{
	Super::BeginPlay();

	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	PlayerController = GI->DefaultPlayerControllerRef;
	Character = GI->DefaultCharacterRef;

	// Setting our recoil & recovery curves
	if (VerticalRecoilCurve)
	{
		FOnTimelineFloat VerticalRecoilProgressFunction;
		VerticalRecoilProgressFunction.BindUFunction(this, FName("HandleVerticalRecoilProgress"));
		VerticalRecoilTimeline.AddInterpFloat(VerticalRecoilCurve, VerticalRecoilProgressFunction);
	}

	if (HorizontalRecoilCurve)
	{
		FOnTimelineFloat HorizontalRecoilProgressFunction;
		HorizontalRecoilProgressFunction.BindUFunction(this, FName("HandleHorizontalRecoilProgress"));
		HorizontalRecoilTimeline.AddInterpFloat(HorizontalRecoilCurve, HorizontalRecoilProgressFunction);
	}

	if (RecoveryCurve)
	{
		FOnTimelineFloat RecoveryProgressFunction;
		RecoveryProgressFunction.BindUFunction(this, FName("HandleRecoveryProgress"));
		RecoilRecoveryTimeline.AddInterpFloat(RecoveryCurve, RecoveryProgressFunction);
	}
}

// Called every frame
void AGun_AK47::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	VerticalRecoilTimeline.TickTimeline(DeltaTime);
	HorizontalRecoilTimeline.TickTimeline(DeltaTime);
	RecoilRecoveryTimeline.TickTimeline(DeltaTime);

	//InterpRecoil(DeltaTime);
	//InterpFinalRecoil(DeltaTime);

	//PlayerController->SetControlRotation(FMath::FInterpTo(PlayerController->GetControlRotation(), PlayerController->GetControlRotation() + PunchAngle, DeltaTime, 1));

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
				Recoil();
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
		// sets a timer for firing the weapon - if bAutomaticFire is true then this timer will repeat until cleared by StopFire(), leading to fully automatic fire
		GetWorldTimerManager().SetTimer(ShotDelay, this, &AGun_AK47::Fire, 0.10f, true, 0.0f);


		// Simultaneously begins to play the recoil timeline
		StartRecoil();
	}
}

void AGun_AK47::StopFire()
{
	// Stops the gun firing (for automatic fire)
	GetWorldTimerManager().ClearTimer(ShotDelay);
	VerticalRecoilTimeline.Stop();
	HorizontalRecoilTimeline.Stop();
	RecoilRecovery();
}

void AGun_AK47::Recoil()
{
	//const ADefaultCharacter* PlayerCharacter = Cast<ADefaultCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	//ADefaultPlayerController* Controller = PlayerCharacter->GetController<ADefaultPlayerController>();

	//if  (IsValid(VerticalRecoilCurve) && IsValid(HorizontalRecoilCurve))
	//{
		//Controller->AddPitchInput(VerticalRecoilCurve->GetFloatValue(VerticalRecoilTimeline.GetPlaybackPosition()));
		//Controller->AddYawInput(HorizontalRecoilCurve->GetFloatValue(HorizontalRecoilTimeline.GetPlaybackPosition()));

		//RecoilRotation.Pitch += FMath::FRandRange(0.f, 2.f);
		//RecoilRotation.Yaw += FMath::FRandRange(-1.f, 1.f);
		//FRotator LocalRotation = { FMath::FRandRange(0.f, 5.f), FMath::FRandRange(-2.f, 2.f), 0.f };
		//PlayerController->SetControlRotation(PlayerController->GetControlRotation() + LocalRotation);
	//}
	//ShotsFired += 1;
}

void AGun_AK47::EnableFire()
{
	bCanFire = true;
}

void AGun_AK47::StartRecoil()
{
	// Getting a reference to the Character Controller
	//const ADefaultCharacter* PlayerCharacter = Cast<ADefaultCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	//const ADefaultPlayerController* Controller = PlayerCharacter->GetController<ADefaultPlayerController>();

	//ShotsFired = 0;
	//if (bCanFire && Controller)
	//{
		// Plays the recoil timelines and saves the current control rotation in order to recover to it
		//VerticalRecoilTimeline.PlayFromStart();
		//HorizontalRecoilTimeline.PlayFromStart();
		//ControlRotation = Controller->GetControlRotation();
	//}
}

void AGun_AK47::RecoilRecovery()
{
	//const ADefaultCharacter* PlayerCharacter = Cast<ADefaultCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	//ADefaultPlayerController* Controller = PlayerCharacter->GetController<ADefaultPlayerController>();
	// Plays the recovery timeline
	//if (bShouldRecover)
	//{
	//	PunchAngle = FRotator::ZeroRotator;
	//}
}

// Recovering the player's recoil to the pre-fired position
void AGun_AK47::HandleRecoveryProgress(float Value)
{
	// Getting a reference to the Character Controller
	//const ADefaultCharacter* PlayerCharacter = Cast<ADefaultCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	//ADefaultPlayerController* Controller = PlayerCharacter->GetController<ADefaultPlayerController>();

	// Calculating the new control rotation by interpolating between current and target 
	//const FRotator NewControlRotation = FMath::Lerp(Controller->GetControlRotation(), ControlRotation, Value);
	//Controller->SetControlRotation(NewControlRotation);
}

void AGun_AK47::InterpFinalRecoil(float DeltaSeconds)
{
	//UKismetMathLibrary::RInterpTo(FinalRecoilRotation,
	//	FRotator(),
	//	DeltaSeconds,
	//	10);
	//UE_LOG(LogTemp, Display, TEXT("FinalRecoilRotation %s"), *FinalRecoilRotation.ToString());
}

void AGun_AK47::InterpRecoil(float DeltaSeconds)
{
	//UKismetMathLibrary::RInterpTo(RecoilRotation,
	//	FRotator(),
	//	DeltaSeconds,
	//	10);
	//UE_LOG(LogTemp, Display, TEXT("RecoilRotation %s"), *RecoilRotation.ToString());
}

