// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "DefaultHealthComponent.h"
#include "SphereTarget.h"
#include "GameFramework/Actor.h"

UDefaultHealthComponent::UDefaultHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDefaultHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UDefaultHealthComponent::DamageTaken);
	TotalPossibleDamage = 0.f;
}

void UDefaultHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
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

void UDefaultHealthComponent::SetMaxHealth(float NewMaxHealth)
{
	MaxHealth = NewMaxHealth;
	Health = MaxHealth;
}

void UDefaultHealthComponent::DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* Instigator, AActor* DamageCauser)
{
	if (Damage <= 0.f) return;

	Health -= Damage;

	if (ASphereTarget* DamagedTarget = Cast<ASphereTarget>(DamagedActor))
	{
		if (Health <= 0.f || DamagedTarget->GameModeActorStruct.IsBeatGridMode == true)
		{
			DamagedTarget->HandleDestruction();
		}
		// BeatTrack modes, handle score based on damage
		else if (Health > 101 &&
			DamagedTarget->GameModeActorStruct.IsBeatTrackMode == true)
		{
			if (!OnBeatTrackTick.ExecuteIfBound(Damage, TotalPossibleDamage))
			{
				UE_LOG(LogTemp, Display, TEXT("OnBeatTrackTick not bound."));
			}
		}
	}
}

