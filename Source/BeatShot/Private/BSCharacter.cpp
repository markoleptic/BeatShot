// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "BSCharacter.h"
#include "BSCharacterMovementComponent.h"
#include "BSGameInstance.h"
#include "BSInputComponent.h"
#include "BSPlayerController.h"
#include "BSPlayerState.h"
#include "SphereTarget.h"
#include "Gun_AK47.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "BeatShot/BSGameplayTags.h"
#include "GameplayAbility/BSAbilitySystemComponent.h"
#include "GameplayAbility/BSAbilitySet.h"
#include "GameplayAbility/BSGameplayAbility_FireGun.h"
#include "GameplayAbility/AttributeSets/BSAttributeSetBase.h"

ABSCharacter::ABSCharacter(const FObjectInitializer& ObjectInitializer) :
Super(ObjectInitializer.SetDefaultSubobjectClass<UBSCharacterMovementComponent>(CharacterMovementComponentName).
	DoNotCreateDefaultSubobject(MeshComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	bAlwaysRelevant = true;
	bReplicates = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->TargetArmLength = 0.f;
	SpringArmComponent->SetRelativeLocation(FVector(0.f, 0.f, 64.f));
	SpringArmComponent->SetupAttachment(RootComponent);

	CameraRecoilComponent = CreateDefaultSubobject<USceneComponent>("Camera Recoil Component");
	CameraRecoilComponent->SetupAttachment(SpringArmComponent);
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera Component");
	CameraComponent->SetupAttachment(CameraRecoilComponent);
	CameraComponent->SetFieldOfView(103);
	CameraComponent->PostProcessSettings.MotionBlurAmount = 0;
	CameraComponent->PostProcessSettings.bOverride_MotionBlurMax = 0;

	HandsMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Character Mesh");
	HandsMesh->SetupAttachment(CameraComponent);
	HandsMesh->bCastDynamicShadow = false;
	HandsMesh->CastShadow = false;
	HandsMesh->SetRelativeRotation(FRotator(1.53f, -15.20f, 8.32f));
	HandsMesh->SetRelativeLocation(FVector(-17.69f, -10.50f, -149.11f));

	GunComponent = CreateDefaultSubobject<UChildActorComponent>("Gun Component");
	GunComponent->SetupAttachment(HandsMesh, "weapon_r");
	GunComponent->CreateChildActor();

	UBSCharacterMovementComponent* BSMoveComp = CastChecked<UBSCharacterMovementComponent>(GetCharacterMovement());
	BSMoveComp->GravityScale = 1.0f;
	BSMoveComp->MaxAcceleration = 2400.0f;
	BSMoveComp->BrakingFrictionFactor = 1.0f;
	BSMoveComp->BrakingFriction = 6.0f;
	BSMoveComp->GroundFriction = 8.0f;
	BSMoveComp->BrakingDecelerationWalking = 1400.0f;
	BSMoveComp->bUseControllerDesiredRotation = false;
	BSMoveComp->bOrientRotationToMovement = false;
	BSMoveComp->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	BSMoveComp->bAllowPhysicsRotationDuringAnimRootMotion = false;
	BSMoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
	BSMoveComp->bCanWalkOffLedgesWhenCrouching = true;
	BSMoveComp->SetCrouchedHalfHeight(65.0f);

	bEnabled_AimBot = false;
}

USkeletalMeshComponent* ABSCharacter::GetHandsMesh() const
{
	return Cast<USkeletalMeshComponent>(HandsMesh);
}

ASphereTarget* ABSCharacter::PeekActiveTargets()
{
	ASphereTarget* Target;
	while (!ActiveTargets_AimBot.IsEmpty())
	{
		ActiveTargets_AimBot.Peek(Target);
		if (IsValid(Target))
		{
			return Target;
		}
		PopActiveTargets();
	}
	return nullptr;
}

float ABSCharacter::GetAimBotPlaybackSpeed() const
{
	return 1.f / Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->BSConfig.TargetConfig.TargetSpawnCD;
}

UCameraComponent* ABSCharacter::GetCamera() const
{
	return Cast<UCameraComponent>(CameraComponent);
}

AGun_AK47* ABSCharacter::GetGun() const
{
	return Cast<AGun_AK47>(GunComponent->GetChildActor());
}

USceneComponent* ABSCharacter::GetCameraRecoilComponent() const
{
	return Cast<USceneComponent>(CameraRecoilComponent);
}

UAbilitySystemComponent* ABSCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

ABSPlayerController* ABSCharacter::GetBSPlayerController() const
{
	return CastChecked<ABSPlayerController>(Controller, ECastCheckedType::NullAllowed);
}

ABSPlayerState* ABSCharacter::GetBSPlayerState() const
{
	return CastChecked<ABSPlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);
}

UBSAbilitySystemComponent* ABSCharacter::GetBSAbilitySystemComponent() const
{
	return Cast<UBSAbilitySystemComponent>(GetAbilitySystemComponent());
}

void ABSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	OnUserSettingsChange(LoadPlayerSettings());

	if (IsLocallyControlled())
	{
		GetBSPlayerController()->SetInputMode(FInputModeGameOnly());
	}
	
	UBSGameInstance* GameInstance = Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GameInstance->OnPlayerSettingsChange.AddUniqueDynamic(this, &ABSCharacter::OnUserSettingsChange);
}

void ABSCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	/* Make sure that we have a valid PlayerController */
	if (const ABSPlayerController* PlayerController = Cast<ABSPlayerController>(GetController()))
	{
		/* Get the Enhanced Input Local Player Subsystem from the Local Player related to our Player Controller */
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
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
}

void ABSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InitializePlayerInput(InputComponent);
}

void ABSCharacter::OnUserSettingsChange(const FPlayerSettings& PlayerSettings)
{
	Sensitivity = PlayerSettings.Sensitivity;
	if (AGun_AK47* Gun = GetGun())
	{
		Gun->SetShouldRecoil(PlayerSettings.Game.bShouldRecoil);
		Gun->SetFireRate(PlayerSettings.Game.bAutomaticFire);
		Gun->SetShowDecals(PlayerSettings.Game.bShowBulletDecals);
	}
	
	TArray<FGameplayAbilitySpec*> Specs;
	FGameplayTagContainer Container;
	Container.AddTag(FBSGameplayTags::Get().Input_Fire);
	GetAbilitySystemComponent()->GetActivatableGameplayAbilitySpecsByAllMatchingTags(Container, Specs);
	if(!Specs.IsEmpty())
	{
		if (PlayerSettings.Game.bAutomaticFire)
		{
			if (UBSGameplayAbility* Ability = Cast<UBSGameplayAbility>(Specs[0]->Ability))
			{
				Ability->ActivationPolicy = EBSAbilityActivationPolicy::WhileInputActive;
				GetAbilitySystemComponent()->MarkAbilitySpecDirty(*Specs[0]);
			}
		}
		else
		{
			if (UBSGameplayAbility* Ability = Cast<UBSGameplayAbility>(Specs[0]->Ability))
			{
				Ability->ActivationPolicy = EBSAbilityActivationPolicy::OnInputTriggered;
				GetAbilitySystemComponent()->MarkAbilitySpecDirty(*Specs[0]);
			}
		}
	}
}

void ABSCharacter::Input_Move(const FInputActionValue& Value)
{
	// Moving the player
	if (Value.GetMagnitude() != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value[1]);
		AddMovementInput(GetActorRightVector(), Value[0]);
	}
}

void ABSCharacter::Input_Look(const FInputActionValue& Value)
{
	AddControllerPitchInput(Value[1] / SensitivityMultiplier * Sensitivity);
	AddControllerYawInput(Value[0] / SensitivityMultiplier * Sensitivity);
}

void ABSCharacter::Input_Crouch(const FInputActionInstance& Instance)
{
	ToggleCrouch();
}

void ABSCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (UBSAbilitySystemComponent* ASC = Cast<UBSAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		ASC->SetLooseGameplayTagCount(FBSGameplayTags::Get().State_Crouching, 1);
	}
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void ABSCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (UBSAbilitySystemComponent* ASC = Cast<UBSAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		ASC->SetLooseGameplayTagCount(FBSGameplayTags::Get().State_Crouching, 0);
	}
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void ABSCharacter::ToggleCrouch()
{
	const UBSCharacterMovementComponent* MoveComp = CastChecked<UBSCharacterMovementComponent>(GetCharacterMovement());
	if (bIsCrouched || MoveComp->bWantsToCrouch)
	{
		UnCrouch();
	}
	else if (MoveComp->IsMovingOnGround())
	{
		Crouch();
	}
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

void ABSCharacter::OnTargetSpawned_AimBot(ASphereTarget* SpawnedTarget)
{
	ActiveTargets_AimBot.Enqueue(SpawnedTarget);
	OnTargetAddedToQueue.Broadcast();
}

void ABSCharacter::PopActiveTargets()
{
	ActiveTargets_AimBot.Pop();
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
		UE_LOG(LogTemp, Display, TEXT("OnRep_PlayerState() called and Player State Exists"));
		// Set the ASC for clients. Server does this in PossessedBy.
		AbilitySystemComponent = Cast<UBSAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		// Init ASC Actor Info for clients. Server will init its ASC when it possesses a new Actor.
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);

		// Set the AttributeSetBase for convenience attribute functions
		AttributeSetBase = PS->GetAttributeSetBase();
	}
}

void ABSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ABSCharacter::RemoveCharacterAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || !AbilitySystemComponent->bCharacterAbilitiesGiven)
	{
		UE_LOG(LogTemp, Display, TEXT("Something wrong with RemoveCharacterAbilities()"));
		return;
	}

	AbilitySet_GrantedHandles.TakeFromAbilitySystem(Cast<UBSAbilitySystemComponent>(GetAbilitySystemComponent()));
	AbilitySystemComponent->bCharacterAbilitiesGiven = false;
}

void ABSCharacter::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	check(Subsystem);

	/* PawnClientRestart can run more than once in an Actor's lifetime, so start by clearing out any leftover mappings */
	Subsystem->ClearAllMappings();
	/* Add each mapping context, along with their priority values. Higher values out-prioritize lower values */
	Subsystem->AddMappingContext(BaseMappingContext, BaseMappingPriority);

	if (const UBSInputConfig* LoadedConfig = InputConfig)
	{
		UBSInputComponent* BSInputComponent = CastChecked<UBSInputComponent>(PlayerInputComponent);
		const FBSGameplayTags& GameplayTags = FBSGameplayTags::Get();
		TArray<uint32> BindHandles;
		BSInputComponent->BindAbilityActions(LoadedConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ true);
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look, /*bLogIfNotFound=*/ true);
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_Jump, ETriggerEvent::Triggered, this, &ThisClass::Jump, /*bLogIfNotFound=*/ true);
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_Crouch, ETriggerEvent::Started, this, &ThisClass::Input_Crouch, /*bLogIfNotFound=*/ true);
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_Crouch, ETriggerEvent::Completed, this, &ThisClass::Input_Crouch, /*bLogIfNotFound=*/ true);
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_Interact, ETriggerEvent::Started, this, &ThisClass::OnInteractStarted, /*bLogIfNotFound=*/ true);
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_Interact, ETriggerEvent::Completed, this, &ThisClass::OnInteractCompleted, /*bLogIfNotFound=*/ true);
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_ShiftInteract, ETriggerEvent::Started, this, &ThisClass::OnShiftInteractStarted, /*bLogIfNotFound=*/ true);
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_ShiftInteract, ETriggerEvent::Completed, this, &ThisClass::OnShiftInteractCompleted, /*bLogIfNotFound=*/ true);
	}
}

void ABSCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const UBSAbilitySystemComponent* ASC = CastChecked<UBSAbilitySystemComponent, UAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		ASC->GetOwnedGameplayTags(TagContainer);
	}
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

void ABSCharacter::AddCharacterAbilities()
{
	// Grant abilities, but only on the server	
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || AbilitySystemComponent->bCharacterAbilitiesGiven)
	{
		return;
	}

	for (const UBSAbilitySet* AbilitySet : AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(Cast<UBSAbilitySystemComponent>(GetAbilitySystemComponent()), &AbilitySet_GrantedHandles);
		}
	}
	AbilitySystemComponent->bCharacterAbilitiesGiven = true;
}

void ABSCharacter::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	Cast<UBSAbilitySystemComponent>(GetAbilitySystemComponent())->AbilityInputTagPressed(InputTag);
}

void ABSCharacter::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	Cast<UBSAbilitySystemComponent>(GetAbilitySystemComponent())->AbilityInputTagReleased(InputTag);
}
