// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VisualizerBase.h"
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ASimpleBeamLight> SimpleBeamLightClass;

private:
	TArray<FLinearColor> BeatColors;

	const FVector InitialBeamLightLocation = {0, 0, 1340};

	const FRotator BeamLightRotation = {0, 0, 0};

	const FVector BeamLightOffset = {0, 100, 0};
};
