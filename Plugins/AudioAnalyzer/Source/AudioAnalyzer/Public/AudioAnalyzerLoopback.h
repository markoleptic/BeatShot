// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#ifndef AudioAnalyzerLoopback_H
#define AudioAnalyzerLoopback_H

#include "AudioAnalyzerSource.h"
#include "AudioAnalyzerDeviceInfo.h"
#include "AudioAnalyzerCommon.h"
#include "QueueBuffer.h"

#include "miniaudio.h"

#include <atomic>
#include <memory>
#include <functional>

/**
* @brief Audio Analyzer Loopback Source
*
* This class will be used to capture the audio in real time from a microphone, can playback the captured audio too
*/
class AudioAnalyzerLoopback: public AudioAnalyzerSource
{
public:
	/**
	* Constructor
	*/
	AudioAnalyzerLoopback();

	/**
	* Destructor
	* This will unload the audio too
	*/
	~AudioAnalyzerLoopback();

	/**
	* Disable copy constructor
	* @param x	---
	*/
	AudioAnalyzerLoopback(AudioAnalyzerLoopback const &x) = delete;

	/**
	* Disable copy assignment
	* @param x	---
	*/
	void operator=(AudioAnalyzerLoopback const &x) = delete;
	
	/**
	* Initializes the capturer audio info and opens the audio capture (and player) device
	* @param numChannels				Number of channels
	* @param sampleRate					Number of samples per second for the capture (and playback)
	* @param bitDepth					Bits per sample
	* @param audioFormat				unsigned, signed, float indicator
	* @param audioBufferSeconds			Audio buffer size in seconds
	* @param periodSizeInFrames			Device audio buffer size in frames (power of 2), 0 to use default value
	* @param periodSizeInMilliseconds	Device audio buffer size in milliseconds, 0 to use default value
	* @param periods					Device audio buffer size multiplier
	* @return							The audio initilization result
	*/
	bool initializeAudio(uint16 numChannels, uint32 sampleRate, int bitDepth, uint8 audioFormat, float audioBufferSeconds, 
						uint32 periodSizeInFrames, uint32 periodSizeInMilliseconds, uint32 periods);
	
	/**
	* Closes the audio capturer (and player) device
	*/
	void unloadAudio();

	/**
	* Starts the audio capturer (and player playback)
	* @param captureDataBroadcast	Broadcast the captured data buffer
	*/
	void start(bool captureDataBroadcast);

	/**
	* Stops the audio capturer (and player playback)
	*/
	void stop();

	/**
	* Returns if the device is currently capturing audio
	* @return Device is playing
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
	* @param capturerDeviceName		Audio device name
	*/
	void setDefaultAudioDevice(const FString& capturerDeviceName);

	/**
	* Get default source audio device 
	* @param[out] capturerDeviceName	Audio device name
	*/
	void getDefaultAudioDevice(FString& capturerDeviceName);

	/**
	* Returns the audio device info
	* @return Audio device info structure
	*/
	const AudioAnalyzerDeviceInfo& getAudioDeviceInfo() const;

	/**
	* Returns if only has been loaded the header data
	* @return Has onkly header data
	*/
	bool hasOnlyHeader() const { return false; };

	/**
	* Returns if the buffer needs to be offset
	* @return Needs buffer offset
	*/
	bool needsBufferOffset() const { return true; };

	/**
	* Registers the called function when the audio capture device has data
	* @param callback		Function callback
	*/
	void registerOnCapturedData(std::function<void(TArray<uint8>)> callback);


private:
	/**
	* @brief Audio device
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
	* Is Capturing Flag
	*/
	std::atomic<bool>						_isCapturing;

	/**
	* Enables captured audio buffer broadcast
	*/
	bool									_enableOnCapturedData;

	/**
	* Playback device manager
	*/
	AUDIO_DEVICE_MANAGER					_captureDeviceManager;

	/**
	* Capturer buffer raw data
	*/
	std::unique_ptr<QueueBuffer>			_audioBuffer;

	/**
	* Capturer buffer total size
	*/
	unsigned long long int					_totalBufferSize;

	/**
	* Default capturer audio device info
	*/
	AudioAnalyzerDeviceInfo					_deviceCapturerInfo;

	/**
	* Capturer Buffer capacity (Seconds) 
	*/
	float									_captureBufferSeconds;

	/**
	* Buffer to be returned by onCapturedData callback
	*/
	TArray<uint8>							_capturedBuffer;

	/**
	* Initializes the capture device and buffers
	* @param periodSizeInFrames			Device audio buffer size in frames (power of 2), 0 to use default value
	* @param periodSizeInMilliseconds	Device audio buffer size in milliseconds, 0 to use default value
	* @param periods					Device audio buffer size multiplier
	* @return	The audio initilization result
	*/
	bool loadAudio(uint32 periodSizeInFrames, uint32 periodSizeInMilliseconds, uint32 periods);

	/**
	* static audio callback function
	* @param pDevice		Audio device pointer
	* @param pOutput		Output audio buffer pointer
	* @param pInput			Input audio buffer pointer
	* @param frameCount		The length of that buffer in frames
	*/
	static void s_captureCallBack(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

	/**
	* non-static audio callback function
	* @param pOutput		Output audio buffer pointer
	* @param pInput			Input audio buffer pointer
	* @param frameCount		The length of that buffer in frames
	*/
	void captureCallBack(void* pOutput, const void* pInput, ma_uint32 frameCount);

	/**
	* Reference to the audio capture function callback
	*/
	std::function<void(const TArray<uint8>&)> onCapturedData;

};

#endif