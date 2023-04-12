// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "Components/TimelineComponent.h"
#include "Engine/LevelScriptActor.h"
#include "BeatShot/BeatShot.h"
#include "RangeLevelScriptActor.generated.h"

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
class BEATSHOT_API ARangeLevelScriptActor : public ALevelScriptActor, public ISaveLoadInterface
{
	GENERATED_BODY()

protected:
	ARangeLevelScriptActor();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	/** Changes TimeOfDay */
	UFUNCTION()
	void OnStreakThresholdPassed();

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

	/** Hard Reference to SkySphere in Range level */
	UPROPERTY()
	AActor* SkySphere;

	/** Hard Reference to Moon in Range level */
	UPROPERTY()
	AMoon* Moon;

	/** Reference to left roof mesh to move */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	AStaticMeshActor* LeftWindowCover;

	/** Reference to right roof mesh to move */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	AStaticMeshActor* RightWindowCover;

	/** Reference to SkySphere dynamic material instance */
	UPROPERTY()
	UMaterialInstanceDynamic* SkySphereMaterial;

	/** Reference to Daylight directional light in Range level */
	UPROPERTY()
	ADirectionalLight* Daylight;

	/** Reference to Daylight directional light in Range level */
	UPROPERTY()
	UDirectionalLightComponent* Moonlight;

	/** Reference to Skylight directional light in Range level */
	UPROPERTY()
	ASkyLight* Skylight;

	/** The curve to link to OnTimelineVector and TransitionTimeline */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveVector* LightCurve;

	/** The curve to link to OnTransitionMaterialTick and TransitionTimeline */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* SkyMaterialCurve;

	ETimeOfDay TimeOfDay;

	FTimeline TransitionTimeline;

	/** Link TransitionTimeline, LightCurve to the function SetLightFromVectorCurve() */
	FOnTimelineVector OnTimelineVector;

	/** Link TransitionTimeline, SkyMaterialCurve to the function TransitionSkySphereMaterial() */
	FOnTimelineFloat OnTransitionMaterialTick;

	/** Link TransitionTimeline to the function OnTimelineCompletedCallback() */
	FOnTimelineEvent OnTimelineCompleted;

	/** Changes TimeOfDay and plays TransitionTimeline */
	UFUNCTION(BlueprintCallable)
	void BeginTransitionToNight();

	/** Changes TimeOfDay and plays TransitionTimeline */
	UFUNCTION(BlueprintCallable)
	void BeginTransitionToDay();

	/** Instantly changes all settings to Night */
	void SetTimeOfDayToNight();

	/** Changes TimeOfDay after TransitionTimeline completes */
	UFUNCTION()
	void OnTimelineCompletedCallback();

	/** Executes on every tick of TransitionTimeline, reads from LightCurve */
	UFUNCTION()
	void TransitionTimeOfDay(const FVector Vector);

	/** Executes on every tick of TransitionTimeline, reads from SkyMaterialCurve */
	UFUNCTION()
	void TransitionSkySphereMaterial(float Alpha);

	/** Callback function to respond to NightMode change from WallMenu */
	virtual void OnPlayerSettingsChanged_Game(const FPlayerSettings_Game& GameSettings) override;

	FVector InitialLeftWindowCoverLoc;

	FVector InitialRightWindowCoverLoc;

	UPROPERTY(EditAnywhere)
	float MaxMoonlightIntensity = 1.f;

	UPROPERTY(EditAnywhere)
	float MaxDaylightIntensity = 15.f;

	UPROPERTY(EditAnywhere)
	float DaySkylightIntensity = 1.5f;

	UPROPERTY(EditAnywhere)
	float NightSkylightIntensity = 35.f;

	float LastLerpRotation;
};
