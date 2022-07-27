// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Projectile.h"
#include "BeatAimGameModeBase.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"

// Sets default values
ADefaultCharacter::ADefaultCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(40.f, 95.f);
	Sensitivity = 45.f;

	Camera = CreateDefaultSubobject<UCameraComponent>("First Person Camera");
	Camera->SetupAttachment(GetCapsuleComponent());
	Camera->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f + BaseEyeHeight));
	Camera->bUsePawnControlRotation = true;
	Camera->SetFieldOfView(110);

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

	MuzzleLocation = CreateDefaultSubobject<USceneComponent>("Muzzle Location");
	MuzzleLocation->SetupAttachment(GunMesh);
	MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	GunOffset = FVector(100.f, 0.f, 10.f);
}

// Called when the game starts or when spawned
void ADefaultCharacter::BeginPlay()
{
	Super::BeginPlay();
	GunMesh->AttachToComponent(HandsMesh,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		"GripPoint");
	AnimInstance = HandsMesh->GetAnimInstance();
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

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			// Spawn the projectile at the muzzle.
			AProjectile* Projectile = World->SpawnActor<AProjectile>(ProjectileClass, Muzzle, MuzzleRotation, SpawnParams);
			if (Projectile)
			{
				// Set the projectile's initial trajectory.
				FVector LaunchDirection = MuzzleRotation.Vector();
				DrawDebugLine(GetWorld(), Muzzle, LaunchDirection, FColor::Red, false, 3);
				Projectile->FireInDirection(LaunchDirection);
			}

		}
	}
	//if (GetWorld() != nullptr)
	//{
	//	FRotator SpawnRotation = GetControlRotation();
	//	FVector SpawnLocation = MuzzleLocation != nullptr ?
	//		MuzzleLocation->GetComponentLocation() :
	//		GetActorLocation() + SpawnRotation.RotateVector(GunOffset);
	//	GetWorld()->SpawnActor<AProjectile>(ProjectileClass,
	//		SpawnLocation,
	//		SpawnRotation);
	//	UE_LOG(LogTemp, Display, TEXT("cum"));
	//}
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
	AddControllerYawInput(Value * Sensitivity * GetWorld()->GetDeltaSeconds());
}

void ADefaultCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value * Sensitivity * GetWorld()->GetDeltaSeconds());
}

