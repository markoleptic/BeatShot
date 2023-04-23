// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VisualizerBase.h"
#include "GameFramework/Actor.h"
#include "StaticCubeVisualizer.generated.h"

class UStaticMeshComponent;
class UInstancedStaticMeshComponent;

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

	virtual void MarkRenderStateDirty() override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* RootSceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInstancedStaticMeshComponent* InstancedBaseMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInstancedStaticMeshComponent* InstancedVerticalOutlineMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInstancedStaticMeshComponent* InstancedTopMesh;

	/* The base mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meshes")
	UStaticMesh* BaseMesh;

	/* The mesh used as the vertical outline */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meshes")
	UStaticMesh* VerticalOutlineMesh;

	/* The mesh used as the outline for the top of the cube */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meshes")
	UStaticMesh* TopMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Materials")
	UMaterialInterface* BaseCubeMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Materials")
	UMaterialInterface* OutlineMaterial;

private:
	/** Returns the SpectrumValue scaled between MinCubeHeightScale and MaxCubeHeightScale */
	float GetScaledHeight(const float SpectrumValue) const;

	void AddInstancedCubeMesh(const FVector& RelativePosition);

	const float MeshScale = 0.5f;

	const float CubeHeight = 100.f;
};
