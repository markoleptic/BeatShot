// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "SimpleBeamLight.generated.h"

class USpotLightComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class UCurveLinearColor;
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

private:
	/** Deactivates the Niagara System and stops the ColorTimeline */
	UFUNCTION()
	void OnNiagaraBeamFinished(UNiagaraComponent* NiagaraComponent);

	/** Main function that calls all other SetColor functions */
	UFUNCTION()
	void SetLightColor(const FLinearColor Color);

	/** Sets the color and intensity of the EmissiveLightBulb */
	void SetEmissiveBulbColor(const FLinearColor Color);

	/** Sets the color and intensity of the Spotlight */
	void SetSpotlightColor(const FLinearColor Color);

	/** Sets the color and intensity of the BeamEndLight */
	void SetBeamEndLightColor(const FLinearColor Color);

	/** Traces a line forward from SpotlightHead and if a blocking hit is found, sets the BeamEndLight to the correct
	 *  location and rotation. Disables the BeamEndLight if not blocking hit is found */
	UFUNCTION()
	void LineTrace();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meshes")
	UStaticMeshComponent* SpotlightBase;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meshes")
	UStaticMeshComponent* SpotlightLimb;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meshes")
	UStaticMeshComponent* SpotlightHead;

	/** Niagara system for the visible beam of light */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Niagara")
	UNiagaraSystem* SimpleBeam;

	/** Niagara component that SimpleBeam resides in */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Niagara")
	UNiagaraComponent* SimpleBeamComponent;

	/** The actual spotlight light that appears at the end of the SimpleBeam */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lights")
	USpotLightComponent* Spotlight;

	/** Indirect lighting applied to the area at the end of the SimpleBeam */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lights")
	USpotLightComponent* BeamEndLight;

	/** The color values over time to apply to the lights */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lights | Curves")
	UCurveLinearColor* LightColorCurve;

	/** The base color to apply to all lights */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lights | Color")
	FLinearColor LightColor = FLinearColor::White;

	/** The light inside the SpotlightHead */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Materials")
	UMaterialInstanceDynamic* EmissiveLightBulb;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Materials")
	UMaterialInterface* EmissiveLightBulbMaterial;

private:
	/** The timeline to link to the LightColorCurve */
	FTimeline ColorTimeline;

	/** Delegate that binds the ColorTimeline to SetColor() */
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

	/** The distance to trace the line */
	const float MaxBeamWidth = 50;

	/** Offset to apply to the location of the BeamEndLight */
	const float BeamEndLightOffset = 5;
};
