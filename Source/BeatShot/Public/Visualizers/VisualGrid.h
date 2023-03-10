// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VisualGrid.generated.h"

class UInstancedStaticMeshComponent;

UCLASS()
class BEATSHOT_API AVisualGrid : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AVisualGrid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void CreateGrid(const int32 X, const int32 Y);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInstancedStaticMeshComponent* InstancedMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Mesh;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* VisualGridMaterial;

	void SetCustomDataValues(const TArray<FVector> Points, const FVector Center, const FVector Extents, const float RowScale, const float ColScale, const int32 Value);

	int32 NumRowsGrid;
	int32 NumColsGrid;
};
