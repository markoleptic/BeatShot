// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "Character/BSCharacter.h"
#include "Character/BSCharacterMovementComponent.h"
#include "BSGameInstance.h"
#include "Input/BSInputComponent.h"
#include "Player/BSPlayerController.h"
#include "Player/BSPlayerState.h"
#include "Character/BSRecoilComponent.h"
#include "Target/Target.h"
#include "Equipment/BSGun.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/DamageType.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputSubsystems.h"
#include "BeatShot/BSGameplayTags.h"
#include "Components/CapsuleComponent.h"
#include "Equipment/BSEquipmentManagerComponent.h"
#include "AbilitySystem/BSAbilitySystemComponent.h"
#include "AbilitySystem/Globals/BSAbilitySet.h"
#include "AbilitySystem/Abilities/BSGA_FireGun.h"
#include "AbilitySystem/Globals/BSAttributeSetBase.h"
#include "Inventory/BSInventoryItemDefinition.h"
#include "Inventory/BSInventoryItemInstance.h"
#include "Inventory/BSInventoryManagerComponent.h"

static TAutoConsoleVariable CVarAutoBHop(TEXT("move.Pogo"), 0, TEXT("If holding spacebar should make the player jump whenever possible.\n"), ECVF_Default);
static TAutoConsoleVariable CVarJumpBoost(TEXT("move.JumpBoost"), 1, TEXT("If the player should boost in a movement direction while jumping.\n0 - disables jump boosting entirely\n1 - boosts in the direction of input, even when moving in another direction\n2 - boosts in the direction of input when moving in the same direction\n"), ECVF_Default);
static TAutoConsoleVariable CVarBunnyHop(TEXT("move.BunnyHopping"), 0, TEXT("Enable normal bunnyhopping.\n"), ECVF_Default);

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

	RecoilComponent = CreateDefaultSubobject<UBSRecoilComponent>("Recoil Component");
	RecoilComponent->SetupAttachment(SpringArmComponent);
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera Component");
	CameraComponent->SetupAttachment(RecoilComponent);
	CameraComponent->SetFieldOfView(103);
	CameraComponent->PostProcessSettings.MotionBlurAmount = 0;
	CameraComponent->PostProcessSettings.bOverride_MotionBlurMax = 0;

	HandsMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Character Mesh");
	HandsMesh->SetupAttachment(CameraComponent);
	HandsMesh->bCastDynamicShadow = false;
	HandsMesh->CastShadow = false;
	HandsMesh->SetRelativeRotation(FRotator(1.53f, -15.20f, 8.32f));
	HandsMesh->SetRelativeLocation(FVector(-17.69f, -10.50f, -149.11f));

	EquipmentManagerComponent = CreateDefaultSubobject<UBSEquipmentManagerComponent>("Equipment Manager Component");
	InventoryManagerComponent = CreateDefaultSubobject<UBSInventoryManagerComponent>("Inventory Manager Component");

	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.f);
	// Set collision settings. We are the invisible player with no 3rd person mesh.
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);

	// Camera eye level
	BaseEyeHeight = 64.f;
	CrouchedEyeHeight = 44.f;
	MinSpeedForFallDamage = 1002.9825f;
	MinLandBounceSpeed = 329.565f;
	CapDamageMomentumZ = 476.25f;
	bEnabled_AimBot = false;
}

USkeletalMeshComponent* ABSCharacter::GetHandsMesh() const
{
	return Cast<USkeletalMeshComponent>(HandsMesh);
}

ATarget* ABSCharacter::PeekActiveTargets()
{
	ATarget* Target;
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
	return 1.f / Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->GetBSConfig().TargetConfig.TargetSpawnCD;
}

UBSEquipmentManagerComponent* ABSCharacter::GetEquipmentManager() const
{
	return EquipmentManagerComponent.Get();
}

UBSInventoryManagerComponent* ABSCharacter::GetInventoryManager() const
{
	return InventoryManagerComponent.Get();
}

UBSCharacterMovementComponent* ABSCharacter::GetBSCharacterMovement() const
{
	return Cast<UBSCharacterMovementComponent>(GetCharacterMovement());
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

UBSRecoilComponent* ABSCharacter::GetRecoilComponent() const
{
	return RecoilComponent.Get();
}

void ABSCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocallyControlled())
	{
		GetBSPlayerController()->SetInputMode(FInputModeGameOnly());
	}

	// Max jump time to get to the top of the arc
	MaxJumpTime = -4.0f * GetCharacterMovement()->JumpZVelocity / (3.0f * GetCharacterMovement()->GetGravityZ());

	OnPlayerSettingsChanged_Game(LoadPlayerSettings().Game);
	OnPlayerSettingsChanged_User(LoadPlayerSettings().User);

	UBSGameInstance* GI = Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GI->AddDelegateToOnPlayerSettingsChanged(OnPlayerSettingsChangedDelegate_Game);
	GI->AddDelegateToOnPlayerSettingsChanged(OnPlayerSettingsChangedDelegate_User);
	GI->GetPublicGameSettingsChangedDelegate().AddUniqueDynamic(this, &ABSCharacter::OnPlayerSettingsChanged_Game);
	GI->GetPublicUserSettingsChangedDelegate().AddUniqueDynamic(this, &ABSCharacter::OnPlayerSettingsChanged_User);
}

void ABSCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (bDeferJumpStop)
	{
		bDeferJumpStop = false;
		Super::StopJumping();
	}
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

void ABSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InitializePlayerInput(InputComponent);
}

void ABSCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ABSPlayerState* PS = GetPlayerState<ABSPlayerState>();
	if (PS)
	{
		//Attribute change callbacks
		//HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &ThisClass::HealthChanged);
		//MaxHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxHealthAttribute()).AddUObject(this, &ThisClass::MaxHealthChanged);
		//MoveSpeedChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMoveSpeedAttribute()).AddUObject(this, &ThisClass::MoveSpeedChanged);
		//Tag change callbacks
		//AbilitySystemComponent->RegisterGameplayTagEvent(FBSGameplayTags::Get().Input_Sprint, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::TagChange_State_Sprint);
		
		// Set the ASC on the Server. Clients do this in OnRep_PlayerState()
		AbilitySystemComponent = Cast<UBSAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		// AI won't have PlayerControllers so we can init again here just to be sure. No harm in initing twice for heroes that have PlayerControllers.
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);

		// Set the AttributeSetBase for convenience attribute functions
		AttributeSetBase = PS->GetAttributeSetBase();

		// If we handle players disconnecting and rejoining in the future, we'll have to change this so that possession from rejoining doesn't reset attributes.
		// For now assume possession = spawn/respawn.
		AddCharacterAbilities();

		AddCharacterInventoryItems();
	}
}

void ABSCharacter::UnPossessed()
{
	Super::UnPossessed();
	RemoveCharacterAbilities();
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
		
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_Move_Forward, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ true);
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_Move_Backward, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ true);
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_Move_Left, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ true);
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_Move_Right, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ true);

		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_Look, this, &ThisClass::Input_Look, /*bLogIfNotFound=*/ true);
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_Jump, this, &ThisClass::Jump, /*bLogIfNotFound=*/ true);
		
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_Walk, ETriggerEvent::Started, this, &ThisClass::Input_WalkStart, /*bLogIfNotFound=*/ true);
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_Walk, ETriggerEvent::Completed, this, &ThisClass::Input_WalkEnd, /*bLogIfNotFound=*/ true);

		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_Crouch, ETriggerEvent::Started, this, &ThisClass::Input_Crouch, /*bLogIfNotFound=*/ true);
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_Crouch, ETriggerEvent::Completed, this, &ThisClass::Input_Crouch, /*bLogIfNotFound=*/ true);
		
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_Interact, ETriggerEvent::Started, this, &ThisClass::Input_OnInteractStarted, /*bLogIfNotFound=*/ true);
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_Interact, ETriggerEvent::Completed, this, &ThisClass::Input_OnInteractCompleted, /*bLogIfNotFound=*/ true);
		
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_ShiftInteract, ETriggerEvent::Started, this, &ThisClass::Input_OnShiftInteractStarted, /*bLogIfNotFound=*/ true);
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_ShiftInteract, ETriggerEvent::Completed, this, &ThisClass::Input_OnShiftInteractCompleted, /*bLogIfNotFound=*/ true);
		
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_EquipmentSlot_1, this, &ThisClass::Input_OnEquipmentSlot1Started, /*bLogIfNotFound=*/ true);
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_EquipmentSlot_2, this, &ThisClass::Input_OnEquipmentSlot2Started, /*bLogIfNotFound=*/ true);
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_EquipmentSlot_3, this, &ThisClass::Input_OnEquipmentSlot3Started, /*bLogIfNotFound=*/ true);
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_EquipmentSlot_LastEquipped, this, &ThisClass::Input_OnEquipmentSlotLastEquippedStarted, /*bLogIfNotFound=*/ true);

		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_Pause, this, &ThisClass::Input_OnPause, /*bLogIfNotFound=*/ true);
	}
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

void ABSCharacter::AddCharacterInventoryItems()
{
	if (GetLocalRole() != ROLE_Authority || !GetInventoryManager() || !GetEquipmentManager())
	{
		UE_LOG(LogTemp, Display, TEXT("Something wrong with AddCharacterInventoryItems()"));
		return;
	}
	
	int32 Index = 0;
	for (TSubclassOf<UBSInventoryItemDefinition> Definition : InitialInventoryItems)
	{
		if (UBSInventoryItemInstance* ItemInstance = GetInventoryManager()->AddItemDefinition(Definition, 1))
		{
			GetInventoryManager()->AddItemInstance(ItemInstance);
			GetInventoryManager()->AddItemToSlot(Index, ItemInstance);
			Index++;
		}
	}
	if (InitialInventoryItems.Num() > 0)
	{
		GetInventoryManager()->SetActiveSlotIndex(0);
	}
}

void ABSCharacter::RemoveCharacterInventoryItems()
{
	if (GetLocalRole() != ROLE_Authority || !InventoryManagerComponent || !EquipmentManagerComponent)
	{
		UE_LOG(LogTemp, Display, TEXT("Something wrong with RemoveCharacterInventoryItems()"));
		return;
	}

	UBSInventoryManagerComponent* InventoryManager = GetInventoryManager();
	for (int32 Index = 0; Index < InventoryManager->GetSlots().Num(); Index++)
	{
		UBSInventoryItemInstance* Removed = InventoryManager->RemoveItemFromSlot(Index);
		if (Removed)
		{
			InventoryManager->RemoveItemInstance(Removed);
		}
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

void ABSCharacter::Input_Move(const FInputActionValue& Value)
{
	// Moving the player
	if (Value.IsNonZero())
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

void ABSCharacter::Input_Crouch(const FInputActionValue& Value)
{
	ToggleCrouch();
}

void ABSCharacter::Input_WalkStart(const FInputActionValue& Value)
{
	bWantsToWalk = true;
	if (UBSAbilitySystemComponent* ASC = Cast<UBSAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		ASC->SetLooseGameplayTagCount(FBSGameplayTags::Get().Input_Walk, 1);
	}
}

void ABSCharacter::Input_WalkEnd(const FInputActionValue& Value)
{
	bWantsToWalk = false;
	if (UBSAbilitySystemComponent* ASC = Cast<UBSAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		ASC->SetLooseGameplayTagCount(FBSGameplayTags::Get().Input_Walk, 0);
	}
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
	else 
	{
		Crouch();
	}
}

bool ABSCharacter::IsSprinting() const
{
	if (HasMatchingGameplayTag(FBSGameplayTags::Get().Input_Walk))
	{
		return false;
	}
	if (GetVelocity().IsNearlyZero(0.01))
	{
		return false;
	}
	return true;
}

void ABSCharacter::ApplyDamageMomentum(float DamageTaken, FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser)
{
	Super::ApplyDamageMomentum(DamageTaken, DamageEvent, PawnInstigator, DamageCauser);
	UDamageType const* const DmgTypeCDO = DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>();
	if (GetCharacterMovement())
	{
		FVector ImpulseDir;

		if (IsValid(DamageCauser))
		{
			ImpulseDir = (GetActorLocation() - DamageCauser->GetActorLocation()).GetSafeNormal();
		}
		else
		{
			FHitResult HitInfo;
			DamageEvent.GetBestHitInfo(this, DamageCauser, HitInfo, ImpulseDir);
		}

		const float SizeFactor = (60.96f * 60.96f * 137.16f) / (FMath::Square(GetCapsuleComponent()->GetScaledCapsuleRadius() * 2.0f) * GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.0f);

		float Magnitude = 1.905f * DamageTaken * SizeFactor * 5.0f;
		Magnitude = FMath::Min(Magnitude, 1905.0f);

		FVector Impulse = ImpulseDir * Magnitude;
		bool const bMassIndependentImpulse = !DmgTypeCDO->bScaleMomentumByMass;
		float MassScale = 1.f;
		if (!bMassIndependentImpulse && GetCharacterMovement()->Mass > SMALL_NUMBER)
		{
			MassScale = 1.f / GetCharacterMovement()->Mass;
		}
		if (CapDamageMomentumZ > 0.f)
		{
			Impulse.Z = FMath::Min(Impulse.Z * MassScale, CapDamageMomentumZ) / MassScale;
		}

		GetCharacterMovement()->AddImpulse(Impulse, bMassIndependentImpulse);
	}
}

void ABSCharacter::ClearJumpInput(float DeltaTime)
{
	// Don't clear jump input right away if we're auto hopping or no-clipping (holding to go up), or if we are deferring a jump stop
	if (CVarAutoBHop.GetValueOnGameThread() != 0 || bAutoBunnyHop || GetCharacterMovement()->bCheatFlying || bDeferJumpStop)
	{
		return;
	}
	Super::ClearJumpInput(DeltaTime);
}

void ABSCharacter::Jump()
{
	if (GetCharacterMovement()->IsFalling())
	{
		bDeferJumpStop = true;
	}
	Super::Jump();
}

void ABSCharacter::StopJumping()
{
	if (!bDeferJumpStop)
	{
		Super::StopJumping();
	}
}

void ABSCharacter::OnJumped_Implementation()
{
		const int32 JumpBoost = CVarJumpBoost->GetInt();
	if (GetBSCharacterMovement()->IsOnLadder())
	{
		return;
	}

	if (GetWorld()->GetTimeSeconds() >= LastJumpBoostTime + MaxJumpTime && JumpBoost)
	{
		LastJumpBoostTime = GetWorld()->GetTimeSeconds();
		// Boost forward speed on jump
		const FVector Facing = GetActorForwardVector();
		// Use input direction
		FVector Input = GetCharacterMovement()->GetCurrentAcceleration();
		if (JumpBoost != 1)
		{
			// Only boost input in the direction of current movement axis (prevents ABH).
			Input *= FMath::Max(Input.GetSafeNormal2D() | GetCharacterMovement()->Velocity.GetSafeNormal2D(), 0.0f);
		}
		const float ForwardSpeed = Input | Facing;
		// Adjust how much the boost is
		const float SpeedBoostPerc = IsSprinting() || bIsCrouched ? 0.1f : 0.5f;
		// How much we are boosting by
		float SpeedAddition = FMath::Abs(ForwardSpeed * SpeedBoostPerc);
		// We can only boost up to this much
		const float MaxBoostedSpeed = GetCharacterMovement()->GetMaxSpeed() + GetCharacterMovement()->GetMaxSpeed() * SpeedBoostPerc;
		// Calculate new speed
		const float NewSpeed = SpeedAddition + GetMovementComponent()->Velocity.Size2D();
		float SpeedAdditionNoClamp = SpeedAddition;

		// Scale the boost down if we are going over
		if (NewSpeed > MaxBoostedSpeed)
		{
			SpeedAddition -= NewSpeed - MaxBoostedSpeed;
		}

		if (ForwardSpeed < - GetBSCharacterMovement()->GetMaxAcceleration() * FMath::Sin(0.6981f))
		{
			// Boost backwards if we're going backwards
			SpeedAddition *= -1.0f;
			SpeedAdditionNoClamp *= -1.0f;
		}

		// Boost our velocity
		FVector JumpBoostedVel = GetMovementComponent()->Velocity + Facing * SpeedAddition;
		float JumpBoostedSizeSq = JumpBoostedVel.SizeSquared2D();
		if (CVarBunnyHop.GetValueOnGameThread() != 0)
		{
			const FVector JumpBoostedUnClampVel = GetMovementComponent()->Velocity + Facing * SpeedAdditionNoClamp;
			const float JumpBoostedUnClampSizeSq = JumpBoostedUnClampVel.SizeSquared2D();
			if (JumpBoostedUnClampSizeSq > JumpBoostedSizeSq)
			{
				JumpBoostedVel = JumpBoostedUnClampVel;
				JumpBoostedSizeSq = JumpBoostedUnClampSizeSq;
			}
		}
		if (GetMovementComponent()->Velocity.SizeSquared2D() < JumpBoostedSizeSq)
		{
			GetMovementComponent()->Velocity = JumpBoostedVel;
		}
	}
}

bool ABSCharacter::CanJumpInternal_Implementation() const
{
	bool bCanJump = GetCharacterMovement() && GetCharacterMovement()->IsJumpAllowed();
	if (bCanJump)
	{
		// Ensure JumpHoldTime and JumpCount are valid.
		if (!bWasJumping || GetJumpMaxHoldTime() <= 0.0f)
		{
			if (JumpCurrentCount == 0 && GetCharacterMovement()->IsFalling())
			{
				bCanJump = JumpCurrentCount + 1 < JumpMaxCount;
			}
			else
			{
				bCanJump = JumpCurrentCount < JumpMaxCount;
			}
		}
		else
		{
			// Only consider JumpKeyHoldTime as long as:
			// A) We are on the ground
			// B) The jump limit hasn't been met OR
			// C) The jump limit has been met AND we were already jumping
			const bool bJumpKeyHeld = (bPressedJump && JumpKeyHoldTime < GetJumpMaxHoldTime());
			bCanJump = bJumpKeyHeld &&
					   (GetCharacterMovement()->IsMovingOnGround() || (JumpCurrentCount < JumpMaxCount) || (bWasJumping && JumpCurrentCount == JumpMaxCount));
		}
		if (GetCharacterMovement()->IsMovingOnGround())
		{
			const float FloorZ = FVector(0.0f, 0.0f, 1.0f) | GetCharacterMovement()->CurrentFloor.HitResult.ImpactNormal;
			const float WalkableFloor = GetCharacterMovement()->GetWalkableFloorZ();
			bCanJump &= (FloorZ >= WalkableFloor || FMath::IsNearlyEqual(FloorZ, WalkableFloor));
		}
	}
	return bCanJump;
}

void ABSCharacter::RecalculateBaseEyeHeight()
{
	const ACharacter* DefaultCharacter = GetClass()->GetDefaultObject<ACharacter>();
	const float OldUnscaledHalfHeight = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float CrouchedHalfHeight = GetCharacterMovement()->GetCrouchedHalfHeight();
	const float FullCrouchDiff = OldUnscaledHalfHeight - CrouchedHalfHeight;
	const UCapsuleComponent* CharacterCapsule = GetCapsuleComponent();
	const float CurrentUnscaledHalfHeight = CharacterCapsule->GetUnscaledCapsuleHalfHeight();
	const float CurrentAlpha = 1.0f - (CurrentUnscaledHalfHeight - CrouchedHalfHeight) / FullCrouchDiff;
	BaseEyeHeight = FMath::Lerp(DefaultCharacter->BaseEyeHeight, CrouchedEyeHeight, SimpleSpline(CurrentAlpha));
}

void ABSCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PrevCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PrevCustomMode);
	if (!bPressedJump)
	{
		ResetJumpState();
	}

	if (GetCharacterMovement()->IsFalling())
	{
		// Record jump force start time for proxies. Allows us to expire the jump even if not continually ticking down a timer.
		if (bProxyIsJumpForceApplied)
		{
			ProxyJumpForceStartedTime = GetWorld()->GetTimeSeconds();
		}
	}
	else
	{
		JumpCurrentCount = 0;
		JumpKeyHoldTime = 0.0f;
		JumpForceTimeRemaining = 0.0f;
		bWasJumping = false;
	}

	K2_OnMovementModeChanged(PrevMovementMode, GetCharacterMovement()->MovementMode, PrevCustomMode, GetCharacterMovement()->CustomMovementMode);
	MovementModeChangedDelegate.Broadcast(this, PrevMovementMode, PrevCustomMode);
}

bool ABSCharacter::CanCrouch() const
{
	return !GetCharacterMovement()->bCheatFlying && Super::CanCrouch() && !GetBSCharacterMovement()->IsOnLadder();
}

void ABSCharacter::Input_OnInteractStarted(const FInputActionValue& Value) 
{
	if (!OnInteractDelegate.ExecuteIfBound(0))
	{
		UE_LOG(LogTemp, Display, TEXT("OnInteractDelegate not bound."));
	}
}

void ABSCharacter::Input_OnInteractCompleted(const FInputActionValue& Value) 
{
	if (!OnInteractDelegate.ExecuteIfBound(1))
	{
		UE_LOG(LogTemp, Display, TEXT("OnInteractDelegate not bound."));
	}
}

void ABSCharacter::Input_OnShiftInteractStarted(const FInputActionValue& Value) 
{
	if (!OnShiftInteractDelegate.ExecuteIfBound(0))
	{
		UE_LOG(LogTemp, Display, TEXT("OnInteractDelegate not bound."));
	}
}

void ABSCharacter::Input_OnShiftInteractCompleted(const FInputActionValue& Value) 
{
	if (!OnShiftInteractDelegate.ExecuteIfBound(1))
	{
		UE_LOG(LogTemp, Display, TEXT("OnInteractDelegate not bound."));
	}
}

void ABSCharacter::Input_OnInspectStarted(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("Inspecting"));
}

void ABSCharacter::Input_OnMeleeStarted(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("Knifeattacking"));
}

void ABSCharacter::Input_OnEquipmentSlot1Started(const FInputActionValue& Value)
{
	if (GetInventoryManager()->GetSlots().Num() > 1)
	{
		GetInventoryManager()->SetActiveSlotIndex(0);
	}
}

void ABSCharacter::Input_OnEquipmentSlot2Started(const FInputActionValue& Value)
{
	if (GetInventoryManager()->GetSlots().Num() > 1)
	{
		GetInventoryManager()->SetActiveSlotIndex(1);
	}
}

void ABSCharacter::Input_OnEquipmentSlot3Started(const FInputActionValue& Value)
{
	if (GetInventoryManager()->GetSlots().Num() > 1)
	{
		GetInventoryManager()->SetActiveSlotIndex(2);
	}
}

void ABSCharacter::Input_OnEquipmentSlotLastEquippedStarted(const FInputActionValue& Value)
{
	if (GetInventoryManager()->GetSlots().Num() > 1)
	{
		GetInventoryManager()->SetActiveSlotIndex(GetInventoryManager()->GetLastSlotIndex());
	}
}

void ABSCharacter::Input_OnPause(const FInputActionValue& Value)
{
	ABSPlayerController* BSPlayerController = GetBSPlayerController();
	
	if (!BSPlayerController || BSPlayerController->IsPostGameMenuActive())
	{
		return;
	}
	BSPlayerController->HandlePause();
}

void ABSCharacter::OnTargetSpawned_AimBot(ATarget* SpawnedTarget)
{
	ActiveTargets_AimBot.Enqueue(SpawnedTarget);
	OnTargetAddedToQueue.Broadcast();
}

void ABSCharacter::PopActiveTargets()
{
	ActiveTargets_AimBot.Pop();
}

void ABSCharacter::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	Cast<UBSAbilitySystemComponent>(GetAbilitySystemComponent())->AbilityInputTagPressed(InputTag);
}

void ABSCharacter::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	Cast<UBSAbilitySystemComponent>(GetAbilitySystemComponent())->AbilityInputTagReleased(InputTag);
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

void ABSCharacter::OnPlayerSettingsChanged_Game(const FPlayerSettings_Game& GameSettings)
{
	/* Changing activation policy for FireGun ability based on automatic fire bool */
	if(TArray<FGameplayAbilitySpec*> Specs = GetBSAbilitySystemComponent()->GetAbilitySpecsFromGameplayTag(FBSGameplayTags::Get().Input_Fire); !Specs.IsEmpty())
	{
		if (GameSettings.bAutomaticFire)
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
	SetActorHiddenInGame(!GameSettings.bShowCharacterMesh);
}

void ABSCharacter::OnPlayerSettingsChanged_User(const FPlayerSettings_User& UserSettings)
{
	Sensitivity = UserSettings.Sensitivity;
}
