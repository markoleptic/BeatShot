// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SphereTarget.generated.h"

class UCapsuleComponent;
UCLASS()
class BEATAIM_API ASphereTarget : public AActor
{
	GENERATED_BODY()
public:	
	ASphereTarget();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	UPROPERTY(VisibleAnywhere, Category = "Target Properties", BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		UCapsuleComponent* CapsuleComp;
	UPROPERTY(VisibleAnywhere, Category = "Target Properties", BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		UStaticMeshComponent* BaseMesh;
	// probably make this a parent class of targets in the future
	virtual void Tick(float DeltaTime) override;
	void HandleDestruction();
private:
};
