﻿// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "BSEquipmentDefinition.generated.h"

class UBSAbilitySet;
class UBSEquipmentInstance;

USTRUCT()
struct FBSEquipmentActorToSpawn
{
	GENERATED_BODY()

	FBSEquipmentActorToSpawn()
	{
	}

	UPROPERTY(EditAnywhere, Category="Equipment")
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category="Equipment")
	FName AttachSocket;

	UPROPERTY(EditAnywhere, Category="Equipment")
	FName RootBone;

	UPROPERTY(EditAnywhere, Category="Equipment")
	FTransform AttachTransform;
};

/** Describes a piece of equipment that can be applied to a pawn. */
UCLASS(Blueprintable, Const, Abstract, BlueprintType)
class BEATSHOT_API UBSEquipmentDefinition : public UObject
{
	GENERATED_BODY()

public:
	UBSEquipmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Class default object for the piece of equipment. */
	UPROPERTY(EditDefaultsOnly, Category="Equipment")
	TSubclassOf<UBSEquipmentInstance> InstanceType;

	/** Gameplay ability sets to grant when this piece is equipped. */
	UPROPERTY(EditDefaultsOnly, Category="Equipment")
	TArray<TObjectPtr<const UBSAbilitySet>> AbilitySetsToGrant;

	/** Actors to spawn on the pawn when this piece is equipped. */
	UPROPERTY(EditDefaultsOnly, Category="Equipment")
	TArray<FBSEquipmentActorToSpawn> ActorsToSpawn;

	/** Gameplay tags associated with this piece of equipment. Applied to the Owner's ASC when equipped,
	 *  and removed when unequipped. */
	UPROPERTY(EditDefaultsOnly, Category="Equipment")
	FGameplayTagContainer EquippedTags;
};
