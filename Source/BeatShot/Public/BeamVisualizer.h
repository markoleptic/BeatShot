// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VisualizerBase.h"
#include "GameFramework/Actor.h"
#include "BeamVisualizer.generated.h"

class ASimpleBeamLight;

UCLASS()
class BEATSHOT_API ABeamVisualizer : public AVisualizerBase
{
	GENERATED_BODY()
	
public:	
	ABeamVisualizer();

	/** Calls the parent implementation and spawns a new array of visualizers given the current AASettings */
	virtual void InitializeVisualizer() override;

	/** Calls UpdateNiagaraBeam for the Index of the current visualizer */
	virtual void UpdateVisualizer(const int32 Index, const float SpectrumAlpha) override;

protected:
	virtual void BeginPlay() override;

	virtual void Destroyed() override;
	
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ASimpleBeamLight> SimpleBeamLightClass;
	
private:
	const FVector InitialBeamLightLocation = {0, 1920, 1320};
	
	const FRotator BeamLightRotation = {90, 0, 90};
	
	const FVector BeamLightOffset = {100, 0, 0};
};
