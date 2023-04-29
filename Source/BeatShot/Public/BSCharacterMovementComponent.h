// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.
// Credit to Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "Character/PBPlayerMovement.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BSCharacterMovementComponent.generated.h"

/** Information about the ground under the character.  It only gets updated as needed. */
USTRUCT(BlueprintType)
struct FCharacterGroundInfo
{
	GENERATED_BODY()

	FCharacterGroundInfo()
		: LastUpdateFrame(0)
		, GroundDistance(0.0f)
	{}

	uint64 LastUpdateFrame;

	UPROPERTY(BlueprintReadOnly)
	FHitResult GroundHitResult;

	UPROPERTY(BlueprintReadOnly)
	float GroundDistance;
};


UCLASS()
class BEATSHOT_API UBSCharacterMovementComponent : public UPBPlayerMovement
{
	GENERATED_BODY()

public:
	UBSCharacterMovementComponent();

	void SetSprintSpeedMultiplier(float NewSpringSpeedMultiplier);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprint")
	float SprintSpeedMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	float GroundTraceDistance;

	virtual float GetMaxSpeed() const override;

	// Returns the current ground info.  Calling this will update the ground info if it's out of date.
	UFUNCTION(BlueprintCallable, Category = "CharacterMovement")
	const FCharacterGroundInfo& GetGroundInfo();

protected:

	// Cached ground info for the character.  Do not access this directly!  It's only updated when accessed via GetGroundInfo().
	FCharacterGroundInfo CachedGroundInfo;
};
