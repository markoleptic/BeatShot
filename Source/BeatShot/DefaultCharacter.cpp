// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "SaveGamePlayerSettings.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DefaultGameInstance.h"
#include "DefaultPlayerController.h"
#include "Gun_AK47.h"
#include "C:/Program Files/Epic Games/UE_5.0/Engine/Plugins/Experimental/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "C:/Program Files/Epic Games/UE_5.0/Engine/Plugins/Experimental/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ADefaultCharacter::ADefaultCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(55.f, DefaultCapsuleHalfHeight);

	// Spawning the spring arm component
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComponent->bUsePawnControlRotation = true;
	//SpringArmComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f));
	SpringArmComponent->SetRelativeLocation(FVector(0.f, 0.f, 64.f));
	SpringArmComponent->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>("First Person Camera");
	Camera->SetupAttachment(SpringArmComponent);
	//Camera->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f));
	Camera->bUsePawnControlRotation = true;
	Camera->SetFieldOfView(103);
	Camera->PostProcessSettings.MotionBlurAmount = 0;
	Camera->PostProcessSettings.bOverride_MotionBlurMax = 0;

	HandsMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Character Mesh");
	HandsMesh->SetOnlyOwnerSee(true);
	HandsMesh->SetupAttachment(Camera);
	HandsMesh->bCastDynamicShadow = false;
	HandsMesh->CastShadow = false;
	//HandsMesh->AddRelativeRotation(FRotator(1.9f, -15.19f, 8.2f));
	//HandsMesh->AddRelativeLocation(FVector(-14.5f, -16.4f, -149.7f));
	//HandsMesh->AddRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	//HandsMesh->AddRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));
	HandsMesh->SetRelativeRotation(FRotator(1.53f, -15.20f, 8.32f));
	HandsMesh->SetRelativeLocation(FVector(-17.69f, -10.50f, -149.11f));
	//HandsMesh->AddRelativeRotation(FRotator(1.53f, -15.20f, 8.32f));
	//HandsMesh->AddRelativeLocation(FVector(-17.69f, -10.50f, -149.11f));
}

// Called when the game starts or when spawned
void ADefaultCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Load settings and listen for changes to Player Settings
	if (UDefaultGameInstance* GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		GI->RegisterDefaultCharacter(this);
		Sensitivity = GI->LoadPlayerSettings().Sensitivity;
		GI->OnPlayerSettingsChange.AddDynamic(this, &ADefaultCharacter::SetSensitivity);
	}

	// Spawn gun attached to HandsMesh
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = GetInstigator();
	Gun = GetWorld()->SpawnActor<AGun_AK47>(GunClass, HandsMesh->GetSocketTransform("GripPoint"), SpawnParameters);
	Gun->AttachToComponent(HandsMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	ADefaultPlayerController* PlayerController = GetController<ADefaultPlayerController>();
	if (IsLocallyControlled() && PlayerController)
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
	}
}

// Sets the Mapping Context in Player Controller
void ADefaultCharacter::PawnClientRestart() {

	Super::PawnClientRestart();

	// Make sure that we have a valid PlayerController.
	if (const ADefaultPlayerController* PlayerController = Cast<ADefaultPlayerController>(GetController()))
	{
		// Get the Enhanced Input Local Player Subsystem from the Local Player related to our Player Controller.
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// PawnClientRestart can run more than once in an Actor's lifetime, so start by clearing out any leftover mappings.
			Subsystem->ClearAllMappings();

			// Add each mapping context, along with their priority values. Higher values outprioritize lower values.
			Subsystem->AddMappingContext(BaseMappingContext, BaseMappingPriority);
		}
	}
}

// Called every frame
void ADefaultCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Crouching
	// Sets the new Target Half Height based on whether the player is crouching or standing
	const float TargetHalfHeight = (MovementState == EMovementState::State_Crouch ? CrouchedCapsuleHalfHeight : DefaultCapsuleHalfHeight);
	// Interpolates between the current height and the target height
	const float NewHalfHeight = FMath::FInterpTo(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), TargetHalfHeight, DeltaTime, CrouchSpeed);
	// Sets the half height of the capsule component to the new interpolated half height
	GetCapsuleComponent()->SetCapsuleHalfHeight(NewHalfHeight);
}

// Called to bind functionality to input
void ADefaultCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (JumpAction)
		{
			// Jumping
			PlayerEnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ADefaultCharacter::Jump);
		}
		if (SprintAction)
		{
			// Sprinting
			PlayerEnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ADefaultCharacter::StartWalk);
			PlayerEnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ADefaultCharacter::StopWalk);
		}
		if (MovementAction)
		{
			// Move forward/back + left/right inputs
			PlayerEnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &ADefaultCharacter::Move);
		}
		if (LookAction)
		{
			// Look up/down + left/right
			PlayerEnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADefaultCharacter::Look);
		}
		if (CrouchAction)
		{
			// Crouching
			PlayerEnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ADefaultCharacter::StartCrouch);
			PlayerEnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ADefaultCharacter::ReleaseCrouch);
		}
		if (FiringAction)
		{
			// Firing
			PlayerEnhancedInputComponent->BindAction(FiringAction, ETriggerEvent::Started, this, &ADefaultCharacter::StartFire);
			PlayerEnhancedInputComponent->BindAction(FiringAction, ETriggerEvent::Completed, this, &ADefaultCharacter::StopFire);
		}
	}
}

void ADefaultCharacter::SetSensitivity(FPlayerSettings PlayerSettings)
{
	Sensitivity = PlayerSettings.Sensitivity;
}

void ADefaultCharacter::StartFire() const
{
	Gun->StartFire();
}

void ADefaultCharacter::StopFire() const
{
	Gun->StopFire();
}

void ADefaultCharacter::Move(const FInputActionValue& Value) {

	// Storing movement vectors for animation manipulation
	ForwardMovement = Value[1];
	RightMovement = Value[0];

	// Moving the player
	if (Value.GetMagnitude() != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value[1]);
		AddMovementInput(GetActorRightVector(), Value[0]);
	}
}

void ADefaultCharacter::Look(const FInputActionValue& Value)
{
	// Storing look vectors for animation manipulation
	MouseX = Value[1];
	MouseY = Value[0];

	AddControllerPitchInput(Value[1] / 14.2789148024750118991f * Sensitivity);
	AddControllerYawInput(Value[0] / 14.2789148024750118991f * Sensitivity);
}

void ADefaultCharacter::ReleaseCrouch()
{
	bHoldingCrouch = false;
	if (MovementState == EMovementState::State_Walk)
	{
		return;
	}
	UpdateMovementValues(EMovementState::State_Sprint);
}

void ADefaultCharacter::StartCrouch()
{
	bHoldingCrouch = true;
	if (GetCharacterMovement()->IsMovingOnGround())
	{
		UpdateMovementValues(EMovementState::State_Crouch);
	}
}

void ADefaultCharacter::StartWalk()
{
	bHoldingWalk = true;
	UpdateMovementValues(EMovementState::State_Walk);
}

void ADefaultCharacter::StopWalk()
{
	if (MovementState == EMovementState::State_Walk)
	{
		UpdateMovementValues(EMovementState::State_Sprint);
	}
	bHoldingWalk = false;
}

void ADefaultCharacter::UpdateMovementValues(const EMovementState NewMovementState)
{
	// Clearing sprinting and crouching flags
	bIsWalking = false;
	bIsCrouching = false;

	// Updating the movement state
	MovementState = NewMovementState;

	if (MovementDataMap.Contains(EMovementState::State_Walk))
	{
		//Gun->SetCanFire(MovementDataMap[MovementState].bCanFire);
		GetCharacterMovement()->MaxAcceleration = MovementDataMap[MovementState].MaxAcceleration;
		GetCharacterMovement()->BrakingDecelerationWalking = MovementDataMap[MovementState].BreakingDecelerationWalking;
		GetCharacterMovement()->GroundFriction = MovementDataMap[MovementState].GroundFriction;
		GetCharacterMovement()->MaxWalkSpeed = MovementDataMap[MovementState].MaxWalkSpeed;
	}
	else
	{
		UE_LOG(LogProfilingDebugging, Error, TEXT("Set up data in MovementDataMap!"))
	}

	// Updating sprinting and crouching flags
	if (MovementState == EMovementState::State_Crouch)
	{
		bIsCrouching = true;
	}
	if (MovementState == EMovementState::State_Walk)
	{
		bIsWalking = true;
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
	FVector End = Start + (Rot.Vector() * 5000);

	FCollisionQueryParams TraceParams;
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams);

	DrawDebugLine(GetWorld(), Start, End, FColor::Orange, false, 2.f);

	if (bHit)
	{
		DrawDebugBox(GetWorld(), Hit.ImpactPoint, FVector(5, 5, 5), FColor::Emerald, false, 2.f);
		FString HitName = Hit.GetActor()->GetActorNameOrLabel();
	}
}