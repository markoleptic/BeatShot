// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "Components/TimelineComponent.h"
#include "Engine/LevelScriptActor.h"
#include "RangeLevelScriptActor.generated.h"

class AStaticMeshActor;
class AVolumetricCloud;
class ADirectionalLight;
class ARectLight;
class UCurveFloat;
class AMoon;
class UMaterialInstanceDynamic;

/** Enum representing the different times of the day */
UENUM(BlueprintType)
enum class ETimeOfDay : uint8
{
	Day UMETA(DisplayName, "Day"),
	Night UMETA(DisplayName, "Night"),
	DayToNight UMETA(DisplayName, "DayToNight"),
	NightToDay UMETA(DisplayName, "NightToDay"),
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

	/** Reference to SkySphere in Range level */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	AActor* SkySphere;

	/** Reference to Moon in Range level */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	AMoon* Moon;

	/** Reference to left roof mesh to move */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	AStaticMeshActor* LeftWindowCover;

	/** Reference to right roof mesh to move */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	AStaticMeshActor* RightWindowCover;
	
	/** Reference to SkySphere dynamic material instance */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMaterialInstanceDynamic* SkySphereMaterial;

	/** Reference to Daylight directional light in Range level */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ADirectionalLight* Daylight;

	/** Reference to rectangular light near the TargetSpawner */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ARectLight* TargetSpawnerLight;

	/** The threshold to activate night mode if not yet unlocked */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 StreakThreshold = 1;

	/** The curve to link to OnTransitionTick and TransitionTimeline */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* MovementCurve;

	/** The curve to link to OnTransitionMaterialTick and TransitionTimeline */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* SkyMaterialCurve;

	ETimeOfDay TimeOfDay;

	FTimeline TransitionTimeline;

	/** Link TransitionTimeline, MovementCurve to the function TransitionTimeOfDay() */
	FOnTimelineFloat OnTransitionTick;

	/** Link TransitionTimeline, SkyMaterialCurve to the function TransitionSkySphereMaterial() */
	FOnTimelineFloat OnTransitionMaterialTick;

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

	/** Callback function to respond to NightMode change from WallMenu */
	UFUNCTION()
	void OnPlayerSettingsChanged(const FPlayerSettings& PlayerSettings);

	FVector InitialLeftWindowCoverLoc;

	FVector InitialRightWindowCoverLoc;

	const float CycleSpeed = 20;

	const float WindowCoverOffset = 1700;

	float LastLerpRotation;
};

