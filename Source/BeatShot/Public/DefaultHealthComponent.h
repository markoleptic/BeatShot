// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DefaultHealthComponent.generated.h"

DECLARE_DELEGATE_TwoParams(FOnBeatTrackTick, float, float);

UCLASS(ClassGroup=(Custom), meta= (BlueprintSpawnableComponent))
class BEATSHOT_API UDefaultHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Sets default values for this component's properties */
	UDefaultHealthComponent();

protected:
	/** Called when the game starts */
	virtual void BeginPlay() override;

public:
	/** Called every frame */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	/** Sets the max health of the component */
	void SetMaxHealth(float NewMaxHealth);

	/** The maximum amount of damage that can be dealt to a target */
	float TotalPossibleDamage;

	/** Whether or not to update TotalPossibleDamage */
	bool ShouldUpdateTotalPossibleDamage = false;

	/** Delegate that is executed on tick, or when the sphere target is damaged.
	 *  GameModeActorBase binds to this if a Tracking target has spawned */
	FOnBeatTrackTick OnBeatTrackTick;

private:
	/** Default health value for a target */
	float MaxHealth = 100.f;

	/** Current health value for a target */
	float Health = 0.f;

	/** Called when a target takes damage */
	UFUNCTION()
	void DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* Instigator,
	                 AActor* DamageCauser);
};
