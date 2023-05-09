// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BSMoveStepSound.generated.h"
class USoundCue;

/** Move and Step Sounds to play that CharacterMovementComponent uses */
UCLASS(BlueprintType, Blueprintable)
class BEATSHOT_API UBSMoveStepSound : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION()
	TEnumAsByte<EPhysicalSurface> GetSurfaceMaterial() const { return SurfaceMaterial; }

	UFUNCTION()
	TArray<USoundCue*> GetStepLeftSounds() const { return StepLeftSounds; }

	UFUNCTION()
	TArray<USoundCue*> GetStepRightSounds() const { return StepRightSounds; }

	UFUNCTION()
	TArray<USoundCue*> GetSprintLeftSounds() const { return SprintLeftSounds; }

	UFUNCTION()
	TArray<USoundCue*> GetSprintRightSounds() const { return SprintRightSounds; }

	UFUNCTION()
	TArray<USoundCue*> GetJumpSounds() const { return JumpSounds; }

	UFUNCTION()
	TArray<USoundCue*> GetLandSounds() const { return LandSounds; }

	UFUNCTION()
	float GetWalkVolume() const { return WalkVolume; }

	UFUNCTION()
	float GetSprintVolume() const { return SprintVolume; }

private:
	/** The physical material associated with this move step sound */
	UPROPERTY(EditDefaultsOnly, Category = Material)
	TEnumAsByte<EPhysicalSurface> SurfaceMaterial;

	/** The list of sounds to randomly choose from when stepping left */
	UPROPERTY(EditDefaultsOnly, Category = Sounds)
	TArray<USoundCue*> StepLeftSounds;

	/** The list of sounds to randomly choose from when stepping right */
	UPROPERTY(EditDefaultsOnly, Category = Sounds)
	TArray<USoundCue*> StepRightSounds;

	/** The list of sounds to randomly choose from when sprinting left */
	UPROPERTY(EditDefaultsOnly, Category = Sounds)
	TArray<USoundCue*> SprintLeftSounds;

	/** The list of sounds to randomly choose from when sprinting right */
	UPROPERTY(EditDefaultsOnly, Category = Sounds)
	TArray<USoundCue*> SprintRightSounds;

	/** The list of sounds to randomly choose from when jumping */
	UPROPERTY(EditDefaultsOnly, Category = Sounds)
	TArray<USoundCue*> JumpSounds;

	/** The list of sounds to randomly choose from when landing */
	UPROPERTY(EditDefaultsOnly, Category = Sounds)
	TArray<USoundCue*> LandSounds;

	UPROPERTY(EditDefaultsOnly, Category = Volume)
	float WalkVolume = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = Volume)
	float SprintVolume = 0.5f;
};
