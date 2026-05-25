// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.


#include "Mappings/GameModeAndDifficultyStringMap.h"
#include "BSGameModeConfig/DefiningConfig.h"


UGameModeAndDifficultyStringMap::UGameModeAndDifficultyStringMap()
{
	PopulateEnumTypes(TSet({StaticEnum<EBaseGameMode>(), StaticEnum<EGameModeDifficulty>()}));
}
