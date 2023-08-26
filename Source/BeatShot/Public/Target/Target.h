// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "BeatShot/BeatShot.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "SaveGamePlayerSettings.h"
#include "AbilitySystem/BSAbilitySystemComponent.h"
#include "Target.generated.h"

class UProjectileMovementComponent;
class UBSHealthComponent;
class UBSAbilitySystemComponent;
class UCapsuleComponent;
class UTimelineComponent;
class UNiagaraSystem;
class UCurveFloat;
class UBSAttributeSetBase;
class ATarget;

/** Broadcast when a target takes damage or the the DamageableWindow timer expires */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTargetDamageEventOrTimeout, const FTargetDamageEvent&, TargetDamageEvent);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnDeactivationResponse_ChangeDirection, ATarget* InTarget, const uint8 InSpawnActivationDeactivation);

/** Base target class for this game that is mostly self-managed. TargetManager is responsible for spawning, but the lifetime is mostly controlled by parameters passed to it */
UCLASS()
class BEATSHOT_API ATarget : public AActor, public IAbilitySystemInterface, public IGameplayTagAssetInterface 
{
	GENERATED_BODY()

	friend class ATargetManager;

protected:
	/** Actual hard pointer to AbilitySystemComponent */
	UPROPERTY()
	UBSAbilitySystemComponent* AbilitySystemComponent;

	/** Actual hard pointer to AttributeSetBase */
	UPROPERTY()
	UBSAttributeSetBase* HardRefAttributeSetBase;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Properties")
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Properties")
	UStaticMeshComponent* SphereMesh;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Properties")
	UBSHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, Category = "Target Properties")
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Properties")
	UNiagaraSystem* TargetExplosion;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Properties")
	TSubclassOf<UGameplayEffect> TargetImmunity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Properties")
	TSubclassOf<UGameplayEffect> FireGunImmunity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Properties")
	TSubclassOf<UGameplayEffect> TrackingImmunity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Properties")
	TSubclassOf<UGameplayEffect> ExpirationHealthPenalty;
	
	UPROPERTY(EditDefaultsOnly, Category = "Target Properties")
	UCurveFloat* StartToPeakCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Target Properties")
	UCurveFloat* PeakToEndCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Target Properties")
	UCurveFloat* ShrinkQuickAndGrowSlowCurve;

	UPROPERTY()
	UMaterialInstanceDynamic* TargetColorChangeMaterial;

public:
	ATarget();
	
protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	
public:
	virtual void Tick(float DeltaSeconds) override;

	/** Called in TargetManager to initialize the target */
	virtual void Init(const FBS_TargetConfig& InTargetConfig);

	UFUNCTION()
	void OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

	/** Called by TargetManager if settings were changed that could affect the target */
	void UpdatePlayerSettings(const FPlayerSettings_Game& InPlayerSettings);
	
	/* ~Begin IAbilitySystemInterface */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	/* ~End IAbilitySystemInterface */
	
	/** Applies the TargetImmunity gameplay effect to the target */
	void ApplyImmunityEffect() const;

	/** Applies the TargetImmunity gameplay effect to the target */
	void RemoveImmunityEffect() const;
	
	/** Called when a gameplay effect is blocked because of immunity */
	void OnImmunityBlockGameplayEffect(const FGameplayEffectSpec& Spec, const FActiveGameplayEffect* Effect);
	
	/* ~Begin IGameplayTagAssetInterface */
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	/* ~End IGameplayTagAssetInterface */

	/** Removes a gameplay tag from AbilitySystemComponent */
	void RemoveGameplayTag(FGameplayTag TagToRemove) const;

	/** Broadcast when the target needs a new direction because of a deactivation response */
	FOnDeactivationResponse_ChangeDirection OnDeactivationResponse_ChangeDirection;
	
protected:
	/** Called from HealthComponent when a target receives damage. Calls HandleDeactivation, HandleDestruction,
	 *  and broadcasts OnTargetDamageEventOrTimeout before finally calling HandleDestruction*/
	UFUNCTION()
	virtual void OnHealthChanged(AActor* ActorInstigator, const float OldValue, const float NewValue);

	/** Unlike other modes which use LifeSpanExpired to notify TargetManager of their expiration, BeatGrid modes use
	 *  this function since the the targets aren't going to be destroyed, but instead just deactivated */
	UFUNCTION()
	virtual void OnTargetMaxLifeSpanExpired();
	
	/** Apply damage to self, for example when the DamageableWindow timer expires */
	void DamageSelf(const float Damage);

public:
	/** Activates a target, removes any immunity, starts the DamageableWindow timer, and starts playing the StartToPeakTimeline */
	virtual bool ActivateTarget(const float Lifespan);

protected:
	/** Finds if target should be deactivated, and calls StopAllTimelines and HandleDeactivationResponses if so */
	void HandleDeactivation(const bool bExpired, const float CurrentHealth);

	/** Returns true if the target should be deactivated based on TargetDeactivationConditions */
	virtual bool ShouldDeactivate(const bool bExpired, const float CurrentHealth) const;
	
	/** Performs any responses to the target being deactivated */
	virtual void HandleDeactivationResponses(const bool bExpired);
	
	/** Finds if target should be destroyed, and calls Destroy if so */
	virtual void HandleDestruction(const bool bExpired, const float CurrentHealth);
	
	/** Returns true if the target should be destroyed based on TargetDestructionConditions */
	bool ShouldDestroy(const bool bExpired, const float CurrentHealth) const;
	
	/** Play the StartToPeakTimeline, which corresponds to the StartToPeakCurve */
	UFUNCTION()
	void PlayStartToPeakTimeline();

	/** Play the PeakToEndTimeline, which corresponds to the PeakToEndCurve */
	UFUNCTION()
	void PlayPeakToEndTimeline();

	/** Quickly shrinks the target, then slowly takes it back to original size */
	UFUNCTION()
	void PlayShrinkQuickAndGrowSlowTimeline();

	/** Stops playing all timelines if any are playing */
	void StopAllTimelines();

	/** Interpolates between StartTargetColor and PeakTargetColor. This occurs between initial spawning of target, up to SpawnBeatDelay seconds */
	UFUNCTION()
	void InterpStartToPeak(const float Alpha);

	/** Interpolates between PeakTargetColor and EndTargetColor. This occurs between SpawnBeatDelay seconds, up to TargetMaxLifeSpan seconds */
	UFUNCTION()
	void InterpPeakToEnd(const float Alpha);

	/** Used to shrink the target quickly, and more slowly return it to it's BeatGrid size and color. Interpolates both sphere scale and sphere color */
	UFUNCTION()
	void InterpShrinkQuickAndGrowSlow(const float Alpha);

public:
	/** Sets the color of the Base Target */
	UFUNCTION(BlueprintCallable)
	virtual void SetTargetColor(const FLinearColor& Color);

	/** Sets the color of the Target Outline */
	UFUNCTION(BlueprintCallable)
	virtual void SetTargetOutlineColor(const FLinearColor& Color);

	/** Toggles between using the BaseColor or a separate OutlineColor in the Sphere Material */
	virtual void SetUseSeparateOutlineColor(const bool bUseSeparateOutlineColor);
	
	/** Set the color to BeatGrid color */
	UFUNCTION()
	virtual void SetTargetColorToInactiveColor();
	
	/** Sets the velocity of the ProjectileMovementComponent by multiplying the InitialSpeed and the new direction */
	void SetTargetDirection(const FVector& NewDirection) const;

	/** Sets the InitialSpeed of the ProjectileMovementComponent */
	void SetTargetSpeed(const float NewMovingTargetSpeed) const;
	
	/** Changes the current scale of the target */
	virtual void SetTargetScale(const FVector& NewScale) const;

	/** Sets whether or not the last direction change was horizontally */
	void SetLastDirectionChangeHorizontal(const bool bLastHorizontal) { bLastDirectionChangeHorizontal = bLastHorizontal; }

	/** Play the explosion effect at the location of target, scaled to size with the color of the target when it was destroyed. */
	void PlayExplosionEffect(const FVector& ExplosionLocation, const float SphereRadius, const FLinearColor& InColorWhenDestroyed) const;

	/** Returns the color the target be after SpawnBeatDelay seconds have passed */
	UFUNCTION(BlueprintPure)
	FLinearColor GetPeakTargetColor() const;

	/** Returns the color that the target should change to at the end of it's life */
	UFUNCTION(BlueprintPure)
	FLinearColor GetEndTargetColor() const;

	/** Returns the color that the target should change to at the end of it's life */
	UFUNCTION(BlueprintPure)
	FLinearColor GetInActiveTargetColor() const;

	/** Returns the generated Guid for this target */
	FGuid GetGuid() const { return Guid; }

	/** Returns whether or not the target has been activated before */
	bool HasTargetBeenActivatedBefore() const;

	/** Whether or not the target is immune to all damage */
	bool IsTargetImmune() const;

	/** Whether or not the target is immune to tracking damage */
	UFUNCTION(BlueprintPure)
	bool IsTargetImmuneToTracking() const;

	/** Returns the velocity / speed of the ProjectileMovementComponent (unit direction vector) */
	FVector GetTargetDirection() const;

	/** Returns the scale of the target when it was last activated, or the spawn scale if it has not been activated */
	FVector GetTargetScale_Activation() const;

	/** Returns the current scale of the target */
	FVector GetTargetScale_Current() const;

	/** Returns the scale of the target when it was last deactivated, falling back to activation or spawn scale if not deactivated yet */
	FVector GetTargetScale_Deactivation() const;

	/** Returns the scale of the target when it was spawned */
	FVector GetTargetScale_Spawn() const;

	/** Returns the location of the target when it was activated, falling back to spawn location if not activated yet */
	FVector GetTargetLocation_Activation() const;

	/** Returns the location of the target when it was spawned */
	FVector GetTargetLocation_Spawn() const;

	/** Returns the InitialSpeed of the ProjectileMovementComponent */
	float GetTargetSpeed() const;

	/** Returns the velocity of the ProjectileMovementComponent */
	FVector GetTargetVelocity() const;

	/** Returns whether or not the last direction change was horizontally */
	bool GetLastDirectionChangeHorizontal() const { return bLastDirectionChangeHorizontal; }
	
	/** Broadcast when a target takes damage or the the DamageableWindow timer expires */
	FOnTargetDamageEventOrTimeout OnTargetDamageEventOrTimeout;

protected:
	/** Guid to keep track of a target's properties after it has been destroyed */
	UPROPERTY()
	FGuid Guid;

	/** Locally stored BSConfig to access GameMode properties without storing ref to game instance */
	UPROPERTY()
	FBS_TargetConfig Config;

	/** Timer to track the length of time the target has been damageable for */
	UPROPERTY()
	FTimerHandle DamageableWindow;

	FTimeline StartToPeakTimeline;
	FTimeline PeakToEndTimeline;
	FTimeline ShrinkQuickAndGrowSlowTimeline;

	FOnTimelineFloat OnStartToPeak;
	FOnTimelineFloat OnPeakToFade;
	FOnTimelineFloat OnShrinkQuickAndGrowSlow;

	FOnTimelineEvent OnStartToPeakFinished;

	/** The world scale of the target when spawned */
	FVector TargetScale_Spawn;

	/** The world scale of the target when activated */
	FVector TargetScale_Activation;

	/** The world scale of the target when deactivated */
	FVector TargetScale_Deactivation;

	/** The location of the target when spawned */
	FVector TargetLocation_Spawn;

	/** The location of the target when activated */
	FVector TargetLocation_Activation;

	/** The color of the target when it was destroyed */
	FLinearColor ColorWhenDestroyed;

	/** Playback rate for StartToPeak timeline */
	float StartToPeakTimelinePlayRate;

	/** Playback rate for PeakToEnd timeline */
	float PeakToEndTimelinePlayRate;

	/** Whether or not the last direction change was horizontally */
	bool bLastDirectionChangeHorizontal;

	/** False if the target is currently activated */
	bool bCanBeReactivated;

	/** Whether or not to apply the LifetimeTargetScaling Method */
	bool bApplyLifetimeTargetScaling;

	/** Whether or not the target has ever been activated */
	bool bHasBeenActivated;
};
