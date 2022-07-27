// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#ifndef AudioAnalyzerPlayerUnitFile_H
#define AudioAnalyzerPlayerUnitFile_H

#include "AudioAnalyzerPlayerUnit.h"
#include "AudioAnalyzerSource.h"
#include "AudioAnalyzerCommon.h"

#include "miniaudio.h"

#include <atomic>

class AudioAnalyzerPlayer;

/**
* @brief Audio Analyzer Player Unit File
*
* This class will be used to load audio from a disk file and interact with it using the most basic player options (play/stop/pause)
*/
class AudioAnalyzerPlayerUnitFile : public AudioAnalyzerPlayerUnit
{
public:

	/**
	* Constructor
	*/
	AudioAnalyzerPlayerUnitFile();

	/**
	* Constructor
	*/
	AudioAnalyzerPlayerUnitFile(AudioAnalyzerPlayer*);

	/**
	* Destructor
	* This will unload the loaded audio too
	*/
	~AudioAnalyzerPlayerUnitFile();

	/**
	* Disable copy constructor
	* @param x	---
	*/
	AudioAnalyzerPlayerUnitFile(AudioAnalyzerPlayerUnitFile const &x) = delete;

	/**
	* Disable copy assignment
	* @param x	---
	*/
	void operator=(AudioAnalyzerPlayerUnitFile const &x) = delete;

	/**
	* Initializes the player device
	* @param periodSizeInFrames			Device audio buffer size in frames (power of 2), 0 to use default value
	* @param periodSizeInMilliseconds	Device audio buffer size in milliseconds, 0 to use default value
	* @param periods					Device audio buffer size multiplier
	* @return							Audio initilization execution result
	*/
	bool loadAudio(uint32 periodSizeInFrames, uint32 periodSizeInMilliseconds, uint32 periods);

	/**
	* Closes audio player device
	*/
	void unloadAudio();

	/**
	* Starts audio playback
	* @param maxLoops	Number of loops
	* @param startTime	Starting position(seconds)
	*/
	void play(int maxLoops, float startTime);

	/**
	* Stops audio playback
	*/
	void stop();

	/**
	* Pauses audio playback if the device is in playing state
	* UnPauses audio playback if the device has been paused previously
	* @param forcePause		Set the pause to this state
	*/
	void pause(bool forcePause = true);

	/**
	* Returns if the device is currently playing audio
	* @return Device is playing
	*/
	bool isPlaying() const;

	/**
	* Returns if the device is currently paused
	* @return Device is paused
	*/
	bool isPaused() const;
	
	/**
	* Returns the playback progress and buffer postion time
	* @param[out] bufferPosTime		Buffer position (same as below value)
	* @return float					Progress in seconds
	*/
	float getPlaybackProgress(float& bufferPosTime);

	/**
	* Returns the playback progress
	* @return float					Progress in seconds
	*/
	float getPlaybackProgress();

	/**
	* Sets the playback progress
	* @param time		New playback position in seconds
	*/
	void setPlaybackProgress(float time);

	/**
	* Returns the playback volume
	* @return volume in range 0.0-1.0
	*/
	float getPlaybackVolume();

	/**
	* Sets the playback volume
	* @param volume		New volume in range 0.0-1.0
	*/
	void setPlaybackVolume(float volume);
	
	/**
	* Returns the total audio duration
	* @return Duration in seconds
	*/
	float getTotalDuration() const;

	/**
	* Sets a new buffer audio size
	* @param seconds	Buffer size in seconds
	*/
	void setAudioBufferSize(float seconds);
	
private:

	/**
	* @brief Audio device Manager
	*/
	struct AUDIO_DEVICE_MANAGER
	{
		/**
		* Indicates if the audio context has been initializated
		*/
		std::atomic<bool>	hasContext;

		/**
		* Audio context to select device
		*/
		ma_context			context;

		/**
		* Audio device config
		*/
		ma_device_config	deviceConfig;

		/**
		* Indicates if the audio device has been initializated
		*/
		std::atomic<bool>	hasDevice;

		/**
		* Audio device
		*/
		ma_device			device;

		/**
		* Default constructor
		*/
		AUDIO_DEVICE_MANAGER();
	};

	/**
	* Variable used to store the playing device state
	*/
	std::atomic<bool>		_isPlaying;

	/**
	* Variable used to store the pause device state
	*/
	std::atomic<bool>		_isPaused;

	/**
	* Device manager
	*/
	AUDIO_DEVICE_MANAGER	_audioDeviceManager;

	/**
	* Current pointer to the buffer audio raw data
	*/
	uint8*					_audioBufferPtr;

	/**
	* Buffer audio data size
	*/
	uint32					_audioBufferSize;

	/**
	* Playback volume
	*/
	float					_audioVolume;

	/**
	* static audio callback function
	* @param pDevice		Audio device pointer
	* @param pOutput		Output audio buffer pointer
	* @param pInput			Input audio buffer pointer
	* @param frameCount		The length of that buffer in frames
	*/
	static void s_playbackCallBack(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

	/**
	* non-static audio callback function
	* @param pOutput		Output audio buffer pointer
	* @param pInput			Input audio buffer pointer
	* @param frameCount		The length of that buffer in frames
	*/
	void playbackCallBack(void* pOutput, const void* pInput, ma_uint32 frameCount);

};

#endif