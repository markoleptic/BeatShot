// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GlobalConstants.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "SimpleBeamLight.generated.h"

class USpotLightComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class UCurveLinearColor;
class UCurveVector;
class USceneComponent;
class UMaterialInstanceDynamic;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnBeamLightLifetimeCompleted, const int32 OutIndex);

USTRUCT(BlueprintType)
struct FSimpleBeamLightConfig
{
	GENERATED_BODY()

	/** Whether or not this light will need to use the LightPositionTimeline and LineTracing at runtime */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "General")
	bool bIsMovingLight = false;
	
	/** The length of time this light will be on for */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "General")
	float LightDuration = 1.f;

	/** The base color to apply to Spotlight, BeamEndLight, and SimpleBeamComponent */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "General")
	FLinearColor LightColor;

	/** The index of this instance inside an array of this type */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "General")
	int32 Index = INDEX_NONE;

	/** Whether or not this light was manually placed in a level. This should be checked if manually placed in a level */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "General")
	bool bManuallyPlacedInLevel = false;
	
	/** Whether or not to activate the Spotlight */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spotlight")
	bool bUseSpotlight = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spotlight")
	float MaxSpotlightIntensity = Constants::DefaultMaxSpotlightIntensity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spotlight")
	float MaxBeamEndLightIntensity = Constants::DefaultMaxBeamEndLightIntensity;

	/** Inner cone angle for the Spotlight */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spotlight")
	float InnerConeAngle = Constants::DefaultSimpleBeamLightInnerConeAngle;

	/** Outer cone angle for the Spotlight */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spotlight")
	float OuterConeAngle = Constants::DefaultSimpleBeamLightOuterConeAngle;

	/** Whether or not to activate the BeamEndLight */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BeamEndLight")
	bool bUseBeamEndLight = true;
	
	/** The width of the Niagara beam */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Niagara Beam")
	float BeamWidth = Constants::DefaultSimpleBeamLightBeamWidth;

	/** The length of the Niagara beam */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Niagara Beam")
	float BeamLength = Constants::DefaultSimpleBeamLightBeamLength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Niagara Beam")
	bool bAutoCalculateBeamLength = true;

	
	FSimpleBeamLightConfig()
	{
		bIsMovingLight = false;
		LightDuration = 1.f;
		Index = INDEX_NONE;
		bManuallyPlacedInLevel = false;
		LightColor = FLinearColor::White;
		bUseSpotlight = false;
		MaxSpotlightIntensity = Constants::DefaultMaxSpotlightIntensity;
		MaxBeamEndLightIntensity = Constants::DefaultMaxBeamEndLightIntensity;
		InnerConeAngle = Constants::DefaultSimpleBeamLightInnerConeAngle;
		OuterConeAngle = Constants::DefaultSimpleBeamLightOuterConeAngle;
		bUseBeamEndLight = true;
		BeamWidth = Constants::DefaultSimpleBeamLightBeamWidth;
		BeamLength = Constants::DefaultSimpleBeamLightBeamLength;
		bAutoCalculateBeamLength = true;
	}
};

UCLASS()
class BEATSHOT_API ASimpleBeamLight : public AActor
{
	GENERATED_BODY()

public:
	ASimpleBeamLight();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SimpleBeamLight | Components")
	UStaticMeshComponent* SpotlightBase;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SimpleBeamLight | Components")
	UStaticMeshComponent* SpotlightLimb;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SimpleBeamLight | Components")
	UStaticMeshComponent* SpotlightHead;

	/** Niagara component that Niagara System SimpleBeam resides in */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SimpleBeamLight | Components")
	UNiagaraComponent* SimpleBeamComponent;

	/** The actual spotlight light that appears at the end of the SimpleBeam. Its position is near the base, unlike BeamEndLight */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SimpleBeamLight | Components")
	USpotLightComponent* Spotlight;

	/** Indirect lighting applied to the area at the end of the SimpleBeam */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SimpleBeamLight | Components")
	USpotLightComponent* BeamEndLight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SimpleBeamLight | Components")
	USceneComponent* LightPositionComponent;

	/** The position over time to move the BeamEndLight */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SimpleBeamLight | Curves")
	UCurveVector* LightMovementCurve;

	/** The base config for this light */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SimpleBeamLight | Config")
	FSimpleBeamLightConfig SimpleBeamLightConfig;

	/** The light inside the SpotlightHead */
	UPROPERTY()
	UMaterialInstanceDynamic* EmissiveLightBulb;
	
public:

	/** Initialize the light components when spawning */
	void InitSimpleBeamLight(const FSimpleBeamLightConfig& InConfig);

	/** Activate the light components */
	void ActivateLightComponents();

	/** Deactivate the light components */
	void DeactivateLightComponents();

	/** Returns the config for this simple beam light */
	FSimpleBeamLightConfig GetSimpleBeamLightConfig() const { return SimpleBeamLightConfig; }

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

	/** The timeline to link to the LightMovementCurve */
	FTimeline LightPositionTimeline;

	/** Delegate that binds the LightPositionTimeline to UpdateBeamEndLightPosition() */
	FOnTimelineVector TimelineVectorDelegate;
	
	/** Rotation (Yaw) of the SpotlightLimb from the SpotlightBase */
	UPROPERTY(VisibleAnywhere)
	float GimbalRotation;

	/** Rotation (Pitch) of the SpotlightHead from the SpotlightLimb */
	UPROPERTY(VisibleAnywhere)
	float SpotlightHeadRotation;
};
