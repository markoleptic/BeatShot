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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Abilities")
	TSubclassOf<UGameplayEffect> TargetImmunity;
	
	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* StartToPeakCurve;

	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* PeakToEndCurve;

	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* ShrinkQuickAndGrowSlowCurve;

	UPROPERTY()
	UMaterialInstanceDynamic* MID_TargetColorChanger;

public:

	void InitTarget(const FBSConfig& InBSConfig, const FPlayerSettings_Game& InPlayerSettings);

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	void RemoveGameplayTag(FGameplayTag TagToRemove);

	/** Called by TargetSpawner if settings were changed that could affect the target */
	void UpdatePlayerSettings(const FPlayerSettings_Game& InPlayerSettings);
	
	/** Called in TargetSpawner to activate a BeatGrid target */
	void StartBeatGridTimer(float Lifespan);

	/** Sets the scale for the SphereMesh */
	void SetSphereScale(const FVector& NewScale);

	/** Sets the color of the Base Target */
	UFUNCTION(BlueprintCallable)
	void SetSphereColor(const FLinearColor& Color);

	/** Sets the color of the Target Outline */
	UFUNCTION(BlueprintCallable)
	void SetOutlineColor(const FLinearColor& Color);

	/** Called from HealthComponent when a SphereTarget receives damage */
	UFUNCTION()
	void HandleDestruction();

	/** Called from HealthComponent when a SphereTarget receives damage if the game mode is BeatGrid */
	UFUNCTION()
	void HandleTemporaryDestruction(AActor* ActorInstigator, const float OldValue, const float NewValue, const float TotalPossibleDamage);

	/** Returns the color the target be after SpawnBeatDelay seconds have passed */
	UFUNCTION(BlueprintCallable)
	FLinearColor GetPeakTargetColor() const;

	/** Returns the color that the target should change to at the end of it's life */
	UFUNCTION(BlueprintCallable)
	FLinearColor GetEndTargetColor() const;
	
	/** Target Spawner binds to this function to receive info about how target was destroyed. Broadcast from HandleDestruction functions */
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
	void InterpStartToPeakColor(const float Alpha);

	/** Interpolates between PeakTargetColor and EndTargetColor. This occurs between SpawnBeatDelay seconds, up to TargetMaxLifeSpan seconds */
	UFUNCTION()
	void InterpPeakToEndColor(const float Alpha);

	UFUNCTION()
	void InterpShrinkQuickAndGrowSlow(const float Alpha);

	/** Applies the TargetImmunity gameplay effect to the target */
	void ApplyImmunityEffect();

	/** Unlike other modes which use LifeSpanExpired to notify TargetSpawner of their expiration, BeatGrid modes use
	 *  this function since the the targets aren't going to be destroyed, but instead just deactivated */
	UFUNCTION()
	void OnBeatGridTimerCompleted();

	/** Toggles between using the BaseColor or a separate OutlineColor in the Sphere Material */
	void SetUseSeparateOutlineColor(const bool bUseSeparateOutlineColor);

	/** Play the explosion effect at the location of target, scaled to size with the color of the target when it was destroyed. */
	void PlayExplosionEffect(const FVector& ExplosionLocation, const float SphereRadius, const FLinearColor& InColorWhenDestroyed) const;
	
	FGameplayTagContainer GameplayTags;

	UPROPERTY()
	FPlayerSettings_Game PlayerSettings;

	FTimeline StartToPeakTimeline;
	FTimeline PeakToEndTimeline;
	FTimeline ShrinkQuickAndGrowSlowTimeline;

	/** The scale that was applied when spawned */
	float TargetScale = 1.f;

	FLinearColor ColorWhenDestroyed;
};
