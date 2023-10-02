// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BeatShot.generated.h"

#define ACTOR_ROLE_FSTRING *(FindObject<UEnum>(nullptr, TEXT("/Script/Engine.ENetRole"), true)->GetNameStringByValue(GetLocalRole()))
#define GET_ACTOR_ROLE_FSTRING(Actor) *(FindObject<UEnum>(nullptr, TEXT("/Script/Engine.ENetRole"), true)->GetNameStringByValue(Actor->GetLocalRole()))


/** Enum representing the different times of the day */
UENUM(BlueprintType)
enum class ETimeOfDay : uint8
{
	Day UMETA(DisplayName="Day"),
	Night UMETA(DisplayName="Night"),
	DayToNight UMETA(DisplayName="DayToNight"),
	NightToDay UMETA(DisplayName="NightToDay"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(ETimeOfDay, ETimeOfDay::Day, ETimeOfDay::NightToDay);

/** Enum representing the ways in which the MovablePlatform can move */
UENUM(BlueprintType)
enum class EPlatformTransitionType : uint8
{
	None UMETA(DisplayName="None"),
	MoveUpByInteract UMETA(DisplayName="MoveUpByInteract"),
	MoveDownByInteract UMETA(DisplayName="MoveDownByInteract"),
	MoveDownByStepOff UMETA(DisplayName="MoveDownByStepOff")};
ENUM_RANGE_BY_FIRST_AND_LAST(EPlatformTransitionType, EPlatformTransitionType::None, EPlatformTransitionType::MoveDownByStepOff);

/** A struct representing two consecutively spawned targets, used to keep track of the reward associated between two points */
USTRUCT()
struct FTargetPair
{
	GENERATED_BODY()

	/** The SpawnAreas index of the target spawned before Current */
	int32 Previous;

	/** The SpawnAreas index of the target spawned after Previous */
	int32 Current;

private:

	/** The reward for spawning a target at Previous and then spawning a target at Current */
	float Reward;

public:
	
	FTargetPair()
	{
		Previous = INDEX_NONE;
		Current = INDEX_NONE;
		Reward = 0.f;
	}

	FTargetPair(const int32 PreviousPointIndex, const int32 CurrentPointIndex)
	{
		Previous = PreviousPointIndex;
		Current = CurrentPointIndex;
		Reward = 0.f;
	}

	FORCEINLINE bool operator ==(const FTargetPair& Other) const
	{
		return Previous == Other.Previous && Current == Other.Current;
	}

	/** Sets the reward for this TargetPair */
	void SetReward(const float InReward)
	{
		Reward = InReward;
	}

	/** Returns the reward for this TargetPair */
	float GetReward() const { return Reward; }
};

/** A struct representing the inputs for a Reinforcement Learning Algorithm */
USTRUCT()
struct FAlgoInput
{
	GENERATED_BODY()

	int32 StateIndex;
	int32 ActionIndex;
	int32 StateIndex_2;
	int32 ActionIndex_2;
	float Reward;

	FAlgoInput()
	{
		StateIndex = -1;
		ActionIndex = -1;
		StateIndex_2 = -1;
		ActionIndex_2 = -1;
		Reward = 1;
	}

	FAlgoInput(const int32 InStateIndex, const int32 InActionIndex, const int32 InStateIndex_2, const int32 InActionIndex_2, const float InReward)
	{
		StateIndex = InStateIndex;
		ActionIndex = InActionIndex;
		StateIndex_2 = InStateIndex_2;
		ActionIndex_2 = InActionIndex_2;
		Reward = InReward;
	}
};

/** A struct each each element represents one QTable index mapping to multiple SpawnCounter indices */
USTRUCT()
struct FQTableIndex
{
	GENERATED_BODY()

	int32 QTableIndex;
	TArray<int32> SpawnAreasIndices;

	FQTableIndex()
	{
		QTableIndex = INDEX_NONE;
		SpawnAreasIndices = TArray<int32>();
	}
	FQTableIndex(const int32 InQTableIndex)
	{
		QTableIndex = InQTableIndex;
		SpawnAreasIndices = TArray<int32>();
	}

	FORCEINLINE bool operator ==(const FQTableIndex& Other) const
	{
		if (Other.QTableIndex == QTableIndex)
		{
			return true;
		}
		return false;
	}
};


DECLARE_DELEGATE_RetVal_OneParam(bool, FOnSpawnAreaValidityRequest, const int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnQTableUpdate, const TArray<float>& UpdatedQTable);
DECLARE_DELEGATE(FOnShotFired);


/** Struct containing info about a target that is broadcast when a target takes damage or the the DamageableWindow timer expires */
USTRUCT()
struct FTargetDamageEvent
{
	GENERATED_BODY()

	/** The time the target was alive for before the damage event, or INDEX_NONE if expired */
	float TimeAlive;

	/** The health attribute's NewValue */
	float CurrentHealth;

	/** The absolute value between the health attribute's NewValue and OldValue */
	float DamageDelta;

	/** The total possible damage if tracking */
	float TotalPossibleDamage;

	/** The transform of the target */
	FTransform Transform;
	
	/** A unique ID for the target, used to find the target when it comes time to free the blocked points of a target */
	FGuid Guid;
	
	FTargetDamageEvent()
	{
		TimeAlive = INDEX_NONE;
		DamageDelta = 0.f;
		CurrentHealth = 0.f;
		TotalPossibleDamage = 0.f;
		Transform = FTransform();
	}

	FTargetDamageEvent(const float InTimeAlive, const float InCurrentHealth, const FTransform& InTransform, const FGuid& InGuid, const float InDamageDelta = 0.f)
	{
		TimeAlive = InTimeAlive;
		DamageDelta = InDamageDelta;
		CurrentHealth = InCurrentHealth;
		TotalPossibleDamage = 0.f;
		Transform = InTransform;
		Guid = InGuid;
	}

	float GetDamageDelta(const float OldValue, const float NewValue) const
	{
		return abs(OldValue - NewValue);
	}

	FORCEINLINE bool operator ==(const FTargetDamageEvent& Other) const
	{
		if (Guid == Other.Guid)
		{
			return true;
		}
		return false;
	}
};

class FBeatShot : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
