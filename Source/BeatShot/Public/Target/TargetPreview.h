// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Target.h"
#include "TargetPreview.generated.h"

UCLASS()
class BEATSHOT_API ATargetPreview : public ATarget
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATargetPreview();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
