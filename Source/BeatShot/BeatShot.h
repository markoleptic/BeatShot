// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BeatShot.generated.h"

class FBeatShot : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

#define ACTOR_ROLE_FSTRING *(FindObject<UEnum>(nullptr, TEXT("/Script/Engine.ENetRole"), true)->GetNameStringByValue(GetLocalRole()))
#define GET_ACTOR_ROLE_FSTRING(Actor) *(FindObject<UEnum>(nullptr, TEXT("/Script/Engine.ENetRole"), true)->GetNameStringByValue(Actor->GetLocalRole()))

/** A struct representing the space in the grid that a recently spawned target occupies */
USTRUCT()
struct FRecentTarget
{
	GENERATED_BODY()

	/** An array of points that were inside the SpawnBox and inside the target */
	TArray<FVector> OverlappingPoints;

	/** The location of the center of the target */
	FVector CenterVector;
	
	/** A unique ID for the target, used to find the target when it comes time to free the blocked points of a target */
	FGuid TargetGuid;

	/** The scale of the target relative to the world */
	float TargetScale;

	FRecentTarget()
	{
		CenterVector = FVector::ZeroVector;
		TargetScale = 0.f;
	}

	explicit FRecentTarget(const FGuid Guid)
	{
		CenterVector = FVector::ZeroVector;
		TargetGuid = Guid;
		TargetScale = 0.f;
	}

	FRecentTarget(const FGuid NewTargetGuid, const TArray<FVector> Points, const float NewTargetScale, const FVector NewCenter)
	{
		TargetGuid = NewTargetGuid;
		OverlappingPoints = Points;
		TargetScale = NewTargetScale;
		CenterVector = NewCenter;
	}

	FORCEINLINE bool operator == (const FRecentTarget& Other) const
	{
		if (TargetGuid == Other.TargetGuid)
		{
			return true;
		}
		return false;
	}
};

/** A struct representing a point in a 2D grid with information about that point */
USTRUCT()
struct FVectorCounter
{
	GENERATED_BODY()

	/** Unscaled, world spawn location point */
	FVector Point;
	
	/** The total number of target spawns at this point */
	int32 TotalSpawns;

	/** The total number of target hits by player at this point */
	int32 TotalHits;

	FVectorCounter()
	{
		Point = FVector();
		TotalSpawns = 0;
		TotalHits = 0;
	}

	FVectorCounter(const FVector NewPoint)
	{
		Point = NewPoint;
		TotalSpawns = -1;
		TotalHits = 0;
	}

	FORCEINLINE bool operator == (const FVectorCounter& Other) const
	{
		if (Point.Equals(Other.Point))
		{
			return true;
		}
		return false;
	}

	FORCEINLINE bool operator < (const FVectorCounter& Other) const
	{
		if (Point.Z > Other.Point.Z)
		{
			return true;
		}
		if (Point.Z == Other.Point.Z && Point.Y < Other.Point.Y)
		{
			return true;
		}
		return false;
	}
};

UENUM(BlueprintType)
enum class EMovementType : uint8
{
	Sprinting UMETA(DisplayName="Sprinting"),
	Walking UMETA(DisplayName="Walking"),
	Crouching UMETA(DisplayName="Crouching")
};

/** Enum representing the different times of the day */
UENUM(BlueprintType)
enum class ETimeOfDay : uint8
{
	Day UMETA(DisplayName="Day"),
	Night UMETA(DisplayName="Night"),
	DayToNight UMETA(DisplayName="DayToNight"),
	NightToDay UMETA(DisplayName="NightToDay"),
};

/** Used to store movement properties for different movement types */
USTRUCT(BlueprintType)
struct FMovementTypeVariables
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Movement Variables")
	float MaxAcceleration;

	UPROPERTY(EditDefaultsOnly, Category = "Movement Variables")
	float BreakingDecelerationWalking;

	UPROPERTY(EditDefaultsOnly, Category = "Movement Variables")
	float GroundFriction;

	UPROPERTY(EditDefaultsOnly, Category = "Movement Variables")
	float MaxWalkSpeed;

	FMovementTypeVariables()
	{
		MaxAcceleration = 0.f;
		BreakingDecelerationWalking = 0.f;
		GroundFriction = 0.f;
		MaxWalkSpeed = 0.f;
	}
};

UENUM(BlueprintType)
enum class EPlatformTransitionType : uint8
{
	None UMETA(DisplayName="MoveUpByInteract"),
	MoveUpByInteract UMETA(DisplayName="MoveUpByInteract"),
	MoveDownByInteract UMETA(DisplayName="MoveDownByInteract"),
	MoveDownByStepOff UMETA(DisplayName="MoveDownByStepOff")
};
