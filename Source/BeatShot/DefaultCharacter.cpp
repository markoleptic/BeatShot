// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DefaultGameInstance.h"
#include "DefaultPlayerController.h"
#include "Gun_AK47.h"
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
	Camera->PostProcessSettings.bOverride_MotionBlurMax = 0;

	HandsMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Character Mesh");
	HandsMesh->SetOnlyOwnerSee(true);
	HandsMesh->SetupAttachment(Camera);
	HandsMesh->bCastDynamicShadow = false;
	HandsMesh->CastShadow = false;
	HandsMesh->AddRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	HandsMesh->AddRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));
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

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = GetInstigator();
	Gun = GetWorld()->SpawnActor<AGun_AK47>(GunClass, HandsMesh->GetSocketTransform("GripPoint"), SpawnParameters);

	Gun->AttachToComponent(HandsMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	AnimInstance = HandsMesh->GetAnimInstance();
	Sensitivity = GI->LoadPlayerSettings().Sensitivity;

	PlayerController = GetController<ADefaultPlayerController>();
	if (IsLocallyControlled() && PlayerController)
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
	}
}

void ADefaultCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
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

void ADefaultCharacter::Fire()
{
	Gun->Fire();
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

