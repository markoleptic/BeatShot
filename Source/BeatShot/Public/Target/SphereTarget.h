// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "BeatShot/BeatShot.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "SaveGamePlayerSettings.h"
#include "AbilitySystem/BSAbilitySystemComponent.h"
#include "SphereTarget.generated.h"

class UProjectileMovementComponent;
class UBSHealthComponent;
class UBSAbilitySystemComponent;
class UCapsuleComponent;
class UTimelineComponent;
class UNiagaraSystem;
class UCurveFloat;
class UBSAttributeSetBase;
class ASphereTarget;

/** Broadcast when a target takes damage or the the DamageableWindow timer expires */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTargetDamageEventOrTimeout, const FTargetDamageEvent&, TargetDamageEvent);

/** Base target class for this game that is mostly self-managed. TargetManager is responsible for spawning, but the lifetime is mostly controlled by parameters passed to it */
UCLASS()
class BEATSHOT_API ASphereTarget : public AActor, public IAbilitySystemInterface, public IGameplayTagAssetInterface 
{
	GENERATED_BODY()

	friend class ATargetManager;

	/** Sets default values for this actor's properties */
	ASphereTarget();

protected:
	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	/** Ticks the timelines */
	virtual void Tick(float DeltaSeconds) override;

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

	/** Called in TargetManager to initialize the target */
	void InitTarget(const FBS_TargetConfig& InTargetConfig);
	
	/* ~Begin IAbilitySystemInterface */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	/* ~End IAbilitySystemInterface */
	
	/* ~Begin IGameplayTagAssetInterface */
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	/* ~End IGameplayTagAssetInterface */

	UFUNCTION()
	virtual void OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

	/** Removes a gameplay tag from AbilitySystemComponent */
	void RemoveGameplayTag(FGameplayTag TagToRemove) const;

	/** Called when a gameplay effect is blocked because of immunity */
	void OnImmunityBlockGameplayEffect(const FGameplayEffectSpec& Spec, const FActiveGameplayEffect* Effect);

	/** Called by TargetManager if settings were changed that could affect the target */
	void UpdatePlayerSettings(const FPlayerSettings_Game& InPlayerSettings);

	/** Activates a target, removes any immunity, starts the DamageableWindow timer, and starts playing the StartToPeakTimeline */
	bool ActivateTarget(const float Lifespan);

	/** Sets the color of the Base Target */
	UFUNCTION(BlueprintCallable)
	void SetSphereColor(const FLinearColor& Color);

	/** Sets the color of the Target Outline */
	UFUNCTION(BlueprintCallable)
	void SetOutlineColor(const FLinearColor& Color);

	/** Called from HealthComponent when a SphereTarget receives damage. Calls HandleDeactivation, HandleDestruction,
	 *  and broadcasts OnTargetDamageEventOrTimeout before finally calling HandleDestruction*/
	UFUNCTION()
	void OnHealthChanged(AActor* ActorInstigator, const float OldValue, const float NewValue);

	/** Returns the color the target be after SpawnBeatDelay seconds have passed */
	UFUNCTION(BlueprintCallable)
	FLinearColor GetPeakTargetColor() const;

	/** Returns the color that the target should change to at the end of it's life */
	UFUNCTION(BlueprintCallable)
	FLinearColor GetEndTargetColor() const;

	/** Returns the generated Guid for this target */
	FGuid GetGuid() const { return Guid; }

	/** Whether or not the target is immune to damage */
	bool IsTargetImmune() const;

	/** Whether or not the target is immune to damage */
	bool IsTargetImmuneToTracking() const;

	/** Whether or not the DamageableWindow timer is active */
	bool IsDamageWindowActive() const;

	/** Returns the velocity / speed of the ProjectileMovementComponent (unit direction vector) */
	FVector GetTargetDirection() const;

	/** Returns the InitialSpeed of the ProjectileMovementComponent */
	float GetTargetSpeed() const;

	/** Returns the velocity of the ProjectileMovementComponent */
	FVector GetTargetVelocity() const;
	
	/** Sets the velocity of the ProjectileMovementComponent by multiplying the InitialSpeed and the new direction */
	void SetTargetDirection(const FVector& NewDirection) const;

	/** Sets the InitialSpeed of the ProjectileMovementComponent */
	void SetTargetSpeed(const float NewMovingTargetSpeed) const;

	void SetLastDirectionChangeHorizontal(const bool bLastHorizontal) { bLastDirectionChangeHorizontal = bLastHorizontal; }

	bool GetLastDirectionChangeHorizontal() const { return bLastDirectionChangeHorizontal; }
	
	/** Applies the TargetImmunity gameplay effect to the target */
	void ApplyImmunityEffect() const;

	/** Applies the TargetImmunity gameplay effect to the target */
	void RemoveImmunityEffect() const;
	
	/** Broadcast when a target takes damage or the the DamageableWindow timer expires */
	FOnTargetDamageEventOrTimeout OnTargetDamageEventOrTimeout;
	
	/** Timer to track the length of time the target has been damageable for */
	UPROPERTY()
	FTimerHandle DamageableWindow;

	/** Locally stored BSConfig to access GameMode properties without storing ref to game instance */
	UPROPERTY()
	FBS_TargetConfig Config;

private:
	/** Finds if target should be deactivated, and calls StopAllTimelines and HandleDeactivationResponses if so */
	void HandleDeactivation(const bool bExpired, const float CurrentHealth);

	/** Finds if target should be destroyed, and calls Destroy if so */
	void HandleDestruction(const bool bExpired, const float CurrentHealth);

	/** Performs any responses to the target being deactivated */
	void HandleDeactivationResponses(const bool bExpired);
	
	/** Returns true if the target should be deactivated based on TargetDeactivationConditions */
	bool ShouldDeactivate(const bool bExpired, const float CurrentHealth) const;

	/** Returns true if the target should be destroyed based on TargetDestructionConditions */
	bool ShouldDestroy(const bool bExpired, const float CurrentHealth) const;
	
	/** Apply damage to self, for example when the DamageableWindow timer expires */
	void DamageSelf(const float Damage);
	
	/** Play the StartToPeakTimeline, which corresponds to the StartToPeakCurve */
	UFUNCTION()
	void PlayStartToPeakTimeline();

	/** Play the PeakToEndTimeline, which corresponds to the PeakToEndCurve */
	UFUNCTION()
	void PlayPeakToEndTimeline();

	/** Quickly shrinks the target, then slowly takes it back to original size */
	UFUNCTION()
	void PlayShrinkQuickAndGrowSlowTimeline();

	/** Play the explosion effect at the location of target, scaled to size with the color of the target when it was destroyed. */
	void PlayExplosionEffect(const FVector& ExplosionLocation, const float SphereRadius, const FLinearColor& InColorWhenDestroyed) const;

	/** Stops playing all timelines if any are playing */
	void StopAllTimelines();

	/** Set the color to BeatGrid color */
	UFUNCTION()
	void SetColorToInactiveColor();

	/** Change the sphere scale*/
	void SetSphereScale(const FVector& NewScale) const;

	/** Toggles between using the BaseColor or a separate OutlineColor in the Sphere Material */
	void SetUseSeparateOutlineColor(const bool bUseSeparateOutlineColor);

	/** Returns the current scale of the target */
	FVector GetCurrentTargetScale() const;

	/** Interpolates between StartTargetColor and PeakTargetColor. This occurs between initial spawning of target, up to SpawnBeatDelay seconds */
	UFUNCTION()
	void InterpStartToPeak(const float Alpha);

	/** Interpolates between PeakTargetColor and EndTargetColor. This occurs between SpawnBeatDelay seconds, up to TargetMaxLifeSpan seconds */
	UFUNCTION()
	void InterpPeakToEnd(const float Alpha);

	/** Used to shrink the target quickly, and more slowly return it to it's BeatGrid size and color. Interpolates both sphere scale and sphere color */
	UFUNCTION()
	void InterpShrinkQuickAndGrowSlow(const float Alpha);

	/** Unlike other modes which use LifeSpanExpired to notify TargetManager of their expiration, BeatGrid modes use
	 *  this function since the the targets aren't going to be destroyed, but instead just deactivated */
	UFUNCTION()
	void OnTargetMaxLifeSpanExpired();

	/** Guid to keep track of a target's properties after it has been destroyed */
	UPROPERTY()
	FGuid Guid;

	FTimeline StartToPeakTimeline;
	FTimeline PeakToEndTimeline;
	FTimeline ShrinkQuickAndGrowSlowTimeline;

	FOnTimelineFloat OnStartToPeak;
	FOnTimelineFloat OnPeakToFade;
	FOnTimelineFloat OnShrinkQuickAndGrowSlow;

	FOnTimelineEvent OnStartToPeakFinished;

	/** The scale that was applied when spawned */
	FVector InitialTargetScale;

	/** The scale that was applied when spawned */
	FVector InitialTargetLocation;

	/** The color of the target when it was destroyed */
	FLinearColor ColorWhenDestroyed;

	/** Playback rate for StartToPeak timeline */
	float StartToPeakTimelinePlayRate;

	/** Playback rate for PeakToEnd timeline */
	float PeakToEndTimelinePlayRate;

	bool bLastDirectionChangeHorizontal;
	
	bool bCanBeReactivated = true;
};
