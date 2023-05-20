// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "SaveGamePlayerSettings.h"
#include "GameplayAbility/BSAbilitySystemComponent.h"
#include "SphereTarget.generated.h"

class UBSHealthComponent;
class UBSAbilitySystemComponent;
class UCapsuleComponent;
class UTimelineComponent;
class UNiagaraSystem;
class UCurveFloat;
class UBSAttributeSetBase;

/** Broadcasts info about the destroyed target state */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnLifeSpanExpired, bool, DidExpire, float, TimeAlive, ASphereTarget*, DestroyedTarget);

/** Base target class for this game that is mostly self-managed. TargetSpawner is responsible for spawning, but the lifetime is mostly controlled by parameters passed to it */
UCLASS()
class BEATSHOT_API ASphereTarget : public AActor, public IAbilitySystemInterface, public IGameplayTagAssetInterface 
{
	GENERATED_BODY()

	friend class ATargetSpawner;

	/** Sets default values for this actor's properties */
	ASphereTarget();

protected:
	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	/** Ticks the timelines */
	virtual void Tick(float DeltaSeconds) override;

	/** Called when a non BeatGrid target lifespan has expired */
	virtual void LifeSpanExpired() override;

	// Actual hard pointer to AbilitySystemComponent
	UPROPERTY()
	UBSAbilitySystemComponent* AbilitySystemComponent;

	// Actual hard pointer to AttributeSetBase
	UPROPERTY()
	UBSAttributeSetBase* HardRefAttributeSetBase;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Properties")
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Properties")
	UStaticMeshComponent* SphereMesh;
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Properties")
	UBSHealthComponent* HealthComponent;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Properties")
	UNiagaraSystem* TargetExplosion;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Properties")
	TSubclassOf<UGameplayEffect> TargetImmunity;
	
	UPROPERTY(EditDefaultsOnly, Category = "Target Properties")
	UCurveFloat* StartToPeakCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Target Properties")
	UCurveFloat* PeakToEndCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Target Properties")
	UCurveFloat* ShrinkQuickAndGrowSlowCurve;

	UPROPERTY()
	UMaterialInstanceDynamic* TargetColorChangeMaterial;

public:

	/** Called in TargetSpawner to initialize the target */
	void InitTarget(const FBSConfig& InBSConfig, const FPlayerSettings_Game& InPlayerSettings);
	
	/* ~Begin IAbilitySystemInterface */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	/* ~End IAbilitySystemInterface */
	
	/* ~Begin IGameplayTagAssetInterface */
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	/* ~End IGameplayTagAssetInterface */
	
	void RemoveGameplayTag(FGameplayTag TagToRemove);

	/** Called by TargetSpawner if settings were changed that could affect the target */
	void UpdatePlayerSettings(const FPlayerSettings_Game& InPlayerSettings);
	
	/** Called in TargetSpawner to activate a BeatGrid target */
	void ActivateBeatGridTarget(const float Lifespan);

	/** Called in TargetSpawner to activate a Charged target, instead of spawning a new one */
	void ActivateChargedTarget(const float Lifespan);

	/** Sets the scale for the SphereMesh, should only be called by TargetSpawner */
	void SetInitialSphereScale(const FVector& NewScale);

	/** Sets the color of the Base Target */
	UFUNCTION(BlueprintCallable)
	void SetSphereColor(const FLinearColor& Color);

	/** Sets the color of the Target Outline */
	UFUNCTION(BlueprintCallable)
	void SetOutlineColor(const FLinearColor& Color);

	/** Called from HealthComponent when a SphereTarget receives lethal damage */
	UFUNCTION()
	void OnOutOfHealth();

	/** Called from HealthComponent when a SphereTarget receives damage if the game mode is BeatGrid */
	UFUNCTION()
	void OnHealthChanged(AActor* ActorInstigator, const float OldValue, const float NewValue, const float TotalPossibleDamage);

	/** Returns the color the target be after SpawnBeatDelay seconds have passed */
	UFUNCTION(BlueprintCallable)
	FLinearColor GetPeakTargetColor() const;

	/** Returns the color that the target should change to at the end of it's life */
	UFUNCTION(BlueprintCallable)
	FLinearColor GetEndTargetColor() const;
	
	/** Target Spawner binds to this function to receive info about how target was destroyed. Broadcast from OnOutOfHealth functions */
	FOnLifeSpanExpired OnLifeSpanExpired;

	/** Timer to track the length of time the target was alive for */
	UPROPERTY()
	FTimerHandle TimeSinceSpawn;

	/** Locally stored BSConfig to access GameMode properties without storing ref to game instance */
	UPROPERTY()
	FBSConfig BSConfig;
	
	UPROPERTY()
	FGuid Guid;

private:
	/** Play the StartToPeakTimeline, which corresponds to the StartToPeakCurve */
	UFUNCTION()
	void PlayStartToPeakTimeline();

	/** Play the PeakToEndTimeline, which corresponds to the PeakToEndCurve */
	UFUNCTION()
	void PlayPeakToEndTimeline();

	/** Quickly shrinks the target, then slowly takes it back to original size */
	UFUNCTION()
	void PlayShrinkQuickAndGrowSlowTimeline();

	/** Set the color to BeatGrid color */
	UFUNCTION()
	void SetColorToBeatGridColor();

	/** Interpolates between StartTargetColor and PeakTargetColor. This occurs between initial spawning of target, up to SpawnBeatDelay seconds */
	UFUNCTION()
	void InterpStartToPeak(const float Alpha);

	/** Interpolates between PeakTargetColor and EndTargetColor. This occurs between SpawnBeatDelay seconds, up to TargetMaxLifeSpan seconds */
	UFUNCTION()
	void InterpPeakToEnd(const float Alpha);

	/* Used to shrink the target quickly, and more slowly return it to it's BeatGrid size and color. Interpolates both sphere scale and sphere color */
	UFUNCTION()
	void InterpShrinkQuickAndGrowSlow(const float Alpha);

	/** Applies the TargetImmunity gameplay effect to the target */
	void ApplyImmunityEffect();

	/** Unlike other modes which use LifeSpanExpired to notify TargetSpawner of their expiration, BeatGrid modes use
	 *  this function since the the targets aren't going to be destroyed, but instead just deactivated */
	UFUNCTION()
	void OnBeatGridTargetTimeout();

	/** Called if an activated charged target times out */
	void OnChargedTargetTimeout();

	/** Toggles between using the BaseColor or a separate OutlineColor in the Sphere Material */
	void SetUseSeparateOutlineColor(const bool bUseSeparateOutlineColor);

	/** Play the explosion effect at the location of target, scaled to size with the color of the target when it was destroyed. */
	void PlayExplosionEffect(const FVector& ExplosionLocation, const float SphereRadius, const FLinearColor& InColorWhenDestroyed) const;

	/** Change the sphere scale*/
	void SetSphereScale(const FVector& NewScale) const;

	/** Returns the current scale of the target */
	FVector GetCurrentTargetScale() const;

	/** Gameplay tags associated with this target */
	FGameplayTagContainer GameplayTags;

	UPROPERTY()
	FPlayerSettings_Game PlayerSettings;

	FTimeline StartToPeakTimeline;
	FTimeline PeakToEndTimeline;
	FTimeline ShrinkQuickAndGrowSlowTimeline;

	/** The scale that was applied when spawned */
	float InitialTargetScale = 1.f;
	
	FLinearColor ColorWhenDestroyed;

	float StartToPeakTimelinePlayRate;
	float PeakToEndTimelinePlayRate;
};
