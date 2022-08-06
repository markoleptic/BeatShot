// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpiderShotSelector.generated.h"

UCLASS()
class BEATAIM_API ASpiderShotSelector : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpiderShotSelector();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Mesh", meta = (AllowPrivateAccess = true))
		class UCapsuleComponent* CapsuleComp;
	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh", meta = (AllowPrivateAccess = true))
		class UStaticMeshComponent* TargetMesh;
};
