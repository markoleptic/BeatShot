// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveGamePlayerSettings.h"
#include "SaveLoadInterface.h"
#include "GameFramework/Actor.h"
#include "BeamVisualizer.generated.h"

class ASimpleBeamLight;
class ABeamLight;
class ABeamTarget;
UCLASS()
class BEATSHOT_API ABeamVisualizer : public AActor, public ISaveLoadInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABeamVisualizer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ABeamTarget> BeamTargetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ABeamLight> BeamLightClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ASimpleBeamLight> SimpleBeamLightClass;

	UPROPERTY(BlueprintReadOnly)
	UAudioAnalyzerManager* AAManager;

	UPROPERTY(VisibleAnywhere)
	TArray<ABeamTarget*> BeamTargets;

	UPROPERTY(VisibleAnywhere)
	TArray<ABeamLight*> BeamLights;

	UPROPERTY(VisibleAnywhere)
	TArray<ASimpleBeamLight*> SimpleBeamLights;

	UFUNCTION()
	void OnAAPlayerLoaded(UAudioAnalyzerManager* Manager);

	UFUNCTION()
	void InitializeLightBeams(int32 NewNumBandChannels);
	
	UFUNCTION()
	void UpdateLightBeams(TArray<float> SpectrumValues, float DeltaTime);

	UFUNCTION()
	void UpdateAASettings(const FAASettingsStruct AASettingsStruct);

	UPROPERTY(BlueprintReadOnly)
	bool bAAManagerInitialized;

	UPROPERTY(BlueprintReadOnly)
	FAASettingsStruct AASettings;

	UPROPERTY(VisibleAnywhere)
	TArray<float> SpectrumSpectrumValues;
	
	const FVector InitialBeamLocation = {0,1920,1320};
	const FVector InitialTargetLocation = {0,-1920,1320};
	const FRotator BeamRotation = {0,0,90};

	const FActorSpawnParameters SpawnParameters;

	UPROPERTY(VisibleAnywhere)
	TArray<float> MaxSpectrumValues;

	UPROPERTY(VisibleAnywhere)
	TArray<float> MaxAverageValues;

	UPROPERTY(VisibleAnywhere)
	TArray<float> AvgSpectrumValues;
};
