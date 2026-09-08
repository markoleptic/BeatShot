// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SaveGames/SaveGameCustomGameMode.h"
#include "BSGameModeConfig/BSConfig.h"
#include "Kismet/GameplayStatics.h"
#include "Utilities/SaveLoadCommon.h"

namespace
{
constexpr int CustomGameModeSlotIndex = 3;
constexpr auto CustomGameModeSlotName = TEXT("CustomGameModesSlot");
}

USaveGameCustomGameMode::USaveGameCustomGameMode()
{
}

USaveGameCustomGameMode* USaveGameCustomGameMode::LoadFromSlot()
{
	if (UGameplayStatics::DoesSaveGameExist(CustomGameModeSlotName, CustomGameModeSlotIndex))
	{
		return Cast<USaveGameCustomGameMode>(
			UGameplayStatics::LoadGameFromSlot(CustomGameModeSlotName, CustomGameModeSlotIndex));
	}
	else
	{
		auto* NewObject = Cast<USaveGameCustomGameMode>(
			UGameplayStatics::CreateSaveGameObject(USaveGameCustomGameMode::StaticClass()));
		NewObject->LastLoadedVersion = Constants::CustomGameModeVersion;
		NewObject->Version = Constants::CustomGameModeVersion;
		return NewObject;
	}
}

void USaveGameCustomGameMode::SaveToSlot()
{
	SaveLoadCommon::SaveToSlot(this, CustomGameModeSlotName, CustomGameModeSlotIndex);
}

void USaveGameCustomGameMode::Serialize(const FStructuredArchive::FRecord Record)
{
	Super::Serialize(Record);
	if (Record.GetArchiveState().IsLoading())
	{
		LastLoadedVersion = Version;
		if (LastLoadedVersion < Constants::CustomGameModeVersion)
		{
			UpgradeCustomGameModes();
			SaveToSlot();
			UE_LOG(LogTemp, Warning, TEXT("Upgraded USaveGameCustomGameMode from Version %d to %d"), LastLoadedVersion,
			       GetVersion());
		}
	}
}

TArray<FBSConfig> USaveGameCustomGameMode::GetCustomGameModes() const
{
	return CustomGameModes;
}

bool USaveGameCustomGameMode::FindCustomGameMode(const FString& GameModeName, FBSConfig& OutConfig) const
{
	for (const FBSConfig& Mode : CustomGameModes)
	{
		if (Mode.DefiningConfig.CustomGameModeName.Equals(GameModeName))
		{
			OutConfig = Mode;
			return true;
		}
	}
	return false;
}

void USaveGameCustomGameMode::SaveCustomGameMode(const FBSConfig& InCustomGameMode)
{
	const int32 Index = CustomGameModes.Find(InCustomGameMode);
	if (CustomGameModes.IsValidIndex(Index))
	{
		CustomGameModes[Index] = InCustomGameMode;
	}
	else
	{
		CustomGameModes.Add(InCustomGameMode);
	}
}

int32 USaveGameCustomGameMode::RemoveCustomGameMode(const FBSConfig& InCustomGameMode)
{
	const int32 NumRemoved = CustomGameModes.Remove(InCustomGameMode);
	CustomGameModes.Shrink();
	return NumRemoved;
}

int32 USaveGameCustomGameMode::RemoveAll()
{
	const int32 NumRemoved = CustomGameModes.Num();
	CustomGameModes.Empty();
	return NumRemoved - CustomGameModes.Num();
}

bool USaveGameCustomGameMode::IsCustomGameMode(const FString& GameModeName) const
{
	for (const FBSConfig& GameMode : CustomGameModes)
	{
		if (GameMode.DefiningConfig.CustomGameModeName.Equals(GameModeName, ESearchCase::IgnoreCase))
		{
			return true;
		}
	}
	return false;
}

void USaveGameCustomGameMode::UpgradeCustomGameModes()
{
	while (Version < Constants::CustomGameModeVersion)
	{
		switch (Version)
		{
		case 0:
			{
				for (FBSConfig& Mode : CustomGameModes)
				{
					UpgradeCustomGameModeToVersion1(Mode);
				}
			}
			break;
		default:
			break;
		}
		Version++;
	}
}

void USaveGameCustomGameMode::UpgradeCustomGameModeToVersion1(FBSConfig& InConfig)
{
	if (InConfig.TargetConfig.BoundsScalingPolicy == EBoundsScalingPolicy::Static)
	{
		InConfig.DynamicSpawnAreaScaling.StartBounds = InConfig.TargetConfig.BoxBounds;
	}
	if (InConfig.TargetConfig.TargetDeactivationConditions.
	             Contains(ETargetDeactivationCondition::Persistent_DEPRECATED))
	{
		InConfig.TargetConfig.TargetDeactivationConditions.Remove(ETargetDeactivationCondition::Persistent_DEPRECATED);
	}
	if (InConfig.TargetConfig.TargetDestructionConditions.Contains(ETargetDestructionCondition::Persistent_DEPRECATED))
	{
		InConfig.TargetConfig.TargetDestructionConditions.Remove(ETargetDestructionCondition::Persistent_DEPRECATED);
		if (InConfig.TargetConfig.TargetDestructionConditions.IsEmpty())
		{
			InConfig.TargetConfig.TargetDestructionConditions.Add(ETargetDestructionCondition::OnHealthReachedZero);
			InConfig.TargetConfig.MaxHealth = -1.f;
		}
	}
}
