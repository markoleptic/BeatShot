// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "BSAssetManager.h"

#include "BSGameplayTags.h"

UBSAssetManager::UBSAssetManager()
{
}

UBSAssetManager& UBSAssetManager::Get()
{
	check(GEngine);

	UBSAssetManager* MyAssetManager = Cast<UBSAssetManager>(GEngine->AssetManager);
	return *MyAssetManager;
}

void UBSAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	//Load Native Tags
	FBSGameplayTags::InitializeTags();
}
