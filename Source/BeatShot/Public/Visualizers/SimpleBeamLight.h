// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "SimpleBeamLight.generated.h"

class USpotLightComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class UCurveLinearColor;
class UCurveVector;
class USceneComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnBeamLightLifetimeCompleted, const int32 OutIndex);

UCLASS()
class BEATSHOT_API ASimpleBeamLight : public AActor
{
	GENERATED_BODY()

public:
	ASimpleBeamLight();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* SpotlightBase;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* SpotlightLimb;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* SpotlightHead;

	/** Niagara component that Niagara System SimpleBeam resides in */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UNiagaraComponent* SimpleBeamComponent;

	/** The actual spotlight light that appears at the end of the SimpleBeam. Its position is near the base, unlike BeamEndLight */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	USpotLightComponent* Spotlight;

	/** Indirect lighting applied to the area at the end of the SimpleBeam */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	USpotLightComponent* BeamEndLight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	USceneComponent* LightPositionComponent;

	/** The position over time to move the BeamEndLight */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Curves")
	UCurveVector* LightMovementCurve;

	/** The base color to apply to Spotlight, BeamEndLight, and SimpleBeamComponent */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Color")
	FLinearColor LightColor;

	/** The light inside the SpotlightHead */
	UPROPERTY()
	UMaterialInstanceDynamic* EmissiveLightBulb;
	
public:

	/** Initialize the light components when spawning */
	void InitSimpleBeamLight(const FLinearColor& Color, const int32 NewIndex, const float InLightDuration, const bool InbIsMovingLight = false);

	/** Activate the light components */
	void ActivateLightComponents();

	/** Deactivate the light components */
	void DeactivateLightComponents();

	/** Returns the duration of this beam light */
	float GetLightDuration() const { return LightDuration; }

	/** Returns the color of this beam light */
	FLinearColor GetLightColor() const { return LightColor; }

	/** Broadcast when the niagara component has completed */
	FOnBeamLightLifetimeCompleted OnBeamLightLifetimeCompleted;

private:
	/** Calls DeactivateLightComponents */
	UFUNCTION()
	void OnNiagaraBeamFinished(UNiagaraComponent* NiagaraComponent);

	/** Traces a line forward from SpotlightHead and if a blocking hit is found, sets the BeamEndLight to the correct
	 *  location and rotation. Disables the BeamEndLight if not blocking hit is found */
	UFUNCTION()
	void LineTrace(const FVector& EndLocation);

	UFUNCTION(CallInEditor)
	void UpdateBeamEndLightLocation();

	/** Rotates the SpotlightLimb and SpotlightHead and then calls LineTrace */
	UFUNCTION()
	void UpdateBeamEndLightPosition(const FVector& Position);
	
	/** The index of this instance inside an array of this type */
	int32 Index;

	/** The timeline to link to the LightMovementCurve */
	FTimeline LightPositionTimeline;

	/** Delegate that binds the LightPositionTimeline to UpdateBeamEndLightPosition() */
	FOnTimelineVector TimelineVectorDelegate;

	/** Inner cone angle for the Spotlight */
	float InnerConeAngle = 0.5;

	/** Outer cone angle for the Spotlight */
	float OuterConeAngle = 1.5;

	/** The length of time this light will be on for */
	float LightDuration = 0.f;

	/** Rotation (Yaw) of the SpotlightLimb from the SpotlightBase */
	UPROPERTY(VisibleAnywhere)
	float GimbalRotation;

	/** Rotation (Pitch) of the SpotlightHead from the SpotlightLimb */
	UPROPERTY(VisibleAnywhere)
	float SpotlightHeadRotation;

	/** Whether or not this light will need to use the LightPositionTimeline and LineTracing at runtime */
	bool bIsMovingLight = false;
};
