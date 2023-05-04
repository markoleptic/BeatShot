// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VisualizerBase.h"
#include "GameFramework/Actor.h"
#include "StaticCubeVisualizer.generated.h"

class UStaticMeshComponent;
class UInstancedStaticMeshComponent;

USTRUCT(BlueprintType)
struct FCubeVisualizerConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MeshScale = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CubeHeight = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinCubeVisualizerHeightScale = DefaultMinCubeVisualizerHeightScale;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxCubeVisualizerHeightScale = DefaultMaxCubeVisualizerHeightScale;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRotator CubeRotation = DefaultStaticCubeVisualizerRotation;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector CubeVisualizerOffset = DefaultCubeVisualizerOffset;
	
	FCubeVisualizerConfig()
	{
		MeshScale = 0.5f;
		CubeHeight = 100.f;
		MinCubeVisualizerHeightScale = DefaultMinCubeVisualizerHeightScale;
		MaxCubeVisualizerHeightScale = DefaultMaxCubeVisualizerHeightScale;
		CubeVisualizerOffset = DefaultCubeVisualizerOffset;
	}
};

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

	UPROPERTY(EditDefaultsOnly, Category = "Visualizer Config")
	FCubeVisualizerConfig CubeVisualizerConfig;

private:
	/** Returns the SpectrumValue scaled between MinCubeHeightScale and MaxCubeHeightScale */
	float GetScaledHeight(const float SpectrumValue) const;

	void AddInstancedCubeMesh(const FVector& RelativePosition);
};
