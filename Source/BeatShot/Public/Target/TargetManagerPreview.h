// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TargetManager.h"
#include "GameFramework/Actor.h"
#include "TargetManagerPreview.generated.h"

UCLASS()
class BEATSHOT_API ATargetManagerPreview : public ATargetManager
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATargetManagerPreview();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
