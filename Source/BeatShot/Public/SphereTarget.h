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
	UBSAbilitySystemComponent* HardRefAbilitySystemComponent;

	// Actual hard pointer to AttributeSetBase
	UPROPERTY()
	UBSAttributeSetBase* HardRefAttributeSetBase;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Properties")
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Properties")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Properties")
	UStaticMeshComponent* OutlineMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Properties")
	UNiagaraSystem* NS_Standard_Explosion;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Properties")
	UMaterialInterface* Material;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Properties")
	UMaterialInterface* OutlineMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Abilities")
	TSubclassOf<UGameplayEffect> TargetImmunity;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInstanceDynamic* MID_TargetColorChanger;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInstanceDynamic* MID_TargetOutline;

	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* StartToPeakCurve;

	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* PeakToEndCurve;

	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* FadeAndReappearCurve;

public:

	void InitTarget(const FBSConfig& InBSConfig, const FPlayerSettings_Game& InPlayerSettings);

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	
	/** Called in TargetSpawner to activate a BeatGrid target */
	void StartBeatGridTimer(float Lifespan);

	/** Sets the scale for the BaseMesh and the OutlineMesh */
	void SetSphereScale(const FVector& NewScale);

	/** Sets the color of the Target Material */
	UFUNCTION(BlueprintCallable)
	void SetSphereColor(const FLinearColor& Output);

	/** Sets the color of the Target Outline Material */
	UFUNCTION(BlueprintCallable)
	void SetOutlineColor(const FLinearColor& Output);

	/** Called from DefaultHealthComponent when a SphereTarget receives damage. */
	UFUNCTION()
	void HandleDestruction();

	UFUNCTION()
	void HandleTemporaryDestruction(AActor* ActorInstigator, const float OldValue, const float NewValue, const float TotalPossibleDamage);

	UFUNCTION(BlueprintCallable)
	FLinearColor GetPeakTargetColor() const;

	UFUNCTION(BlueprintCallable)
	FLinearColor GetEndTargetColor() const;
	
	/** Target Spawner binds to this function to receive info about how target was destroyed */
	FOnLifeSpanExpired OnLifeSpanExpired;

	/** The length of the time the target was alive for */
	UPROPERTY()
	FTimerHandle TimeSinceSpawn;

	/** Locally stored BSConfig to access GameMode properties without storing ref to game instance */
	UPROPERTY()
	FBSConfig BSConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Properties")
	UBSHealthComponent* HealthComponent;

	UPROPERTY()
	FGuid Guid;

private:
	/** Play the StartToPeakTimeline, which corresponds to the StartToPeakCurve */
	UFUNCTION()
	void PlayStartToPeakTimeline();

	/** Play the PeakToEndTimeline, which corresponds to the PeakToEndCurve */
	UFUNCTION()
	void PlayPeakToEndTimeline();

	/** Briefly makes the target lower opacity, and sets the color back to BeatGridInactive color */
	UFUNCTION()
	void PlayFadeAndReappearTimeline();

	/** Set the color to BeatGrid color */
	UFUNCTION()
	void SetColorToBeatGridColor();

	UFUNCTION()
	void StartToPeak(const float Alpha);

	UFUNCTION()
	void PeakToEnd(const float Alpha);

	UFUNCTION()
	void FadeAndReappear(const float Alpha);

	void ApplyImmunityEffect();

	/** Unlike other modes which use LifeSpanExpired to notify TargetSpawner of their expiration,
	 *  BeatGrid needs to use this function since the the targets aren't going to be destroyed,
	 *  but instead just deactivated */
	UFUNCTION()
	void OnBeatGridTimerTimeOut();

	UFUNCTION()
	void ShowTargetOutline();
	
	FGameplayTagContainer GameplayTags;

	UPROPERTY()
	FPlayerSettings_Game PlayerSettings;

	FTimeline StartToPeakTimeline;
	FTimeline PeakToEndTimeline;
	FTimeline FadeAndReappearTimeline;

	/** Play the explosion effect at the location of target, scaled to size with the color of the target when it was destroyed. */
	void PlayExplosionEffect(const FVector& ExplosionLocation, const float SphereRadius, const FLinearColor& ColorWhenDestroyed) const;

	/** The scale that was applied when spawned */
	float TargetScale = 1.f;
};
