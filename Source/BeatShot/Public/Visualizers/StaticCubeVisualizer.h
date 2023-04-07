// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VisualizerBase.h"
#include "GameFramework/Actor.h"
#include "StaticCubeVisualizer.generated.h"

class UStaticMeshComponent;

UCLASS()
class BEATSHOT_API AStaticCubeVisualizer : public AVisualizerBase
{
	GENERATED_BODY()

public:
	AStaticCubeVisualizer();

	/** Deletes all existing visualizers and generates a new array of visualizers based on AASettings */
	virtual void InitializeVisualizer() override;

	/** Updates the CubeHeightScale and the RedGreenAlpha for a cube at Index */
	virtual void UpdateVisualizer(const int32 Index, const float SpectrumAlpha) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meshes")
	UStaticMesh* CubeMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Materials")
	UMaterialInterface* CubeMaterial;

private:
	/** Returns the SpectrumValue scaled between MinCubeHeightScale and MaxCubeHeightScale */
	float GetScaledHeight(const float SpectrumValue) const;

	/** Using this array instead of Visualizers array */
	UPROPERTY(VisibleAnywhere)
	TArray<UStaticMeshComponent*> Cubes;
};
