// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultCharacter.h"
#include <string>
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Projectile.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "BeatAimGameModeBase.h"
#include "DefaultGameInstance.h"
#include "DefaultPlayerController.h"
#include "DefaultStatSaveGame.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "PlayerHUD.h"
#include "Blueprint/UserWidget.h"

// Sets default values
ADefaultCharacter::ADefaultCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.f);

	Camera = CreateDefaultSubobject<UCameraComponent>("First Person Camera");
	Camera->SetupAttachment(GetCapsuleComponent());
	Camera->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f));
	Camera->bUsePawnControlRotation = true;
	Camera->SetFieldOfView(103);
	Camera->PostProcessSettings.MotionBlurAmount = 0;

	HandsMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Character Mesh");
	HandsMesh->SetOnlyOwnerSee(true);
	HandsMesh->SetupAttachment(Camera);
	HandsMesh->bCastDynamicShadow=false;
	HandsMesh->CastShadow = false;
	HandsMesh->AddRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	HandsMesh->AddRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Gun");
	GunMesh->SetOnlyOwnerSee(true);
	GunMesh->bCastDynamicShadow = false;
	GunMesh->CastShadow = false;
	GunMesh->SetupAttachment(GetCapsuleComponent());

	MuzzleLocation = CreateDefaultSubobject<USceneComponent>("Muzzle Location");
	MuzzleLocation->SetupAttachment(GunMesh);
	MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));
	FTransform MuzzleTransform = GunMesh->GetSocketTransform("Muzzle");
	GunOffset = FVector(100.f, 0.f, 10.f);
	TraceDistance = 2000;
}

// Called when the game starts or when spawned
void ADefaultCharacter::BeginPlay()
{
	Super::BeginPlay();
	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GI)
	{
		GI->RegisterDefaultCharacter(this);
	}
	GunMesh->AttachToComponent(HandsMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	//GunMesh->AttachToComponent(HandsMesh,FAttachmentTransformRules::SnapToTargetNotIncludingScale,"GripPoint");
	AnimInstance = HandsMesh->GetAnimInstance();
	Sensitivity = GI->GetSensitivity();

	PlayerController = GetController<ADefaultPlayerController>();
	if (IsLocallyControlled() && PlayerController)
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
	}
}

void ADefaultCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//SaveGame();
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void ADefaultCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ADefaultCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ADefaultCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ADefaultCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ADefaultCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ADefaultCharacter::LookUp);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ADefaultCharacter::Fire);
}

void ADefaultCharacter::SetSensitivity(float NewSensitivity)
{
	Sensitivity = NewSensitivity;
}

void ADefaultCharacter::PlayRecoilAnim()
{
	AnimInstance->Montage_Play(FireAnim, 1.f);
	RecoilAnimDelay.Invalidate();
}

void ADefaultCharacter::Fire()
{
	// Attempt to fire a projectile.
	if (ProjectileClass)
	{
		// Get the camera transform.
		FVector CameraLocation;
		FRotator CameraRotation;
		GetActorEyesViewPoint(CameraLocation, CameraRotation);

		// Set MuzzleOffset to spawn projectiles slightly in front of the camera.
		MuzzleOffset.Set(100.0f, 0.0f, 0.0f);

		// Transform MuzzleOffset from camera space to world space.
		FVector Muzzle = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);
		// Skew the aim to be slightly upwards. 
		FRotator MuzzleRotation = CameraRotation;
		if (UWorld* World = GetWorld())
		{
			InteractPressed();
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();


			// Spawn the projectile at the muzzle.
			AProjectile* Projectile = World->SpawnActor<AProjectile>(ProjectileClass, Muzzle, MuzzleRotation, SpawnParams);
			if (Projectile)
			{
				//If reached this point, the player has fired
				if (PlayerController->CountdownActive == false)
				{
					//Only updating Shots Fired
					OnShotFired.Broadcast();
				}

				Projectile->SetOwner(this);
				Projectile->SetInstigator(this);

				// Set the projectile's initial trajectory.
				FVector LaunchDirection = MuzzleRotation.Vector();

				// DrawDebugLine(GetWorld(), Muzzle, LaunchDirection, FColor::Red, false, 3);
				Projectile->FireInDirection(LaunchDirection);

				// Spawn muzzle flash niagara system
				if (NS_MuzzleFlash)
				{
					FTransform MuzzleTransform = GunMesh->GetSocketTransform("Muzzle");
					UNiagaraComponent* MuzzleFlashComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS_MuzzleFlash, MuzzleTransform.GetLocation(), MuzzleTransform.Rotator());
				}
				GetWorldTimerManager().SetTimer(RecoilAnimDelay,this, &ADefaultCharacter::PlayRecoilAnim, 0.05f, false);
			}

		}
	}
}

void ADefaultCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ADefaultCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ADefaultCharacter::Turn(float Value)
{
	if (Value != 0.f && PlayerController && PlayerController->IsLocalPlayerController())
	{
		PlayerController->AddYawInput(Value / 14.2789148024750118991f * Sensitivity);
	}
}

void ADefaultCharacter::LookUp(float Value)
{
	if (Value != 0.f && PlayerController && PlayerController->IsLocalPlayerController())
	{
		PlayerController->AddPitchInput(Value / 14.2789148024750118991f * Sensitivity);
	}
}

void ADefaultCharacter::InteractPressed()
{
	TraceForward();
}

void ADefaultCharacter::TraceForward_Implementation()
{
	FVector Loc;
	FRotator Rot;
	FHitResult Hit;

	GetController()->GetPlayerViewPoint(Loc, Rot);

	FVector Start = Loc;
	FVector End = Start + (Rot.Vector() * TraceDistance);

	FCollisionQueryParams TraceParams;
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams);

	DrawDebugLine(GetWorld(), Start, End, FColor::Orange, false, 2.f);

	if (bHit)
	{
		DrawDebugBox(GetWorld(), Hit.ImpactPoint, FVector(5, 5, 5), FColor::Emerald, false, 2.f);
		FString HitName = Hit.GetActor()->GetActorNameOrLabel();
	}
}

