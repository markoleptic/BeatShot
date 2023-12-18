// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Moon.generated.h"

class USphereComponent;
class UDirectionalLightComponent;

/** Represents the moon */
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* MoonGlowMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UMaterialInstanceDynamic* MoonMaterialInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UMaterialInstanceDynamic* MoonGlowMaterialInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UMaterial* MoonMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UDirectionalLightComponent* MoonLight;
};
