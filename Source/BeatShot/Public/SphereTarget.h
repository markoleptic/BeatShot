// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "SaveGameCustomGameMode.h"
#include "SphereTarget.generated.h"

class UDefaultHealthComponent;
class UCapsuleComponent;
class UNiagaraSystem;
class UCurveFloat;
class UCurveLinearColor;

/** Target Spawner binds to this function to receive info about how target was destroyed */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnLifeSpanExpired, bool, DidExpire, float, TimeAlive, ASphereTarget*, DestroyedTarget);

UCLASS()
class BEATSHOT_API ASphereTarget : public AActor
{
	GENERATED_BODY()

public:
	/** Sets default values for this actor's properties */
	ASphereTarget();

	/** Sets the scale for the BaseMesh and the OutlineMesh */
	void SetSphereScale(const FVector NewScale);

protected:
	
	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	/** Ticks the timelines */
	virtual void Tick(float DeltaSeconds) override;
	
	/** Called when a non BeatGrid target lifespan has expired */
	virtual void LifeSpanExpired() override;

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
	UCurveLinearColor* FadeAndReappearCurve;

	UPROPERTY(EditDefaultsOnly)
	UCurveLinearColor* WhiteToGreenCurve;

	UPROPERTY(EditDefaultsOnly)
	UCurveLinearColor* GreenToRedCurve;

public:
	
	/** Called in TargetSpawner to activate a BeatGrid target */
	void StartBeatGridTimer(float Lifespan);

	/* Called from DefaultHealthComponent when a SphereTarget receives damage. */
	void HandleDestruction();
	
	FOnLifeSpanExpired OnLifeSpanExpired;

	/** All of the FOnTimelineLinearColor bind to this function,
	*   so that the color of the sphere is changed based on the timeline positions.
	*   Also called by Gun to change the color for BeatTrack modes */
	UFUNCTION()
	void SetSphereColor(const FLinearColor Output);
	
	UFUNCTION()
	void SetOutlineColor(const FLinearColor Output);

	UFUNCTION()
	void SetSphereAndOutlineColor(const FLinearColor Output);

	UPROPERTY()
	FTimerHandle TimeSinceSpawn;

	UPROPERTY()
	FTimerHandle ValidScoreTargetTime;

	/** Locally stored GameModeActorStruct to access GameMode properties without storing ref to game instance */
	FGameModeActorStruct GameModeActorStruct;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Properties")
	UDefaultHealthComponent* HealthComp;

	FGuid Guid;

private:

	/** Play the WhiteToGreenTimeline, which corresponds to the WhiteToGreenCurve */
	UFUNCTION()
	void PlayWhiteToGreenTimeline();

	/** Play the WhiteToGreenTimeline, which corresponds to the WhiteToGreenCurve */
	UFUNCTION()
	void PlayGreenToRedTimeline();

	/** Briefly makes the target higher opacity. Only used for BeatGrid */
	UFUNCTION()
	void PlayFadeAndReappearTimeline();

	/** Set the color to BeatGrid color */
	UFUNCTION()
	void SetColorToBeatGridColor();
	
	FTimeline FadeAndReappearTimeline;
	FTimeline WhiteToGreenTimeline;
	FTimeline GreenToRedTimeline;

	/** Switch that is changed in the PlayTimeline functions so that all timelines aren't always ticked */
	int32 TimelineSwitch = -1;
	
	/** Unlike other modes which use LifeSpanExpired to notify TargetSpawner of their expiration,
	 *  BeatGrid needs to use this function since the the targets aren't going to be destroyed,
	 *  but instead just deactivated */
	UFUNCTION()
	void OnBeatGridTimerTimeOut();

	/** Set the max health of the target */
	void SetMaxHealth(float NewMaxHealth) const;

	/** Play the explosion effect at the location of target, scaled to size with the color of the target when it was destroyed. */
	void PlayExplosionEffect(const FVector ExplosionLocation, const float SphereRadius,
	                         const FLinearColor ColorWhenDestroyed) const;

	/** Base radius for sphere target. */
	const float BaseSphereRadius = 50.f;

	/** The scale that was applied when spawned */
	float TargetScale = 1.f;

	/** Color for BeatGrid targets that aren't active. */
	FLinearColor BeatGridPurple = {83.f / 255.f, 0.f, 245.f / 255.f, 1.f};

	/** The ratio between the scale of the BaseMesh to OutlineMesh, used to apply a constant proportioned outline
	 *  regardless of the scale of the target */
	const float BaseToOutlineRatio = 0.9;
};


