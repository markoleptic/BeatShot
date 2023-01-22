// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "SimpleBeamLight.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class UCurveLinearColor;
class UMaterialInterface;
class UMaterialInstanceDynamic;

UCLASS()
class BEATSHOT_API ASimpleBeamLight : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASimpleBeamLight();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void InitializeBeam();
	
	UFUNCTION()
	void UpdateNiagaraBeam(float Value);

	UFUNCTION()
	void OnNiagaraBeamFinished(UNiagaraComponent* NiagaraComponent);
	
	UFUNCTION()
	void SetEmissiveLightColor(FLinearColor Value);

	FTimeline EmissiveBulbTimeline;

	UPROPERTY(EditDefaultsOnly)
	UCurveLinearColor* EmissiveLightCurve;
	
	UPROPERTY(EditDefaultsOnly)
	UNiagaraSystem* SimpleBeam;

	UPROPERTY(EditAnywhere)
	UNiagaraComponent* SimpleBeamComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Spots_SpotBase;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Spots_SpotLimb;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Spots_SpotHead;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMaterialInterface* EmissiveLightBulbMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UMaterialInstanceDynamic* EmissiveLightBulb;

	UPROPERTY(EditAnywhere,  BlueprintReadWrite)
	FLinearColor LightColor = FLinearColor::White;
};


