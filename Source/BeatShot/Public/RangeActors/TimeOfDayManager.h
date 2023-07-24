// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "BeatShot/BeatShot.h"
#include "TimeOfDayManager.generated.h"

class AVisualizerBase;
class UDirectionalLightComponent;
class ASkyLight;
class AStaticMeshActor;
class AVolumetricCloud;
class ADirectionalLight;
class UCurveFloat;
class UCurveVector;
class AMoon;
class UMaterialInstanceDynamic;

UCLASS()
class BEATSHOT_API ATimeOfDayManager : public AActor
{
	GENERATED_BODY()

public:
	ATimeOfDayManager();
	
	virtual void Tick(float DeltaTime) override;

	/** Changes TimeOfDay and plays TransitionTimeline */
	UFUNCTION(BlueprintCallable)
	void BeginTransitionToNight();

	/** Changes TimeOfDay and plays TransitionTimeline */
	UFUNCTION(BlueprintCallable)
	void BeginTransitionToDay();

	UFUNCTION(CallInEditor)
	void ToggleTimeOfDay();

	/** Instantly changes the time of day */
	void SetTimeOfDay(const ETimeOfDay InTimeOfDay);

	ETimeOfDay GetTimeOfDay() const { return TimeOfDay; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Calls RefreshMaterial function in SkySphere */
	UFUNCTION(BlueprintImplementableEvent)
	void RefreshSkySphereMaterial();

	/** Soft Reference to SkySphere in Range level */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<AActor> SkySphere_Soft;

	/** Soft Reference to Moon in Range level */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<AMoon> Moon_Soft;

	/** Soft Reference to Daylight in Range level */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<ADirectionalLight> Daylight_Soft;

	/** Soft Reference to Skylight in Range level */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<ASkyLight> Skylight_Soft;

	/** Reference to left roof mesh to move */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<AStaticMeshActor> LeftWindowCover;

	/** Reference to right roof mesh to move */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<AStaticMeshActor> RightWindowCover;

	/** Reference to SkySphere dynamic material instance */
	UPROPERTY()
	UMaterialInstanceDynamic* SkySphereMaterial;

	/** The curve to link to OnTimelineVector and TransitionTimeline */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveVector* LightCurve;

	/** The curve to link to OnTransitionMaterialTick and TransitionTimeline */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* SkyMaterialCurve;

	/** The current time of day */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ETimeOfDay TimeOfDay;

	UPROPERTY(EditAnywhere)
	ETimeOfDay TimeOfDay_Editor;

	FTimeline TransitionTimeline;

	/** Link TransitionTimeline, LightCurve to the function SetLightFromVectorCurve() */
	FOnTimelineVector OnTimelineVector;

	/** Link TransitionTimeline, SkyMaterialCurve to the function TransitionSkySphereMaterial() */
	FOnTimelineFloat OnTransitionMaterialTick;

	/** Link TransitionTimeline to the function OnTimelineCompletedCallback() */
	FOnTimelineEvent OnTimelineCompleted;

	/** Changes TimeOfDay after TransitionTimeline completes */
	UFUNCTION()
	void OnTimelineCompletedCallback();

	/** Executes on every tick of TransitionTimeline, reads from LightCurve */
	UFUNCTION()
	void TransitionTimeOfDay(const FVector Vector);

	/** Executes on every tick of TransitionTimeline, reads from SkyMaterialCurve */
	UFUNCTION()
	void TransitionSkySphereMaterial(float Alpha);
	
	FVector DaytimeLeftRoofLocation = {2980, 0, 3371};
	FVector DaytimeRightRoofLocation = {2020, 0, 3371};
	FVector NighttimeLeftRoofLocation;
	FVector NighttimeRightRoofLocation;

	/** Distance along the x-axis the roofs covering the night time windows have to travel during the day to night transition. This value is reversed when the cycle goes from night to day */
	UPROPERTY(EditAnywhere, Category = "Day-Night Cycle")
	float DayToNightRoofXTravelDistance = -1000.f;

	/** Distance along the z-axis the roofs covering the night time windows have to travel during the day to night transition. This value is reversed when the cycle goes from night to day */
	UPROPERTY(EditAnywhere, Category = "Day-Night Cycle")
	float DayToNightRoofZTravelDistance = 20.f;

	UPROPERTY(EditAnywhere)
	float MaxMoonlightIntensity = 1.f;

	UPROPERTY(EditAnywhere)
	float MaxDaylightIntensity = 15.f;

	UPROPERTY(EditAnywhere)
	float DaySkylightIntensity = 3.f;

	UPROPERTY(EditAnywhere)
	float NightSkylightIntensity = 10.f;

	float LastLerpRotation;
};
