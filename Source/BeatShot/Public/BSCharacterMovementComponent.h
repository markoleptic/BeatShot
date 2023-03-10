// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.
// Credit to Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BSCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class BEATSHOT_API UBSCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UBSCharacterMovementComponent();

	void SetSprintSpeedMultiplier(float NewSpringSpeedMultiplier);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprint")
	float SprintSpeedMultiplier;

	uint8 bRequestToStartSprinting : 1;

	virtual float GetMaxSpeed() const override;

	// Sprint
	UFUNCTION(BlueprintCallable, Category = "Sprint")
	void StartSprinting();
	UFUNCTION(BlueprintCallable, Category = "Sprint")
	void StopSprinting();
};
