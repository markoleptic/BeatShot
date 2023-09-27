// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Target.h"
#include "TargetPreview.generated.h"

class UTargetWidget;

UCLASS()
class BEATSHOT_API ATargetPreview : public ATarget
{
	GENERATED_BODY()

public:
	ATargetPreview();

	virtual void Tick(float DeltaSeconds) override;

	/** Called in TargetManager to initialize the target */
	virtual void Init(const FBS_TargetConfig& InTargetConfig) override;

	virtual void Destroyed() override;

	/** Sets the values of TargetWidget and its position in the viewport */
	void InitTargetWidget(const TObjectPtr<UTargetWidget> InTargetWidget, const FVector& InBoxBoundsOrigin, const FVector& InStartLocation, const float BoundsHeight);

	/** Sets the values of bSimulatePlayerDestroying and DestroyChance */
	void SetSimulatePlayerDestroying(const bool bInSimulatePlayerDestroying, const float InDestroyChance);

	/** Override to change the target widget opacity */
	virtual void SetActorHiddenInGame(bool bNewHidden) override;
	
	/** Activates a target, removes any immunity, starts the DamageableWindow timer, and starts playing the StartToPeakTimeline */
	virtual bool ActivateTarget(const float Lifespan) override;

	/** Called when SimulatePlayerDestroyingTimer expires */
	UFUNCTION()
	void OnSimulatePlayerDestroyingTimerExpired();

	/** Timer that if set, will call OnHealthChanged upon expiration, simulating a player destroying it */
	UPROPERTY()
	FTimerHandle SimulatePlayerDestroyingTimer;

	/** Whether or not to artificially destroy itself early, simulating a player destroying it */
	bool bSimulatePlayerDestroying = false;

	/** The chance to simulating a player destroying this target */
	float DestroyChance = 0.f;
	
protected:
	/** Called from HealthComponent when a target receives damage. Calls HandleDeactivation, HandleDestruction,
	 *  and broadcasts OnTargetDamageEventOrTimeout before finally calling HandleDestruction*/
	virtual void OnHealthChanged(AActor* ActorInstigator, const float OldValue, const float NewValue) override;
	
	/** Unlike other modes which use LifeSpanExpired to notify TargetManager of their expiration, BeatGrid modes use
	 *  this function since the the targets aren't going to be destroyed, but instead just deactivated */
	virtual void OnTargetMaxLifeSpanExpired() override;
	
	/** Performs any responses to the target being deactivated */
	virtual void HandleDeactivationResponses(const bool bExpired) override;
	
	/** Finds if target should be destroyed, and calls Destroy if so */
	virtual void HandleDestruction(const bool bExpired, const float CurrentHealth) override;
	
	/** Returns the position that the TargetWidget should be placed based on world position */
	FVector2d GetWidgetPositionFromWorldPosition(const FVector& InPosition) const;

	/** Reference to the target widget in CustomGameModesWidget_Preview */
	TObjectPtr<UTargetWidget> TargetWidget;
	
	FVector BoxBoundsOrigin;

	float BoxBoundsHeight = 1000.f;

public:
	/** Change the target widget color in the viewport */
	virtual void SetTargetColor(const FLinearColor& Color) override;

	/** Change the target widget scale in the viewport */
	virtual void SetTargetScale(const FVector& NewScale) const override;
	
	/** Change the position of the target widget in the viewport */
	void SetTargetWidgetLocation(const FVector& NewLocation) const;
};
