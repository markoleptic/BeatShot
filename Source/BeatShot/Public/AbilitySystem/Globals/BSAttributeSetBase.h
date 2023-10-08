// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "BSAttributeSetBase.generated.h"

// Delegate used to broadcast attribute events.
DECLARE_MULTICAST_DELEGATE_SixParams(FBSAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/,
	const FGameplayEffectSpec* /*EffectSpec*/, float /*EffectMagnitude*/, float /*OldValue*/, float /*NewValue*/);

/** The base AttributeSet used for this game */
UCLASS()
class BEATSHOT_API UBSAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()

public:
	UBSAttributeSetBase();

	//~ Helper functions for "Health" attributes
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UBSAttributeSetBase, Health);
	GAMEPLAYATTRIBUTE_VALUE_GETTER(Health);
	GAMEPLAYATTRIBUTE_VALUE_SETTER(Health);
	GAMEPLAYATTRIBUTE_VALUE_INITTER(Health);

	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UBSAttributeSetBase, MaxHealth);
	GAMEPLAYATTRIBUTE_VALUE_GETTER(MaxHealth);
	GAMEPLAYATTRIBUTE_VALUE_SETTER(MaxHealth);
	GAMEPLAYATTRIBUTE_VALUE_INITTER(MaxHealth);

	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UBSAttributeSetBase, HitDamage);
	GAMEPLAYATTRIBUTE_VALUE_GETTER(HitDamage);
	GAMEPLAYATTRIBUTE_VALUE_SETTER(HitDamage);
	GAMEPLAYATTRIBUTE_VALUE_INITTER(HitDamage);

	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UBSAttributeSetBase, TrackingDamage);
	GAMEPLAYATTRIBUTE_VALUE_GETTER(TrackingDamage);
	GAMEPLAYATTRIBUTE_VALUE_SETTER(TrackingDamage);
	GAMEPLAYATTRIBUTE_VALUE_INITTER(TrackingDamage);

	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UBSAttributeSetBase, TotalDamage);
	GAMEPLAYATTRIBUTE_VALUE_GETTER(TotalDamage);
	GAMEPLAYATTRIBUTE_VALUE_SETTER(TotalDamage);
	GAMEPLAYATTRIBUTE_VALUE_INITTER(TotalDamage);

	// Delegate when health changes due to damage/healing, some information may be missing on the client
	mutable FBSAttributeEvent OnHealthChanged;

	// Delegate when max health changes
	mutable FBSAttributeEvent OnMaxHealthChanged;

	// Delegate to broadcast when the health attribute reaches zero
	mutable FBSAttributeEvent OnOutOfHealth;

protected:
	/**
	 *	Called just before any modification happens to an attribute. This is lower level than PreAttributeModify/PostAttribute modify.
	 *	There is no additional context provided here since anything can trigger this. Executed effects, duration based effects, effects being removed, immunity being applied, stacking rules changing, etc.
	 *	This function is meant to enforce things like "Health = Clamp(Health, 0, MaxHealth)" and NOT things like "trigger this extra thing if damage is applied, etc".
	 *	
	 *	NewValue is a mutable reference so you are able to clamp the newly applied value as well.
	 */
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	/** Called just after any modification happens to an attribute. */
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	/**
	 *	Called just before modifying the value of an attribute. AttributeSet can make additional modifications here. Return true to continue, or false to throw out the modification.
	 *	Note this is only called during an 'execute'. E.g., a modification to the 'base value' of an attribute. It is not called during an application of a GameplayEffect, such as a 5 ssecond +10 movement speed buff.
	 */
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;

	/**
	 *	Called just before a GameplayEffect is executed to modify the base value of an attribute. No more changes can be made.
	 *	Note this is only called during an 'execute'. E.g., a modification to the 'base value' of an attribute. It is not called during an application of a GameplayEffect, such as a 5 ssecond +10 movement speed buff.
	 */
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** These OnRep functions exist to make sure that the ability system internal representations are synchronized properly during replication */

	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);
	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
	UFUNCTION()
	virtual void OnRep_HitDamage(const FGameplayAttributeData& OldHitDamage);
	UFUNCTION()
	virtual void OnRep_TrackingDamage(const FGameplayAttributeData& OldTrackingDamage);

private:
	/** Second Health, when 0 we expect owner to die unless prevented by an ability. Capped by MaxHealth. Positive changes can directly use this.
	 *  Negative changes to Health should go through HitDamage meta attribute. */
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health,
		Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	/** MaxHealth is its own attribute since GameplayEffects may modify it */
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth,
		Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	/* HitDamage is is the amount of Hit-Based damage a player can deal in a single damage execution */
	UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_HitDamage,
		Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData HitDamage;

	/* TrackingDamage is is the amount of Tracking-Based damage a player can deal in a single damage execution  */
	UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_TrackingDamage,
		Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData TrackingDamage;

	/* TotalDamage is a meta attribute used by the DamageExecution to calculate final damage, which then turns into -Health. Temporary value that only exists on the Server. Not replicated. */
	UPROPERTY(BlueprintReadOnly, Category = "Damage", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData TotalDamage;

	/* Helper function to proportionally adjust the value of an attribute when it's associated max attribute changes.
	 * (i.e. When MaxHealth increases, Health increases by an amount that maintains the same percentage as before) */
	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute,
		const FGameplayAttributeData& MaxAttribute, float NewMaxValue,
		const FGameplayAttribute& AffectedAttributeProperty);

	// Store the health before any changes 
	float MaxHealthBeforeAttributeChange;
	float HealthBeforeAttributeChange;

	// Used to track when the health reaches 0
	bool bOutOfHealth;

	// Min possible health
	const float MinPossibleHealth = 0.0f;
};
