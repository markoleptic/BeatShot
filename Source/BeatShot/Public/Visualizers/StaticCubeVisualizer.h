// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VisualizerBase.h"
#include "GameFramework/Actor.h"
#include "StaticCubeVisualizer.generated.h"

class UStaticMeshComponent;
class UInstancedStaticMeshComponent;

/** Configuration unique to a CubeVisualizer */
USTRUCT(BlueprintType, Category = "Visualizer Config")
struct FCubeVisualizerConfig
{
	GENERATED_BODY()

	/** Uniform scale applied to the instanced static mesh */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Scale", meta=(DisplayPriority=0))
	float MeshScale = 0.5f;

	/** Base height of the cube */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Scale", meta=(DisplayPriority=1))
	float CubeHeight = 100.f;

	/** Min height of the cube when receiving no input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Scale", meta=(DisplayPriority=2))
	float MinCubeVisualizerHeightScale = DefaultMinCubeVisualizerHeightScale;

	/** Max height of the cube when receiving max input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Scale", meta=(DisplayPriority=3))
	float MaxCubeVisualizerHeightScale = DefaultMaxCubeVisualizerHeightScale;
	
	FCubeVisualizerConfig()
	{
		MeshScale = 0.5f;
		CubeHeight = 100.f;
		MinCubeVisualizerHeightScale = DefaultMinCubeVisualizerHeightScale;
		MaxCubeVisualizerHeightScale = DefaultMaxCubeVisualizerHeightScale;
	}
};


/** Visualizer that uses instanced static meshes and updates them using CustomDataValues in the materials and adjusting their transforms using UpdateInstanceTransform */
UCLASS()
class BEATSHOT_API AStaticCubeVisualizer : public AVisualizerBase
{
	GENERATED_BODY()

public:
	AStaticCubeVisualizer();

	/** Clears all StaticMeshInstances and generates new ones based on AASettings */
	virtual void InitializeVisualizer(const FPlayerSettings_AudioAnalyzer& InAASettings) override;

	/** Updates the CubeHeightScale and the RedGreenAlpha for a cube at Index */
	virtual void UpdateVisualizer(const int32 Index, const float SpectrumAlpha) override;

	/** Marks all instanced static meshes render states as dirty. Should be called by a VisualizerManager to limit the frequency of calls */
	virtual void MarkRenderStateDirty() override;

	FCubeVisualizerConfig& GetCubeVisualizerConfig() { return CubeVisualizerConfig; }

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* RootSceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInstancedStaticMeshComponent* InstancedBaseMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInstancedStaticMeshComponent* InstancedVerticalOutlineMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInstancedStaticMeshComponent* InstancedTopMesh;

	/* The base mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualizer Config | Meshes")
	UStaticMesh* BaseMesh;

	/* The mesh used as the vertical outline */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualizer Config | Meshes")
	UStaticMesh* VerticalOutlineMesh;

	/* The mesh used as the outline for the top of the cube */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualizer Config | Meshes")
	UStaticMesh* TopMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Visualizer Config | Materials")
	UMaterialInterface* BaseCubeMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Visualizer Config | Materials")
	UMaterialInterface* OutlineMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Visualizer Config", meta=(DisplayPriority=0))
	FCubeVisualizerConfig CubeVisualizerConfig;

private:
	/** Returns the SpectrumValue scaled between MinCubeHeightScale and MaxCubeHeightScale */
	float GetScaledHeight(const float SpectrumValue) const;

	/** Returns the scale to be supplied to an instanced static mesh, combining OffsetTransform and MeshScale */
	FVector GetScale3D(const float ScaledHeight);

	/** Returns the transform to be supplied to the InstancedTopMesh */
	FTransform GetTopMeshTransform(const int32 Index, const float SpectrumValue);

	/** Returns the transform to be supplied to the InstancedBaseMesh and InstancedVerticalOutlineMesh */
	FTransform GetSideAndBaseTransform(const int32 Index, const float SpectrumValue);

	/** Adds an instance for each mesh that is updated, i.e. InstancedBaseMesh, InstancedVerticalOutlineMesh, and InstancedTopMesh with the given RelativeTransform*/
	void AddInstancedCubeMesh(const FTransform& RelativeTransform);
};
