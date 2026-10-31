// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "Score/PlayerScore.h"
#include "Score/AccuracyData.h"
#include "BSGameModeConfig/AudioConfig.h"

DEFINE_LOG_CATEGORY(LogAccuracyData);

void FPlayerScore::PopulateAudioInfo(const FBS_AudioConfig& AudioConfig)
{
	SongMetadata = AudioConfig.SongMetadata;
}