// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BeatShot.generated.h"

#define ACTOR_ROLE_FSTRING *(FindObject<UEnum>(nullptr, TEXT("/Script/Engine.ENetRole"), true)->GetNameStringByValue(GetLocalRole()))
#define GET_ACTOR_ROLE_FSTRING(Actor) *(FindObject<UEnum>(nullptr, TEXT("/Script/Engine.ENetRole"), true)->GetNameStringByValue(Actor->GetLocalRole()))

DECLARE_MULTICAST_DELEGATE_OneParam(FOnQTableUpdate, const TArray<float>& UpdatedQTable);
DECLARE_DELEGATE(FOnShotFired);

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
	None UMETA(DisplayName="MoveUpByInteract"),
	MoveUpByInteract UMETA(DisplayName="MoveUpByInteract"),
	MoveDownByInteract UMETA(DisplayName="MoveDownByInteract"),
	MoveDownByStepOff UMETA(DisplayName="MoveDownByStepOff")};

ENUM_RANGE_BY_FIRST_AND_LAST(EPlatformTransitionType, EPlatformTransitionType::None, EPlatformTransitionType::MoveDownByStepOff);

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
