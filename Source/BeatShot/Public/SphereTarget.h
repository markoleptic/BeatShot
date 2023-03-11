// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "SaveGameCustomGameMode.h"
#include "SaveGamePlayerSettings.h"
#include "SaveLoadInterface.h"
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
class BEATSHOT_API ASphereTarget : public AActor, public ISaveLoadInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()

	/** Sets default values for this actor's properties */
	ASphereTarget();

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

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
	/** Called in TargetSpawner to activate a BeatGrid target */
	void StartBeatGridTimer(float Lifespan);

	/** Sets the scale for the BaseMesh and the OutlineMesh */
	void SetSphereScale(const FVector NewScale);

	/** Sets the color of the Target Material */
	UFUNCTION()
	void SetSphereColor(const FLinearColor Output);

	/** Sets the color of the Target Outline Material */
	UFUNCTION()
	void SetOutlineColor(const FLinearColor Output);

	/** Called from DefaultHealthComponent when a SphereTarget receives damage. */
	UFUNCTION()
	void HandleDestruction();

	/** Target Spawner binds to this function to receive info about how target was destroyed */
	FOnLifeSpanExpired OnLifeSpanExpired;

	/** The length of the time the target was alive for */
	UPROPERTY()
	FTimerHandle TimeSinceSpawn;

	/** Locally stored GameModeActorStruct to access GameMode properties without storing ref to game instance */
	UPROPERTY()
	FGameModeActorStruct GameModeActorStruct;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Properties")
	UBSHealthComponent* HealthComponent;

	UPROPERTY()
	FGuid Guid;

	/** Returns whether or not this target is a BeatTrack target */
	bool IsBeatTrackTarget() const { return bIsBeatTrackTarget; }

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

	/** Unlike other modes which use LifeSpanExpired to notify TargetSpawner of their expiration,
	 *  BeatGrid needs to use this function since the the targets aren't going to be destroyed,
	 *  but instead just deactivated */
	UFUNCTION()
	void OnBeatGridTimerTimeOut();

	UFUNCTION()
	void ShowTargetOutline();

	UPROPERTY()
	FPlayerSettings_Game PlayerSettings;

	FTimeline StartToPeakTimeline;
	FTimeline PeakToEndTimeline;
	FTimeline FadeAndReappearTimeline;

	/** Set the max health of the target */
	void SetMaxHealth(float NewMaxHealth) const;

	/** Play the explosion effect at the location of target, scaled to size with the color of the target when it was destroyed. */
	void PlayExplosionEffect(const FVector ExplosionLocation, const float SphereRadius, const FLinearColor ColorWhenDestroyed) const;

	bool bIsBeatTrackTarget = false;

	/** Base radius for sphere target. */
	const float BaseSphereRadius = 50.f;

	/** The scale that was applied when spawned */
	float TargetScale = 1.f;

	/** The ratio between the scale of the BaseMesh to OutlineMesh, used to apply a constant proportioned outline
	 *  regardless of the scale of the target */
	const float BaseToOutlineRatio = 0.9;
};
