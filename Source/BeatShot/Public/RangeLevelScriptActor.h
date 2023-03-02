// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "Components/TimelineComponent.h"
#include "Engine/LevelScriptActor.h"
#include "RangeLevelScriptActor.generated.h"

class ASkyLight;
class AStaticMeshActor;
class AVolumetricCloud;
class ADirectionalLight;
class UCurveFloat;
class AMoon;
class UMaterialInstanceDynamic;

/** Enum representing the different times of the day */
UENUM(BlueprintType)
enum class ETimeOfDay : uint8
{
	Day UMETA(DisplayName="Day"),
	Night UMETA(DisplayName="Night"),
	DayToNight UMETA(DisplayName="DayToNight"),
	NightToDay UMETA(DisplayName="NightToDay"),
};

UCLASS()
class BEATSHOT_API ARangeLevelScriptActor : public ALevelScriptActor, public ISaveLoadInterface
{
	
	GENERATED_BODY()

protected:
	
	ARangeLevelScriptActor();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	/** Changes TimeOfDay if NewStreak is greater than StreakThreshold */
	UFUNCTION()
	void OnTargetDestroyed(const float TimeAlive, const int32 NewStreak, const FVector Position);

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

	/** Reference to Skylight directional light in Range level */
	UPROPERTY()
	ASkyLight* Skylight;

	/** The threshold to activate night mode if not yet unlocked */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 StreakThreshold = 1;

	/** The curve to link to OnTransitionTick and TransitionTimeline */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* MovementCurve;

	/** The curve to link to OnTransitionMaterialTick and TransitionTimeline */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* SkyMaterialCurve;

	/** The curve to link to OnTransitionSkylightTick and TransitionTimeline */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* SkylightIntensityCurve;

	/** The curve to link to OnTransitionSkylightTick and TransitionTimeline */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* SkylightIntensityCurveReverse;

	ETimeOfDay TimeOfDay;

	FTimeline TransitionTimeline;

	FTimeline SkylightIntensityTimeline;

	FTimeline SkylightIntensityReverseTimeline;

	/** Link TransitionTimeline, MovementCurve to the function TransitionTimeOfDay() */
	FOnTimelineFloat OnTransitionTick;

	/** Link TransitionTimeline, SkyMaterialCurve to the function TransitionSkySphereMaterial() */
	FOnTimelineFloat OnTransitionMaterialTick;

	/** Link SkylightIntensityTimeline, SkylightIntensityCurve to the function TransitionSkylightIntensity() */
	FOnTimelineFloat OnTransitionSkylightTick;

	/** Link TransitionTimeline to the function OnTimelineCompletedCallback() */
	FOnTimelineEvent OnTimelineCompleted;

	/** Changes TimeOfDay and plays TransitionTimeline */
	void BeginTransitionToNight();

	/** Changes TimeOfDay and plays TransitionTimeline */
	void BeginTransitionToDay();

	/** Instantly changes all settings to Night */
	void SetTimeOfDayToNight();

	/** Changes TimeOfDay after TransitionTimeline completes */
	UFUNCTION()
	void OnTimelineCompletedCallback();

	/** Executes on every tick of TransitionTimeline, reads from MovementCurve */
	UFUNCTION()
	void TransitionTimeOfDay(float Alpha);

	/** Executes on every tick of TransitionTimeline, reads from SkyMaterialCurve */
	UFUNCTION()
	void TransitionSkySphereMaterial(float Alpha);

	/** Executes on every tick of SkylightIntensityTimeline, reads from SkylightIntensityCurve */
	UFUNCTION()
	void TransitionSkylightIntensity(float Alpha);

	/** Callback function to respond to NightMode change from WallMenu */
	UFUNCTION()
	void OnPlayerSettingsChanged(const FPlayerSettings& PlayerSettings);

	FVector InitialLeftWindowCoverLoc;

	FVector InitialRightWindowCoverLoc;

	const float CycleSpeed = 20;

	const float WindowCoverOffset = -1700;

	float LastLerpRotation;
};
