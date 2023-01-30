// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "RangeLevelScriptActor.generated.h"

class AVolumetricCloud;
class ADirectionalLight;
class ARectLight;
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
	TSubclassOf<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMaterialInterface* StarrySkyMat;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMaterialInterface* MoonMaterial;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMaterialInstanceDynamic* StarrySkyMatDynamic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMaterialInstanceDynamic* MoonMatDynamic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	AVolumetricCloud* VolumetricCloud;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ADirectionalLight* Daylight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ARectLight* TargetSpawnerLight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AMoon> MoonClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	AMoon* Moon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 StreakThreshold = 1;
	
	void BeginNightTransition();

	void RotateSun();

	void RotateMoon();

	void TransitionToNight(float DeltaTime);

	bool bStreakActive;

	bool bMoonPositionReached;

	const float CycleSpeed = 30;

	const FRotator StartMoonRotation = {0,0,180};
	
	const FRotator EndMoonRotation = {0,0,0};

	const FRotator StartSunRotation = {-46, 0, 0};
	
	const FRotator EndSunRotation = {60,-180,0};
};