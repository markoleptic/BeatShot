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
	
protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
public:
	
	/** Called in TargetManager to initialize the target */
	virtual void Init(const FBS_TargetConfig& InTargetConfig) override;

	void InitTargetWidget(const TObjectPtr<UTargetWidget> InTargetWidget, const FVector2d& NewLocation);

	/** Called from HealthComponent when a target receives damage. Calls HandleDeactivation, HandleDestruction,
	 *  and broadcasts OnTargetDamageEventOrTimeout before finally calling HandleDestruction*/
	virtual void OnHealthChanged(AActor* ActorInstigator, const float OldValue, const float NewValue) override;

	/** Unlike other modes which use LifeSpanExpired to notify TargetManager of their expiration, BeatGrid modes use
	 *  this function since the the targets aren't going to be destroyed, but instead just deactivated */
	virtual void OnTargetMaxLifeSpanExpired() override;
	
	/** Returns true if the target should be deactivated based on TargetDeactivationConditions */
	virtual bool ShouldDeactivate(const bool bExpired, const float CurrentHealth) const override;
	
	/** Performs any responses to the target being deactivated */
	virtual void HandleDeactivationResponses(const bool bExpired) override;
	
	/** Finds if target should be destroyed, and calls Destroy if so */
	virtual void HandleDestruction(const bool bExpired, const float CurrentHealth) override;

	TObjectPtr<UTargetWidget> TargetWidget;

public:
	/** Sets the color of the Base Target */
	virtual void SetTargetColor(const FLinearColor& Color) override;

	/** Sets the color of the Target Outline */
	virtual void SetTargetOutlineColor(const FLinearColor& Color) override;

	/** Change the sphere scale*/
	virtual void SetTargetScale(const FVector& NewScale) const override;

	virtual void SetUseSeparateOutlineColor(const bool bUseSeparateOutlineColor) override;

	void SetTargetLocation(const FVector2d& NewLocation) const;
};
