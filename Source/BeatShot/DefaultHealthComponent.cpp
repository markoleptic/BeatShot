// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultHealthComponent.h"
#include "DefaultGameInstance.h"
#include "GameModeActorBase.h"
#include "SphereTarget.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

UDefaultHealthComponent::UDefaultHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDefaultHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	Health = MaxHealth;
	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UDefaultHealthComponent::DamageTaken);
	ShouldUpdateTotalPossibleDamage = false;
	TotalPossibleDamage = 0.f;
	if (Cast<ASphereTarget>(GetOwner()) && GI->GameModeActorStruct.IsBeatTrackMode)
	{
		ShouldUpdateTotalPossibleDamage = true;
	}
}

void UDefaultHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (ShouldUpdateTotalPossibleDamage)
	{
		TotalPossibleDamage++;
		GI->GameModeActorBaseRef->UpdateTrackingScore(0.f, TotalPossibleDamage);
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
		if (Health <= 0.f)
		{
			DamagedTarget->HandleDestruction();
		}
		// BeatTrack modes
		else if (Health > 101 &&
			GI->GameModeActorStruct.IsBeatTrackMode == true)
		{
			GI->GameModeActorBaseRef->UpdateTrackingScore(Damage, TotalPossibleDamage);
		}
		// BeatGrid modes
		else if (Health > 101 &&
			GI->GameModeActorStruct.IsBeatGridMode == true &&
			DamagedTarget->GetWorldTimerManager().GetTimerElapsed(DamagedTarget->TimeSinceSpawn) > 0.f)
		{
			GI->GameModeActorBaseRef->UpdateTargetsHit();
			DamagedTarget->HandleDestruction();
		}
	}
}

