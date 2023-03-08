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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprint")
	float SprintSpeedMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Down Sights")
	float ADSSpeedMultiplier;

	uint8 RequestToStartSprinting : 1;
	uint8 RequestToStartADS : 1;

	virtual float GetMaxSpeed() const override;
	
	// Sprint
	UFUNCTION(BlueprintCallable, Category = "Sprint")
	void StartSprinting();
	UFUNCTION(BlueprintCallable, Category = "Sprint")
	void StopSprinting();

	// Aim Down Sights
	UFUNCTION(BlueprintCallable, Category = "Aim Down Sights")
	void StartAimDownSights();
	UFUNCTION(BlueprintCallable, Category = "Aim Down Sights")
	void StopAimDownSights();
};

