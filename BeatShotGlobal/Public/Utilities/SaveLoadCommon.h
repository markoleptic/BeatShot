// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "Kismet/GameplayStatics.h"

BEATSHOTGLOBAL_API DECLARE_LOG_CATEGORY_EXTERN(LogSaveLoadCommon, Log, All);

namespace SaveLoadCommon
{
template <typename T>
T* LoadFromSlot(const FString& InSlotName, const int32 InSlotIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(InSlotName, InSlotIndex))
	{
		if (T* SaveGameObject = Cast<T>(UGameplayStatics::LoadGameFromSlot(InSlotName, InSlotIndex)))
		{
			return SaveGameObject;
		}
	}
	else
	{
		if (T* SaveGameObject = Cast<T>(UGameplayStatics::CreateSaveGameObject(T::StaticClass())))
		{
			return SaveGameObject;
		}
	}
	return nullptr;
}

template <typename T>
bool SaveToSlot(T* SaveGameClass, const FString& InSlotName, const int32 InSlotIndex)
{
	if (UGameplayStatics::SaveGameToSlot(SaveGameClass, InSlotName, InSlotIndex))
	{
		UE_LOG(LogSaveLoadCommon, Warning, TEXT("Save Succeeded"));
		return true;
	}
	return false;
}
}
