// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSInventoryItemDefinition.h"
#include "InventoryFragment_Equippable.generated.h"

class UBSEquipmentDefinition;

/** An inventory fragment that can be equipped */
UCLASS()
class BEATSHOT_API UInventoryFragment_Equippable : public UBSInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Equipment")
	TSubclassOf<UBSEquipmentDefinition> EquipmentDefinition;
};
