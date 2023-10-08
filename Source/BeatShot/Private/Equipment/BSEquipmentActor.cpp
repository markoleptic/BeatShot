// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Equipment/BSEquipmentActor.h"


ABSEquipmentActor::ABSEquipmentActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABSEquipmentActor::BeginPlay()
{
	Super::BeginPlay();
}

void ABSEquipmentActor::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer.AppendTags(GameplayTags);
}

bool ABSEquipmentActor::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	return IGameplayTagAssetInterface::HasMatchingGameplayTag(TagToCheck);
}

bool ABSEquipmentActor::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return IGameplayTagAssetInterface::HasAllMatchingGameplayTags(TagContainer);
}

bool ABSEquipmentActor::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return IGameplayTagAssetInterface::HasAnyMatchingGameplayTags(TagContainer);
}

void ABSEquipmentActor::AddGameplayTag(const FGameplayTag& Tag)
{
	GameplayTags.AddTag(Tag);
}

void ABSEquipmentActor::RemoveGameplayTag(const FGameplayTag& Tag)
{
	GameplayTags.RemoveTag(Tag);
}
