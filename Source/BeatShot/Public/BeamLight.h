// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BeamLight.generated.h"

class UPointLightComponent;
class UCurveFloat;
class ABeamTarget;
class USpotLightComponent;
class UNiagaraComponent;

UCLASS()
class BEATSHOT_API ABeamLight : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABeamLight();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	void UpdateNiagaraConeProps();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Spots_SpotBase;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Spots_SpotLimb;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Spots_SpotHead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraComponent* NiagaraCone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USpotLightComponent* SpotLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USpotLightComponent* FakeGISpotlight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPointLightComponent* LightShaftIllumination;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* AngleCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMesh* BaseMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMesh* LimbMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMesh* HeadMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LightAttenuation = 3000;

	UPROPERTY(EditAnywhere,  BlueprintReadWrite)
	float LightIntensity = 1;
	
	UPROPERTY(EditAnywhere,  BlueprintReadWrite)
	float LightIntensityMax = 2;
	
	UPROPERTY(EditAnywhere,  BlueprintReadWrite)
	float ConeOuter = 2;

	UPROPERTY(EditAnywhere,  BlueprintReadWrite)
	float ConeInner = 2;

	UPROPERTY(EditAnywhere,  BlueprintReadWrite)
	FLinearColor LightColor = FLinearColor::White;

	UPROPERTY(EditAnywhere,  BlueprintReadWrite)
	FLinearColor LightMultiplier= FLinearColor(3,3,3,1);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ConeLengthRatio = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OcclusionMaskDistance = 180;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LightEmitterLength = 300;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CustomAngle = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCustomAngle = true;
};
