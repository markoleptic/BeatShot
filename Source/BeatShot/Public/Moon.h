// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Moon.generated.h"

class USphereComponent;
class UDirectionalLightComponent;
UCLASS()
class BEATSHOT_API AMoon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMoon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USphereComponent* SphereComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* MoonMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UMaterialInstanceDynamic* MoonMaterialInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UMaterial* MoonMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UDirectionalLightComponent* MoonLight;

	const FVector MoonMeshOffset = {250000, 0, 250000};
	
	const FVector MoonMeshScale = {400, 400, 400};

	const FVector MoonLightScale = {0.0025, 0.0025, 0.0025};

	const FRotator DaytimeMoonRotation = {0, 0, -180};

	const FRotator NighttimeMoonRotation = {0, 0, 0};

	const float SphereCompRadius = 400000;
};
