// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#ifndef AudioAnalyzerStream_H
#define AudioAnalyzerStream_H

#include "AudioAnalyzerSource.h"
#include "AudioAnalyzerDeviceInfo.h"
#include "AudioAnalyzerCommon.h"
#include "QueueBuffer.h"

#include "miniaudio.h"

#include <atomic>
#include <memory>

/**
* @brief Audio Analyzer Stream Source
*
*/
class AudioAnalyzerStream: public AudioAnalyzerSource
{
public:
	/**
	* Constructor
	*/
	AudioAnalyzerStream();

	/**
	* Destructor
	* This will unload the audio too
	*/
	~AudioAnalyzerStream();

	/**
	* Disable copy constructor
	* @param x	---
	*/
	AudioAnalyzerStream(AudioAnalyzerStream const &x) = delete;

	/**
	* Disable copy assignment
	* @param x	---
	*/
	void operator=(AudioAnalyzerStream const &x) = delete;
	
	/**
	* Initializes the capturer audio info and opens the audio capture (and player) device
	* @param numChannels				Number of channels
	* @param sampleRate					Samples per second
	* @param bitDepth					Bits per sample 
	* @param audioFormat				Audio format
	* @param audioBufferSeconds			Audio buffer size in seconds
	* @param enablePlayback				Enables a playback of the audio
	* @param periodSizeInFrames			Device audio buffer size in frames (power of 2), 0 to use default value
	* @param periodSizeInMilliseconds	Device audio buffer size in milliseconds, 0 to use default value
	* @param periods					Device audio buffer size multiplier
	* @return							The audio initilization result
	*/
	bool initializeAudio(uint16 numChannels, uint32 sampleRate, int bitDepth, uint8 audioFormat, float audioBufferSeconds, bool enablePlayback, 
		uint32 periodSizeInFrames,uint32 periodSizeInMilliseconds, uint32 periods);
	
	/**
	* Closes the audio capturer (and player) device
	*/
	void unloadAudio();

	/**
	* Open the audio stream capture (and start playback)
	* @param enablePlayback		Indicates if we want to playback the captured audio too
	*/
	void open(bool enablePlayback);

	/**
	* Starts the audio stream capture (and player playback)
	* @param data			Buffer of audio samples
	* @param dataSize		Size of the buffer
	*/
	void feed(unsigned char* data, size_t dataSize);

	/**
	* Closes the audio stream capture (and stop player playback)
	*/
	void close();

	/**
	* Returns if the device is currently capturing stream audio
	* @return Device has data into the buffer
	*/
	bool isCapturing() const;

	/**
	* Returns the total capture duration and buffer postion time
	* @param[out] bufferPosTime		Current buffer position (seconds)
	* @return float					Duration in seconds
	*/
	float getPlaybackProgress(float& bufferPosTime);

	/**
	* Returns the total capture duration
	* @return float					Duration in seconds
	*/
	float getPlaybackProgress();

	/**
	* Set default source audio device
	* @param sourceDeviceName		Audio device name
	*/
	void setDefaultAudioDevice(const FString& sourceDeviceName);

	/**
	* Get default source audio device 
	* @param[out] sourceDeviceName	Audio device name
	*/
	void getDefaultAudioDevice(FString& sourceDeviceName);

	/**
	* Returns if only has been loaded the header data
	* @return Has onkly header data
	*/
	bool hasOnlyHeader() const { return false; };

	/**
	* Returns if the buffer needs to be offset
	* @return Needs buffer offset
	*/
	bool needsBufferOffset() const { return false; };

	/**
	* (Const) Returns the player audio device info
	* @return	Audio device info structure
	*/
	const AudioAnalyzerDeviceInfo& getAudioPlayerDeviceInfo() const;


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
	* Device manager
	*/
	AUDIO_DEVICE_MANAGER			_audioDeviceManager;

	/**
	* Playback the input audio
	*/
	bool							_initPlayBackDevice;

	/**
	* Enables the playback device output
	*/
	bool							_enablePlayBack;

	/**
	* Stream is ready to receive data
	*/
	std::atomic<bool>				_isOpen;

	/**
	* Audio buffer (the analysis audio buffer)
	*/
	std::unique_ptr<QueueBuffer>	_audioBuffer;

	/**
	* Capturer buffer raw data to feed the playback device
	*/
	std::unique_ptr<QueueBuffer>	_feedBuffer;

	/**
	* Capturer buffer total size
	*/
	unsigned long long int			_totalBufferSize;

	/**
	* Default player audio device info
	*/
	AudioAnalyzerDeviceInfo			_devicePlayerInfo;

	/**
	* Initializes the capture device and buffers
	* @param periodSizeInFrames			Device audio buffer size in frames (power of 2), 0 to use default value
	* @param periodSizeInMilliseconds	Device audio buffer size in milliseconds, 0 to use default value
	* @param periods					Device audio buffer size multiplier
	* @return							The audio initilization result
	*/
	bool loadAudio(uint32 periodSizeInFrames, uint32 periodSizeInMilliseconds, uint32 periods);

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