// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
/** @file AudioAnalyzerCommon.h
* @brief Common Structures
*/
#ifndef AudioAnalyzerCommon_H
#define AudioAnalyzerCommon_H

#include "CoreMinimal.h"

/**
* Plugin Log category
*/
DECLARE_LOG_CATEGORY_EXTERN(LogParallelcubeAudioAnalyzer, Log, All);

namespace PCAudioAnalyzer
{
	/** Enum for audio format */
	enum AUDIOFILE_INFO_FORMAT {
		AUDIOFORMAT_NONE,
		AUDIOFORMAT_INT,
		AUDIOFORMAT_FLOAT
	};
	/**
	* @brief Audio Info structure
	*/
	struct AUDIOFILE_INFO {
		//unsigned int length; /**< File data size */
		AUDIOFILE_INFO_FORMAT audioFormat; /**< Data format (none / int / float) */
		int16_t numChannels; /**< Number of channels */
		uint32_t sampleRate; /**< Samples per second */
		int bitDepth; /**< Bits per sample */
		uint32_t dataChunkSize; /**< Samples raw data size */
		void* dataChunkRawData; /**< Pointer to samples raw data */
	};

	void ma_formatToFormat(const uint8_t ma_format, AUDIOFILE_INFO_FORMAT& audioFormat, int& bitdepth);

	const uint8_t formatToMa_format(const AUDIOFILE_INFO_FORMAT audioFormat, const int bitdepth);
}

#endif