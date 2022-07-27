// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#include "AudioAnalyzerCommon.h"

DEFINE_LOG_CATEGORY(LogParallelcubeAudioAnalyzer);


void PCAudioAnalyzer::ma_formatToFormat(const uint8_t ma_format, AUDIOFILE_INFO_FORMAT& audioFormat, int& bitdepth)
{
	//ma_format_unknown = 0,     
	//	ma_format_u8 = 1,
	//	ma_format_s16 = 2,    
	//	ma_format_s24 = 3,     
	//	ma_format_s32 = 4,
	//	ma_format_f32 = 5,
	//	ma_format_count

	//format
	// 0 unsigned int
	// 1 signed int
	// 2 float
	//

	switch (ma_format)
	{
	case 1: audioFormat = AUDIOFILE_INFO_FORMAT::AUDIOFORMAT_NONE; bitdepth = 8; break;
	case 2: audioFormat = AUDIOFILE_INFO_FORMAT::AUDIOFORMAT_INT; bitdepth = 16; break;
	case 3: audioFormat = AUDIOFILE_INFO_FORMAT::AUDIOFORMAT_INT; bitdepth = 24; break;
	case 4: audioFormat = AUDIOFILE_INFO_FORMAT::AUDIOFORMAT_INT; bitdepth = 32; break;
	case 5: audioFormat = AUDIOFILE_INFO_FORMAT::AUDIOFORMAT_FLOAT; bitdepth = 32; break;
	default: audioFormat = AUDIOFILE_INFO_FORMAT::AUDIOFORMAT_NONE; bitdepth = 0;
	}
}

const uint8_t PCAudioAnalyzer::formatToMa_format(const AUDIOFILE_INFO_FORMAT audioFormat, const int bitdepth)
{
	if (audioFormat == AUDIOFILE_INFO_FORMAT::AUDIOFORMAT_INT)
	{
		if (bitdepth == 16)
		{
			return 2;
		}

		if (bitdepth == 24)
		{
			return 3;
		}

		if (bitdepth == 32)
		{
			return 4;
		}
	}
	else if (audioFormat == AUDIOFILE_INFO_FORMAT::AUDIOFORMAT_FLOAT)
	{
		return 5;
	}
	else
	{
		if (bitdepth == 8)
		{
			return 1;
		}
	}

	return 0;
}