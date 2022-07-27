// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
/** @file AudioAnalyzerCommon.h
* @brief Common Structures
*/
#ifndef AudioAnalyzerDeviceInfo_H
#define AudioAnalyzerDeviceInfo_H

#include "AudioAnalyzerCommon.h"

#include <string>

/**
* @brief Audio Device Info
*
* Basic buffer history manager implementation using a deque
*/
class AudioAnalyzerDeviceInfo
{
public: 
	/**
	* Default device system name
	*/
	static const std::string SYSTEM_DEVICE_NAME;

	/**
	* Device system name
	*/
	std::string name;

	/**
	* Sample rate
	*/
	uint32_t sampleRate;

	/**
	* Number of channels
	*/
	uint32_t numChannels;

	/**
	* Size of the audio buffer
	*/
	uint16_t bufferSamples;

	/**
	* Size of the playback audio buffer
	*/
	uint32_t bufferFrames;

	/**
	* Audio format
	*/
	PCAudioAnalyzer::AUDIOFILE_INFO_FORMAT format;

	/**
	* Audio bit depth
	*/
	int bitDepth;

	/**
	* Constructor
	*/
	AudioAnalyzerDeviceInfo();

	/**
	* Destructor
	*/
	~AudioAnalyzerDeviceInfo();

	/**
	* Disable copy constructor
	* @param x	---
	*/
	AudioAnalyzerDeviceInfo(AudioAnalyzerDeviceInfo const& x) = delete;

	/**
	* Disable copy assignment
	* @param x	---
	*/
	void operator=(AudioAnalyzerDeviceInfo const& x) = delete;

	/**
	* Check if the device has been set manually
	* @return Is the system audio device
	*/
	const bool isDefaultDevice() const;

private:
};
#endif