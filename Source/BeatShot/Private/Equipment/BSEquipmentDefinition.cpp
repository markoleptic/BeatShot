// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Equipment/BSEquipmentDefinition.h"
#include "Equipment/BSEquipmentInstance.h"

UBSEquipmentDefinition::UBSEquipmentDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstanceType = UBSEquipmentInstance::StaticClass();
}

