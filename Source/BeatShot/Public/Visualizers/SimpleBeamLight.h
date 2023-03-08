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

UCLASS()
class BEATSHOT_API ASimpleBeamLight : public AActor
{
	GENERATED_BODY()

public:
	ASimpleBeamLight();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

public:
	/** Activates the Niagara System, sets the BeamWidth parameter, and plays the ColorTimeline */
	UFUNCTION()
	void UpdateNiagaraBeam(const float Alpha);
	
	void SetTrackOnUpdate(const bool bShouldTrackOnUpdate) { bTrackOnUpdate = bShouldTrackOnUpdate; }

	/** Main function that calls all other SetColor functions */
	UFUNCTION()
	void SetLightColor(const FLinearColor Color);

	/** Main function that varies all light intensities based on the alpha value from LightColorCurve */
	UFUNCTION()
	void SetLightIntensities(const FLinearColor Color);

	/** Set the index of this SimpleBeamLight */
	void SetIndex(const int32 NewIndex) { Index = NewIndex; }

private:
	/** Deactivates the Niagara System and stops the ColorTimeline */
	UFUNCTION()
	void OnNiagaraBeamFinished(UNiagaraComponent* NiagaraComponent);
	
	/** Traces a line forward from SpotlightHead and if a blocking hit is found, sets the BeamEndLight to the correct
	 *  location and rotation. Disables the BeamEndLight if not blocking hit is found */
	UFUNCTION()
	void LineTrace(const FVector EndLocation);

	/** Rotates the SpotlightLimb and SpotlightHead and then calls LineTrace */
	UFUNCTION()
	void UpdateBeamEndLightPosition(const FVector& Position);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* SpotlightBase;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* SpotlightLimb;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* SpotlightHead;

	/** Niagara component that Niagara System SimpleBeam resides in */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UNiagaraComponent* SimpleBeamComponent;
	
	/** The actual spotlight light that appears at the end of the SimpleBeam */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	USpotLightComponent* Spotlight;

	/** Indirect lighting applied to the area at the end of the SimpleBeam */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	USpotLightComponent* BeamEndLight;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	USceneComponent* LightPositionComponent;
	
	/** Niagara system for the visible beam of light */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Niagara")
	UNiagaraSystem* SimpleBeam;

	/** The color values over time to apply to the lights */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Curves")
	UCurveLinearColor* LightColorCurve;

	/** The position over time to move the BeamEndLight */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Curves")
	UCurveVector* LightMovementCurve;

	/** The base color to apply to all lights */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Color")
	FLinearColor LightColor;

	/** The light inside the SpotlightHead */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Materials")
	UMaterialInstanceDynamic* EmissiveLightBulb;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Materials")
	UMaterialInterface* EmissiveLightBulbMaterial;

private:
	/** The index of this instance inside an array of this type */
	int32 Index;
	
	/** The timeline to link to the LightColorCurve */
	FTimeline ColorTimeline;
	
	/** The timeline to link to the LightMovementCurve */
	FTimeline LightPositionTimeline;

	/** Delegate that binds the LightPositionTimeline to UpdateBeamEndLightPosition() */
	FOnTimelineVector TimelineVectorDelegate;

	/** Delegate that binds the ColorTimeline to SetLightIntensities() */
	FOnTimelineLinearColor ColorTimelineDelegate;

	/** Inner cone angle for the Spotlight */
	float InnerConeAngle = 0.5;

	/** Outer cone angle for the Spotlight */
	float OuterConeAngle = 1.5;

	/** The distance to trace the line */
	const float TraceDistance = 999999;

	/** The max value of the Spotlight intensity */
	const float MaxSpotlightIntensity = 16000000;

	/** The max value of the BeamEndLight intensity */
	const float MaxBeamEndLightIntensity = 80000;

	const FVector DefaultSpotlightLimbOffset = {0, 0, -18};
	const FVector DefaultSpotlightHeadOffset = {0, 0, -41};
	const FRotator DefaultSpotlightHeadRotation = {-90, 0, 0};
	const FVector DefaultSpotlightOffset = {22, 0, 0};
	
	/** The maximum width of the beam, used to scale the alpha value */
	const float MaxBeamWidth = 50;

	/** Offset to apply to the location of the BeamEndLight */
	const float BeamEndLightOffset = 5;

	/** Rotation (Yaw) of the SpotlightLimb from the SpotlightBase */
	UPROPERTY(VisibleAnywhere)
	float GimbalRotation;

	/** Rotation (Pitch) of the SpotlightHead from the SpotlightLimb */
	UPROPERTY(VisibleAnywhere)
	float SpotlightHeadRotation;

	bool bTrackOnUpdate = false;
};
