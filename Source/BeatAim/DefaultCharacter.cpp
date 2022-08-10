// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultCharacter.h"
#include <string>
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Projectile.h"
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
	HUDActive = false;
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
	GunMesh->AttachToComponent(HandsMesh,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		"GripPoint");
	//AnimInstance = HandsMesh->GetAnimInstance();

	if (IsLocallyControlled() && PlayerHUDClass)
	{
		// Want HUD to be owned by DefaultPlayerController,
		// bc it will have references to local player and viewports attached to it
		PlayerController = GetController<ADefaultPlayerController>();
		PlayerHUD = CreateWidget<UPlayerHUD>(PlayerController, PlayerHUDClass);
	}
}

void ADefaultCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//SaveGame();
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

void ADefaultCharacter::ShowPlayerHUD(bool ShouldShow)
{
	if (ShouldShow == true)
	{
		//check(PlayerHUD);
		if (PlayerHUD)
		{
			PlayerHUD->AddToPlayerScreen();
			HUDActive = true;
		}
	}
	else if (ShouldShow == false)
	{
		if (PlayerHUD)
		{
			PlayerHUD->RemoveFromParent();
			HUDActive = false;
		}
	}
}

void ADefaultCharacter::SetSensitivity(float InputSensitivity)
{
	Sensitivity = InputSensitivity;
}

float ADefaultCharacter::GetSensitivity()
{
	return Sensitivity;
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
				// If reached this point, the player has fired
				if (GI->GameModeBaseRef->GameModeSelected)
				{
					// Only updating Shots Fired
					GI->GameModeBaseRef->UpdatePlayerStats(true, false, false);
				}

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

void ADefaultCharacter::SaveGame()
{
	UDefaultStatSaveGame* SaveGameInstance = Cast<UDefaultStatSaveGame>(UGameplayStatics::CreateSaveGameObject(UDefaultStatSaveGame::StaticClass()));
	//TArray<float> SpiderShotScoreArray;
	//SpiderShotScoreArray.Add(GI->GetTargetsHit());
	//SpiderShotScoreArray.Add(GI->GetShotsFired());
	//SpiderShotScoreArray.Add(GI->GetTargetsSpawned());
	//SaveGameInstance->InsertToArrayOfSpiderShotScores(SpiderShotScoreArray);
	SaveGameInstance->PlayerLocation = this->GetActorLocation();
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("MySlot"), 0);
}

void ADefaultCharacter::LoadGame()
{
	UDefaultStatSaveGame* SaveGameInstance = Cast<UDefaultStatSaveGame>(UGameplayStatics::CreateSaveGameObject(UDefaultStatSaveGame::StaticClass()));
	SaveGameInstance = Cast<UDefaultStatSaveGame>(UGameplayStatics::LoadGameFromSlot("MySlot", 0));
	this->SetActorLocation(SaveGameInstance->PlayerLocation);
	TArray<TArray<float>> SpiderShotScores = SaveGameInstance->GetArrayOfSpiderShotScores();
	if (SpiderShotScores.Num() > 1)
	{
		GI->SetTargetsHit(SpiderShotScores[SpiderShotScores.Num()-1][0]);
		GI->SetShotsFired(SpiderShotScores[SpiderShotScores.Num() - 1][2]);
		GI->SetTargetsSpawned(SpiderShotScores[SpiderShotScores.Num() - 1][3]);
	}
	UE_LOG(LogTemp, Display, TEXT("SpiderShotScore Array Size: %f"), SpiderShotScores.Num());
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

