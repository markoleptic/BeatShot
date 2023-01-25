// Fill out your copyright notice in the Description page of Project Settings.

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
	// Sets default values for this actor's properties
	AStaticCubeVisualizer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void InitializeVisualizer() override;
	
	virtual void UpdateVisualizer(const int32 Index, const float SpectrumAlpha) override;

	float GetScaledHeight(const float SpectrumValue);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMesh* CubeMesh;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMaterialInterface* CubeMaterial;

	const float MaxZScale = 10.f;

	UPROPERTY(VisibleAnywhere)
	TArray<UStaticMeshComponent*> Cubes;
};


