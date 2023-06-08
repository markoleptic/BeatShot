// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEffectTypes.h"
#include "BSHealthComponent.generated.h"

class UBSAbilitySystemComponent;
class UBSAttributeSetBase;

DECLARE_MULTICAST_DELEGATE(FOnOutOfHealth);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnHealthChanged, AActor* Instigator, const float OldValue, const float NewValue);

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

	/** Broadcasts when out of health */
	FOnOutOfHealth OnOutOfHealth;

	/** Broadcasts when the health attribute changes */
	FOnHealthChanged OnHealthChanged;
};
