// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnAreaSpotLight.generated.h"

class USpotLightComponent;

UCLASS()
class BEATSHOT_API ASpawnAreaSpotLight : public AActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintPure)
	USpotLightComponent* GetLightComponent() const;
};
