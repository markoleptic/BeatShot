// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "Score/PlayerScore.h"
#include "BSGameModeConfig/AudioConfig.h"

void FPlayerScore::PopulateAudioInfo(const FBS_AudioConfig& AudioConfig)
{
	SongMetadata = AudioConfig.SongMetadata;
}