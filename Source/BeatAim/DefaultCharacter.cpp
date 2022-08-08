// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Projectile.h"
#include "BeatAimGameModeBase.h"
#include "DefaultGameInstance.h"
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
	Sensitivity = 12.59;

	Camera = CreateDefaultSubobject<UCameraComponent>("First Person Camera");
	Camera->SetupAttachment(GetCapsuleComponent());
	Camera->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f));
	Camera->bUsePawnControlRotation = true;
	Camera->SetFieldOfView(103);

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
	TraceDistance = 2000;

	//HUD
	PlayerHUDClass = nullptr;
	PlayerHUD = nullptr;

	// Targets
	ShotsFired = 0.f;
	TargetsHit = 0.f;
	Accuracy = 0.f;
}

// Called when the game starts or when spawned
void ADefaultCharacter::BeginPlay()
{
	Super::BeginPlay();

	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	GI->RegisterDefaultCharacter(this);

	GunMesh->AttachToComponent(HandsMesh,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		"GripPoint");
	AnimInstance = HandsMesh->GetAnimInstance();

	if (IsLocallyControlled() && PlayerHUDClass)
	{
		// Want HUD to be owned by local player controller,
		// bc it will have references to local player and viewports attached to it
		APlayerController* FPC = GetController<APlayerController>();
		check(FPC);
		PlayerHUD = CreateWidget<UPlayerHUD>(FPC, PlayerHUDClass);
		check(PlayerHUD);
		PlayerHUD->AddToPlayerScreen();
		PlayerHUD->SetTargetBar(TargetsHit, ShotsFired);
	}
}

void ADefaultCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (PlayerHUD)
	{
		PlayerHUD->RemoveFromParent();
		// Can't destroy widget directly, garbage collector will take care of it when safe to do so
		PlayerHUD = nullptr;
	}

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

void ADefaultCharacter::SetSensitivity(float InputSensitivity)
{
	Sensitivity = InputSensitivity;
}

float ADefaultCharacter::GetSensitivity()
{
	return Sensitivity;
}

void ADefaultCharacter::SetTargetsHit(bool Hit)
{
	if (PlayerHUD)
	{
		if (Hit == true)
		{
			TargetsHit++;
		}
		PlayerHUD->SetTargetsHit(TargetsHit);
		PlayerHUD->SetTargetBar(TargetsHit, ShotsFired);
		PlayerHUD->SetAccuracy(TargetsHit, ShotsFired);
		PlayerHUD->SetShotsFired(ShotsFired);
	}
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
			InteractPressed();
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			// Spawn the projectile at the muzzle.
			AProjectile* Projectile = World->SpawnActor<AProjectile>(ProjectileClass, Muzzle, MuzzleRotation, SpawnParams);
			if (Projectile)
			{
				SetShotsFired();
				SetTargetsHit(false);
				Projectile->SetOwner(this);
				Projectile->SetInstigator(this);
				// Set the projectile's initial trajectory.
				FVector LaunchDirection = MuzzleRotation.Vector();
				// DrawDebugLine(GetWorld(), Muzzle, LaunchDirection, FColor::Red, false, 3);
				Projectile->FireInDirection(LaunchDirection);
			}

		}
	}
}

void ADefaultCharacter::SetShotsFired()
{
	ShotsFired++;
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

