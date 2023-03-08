// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.
// Credit to Dan Kestranek.

#include "BSCharacter.h"
#include "BSCharacterMovementComponent.h"
#include "SaveGamePlayerSettings.h"
#include "BSGameInstance.h"
#include "BSInputComponent.h"
#include "BSPlayerController.h"
#include "BSPlayerState.h"
#include "SphereTarget.h"
#include "Gun_AK47.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameplayAbilitySpec.h"
#include "GameplayAbilitySpecHandle.h"
#include "BeatShot/BSGameplayTags.h"
#include "GameplayAbility/BSAbilitySystemComponent.h"
#include "GameplayAbility/BSGameplayAbility.h"
#include "GameplayAbility/AttributeSets/BSAttributeSetBase.h"
#include "Kismet/KismetMathLibrary.h"

UE_DEFINE_GAMEPLAY_TAG(InputTag_Look,"InputTag.Look");
UE_DEFINE_GAMEPLAY_TAG(InputTag_Move,"InputTag.Move");
UE_DEFINE_GAMEPLAY_TAG(InputTag_Sprint,"InputTag.Sprint");


ABSCharacter::ABSCharacter(const FObjectInitializer& ObjectInitializer) :
Super(ObjectInitializer.SetDefaultSubobjectClass<UBSCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bAlwaysRelevant = true;
	
	GetCapsuleComponent()->InitCapsuleSize(55.f, DefaultCapsuleHalfHeight);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->SetRelativeLocation(FVector(0.f, 0.f, 64.f));
	SpringArmComponent->SetupAttachment(RootComponent);

	ShotDirection = CreateDefaultSubobject<UArrowComponent>("ShotDirection");
	ShotDirection->SetupAttachment(SpringArmComponent);

	CameraRecoilComp = CreateDefaultSubobject<USceneComponent>("CameraRecoilComp");
	CameraRecoilComp->SetupAttachment(ShotDirection);

	Camera = CreateDefaultSubobject<UCameraComponent>("First Person Camera");
	Camera->SetupAttachment(CameraRecoilComp);
	Camera->bUsePawnControlRotation = false;
	Camera->SetFieldOfView(103);
	Camera->PostProcessSettings.MotionBlurAmount = 0;
	Camera->PostProcessSettings.bOverride_MotionBlurMax = 0;

	HandsMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Character Mesh");
	HandsMesh->SetOnlyOwnerSee(true);
	HandsMesh->SetupAttachment(Camera);
	HandsMesh->bCastDynamicShadow = false;
	HandsMesh->CastShadow = false;
	HandsMesh->SetRelativeRotation(FRotator(1.53f, -15.20f, 8.32f));
	HandsMesh->SetRelativeLocation(FVector(-17.69f, -10.50f, -149.11f));

	GunActorComp = CreateDefaultSubobject<UChildActorComponent>("GunActorComp");
	GunActorComp->SetChildActorClass(GunClass);
	GunActorComp->SetupAttachment(HandsMesh, "GripPoint");
	GunActorComp->CreateChildActor();
}

void ABSCharacter::BeginPlay()
{
	Super::BeginPlay();

	Gun = Cast<AGun_AK47>(GunActorComp->GetChildActor());
	OnUserSettingsChange(LoadPlayerSettings());
	ABSPlayerController* PlayerController = GetController<ABSPlayerController>();
	if (IsLocallyControlled())
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
	}
	Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->OnPlayerSettingsChange.AddUniqueDynamic(
		this, &ABSCharacter::OnUserSettingsChange);

	FOnTimelineFloat OnTimelineFloat;
	OnTimelineFloat.BindUFunction(this, FName("OnTimelineTick_AimBot"));
	FOnTimelineEvent OnTimelineEvent;
	OnTimelineEvent.BindUFunction(this, FName("OnTimelineCompleted_AimBot"));
	AimBotTimeline.SetTimelineFinishedFunc(OnTimelineEvent);
	AimBotTimeline.AddInterpFloat(Curve_AimBotRotationSpeed, OnTimelineFloat);
	bEnabled_AimBot = false;
}

void ABSCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	/* Make sure that we have a valid PlayerController */
	if (const ABSPlayerController* PlayerController = Cast<ABSPlayerController>(GetController()))
	{
		/* Get the Enhanced Input Local Player Subsystem from the Local Player related to our Player Controller */
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			/* PawnClientRestart can run more than once in an Actor's lifetime, so start by clearing out any leftover mappings */
			Subsystem->ClearAllMappings();
			/* Add each mapping context, along with their priority values. Higher values out-prioritize lower values */
			Subsystem->AddMappingContext(BaseMappingContext, BaseMappingPriority);
		}
	}
}

void ABSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// /* Sets the new Target Half Height based on whether the player is crouching or standing */
	// const float TargetHalfHeight = (MovementState == EMovementType::Crouching
	// 	                                ? CrouchedCapsuleHalfHeight
	// 	                                : DefaultCapsuleHalfHeight);
	// /* Interpolates between the current height and the target height */
	// const float NewHalfHeight = FMath::FInterpTo(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), TargetHalfHeight,
	//                                              DeltaTime, CrouchSpeed);
	// /* Sets the half height of the capsule component to the new interpolated half height */
	// GetCapsuleComponent()->SetCapsuleHalfHeight(NewHalfHeight);

	if (!bEnabled_AimBot || ActiveTargets_AimBot.IsEmpty())
	{
		return;
	}

	FVector Loc;
	FRotator Rot;
	GetController()->GetActorEyesViewPoint(Loc, Rot);
	Rot.Normalize();
	ASphereTarget* Target;
	ActiveTargets_AimBot.Peek(Target);
	if (!Target->IsBeatTrackTarget())
	{
		AimBotTimeline.TickTimeline(DeltaTime);
		return;
	}
	
	FRotator NewRot;
	if (bIsLagging)
	{
		NewRot = UKismetMathLibrary::RInterpTo_Constant(
			Rot, UKismetMathLibrary::FindLookAtRotation(Loc, LagLocation), DeltaTime,
			FMath::FRandRange(10.f, 15.f));
	}
	else
	{
		NewRot = UKismetMathLibrary::RInterpTo(
			Rot, UKismetMathLibrary::FindLookAtRotation(Loc,  Target->GetActorLocation()), DeltaTime,
			FMath::FRandRange(11.f, 15.f));
	}
	GetController()->SetControlRotation(NewRot);
}

void ABSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (JumpAction)
		{
			// Jumping
			PlayerEnhancedInputComponent->
				BindAction(JumpAction, ETriggerEvent::Started, this, &ABSCharacter::Jump);
		}
		// if (SprintAction)
		// {
		// 	// Sprinting
		// 	PlayerEnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this,
		// 	                                         &ABSCharacter::StartWalk);
		// 	PlayerEnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this,
		// 	                                         &ABSCharacter::StopWalk);
		// }
		// if (MovementAction)
		// {
		// 	// Move forward/back + left/right inputs
		// 	PlayerEnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this,
		// 	                                         &ABSCharacter::Move);
		// }
		// if (LookAction)
		// {
		// 	// Look up/down + left/right
		// 	PlayerEnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this,
		// 	                                         &ABSCharacter::Look);
		// }
		if (CrouchAction)
		{
			// Crouching
			PlayerEnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this,
			                                         &ABSCharacter::StartCrouch);
			PlayerEnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this,
			                                         &ABSCharacter::ReleaseCrouch);
		}
		if (FiringAction)
		{
			// Firing
			PlayerEnhancedInputComponent->BindAction(FiringAction, ETriggerEvent::Started, this,
			                                         &ABSCharacter::StartFire);
			PlayerEnhancedInputComponent->BindAction(FiringAction, ETriggerEvent::Completed, this,
			                                         &ABSCharacter::StopFire);
		}
		if (InteractAction)
		{
			PlayerEnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this,
			                                         &ABSCharacter::OnInteractStarted);
			PlayerEnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this,
			                                         &ABSCharacter::OnInteractCompleted);
		}
		if (ShiftInteractAction)
		{
			PlayerEnhancedInputComponent->BindAction(ShiftInteractAction, ETriggerEvent::Started, this,
			                                         &ABSCharacter::OnShiftInteractStarted);
			PlayerEnhancedInputComponent->BindAction(ShiftInteractAction, ETriggerEvent::Completed, this,
			                                         &ABSCharacter::OnShiftInteractCompleted);
		}
		InitializePlayerInput(InputComponent);
	}
}

void ABSCharacter::OnUserSettingsChange(const FPlayerSettings& PlayerSettings)
{
	Sensitivity = PlayerSettings.Sensitivity;
	if (Gun)
	{
		if (Gun->bAutomaticFire != PlayerSettings.Game.bAutomaticFire ||
			Gun->bShouldRecoil != PlayerSettings.Game.bShouldRecoil)
		{
			Gun->StopFire();
			Camera->SetRelativeRotation(FRotator(0, 0, 0));
			CameraRecoilComp->SetRelativeRotation(FRotator(0, 0, 0));
			Gun->bShouldRecoil = PlayerSettings.Game.bShouldRecoil;
			Gun->bAutomaticFire = PlayerSettings.Game.bAutomaticFire;
		}
		if (Gun->bShowBulletDecals != PlayerSettings.Game.bShowBulletDecals)
		{
			Gun->bShowBulletDecals = PlayerSettings.Game.bShowBulletDecals;
		}
		Gun->PlayerSettings = PlayerSettings;
	}
}

void ABSCharacter::StartFire()
{
	Gun->StartFire();
}

void ABSCharacter::StopFire()
{
	Gun->StopFire();
}

void ABSCharacter::Move(const FInputActionValue& Value)
{
	// Moving the player
	if (Value.GetMagnitude() != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value[1]);
		AddMovementInput(GetActorRightVector(), Value[0]);
	}
}

void ABSCharacter::Look(const FInputActionValue& Value)
{
	AddControllerPitchInput(Value[1] / 14.2789148024750118991f * Sensitivity);
	AddControllerYawInput(Value[0] / 14.2789148024750118991f * Sensitivity);
}

void ABSCharacter::ReleaseCrouch()
{
	// bHoldingCrouch = false;
	// if (MovementState == EMovementType::Walking)
	// {
	// 	return;
	// }
	// UpdateMovementValues(EMovementType::Sprinting);
}

void ABSCharacter::StartCrouch()
{
	// bHoldingCrouch = true;
	// if (GetCharacterMovement()->IsMovingOnGround())
	// {
	// 	UpdateMovementValues(EMovementType::Crouching);
	// }
}

void ABSCharacter::StartWalk()
{
	// UE_LOG(LogTemp, Display, TEXT("StartWalk called"));
	// bHoldingWalk = true;
	// UpdateMovementValues(EMovementType::Walking);
}

void ABSCharacter::StopWalk()
{
	// if (MovementState == EMovementType::Walking)
	// {
	// 	UpdateMovementValues(EMovementType::Sprinting);
	// }
	// bHoldingWalk = false;
}

void ABSCharacter::OnInteractStarted(const FInputActionInstance& Instance)
{
	if (!OnInteractDelegate.ExecuteIfBound(0))
	{
		UE_LOG(LogTemp, Display, TEXT("OnInteractDelegate not bound."));
	}
}

void ABSCharacter::OnInteractCompleted(const FInputActionInstance& Instance)
{
	if (!OnInteractDelegate.ExecuteIfBound(1))
	{
		UE_LOG(LogTemp, Display, TEXT("OnInteractDelegate not bound."));
	}
}

void ABSCharacter::OnShiftInteractStarted(const FInputActionInstance& Instance)
{
	if (!OnShiftInteractDelegate.ExecuteIfBound(0))
	{
		UE_LOG(LogTemp, Display, TEXT("OnInteractDelegate not bound."));
	}
}

void ABSCharacter::OnShiftInteractCompleted(const FInputActionInstance& Instance)
{
	if (!OnShiftInteractDelegate.ExecuteIfBound(1))
	{
		UE_LOG(LogTemp, Display, TEXT("OnInteractDelegate not bound."));
	}
}

void ABSCharacter::OnBeatTrackDirectionChanged(const FVector Location)
{
	LagLocation = Location;
	bIsLagging = true;
	LagDelegate.BindLambda([this]
	{
		bIsLagging = false;
	});
	GetWorld()->GetTimerManager().SetTimer(LagHandle, LagDelegate, FMath::FRandRange(0.1f, 0.2f), false);
}

void ABSCharacter::UpdateMovementValues(const EMovementType NewMovementType)
{
	// Clearing sprinting and crouching flags
	bIsWalking = false;
	bIsCrouching = false;

	// Updating the movement state
	MovementState = NewMovementType;

	//GunActorComp->SetCanFire(MovementDataMap[MovementState].bCanFire);
	GetCharacterMovement()->MaxAcceleration = MovementDataMap[MovementState].MaxAcceleration;
	GetCharacterMovement()->BrakingDecelerationWalking = MovementDataMap[MovementState].BreakingDecelerationWalking;
	GetCharacterMovement()->GroundFriction = MovementDataMap[MovementState].GroundFriction;
	GetCharacterMovement()->MaxWalkSpeed = MovementDataMap[MovementState].MaxWalkSpeed;


	// Updating sprinting and crouching flags
	if (MovementState == EMovementType::Crouching)
	{
		bIsCrouching = true;
	}
	if (MovementState == EMovementType::Walking)
	{
		bIsWalking = true;
	}
}

void ABSCharacter::OnTargetSpawned_AimBot(ASphereTarget* SpawnedTarget)
{
	ActiveTargets_AimBot.Enqueue(SpawnedTarget);
	if (!AimBotTimeline.IsPlaying())
	{
		DestroyNextTarget_AimBot();
	}
}

void ABSCharacter::OnTimelineTick_AimBot(const float Alpha)
{
	FVector Loc;
	FRotator Rot;
	GetController()->GetActorEyesViewPoint(Loc, Rot);
	ASphereTarget* Target;
	ActiveTargets_AimBot.Peek(Target);
	GetController()->SetControlRotation(UKismetMathLibrary::RLerp(StartRotation_AimBot,
	                                                              UKismetMathLibrary::FindLookAtRotation(
		                                                              Loc, Target->GetActorLocation()), Alpha, true));
}

void ABSCharacter::OnTimelineCompleted_AimBot()
{
	ActiveTargets_AimBot.Pop();
	Gun->Fire_AimBot();
	if (!AimBotTimeline.IsPlaying())
	{
		DestroyNextTarget_AimBot();
	}
}

void ABSCharacter::DestroyNextTarget_AimBot()
{
	if (!ActiveTargetLocations_AimBot.IsEmpty())
	{
		FVector TargetLocation;
		if (ActiveTargetLocations_AimBot.Peek(TargetLocation))
		{
			StartRotation_AimBot = GetController()->GetControlRotation();
			StartRotation_AimBot.Normalize();
			AimBotTimeline.SetPlayRate(TimelinePlaybackRate_AimBot);
			AimBotTimeline.PlayFromStart();
		}
	}
}

void ABSCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	ABSPlayerState* PS = GetPlayerState<ABSPlayerState>();
	if (PS)
	{
		// Set the ASC on the Server. Clients do this in OnRep_PlayerState()
		AbilitySystemComponent = Cast<UBSAbilitySystemComponent>(PS->GetAbilitySystemComponent());
	
		// AI won't have PlayerControllers so we can init again here just to be sure. No harm in initing twice for heroes that have PlayerControllers.
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);
	
		// Set the AttributeSetBase for convenience attribute functions
		AttributeSetBase = PS->GetAttributeSetBase();
	
		// If we handle players disconnecting and rejoining in the future, we'll have to change this so that possession from rejoining doesn't reset attributes.
		// For now assume possession = spawn/respawn.
		AddCharacterAbilities();
	}
}

void ABSCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	ABSPlayerState* PS = GetPlayerState<ABSPlayerState>();
	if (PS)
	{
		UE_LOG(LogTemp, Display, TEXT("PS Exists"));
		// Set the ASC for clients. Server does this in PossessedBy.
		AbilitySystemComponent = Cast<UBSAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		// Init ASC Actor Info for clients. Server will init its ASC when it possesses a new Actor.
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);

		// Set the AttributeSetBase for convenience attribute functions
		AttributeSetBase = PS->GetAttributeSetBase();
	}
}

UAbilitySystemComponent* ABSCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

int32 ABSCharacter::GetAbilityLevel(EBSAbilityInputID AbilityID) const
{
	return 1;
}

void ABSCharacter::RemoveCharacterAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || !AbilitySystemComponent->bCharacterAbilitiesGiven)
	{
		UE_LOG(LogTemp, Display, TEXT("Something wrong with RemoveCharacterAbilities()"));
		return;
	}

	// Remove any abilities added from a previous call. This checks to make sure the ability is in the startup 'CharacterAbilities' array.
	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if ((Spec.SourceObject == this) && CharacterAbilities.Contains(Spec.Ability->GetClass()))
		{
			AbilitiesToRemove.Add(Spec.Handle);
		}
	}

	// Do in two passes so the removal happens after we have the full list
	for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
	{
		AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
	}

	AbilitySystemComponent->bCharacterAbilitiesGiven = false;
}

float ABSCharacter::GetMoveSpeed() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMoveSpeed();
	}

	return 0.0f;
}

float ABSCharacter::GetMoveSpeedBaseValue() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMoveSpeedAttribute().GetGameplayAttributeData(AttributeSetBase.Get())->GetBaseValue();
	}

	return 0.0f;
}

void ABSCharacter::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	
	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	check(Subsystem);

	Subsystem->ClearAllMappings();
	
	if (UBSInputConfig* IC = InputConfig)
	{
		UBSInputComponent* BSIC = CastChecked<UBSInputComponent>(PlayerInputComponent);
		const FBSGameplayTags& GameplayTags = FBSGameplayTags::Get();
		BSIC->AddInputMappings(IC, Subsystem);

		TArray<uint32> BindHandles;
		BSIC->BindAbilityActions(IC, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
		BSIC->BindNativeAction(IC, InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Move, /*bLogIfNotFound=*/ true);
		BSIC->BindNativeAction(IC, InputTag_Look, ETriggerEvent::Triggered, this, &ThisClass::Look, /*bLogIfNotFound=*/ true);
	}
}

void ABSCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
}

bool ABSCharacter::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	return IGameplayTagAssetInterface::HasMatchingGameplayTag(TagToCheck);
}

bool ABSCharacter::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return IGameplayTagAssetInterface::HasAllMatchingGameplayTags(TagContainer);
}

bool ABSCharacter::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return IGameplayTagAssetInterface::HasAnyMatchingGameplayTags(TagContainer);
}

void ABSCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void ABSCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void ABSCharacter::AddCharacterAbilities()
{
	// Grant abilities, but only on the server	
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || AbilitySystemComponent->bCharacterAbilitiesGiven)
	{
		return;
	}

	for (TSubclassOf<UBSGameplayAbility>& StartupAbility : CharacterAbilities)
	{
		AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(StartupAbility, GetAbilityLevel(StartupAbility.GetDefaultObject()->AbilityID), static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));
	}

	AbilitySystemComponent->bCharacterAbilitiesGiven = true;
}

void ABSCharacter::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	UE_LOG(LogTemp, Display, TEXT("An input ability tag was pressed"));
	Cast<UBSAbilitySystemComponent>(GetAbilitySystemComponent())->AbilityInputTagPressed(InputTag);
}

void ABSCharacter::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	Cast<UBSAbilitySystemComponent>(GetAbilitySystemComponent())->AbilityInputTagReleased(InputTag);
}
