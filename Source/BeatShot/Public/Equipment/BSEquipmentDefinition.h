// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BSEquipmentDefinition.generated.h"

class UBSAbilitySet;
class UBSEquipmentInstance;

UENUM()
enum class EEquipmentType : uint8
{
	None UMETA(DisplayName="None"),
	Gun UMETA(DisplayName="Gun"),
	Knife UMETA(DisplayName="Knife")
};
ENUM_RANGE_BY_FIRST_AND_LAST(EEquipmentType, EEquipmentType::None, EEquipmentType::Knife);

USTRUCT()
struct FBSEquipmentActorToSpawn
{
	GENERATED_BODY()

	FBSEquipmentActorToSpawn()
	{}

	UPROPERTY(EditAnywhere, Category="Equipment")
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category="Equipment")
	FName AttachSocket;

	UPROPERTY(EditAnywhere, Category="Equipment")
	FTransform AttachTransform;
};

/** Describes a piece of equipment that can be applied to a pawn */
UCLASS(Blueprintable, Const, Abstract, BlueprintType)
class BEATSHOT_API UBSEquipmentDefinition : public UObject
{
	GENERATED_BODY()

public:
	UBSEquipmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Class to spawn
	UPROPERTY(EditDefaultsOnly, Category="Equipment")
	TSubclassOf<UBSEquipmentInstance> InstanceType;

	// Type of equipment
	UPROPERTY(EditDefaultsOnly, Category="Equipment")
	EEquipmentType EquipmentType;

	// Gameplay ability sets to grant when this is equipped
	UPROPERTY(EditDefaultsOnly, Category="Equipment")
	TArray<TObjectPtr<const UBSAbilitySet>> AbilitySetsToGrant;

	// Actors to spawn on the pawn when this is equipped
	UPROPERTY(EditDefaultsOnly, Category="Equipment")
	TArray<FBSEquipmentActorToSpawn> ActorsToSpawn;
};
