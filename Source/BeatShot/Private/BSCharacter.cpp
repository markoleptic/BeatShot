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
#include "BeatShot/BSGameplayTags.h"
#include "GameplayAbility/BSAbilitySystemComponent.h"
#include "GameplayAbility/BSAbilitySet.h"
#include "GameplayAbility/AttributeSets/BSAttributeSetBase.h"
#include "Kismet/KismetMathLibrary.h"

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

void ABSCharacter::Input_StartFire()
{
	Gun->StartFire();
}

void ABSCharacter::Input_StopFire()
{
	Gun->StopFire();
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
	AddControllerPitchInput(Value[1] / 14.2789148024750118991f * Sensitivity);
	AddControllerYawInput(Value[0] / 14.2789148024750118991f * Sensitivity);
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
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
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
		UE_LOG(LogTemp, Display, TEXT("OnRep_PlayerState() called and Player State Exists"));
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
	
	if (const UBSInputConfig* LoadedConfig = InputConfig)
	{
		UBSInputComponent* BSInputComponent = CastChecked<UBSInputComponent>(PlayerInputComponent);
		const FBSGameplayTags& GameplayTags = FBSGameplayTags::Get();
		BSInputComponent->AddInputMappings(LoadedConfig, Subsystem);
		
		TArray<uint32> BindHandles;
		BSInputComponent->BindAbilityActions(LoadedConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ true);
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look, /*bLogIfNotFound=*/ true);
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_Crouch, ETriggerEvent::Started, this, &ThisClass::Input_Crouch, /*bLogIfNotFound=*/ true);
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_Crouch, ETriggerEvent::Completed, this, &ThisClass::Input_Crouch, /*bLogIfNotFound=*/ true);
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_Jump, ETriggerEvent::Triggered, this, &ThisClass::Jump, /*bLogIfNotFound=*/ true);
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_Fire, ETriggerEvent::Started, this, &ThisClass::Input_StartFire, /*bLogIfNotFound=*/ true);
		BSInputComponent->BindNativeAction(LoadedConfig, GameplayTags.Input_Fire, ETriggerEvent::Completed, this, &ThisClass::Input_StopFire, /*bLogIfNotFound=*/ true);
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
		UE_LOG(LogTemp, Display, TEXT("Not authorized to grant abilities %s"), *FString(__FUNCTION__));
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
	UE_LOG(LogTemp, Display, TEXT("%s , an Input_Ability was released."), *InputTag.ToString());
	FGameplayTagContainer TagContainer;
	GetOwnedGameplayTags(TagContainer);
	for (FGameplayTag Tag : TagContainer)
	{
		if (Tag.IsValid())
			UE_LOG(LogTemp, Display, TEXT("Tag: %s"), *Tag.ToString());
	}
	Cast<UBSAbilitySystemComponent>(GetAbilitySystemComponent())->AbilityInputTagReleased(InputTag);
}
