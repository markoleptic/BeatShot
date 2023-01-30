// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "Engine/LevelScriptActor.h"
#include "RangeLevelScriptActor.generated.h"

class AVolumetricCloud;
class ADirectionalLight;
class ARectLight;
class UCurveFloat;
class AMoon;
class UMaterialInstanceDynamic;

UCLASS()
class BEATSHOT_API ARangeLevelScriptActor : public ALevelScriptActor
{
	
	GENERATED_BODY()

	ARangeLevelScriptActor();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void OnStreakUpdated(const int32 NewStreak, const FVector Position);
	
protected:

	UFUNCTION(BlueprintImplementableEvent)
	void RefreshSkySphereMaterial();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	AActor* SkySphere;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	AMoon* Moon;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMaterialInstanceDynamic* SkySphereMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	AVolumetricCloud* VolumetricCloud;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ADirectionalLight* Daylight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ARectLight* TargetSpawnerLight;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 StreakThreshold = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* MovementCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* SkyMaterialCurve;

	FTimeline TransitionTimeline;

	FOnTimelineFloat OnTransitionTick;

	FOnTimelineFloat OnTransitionMaterialTick;

	FOnTimelineEvent OnTimelineCompleted;

	void BeginNightTransition();

	void BeginDayTransition();

	UFUNCTION()
	void OnTimelineCompletedCallback();
	
	UFUNCTION()
	void TransitionDayState(float Alpha);

	UFUNCTION()
	void TransitionDayStateMaterial(float Alpha);

	bool bIsDaytime;

	bool bIsTransitioning;

	const float CycleSpeed = 20;

	float LastLerpRotation;
};


