// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#include "AudioAnalyzerDeviceInfo.h"

const std::string AudioAnalyzerDeviceInfo::SYSTEM_DEVICE_NAME = "[system default]";

AudioAnalyzerDeviceInfo::AudioAnalyzerDeviceInfo()
{
	name = SYSTEM_DEVICE_NAME;
	sampleRate = 0;
	numChannels = 0;
	bufferSamples = 0;
	bufferFrames = 0;
	format = PCAudioAnalyzer::AUDIOFORMAT_NONE;
	bitDepth = 0;
}

AudioAnalyzerDeviceInfo::~AudioAnalyzerDeviceInfo()
{
}

const bool AudioAnalyzerDeviceInfo::isDefaultDevice() const
{
	return (name.empty() || (name == SYSTEM_DEVICE_NAME));
}