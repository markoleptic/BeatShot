// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.
// Credit to Dan Kestranek.

#include "Player/BSPlayerState.h"
#include "AbilitySystem/Globals/BSAttributeSetBase.h"
#include "AbilitySystem/BSAbilitySystemComponent.h"
#include "Character/BSCharacter.h"
#include "Character/BSCharacterMovementComponent.h"
#include "BeatShot/BSGameplayTags.h"

ABSPlayerState::ABSPlayerState()
{
	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<UBSAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Mixed mode means we only are replicated the GEs to ourself, not the GEs to simulated proxies. If another GDPlayerState (Hero) receives a GE,
	// we won't be told about it by the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Create the attribute set, this replicates by default
	// Adding it as a subobject of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent
	AttributeSetBase = CreateDefaultSubobject<UBSAttributeSetBase>(TEXT("AttributeSetBase"));

	// Set PlayerState's NetUpdateFrequency to the same as the Character.
	// Default is very low for PlayerStates and introduces perceived lag in the ability system.
	// 100 is probably way too high for a shipping game, you can adjust to fit your needs.
	NetUpdateFrequency = 100.0f;
}

UAbilitySystemComponent* ABSPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ABSPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());
}

UBSAttributeSetBase* ABSPlayerState::GetAttributeSetBase() const
{
	return AttributeSetBase;
}

float ABSPlayerState::GetMoveSpeed() const
{
	return AttributeSetBase->GetMoveSpeed();
}

float ABSPlayerState::GetHealth() const
{
	return AttributeSetBase->GetHealth();
}

float ABSPlayerState::GetMaxHealth() const
{
	return AttributeSetBase->GetMaxHealth();
}

void ABSPlayerState::BeginPlay()
{
	Super::BeginPlay();

		if (AbilitySystemComponent)
	{
		// Attribute change callbacks
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &ABSPlayerState::HealthChanged);
		MaxHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxHealthAttribute()).AddUObject(this, &ABSPlayerState::MaxHealthChanged);
		MoveSpeedChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMoveSpeedAttribute()).AddUObject(this, &ABSPlayerState::MoveSpeedChanged);

		// Tag change callbacks
		AbilitySystemComponent->RegisterGameplayTagEvent(FBSGameplayTags::Get().Input_Sprint, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ABSPlayerState::TagChange_State_Sprint);
	}
}

void ABSPlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{
	float Health = Data.NewValue;

	// // Update floating status bar
	// AGDHeroCharacter* Hero = Cast<AGDHeroCharacter>(GetPawn());
	// if (Hero)
	// {
	// 	UGDFloatingStatusBarWidget* HeroFloatingStatusBar = Hero->GetFloatingStatusBar();
	// 	if (HeroFloatingStatusBar)
	// 	{
	// 		HeroFloatingStatusBar->SetHealthPercentage(Health / GetMaxHealth());
	// 	}
	// }
	//
	// // Update the HUD
	// // Handled in the UI itself using the AsyncTaskAttributeChanged node as an example how to do it in Blueprint
	//
	// // If the player died, handle death
	// if (!IsAlive() && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag))
	// {
	// 	if (Hero)
	// 	{
	// 		Hero->Die();
	// 	}
	// }
}

void ABSPlayerState::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	float MaxHealth = Data.NewValue;

	// Update floating status bar
	// ABSCharacter* Hero = Cast<ABSCharacter>(GetPawn());
	// if (Hero)
	// {
	// 	UGDFloatingStatusBarWidget* HeroFloatingStatusBar = Hero->GetFloatingStatusBar();
	// 	if (HeroFloatingStatusBar)
	// 	{
	// 		HeroFloatingStatusBar->SetHealthPercentage(GetHealth() / MaxHealth);
	// 	}
	// }

	// Update the HUD
	// ABSPlayerController* PC = Cast<ABSPlayerController>(GetOwner());
	// if (PC)
	// {
	// 	UGDHUDWidget* HUD = PC->GetHUD();
	// 	if (HUD)
	// 	{
	// 		HUD->SetMaxHealth(MaxHealth);
	// 	}
	// }
}

void ABSPlayerState::MoveSpeedChanged(const FOnAttributeChangeData& Data)
{
	Cast<UBSCharacterMovementComponent>(GetPawn()->GetMovementComponent())->SetSprintSpeedMultiplier(Data.NewValue);
	UE_LOG(LogTemp, Display, TEXT("MoveSpeedChanged Callback called"));
}

void ABSPlayerState::TagChange_State_Sprint(const FGameplayTag CallbackTag, int32 NewCount)
{
	UE_LOG(LogTemp, Display, TEXT("TagChange_State_Sprint Callback called"));
}