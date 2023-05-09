// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BSKnife.generated.h"

class URadialForceComponent;

UCLASS()
class BEATSHOT_API ABSKnife : public AActor
{
	GENERATED_BODY()

public:
	ABSKnife();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Knife;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	//URadialForceComponent* RadialForce;

public:
	virtual void Tick(float DeltaTime) override;
};
