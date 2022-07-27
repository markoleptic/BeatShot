// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#ifndef AudioAnalyzerCapturer_H
#define AudioAnalyzerCapturer_H

#include "AudioAnalyzerSource.h"
#include "AudioAnalyzerDeviceInfo.h"
#include "AudioAnalyzerCommon.h"
#include "QueueBuffer.h"

#include "miniaudio.h"

#include <atomic>
#include <memory>
#include <functional>

/**
* @brief Audio Analyzer Capturer Source
*
* This class will be used to capture the audio in real time from a microphone, can playback the captured audio too
*/
class AudioAnalyzerCapturer: public AudioAnalyzerSource
{
public:
	/**
	* Constructor
	*/
	AudioAnalyzerCapturer();

	/**
	* Destructor
	* This will unload the audio too
	*/
	~AudioAnalyzerCapturer();

	/**
	* Disable copy constructor
	* @param x	---
	*/
	AudioAnalyzerCapturer(AudioAnalyzerCapturer const &x) = delete;

	/**
	* Disable copy assignment
	* @param x	---
	*/
	void operator=(AudioAnalyzerCapturer const &x) = delete;
	
	/**
	* Initializes the capturer audio info and opens the audio capture (and player) device
	* @param sampleRate					Number of samples per second for the capture (and playback)
	* @param bitDepth					Bits per sample
	* @param audioFormat				unsigned, signed, float indicator
	* @param audioBufferSeconds			Audio buffer size in seconds
	* @param initPlaybackDevice			Intializes a playback device
	* @param periodSizeInFrames			Device audio buffer size in frames (power of 2), 0 to use default value
	* @param periodSizeInMilliseconds	Device audio buffer size in milliseconds, 0 to use default value
	* @param periods					Device audio buffer size multiplier
	* @return							The audio initilization result
	*/
	bool initializeAudio(uint32 sampleRate, int bitDepth, uint8 audioFormat, float audioBufferSeconds, bool initPlaybackDevice, 
						uint32 periodSizeInFrames,	uint32 periodSizeInMilliseconds, uint32 periods);
	
	/**
	* Closes the audio capturer (and player) device
	*/
	void unloadAudio();

	/**
	* Starts the audio capturer (and player playback)
	* @param playback						Indicates if we want to playback the captured audio too 
	* @param enableCapturedDataBroadcast	Indicates if we want to broadcast the audio capture data
	*/
	void start(bool playback, bool enableCapturedDataBroadcast);

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
	* Get input audio device names list
	* @param[out] audioDeviceNames		Device names list availables for input
	*/
	static void getInputAudioDeviceNames(TArray<FString>& audioDeviceNames);

	/**
	* Set default capturer and player device
	* @param capturerDeviceName		New default capturer audio device name
	* @param playerDeviceName		New default player audio device name
	*/
	void setDefaultAudioDevices(const FString& capturerDeviceName, const FString& playerDeviceName);

	/**
	* Get default capturer and player device
	* @param[out] capturerDeviceName	Default audio capturer device name
	* @param[out] playerDeviceName		Default audio player device name
	*/
	void getDefaultAudioDevices(FString& capturerDeviceName, FString& playerDeviceName);

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
	* (Const) Returns the capturer audio device info
	* @return audio device info structure
	*/
	const AudioAnalyzerDeviceInfo& getAudioCapturerDeviceInfo() const;

	/**
	* (Const) Returns the player audio device info
	* @return audio device info structure
	*/
	const AudioAnalyzerDeviceInfo& getAudioPlayerDeviceInfo() const;

	/**
	* Returns the volume of the captured audio
	* @return volume in range 0.0-1.0
	*/
	float getCaptureVolume();

	/**
	* Sets the volume of the captured audio
	* @param volume		New volume in range 0.0-1.0
	*/
	void setCaptureVolume(float volume);

	/**
	* Returns the volume of the playback of the captured audio
	* @return volume in range 0.0-1.0
	*/
	float getPlaybackVolume();

	/**
	* Sets the volume of the playback of captured audio
	* @param volume		New volume in range 0.0-1.0
	*/
	void setPlaybackVolume(float volume);

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
	std::atomic<bool>	_isCapturing;

	/**
	* Initializes a playback device output
	*/
	bool				_initPlayBackDevice;

	/**
	* Enables the playback device output
	*/
	bool				_enablePlayBack;

	/**
	* Enables captured audio buffer broadcast
	*/
	bool				_enableOnCapturedData;

	/**
	* Controls the volume of the captured audio
	*/
	float				_audioVolumeCapture;

	/**
	* Controls the volume of the playback of the captured audio
	*/
	float				_audioVolumePlayback;

	/**
	* Playback device manager
	*/
	AUDIO_DEVICE_MANAGER	_captureDeviceManager;

	/**
	* Capturer buffer raw data
	*/
	std::unique_ptr<QueueBuffer>	_audioBuffer;

	/**
	* Capturer buffer total size
	*/
	unsigned long long int			_totalBufferSize;

	/**
	* Default capturer audio device info
	*/
	AudioAnalyzerDeviceInfo			_deviceCapturerInfo;

	/**
	* Default player audio device info
	*/
	AudioAnalyzerDeviceInfo			_devicePlayerInfo;

	/**
	* Buffer to be returned by onCapturedData callback
	*/
	TArray<uint8>					_capturedBuffer;

	/**
	* Initializes the player device
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