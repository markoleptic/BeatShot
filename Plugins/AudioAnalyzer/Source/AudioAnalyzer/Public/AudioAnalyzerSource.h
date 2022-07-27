// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#ifndef AudioAnalyzerSource_H
#define AudioAnalyzerSource_H

#include "AudioAnalyzerCommon.h"
#include <mutex>
#include <memory>

/**
* @brief Audio Analyzer Source
*
* Common base to all audio source types
*/
class AudioAnalyzerSource
{
public:
	/**
	* Constructor
	*/
	AudioAnalyzerSource()
	{
	};

	/**
	* Destructor
	*/
	virtual ~AudioAnalyzerSource() {};

	/**
	* Disable copy constructor
	* @param x	---
	*/
	AudioAnalyzerSource(AudioAnalyzerSource const &x) = delete;

	/**
	* Disable copy assignment
	* @param x	---
	*/
	void operator=(AudioAnalyzerSource const &x) = delete;

	/**
	* Returns the audio info structure pointer
	* @return Pointer to audio info structure
	*/
	virtual inline const PCAudioAnalyzer::AUDIOFILE_INFO* getAudioInfo() { return _audioInfo.get(); };

	/**
	* Update raw data pointer
	* @param dataPtr	Pointer to raw data
	*/
	virtual void setRawDataPtr(uint8* dataPtr) { _audioInfo->dataChunkRawData = dataPtr; };

	/**
	* Returns a mutex to restrict access to the source raw data
	* @return Mutex reference 
	*/
	std::mutex& getSourceMutex() { return _sourceMutex; }

	/**
	* Returns the playback progress in seconds
	* @param bufferPosTime		Buffer position
	* @return					Playback progress in seconds
	*/
	virtual float getPlaybackProgress(float& bufferPosTime) = 0;

	/**
	* Returns if only has been loaded the header data
	* @return Has only header data
	*/
	virtual bool hasOnlyHeader() const = 0;

	/**
	* Returns if the buffer needs to be offset
	* @return Needs buffer offset 
	*/
	virtual bool needsBufferOffset() const = 0;

protected:
	/**
	* Audio info structure
	*/
	std::unique_ptr<PCAudioAnalyzer::AUDIOFILE_INFO> _audioInfo;

	/**
	* Mutex for raw data
	*/
	std::mutex						_sourceMutex;
};

#endif