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
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTargetActivationStateChanged, const bool bIsActivated, const FGameplayTagContainer& TagContainer);

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Properties")
	UNiagaraSystem* TargetExplosion;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Properties")
	TSubclassOf<UGameplayEffect> TargetImmunity;

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

	/** Removes a gameplay tag from AbilitySystemComponent */
	void RemoveGameplayTag(FGameplayTag TagToRemove) const;

	/** Called when a gameplay effect is blocked because of immunity */
	void OnImmunityBlockGameplayEffect(const FGameplayEffectSpec& Spec, const FActiveGameplayEffect* Effect);

	/** Called by TargetManager if settings were changed that could affect the target */
	void UpdatePlayerSettings(const FPlayerSettings_Game& InPlayerSettings);

	/** Activates a target, removes any immunity, starts the DamageableWindow timer, and starts playing the StartToPeakTimeline */
	void ActivateTarget(const float Lifespan);

	/** Deactivates a target, applies immunity, stops the DamageableWindow timer and all timelines, sets to inactive target color, and applies scaling */
	void DeactivateTarget(const float TimeAlive, const bool bExpired);

	/** Sets the scale for the SphereMesh, should only be called by TargetManager */
	void SetInitialSphereScale(const FVector& NewScale);

	/** Sets the color of the Base Target */
	UFUNCTION(BlueprintCallable)
	void SetSphereColor(const FLinearColor& Color);

	/** Sets the color of the Target Outline */
	UFUNCTION(BlueprintCallable)
	void SetOutlineColor(const FLinearColor& Color);

	/** Called from HealthComponent when a SphereTarget receives damage if the game mode is BeatGrid */
	UFUNCTION()
	void OnHealthChanged(AActor* ActorInstigator, const float OldValue, const float NewValue, const float TotalPossibleDamage);

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

	/** Whether or not the DamageableWindow timer is active */
	bool IsDamageWindowActive() const;

	/** Returns the direction the target is travelling towards */
	FVector GetMovingTargetDirection() const { return MovingTargetDirection; }
	
	/** Sets the direction the target is travelling towards */
	void SetMovingTargetDirection(const FVector& NewDirection);

	/** Returns the speed the target is travelling at */
	float GetMovingTargetSpeed() const { return MovingTargetSpeed; }

	/** Sets the speed the target is travelling at */
	void SetMovingTargetSpeed(const float NewMovingTargetSpeed);

	/** Applies the TargetImmunity gameplay effect to the target */
	void ApplyImmunityEffect() const;

	/** Applies the TargetImmunity gameplay effect to the target */
	void RemoveImmunityEffect() const;
	
	/** Broadcast when a target takes damage or the the DamageableWindow timer expires */
	FOnTargetDamageEventOrTimeout OnTargetDamageEventOrTimeout;

	/** Broadcast when the target is activated or deactivated */
	FOnTargetActivationStateChanged OnTargetActivationStateChanged;

	/** Timer to track the length of time the target has been damageable for */
	UPROPERTY()
	FTimerHandle DamageableWindow;

	/** Locally stored BSConfig to access GameMode properties without storing ref to game instance */
	UPROPERTY()
	FBS_TargetConfig Config;

private:
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

	/** Current direction the target is moving */
	FVector MovingTargetDirection;
	
	/** Current speed of the moving target */
	float MovingTargetSpeed;

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
};
