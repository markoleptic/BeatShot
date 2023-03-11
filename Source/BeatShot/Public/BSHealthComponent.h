// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEffectTypes.h"
#include "BSHealthComponent.generated.h"

class UBSAbilitySystemComponent;
class UBSAttributeSetBase;

DECLARE_DELEGATE_TwoParams(FOnBeatTrackTick, float, float);
DECLARE_DELEGATE(FOnOutOfHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FBSHealth_AttributeChanged, UBSHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);

UCLASS(ClassGroup=(Custom), meta= (BlueprintSpawnableComponent))
class BEATSHOT_API UBSHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Sets default values for this component's properties */
	UBSHealthComponent();

protected:
	/** Called when the game starts */
	virtual void BeginPlay() override;

	// Ability system used by this component.
	UPROPERTY()
	TObjectPtr<UBSAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<const UBSAttributeSetBase> AttributeSetBase;

	virtual void HandleHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleMaxHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	/** Called every frame */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Initialize the component using an ability system component.
	UFUNCTION(BlueprintCallable, Category = "Lyra|Health")
	void InitializeWithAbilitySystem(UBSAbilitySystemComponent* InASC);

	/** Sets the max health of the component */
	void SetMaxHealth(float NewMaxHealth);

	/** The maximum amount of damage that can be dealt to a target */
	float TotalPossibleDamage;

	/** Whether or not to update TotalPossibleDamage */
	bool ShouldUpdateTotalPossibleDamage = false;

	/** Delegate that is executed on tick, or when the sphere target is damaged.
	 *  GameModeActorBase binds to this if a Tracking target has spawned */
	FOnBeatTrackTick OnBeatTrackTick;

	FOnOutOfHealth OnOutOfHealth;
	
	FBSHealth_AttributeChanged BSHealth_AttributeChanged;

private:
	/** Default health value for a target */
	//float MaxHealth = 100.f;

	/** Current health value for a target */
	//float Health = 0.f;

	/** Called when a target takes damage */
	//UFUNCTION()
	//void DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* Instigator, AActor* DamageCauser);
};
