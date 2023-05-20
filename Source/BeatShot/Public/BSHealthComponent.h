// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEffectTypes.h"
#include "BSHealthComponent.generated.h"

class UBSAbilitySystemComponent;
class UBSAttributeSetBase;

DECLARE_MULTICAST_DELEGATE(FOnOutOfHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FBSHealth_AttributeChanged, UBSHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);
DECLARE_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, AActor* Instigator, const float OldValue, const float NewValue, const float TotalPossibleDamage);

/** Base HealthComponent for this game */
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

	/** Called every frame */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

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
	
	/** Initialize the component using an ability system component */
	UFUNCTION(BlueprintCallable, Category = "BeatShot|Health")
	void InitializeWithAbilitySystem(UBSAbilitySystemComponent* InASC, const FGameplayTagContainer& GameplayTagContainer);

	/** The maximum amount of damage that can be dealt to a target */
	float TotalPossibleDamage;

	/** Whether or not to update TotalPossibleDamage */
	bool ShouldUpdateTotalPossibleDamage = false;
	
	FOnOutOfHealth OnOutOfHealth;
	
	FBSHealth_AttributeChanged BSHealth_AttributeChanged;

	FOnHealthChanged OnHealthChanged;
};
