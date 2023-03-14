// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "BSHealthComponent.h"

#include "GameplayEffectExtension.h"
#include "SphereTarget.h"
#include "GameplayAbility/AttributeSets/BSAttributeSetBase.h"
#include "GameFramework/Actor.h"

UBSHealthComponent::UBSHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	AbilitySystemComponent = nullptr;
	AttributeSetBase = nullptr;
}

void UBSHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	TotalPossibleDamage = 0.f;
}

void UBSHealthComponent::HandleHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	AActor* Instigator = nullptr;
	if (ChangeData.GEModData != nullptr)
	{
		const FGameplayEffectContextHandle& EffectContext = ChangeData.GEModData->EffectSpec.GetEffectContext();
		Instigator =  EffectContext.GetOriginalInstigator();
	}
	BSHealth_AttributeChanged.Broadcast(this, AttributeSetBase->GetHealth(), AttributeSetBase->GetHealth(), Instigator);
}

void UBSHealthComponent::HandleMaxHealthChanged(const FOnAttributeChangeData& ChangeData)
{
}

void UBSHealthComponent::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
{
	if (!OnOutOfHealth.ExecuteIfBound())
	{
		UE_LOG(LogTemp, Display, TEXT("OnOutOfHealth not bound."));
	}
}

void UBSHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UBSHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (ShouldUpdateTotalPossibleDamage)
	{
		TotalPossibleDamage++;
		if (!OnBeatTrackTick.ExecuteIfBound(0.f, TotalPossibleDamage))
		{
			UE_LOG(LogTemp, Display, TEXT("OnBeatTrackTick not bound."));
		}
	}
}

void UBSHealthComponent::InitializeWithAbilitySystem(UBSAbilitySystemComponent* InASC)
{
	AActor* Owner = GetOwner();
	check(Owner);

	if (AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("BSHealthComponent: Health component for owner [%s] has already been initialized with an ability system."), *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent = InASC;
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("BSHealthComponent: Cannot initialize health component for owner [%s] with NULL ability system."), *GetNameSafe(Owner));
		return;
	}

	AttributeSetBase = AbilitySystemComponent->GetSet<UBSAttributeSetBase>();
	if (!AttributeSetBase)
	{
		UE_LOG(LogTemp, Error, TEXT("BSHealthComponent: Cannot initialize health component for owner [%s] with NULL health set on the ability system."), *GetNameSafe(Owner));
		return;
	}

	// Register to listen for attribute changes.
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UBSAttributeSetBase::GetHealthAttribute()).AddUObject(this, &ThisClass::HandleHealthChanged);
	AttributeSetBase->OnHealthReachZero.AddUObject(this, &ThisClass::HandleOutOfHealth);
}

void UBSHealthComponent::SetMaxHealth(float NewMaxHealth)
{
	//MaxHealth = NewMaxHealth;
	//Health = MaxHealth;
}

// void UBSHealthComponent::DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* Instigator, AActor* DamageCauser)
// {
// 	if (Damage <= 0.f) return;
//
// 	Health -= Damage;
//
// 	if (ASphereTarget* DamagedTarget = Cast<ASphereTarget>(DamagedActor))
// 	{
// 		if (Health <= 0.f || DamagedTarget->GameModeActorStruct.IsBeatGridMode == true)
// 		{
// 			DamagedTarget->HandleDestruction();
// 		}
// 		// BeatTrack modes, handle score based on damage
// 		else if (Health > 101 && DamagedTarget->GameModeActorStruct.IsBeatTrackMode == true)
// 		{
// 			if (!OnBeatTrackTick.ExecuteIfBound(Damage, TotalPossibleDamage))
// 			{
// 				UE_LOG(LogTemp, Display, TEXT("OnBeatTrackTick not bound."));
// 			}
// 		}
// 	}
// }
