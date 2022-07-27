// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#ifndef AudioAnalyzerPlayerUnit_H
#define AudioAnalyzerPlayerUnit_H


#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>

#include <functional>

class AudioAnalyzerPlayer;
/**
* @brief Audio Analyzer Player Unit
*
* This class will be used to load audio from a disk file and interact with it using the most basic player options (play/stop/pause)
*/
class AudioAnalyzerPlayerUnit
{
public:

	/**
	* Constructor
	*/
	AudioAnalyzerPlayerUnit(): _remainingLoops(0), _startTime(0.0)
	{
	};

	/**
	* Constructor
	* @param parentPlayer	Pointer to the parent class
	*/
	AudioAnalyzerPlayerUnit(AudioAnalyzerPlayer* parentPlayer):_parentPlayer(parentPlayer), _remainingLoops(0), _startTime(0.0)
	{
	};

	/**
	* Destructor
	*/
	virtual ~AudioAnalyzerPlayerUnit() 
	{
		_parentPlayer = NULL; 
	};

	/**
	* Disable copy constructor
	* @param x	---
	*/
	AudioAnalyzerPlayerUnit(AudioAnalyzerPlayerUnit const &x) = delete;

	/**
	* Disable copy assignment
	* @param x	---
	*/
	void operator=(AudioAnalyzerPlayerUnit const &x) = delete;

	/**
	* Initializes the player device
	* @param periodSizeInFrames			Device audio buffer size in frames (power of 2), 0 to use default value
	* @param periodSizeInMilliseconds	Device audio buffer size in milliseconds, 0 to use default value
	* @param periods					Device audio buffer size multiplier
	* @return							Audio initilization execution result
	*/
	virtual bool loadAudio(uint32 periodSizeInFrames, uint32 periodSizeInMilliseconds, uint32 periods) = 0;

	/**
	* Closes audio player device
	*/
	virtual void unloadAudio() = 0;

	/**
	* Starts audio playback
	* @param maxLoops	Number of loops
	* @param startTime	Starting position (seconds)
	*/
	virtual void play(int maxLoops, float startTime) = 0;

	/**
	* Stops audio playback
	*/
	virtual void stop() = 0;

	/**
	* Pauses audio playback if the device is in playing state
	* UnPauses audio playback if the device has been paused previously
	* @param forcePause		Set the pause to this state
	*/
	virtual void pause(bool forcePause = true) = 0;

	/**
	* Returns if the device is currently playing audio
	* @return Device is playing
	*/
	virtual bool isPlaying() const = 0;

	/**
	* Returns if the device is currently paused
	* @return Device is paused
	*/
	virtual bool isPaused() const = 0;
	
	/**
	* Returns the playback progress and buffer postion time
	* @param[out] bufferPosTime		Buffer position (same as below value)
	* @return float					Progress in seconds
	*/
	virtual float getPlaybackProgress(float& bufferPosTime) = 0;

	/**
	* Returns the playback progress
	* @return float					Progress in seconds
	*/
	virtual float getPlaybackProgress() = 0;

	/**
	* Sets the playback progress
	* @param time		New playback position in seconds
	*/
	virtual void setPlaybackProgress(float time) = 0;

	/**
	* Returns the playback volume
	* @return volume in range 0.0-1.0
	*/
	virtual float getPlaybackVolume() = 0;

	/**
	* Sets the playback volume
	* @param volume		New volume in range 0.0-1.0
	*/
	virtual void setPlaybackVolume(float volume) = 0;
	
	/**
	* Returns the total audio duration
	* @return Duration in seconds
	*/
	virtual float getTotalDuration() const = 0;

	/**
	* Sets a new buffer audio size
	* @param seconds	Buffer size in seconds
	*/
	virtual void setAudioBufferSize(float seconds) = 0;

	/**
	* Get remaining loops
	* @return remainingLoops
	*/
	const int getRemainingLoops() const {return _remainingLoops; };

protected:

	/**
	* Parent player class pointer
	*/
	AudioAnalyzerPlayer* _parentPlayer;

	/**
	*	Current Loop 
	*/
	int _remainingLoops;

	/**
	*	Start time for loops
	*/
	float _startTime;
};

#endif