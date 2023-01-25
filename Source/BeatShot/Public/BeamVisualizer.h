// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VisualizerBase.h"
#include "GameFramework/Actor.h"
#include "BeamVisualizer.generated.h"

class ASimpleBeamLight;
class ABeamLight;
class ABeamTarget;

UCLASS()
class BEATSHOT_API ABeamVisualizer : public AVisualizerBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABeamVisualizer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void InitializeVisualizer() override;
	
	virtual void UpdateVisualizer(const int32 Index, const float SpectrumAlpha) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ASimpleBeamLight> SimpleBeamLightClass;
	
	const FVector InitialBeamLocation = {0,1920,1320};
	const FRotator BeamRotation = {0,0,90};
	const FVector BeamOffset = {100, 0, 0};
};
