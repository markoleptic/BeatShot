// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#ifndef AudioAnalyzerPlayer_H
#define AudioAnalyzerPlayer_H

#include "AudioAnalyzerSource.h"
#include "AudioAnalyzerDeviceInfo.h"
#include "AudioAnalyzerCommon.h"
#include "AudioAnalyzerDecoder.h"

#include "AudioAnalyzerPlayerUnit.h"

#include <string>
#include <memory>
#include <atomic>
#include <functional>

struct ma_decoder;

/**
* @brief Audio Analyzer Player Source
*
* This class will be used to load audio from a disk file and interact with it using the most basic player options (play/stop/pause)
*/
class AudioAnalyzerPlayer : public AudioAnalyzerSource
{
public:

	/**
	* Constructor
	*/
	AudioAnalyzerPlayer();

	/**
	* Destructor
	* This will unload the loaded audio too
	*/
	~AudioAnalyzerPlayer();

	/**
	* Disable copy constructor
	* @param x	---
	*/
	AudioAnalyzerPlayer(AudioAnalyzerPlayer const &x) = delete;

	/**
	* Disable copy assignment
	* @param x	---
	*/
	void operator=(AudioAnalyzerPlayer const &x) = delete;

	/**
	* Loads the audio wav info and opens the audio player device
	* @param filePath					Path to the audio file
	* @param periodSizeInFrames			Device audio buffer size in frames (power of 2), 0 to use default value
	* @param periodSizeInMilliseconds	Device audio buffer size in milliseconds, 0 to use default value
	* @param periods					Device audio buffer size multiplier
	* @return							Audio initilization execution result
	*/
	bool initializeAudio(const FString& filePath, uint32 periodSizeInFrames, uint32 periodSizeInMilliseconds, uint32 periods);

	/**
	* Loads the audio wav info and opens the audio player device
	* @param filePath					Path to the audio file
	* @param periodSizeInFrames			Device audio buffer size in frames (power of 2), 0 to use default value
	* @param periodSizeInMilliseconds	Device audio buffer size in milliseconds, 0 to use default value
	* @param periods					Device audio buffer size multiplier
	* @return							Audio initilization scheduled
	*/
	bool asyncInitializeAudio(const FString& filePath, uint32 periodSizeInFrames, uint32 periodSizeInMilliseconds, uint32 periods);

	/**
	* Closes audio player device
	*/
	void unloadAudio();

	/**
	* Starts audio playback
	* @param loops			Number of loops
	* @param startTime		Start playback position
	*/
	void play(int loops, float startTime);

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
	* Registers the called function when the audio playback reaches its end 
	* @param callback		Function callback
	*/
	void registerOnPlaybackEnd(std::function<void()> callback);

	/**
	* Registers the called function when one loop of the audio playback reaches its end
	* @param callback		Function callback
	*/
	void registerOnPlaybackLoopEnd(std::function<void()> callback);

	/**
	* Sets the playback buffer size
	* @param bufferSeconds		Buffer size in seconds
	*/
	void adjustBufferSize(float bufferSeconds);

	/**
	* Reference to the onPlaybackEnd function callback
	*/
	std::function<void()> onPlaybackEndCallBack;

	/**
	* Reference to the onPlaybackLoopEnd function callback
	*/
	std::function<void()> onPlaybackLoopEndCallBack;

	/**
	* Enables or disables metadata load
	* @param enableMetadataLoad		Enable metadata load
	*/
	void setEnableMetadataLoad(bool enableMetadataLoad);

	/**
	* Get file metadata
	* @param[out] Filename		Filename of the audio file
	* @param[out] Extension		Extension of the audio file
	* @param[out] MetaType		ID3_V1 | ID3_V2.3 | ID3_V2.4
	* @param[out] Title			Title of the song
	* @param[out] Artist		Artist 
	* @param[out] Album			Album
	* @param[out] Year			Year
	* @param[out] Genre			Genre
	*/
	void getMetadata(FString& Filename, FString& Extension, FString& MetaType, FString& Title, FString& Artist, FString& Album, FString& Year, FString& Genre);

	/**
	* Extract the AlbumArt pictures into the selected folder. Prefix1.png, Prefix2.png,...
	* @param Prefix						Prefix used to name the pictures.
	* @param Folder						Destination folder
	* @param[out] NumberOfPictures		Number of stored pictures
	*/
	void getMetadataArt(const FString& Prefix, const FString& Folder, int32& NumberOfPictures);

	/**
	* Get output audio device names list
	* @param[out] audioDeviceNames		Player device names list
	*/
	static void getOutputAudioDeviceNames(TArray<FString>& audioDeviceNames);

	/**
	* Set default player device
	* @param audioDeviceName		New default audio device name
	*/
	void setDefaultAudioDevice(const FString& audioDeviceName);

	/**
	* Get default player device
	* @param[out] audioDeviceName		Default audio device name
	*/
	void getDefaultAudioDevice(FString& audioDeviceName);

	/**
	* (Const) Returns the audio device info
	* @return audio device info structure
	*/
	const AudioAnalyzerDeviceInfo& cgetAudioDeviceInfo() const;

	/**
	* Returns the audio device info
	* @return audio device info structure
	*/
	AudioAnalyzerDeviceInfo& getAudioDeviceInfo();

	/**
	* Get remaining loops
	* @return	Remaining loops
	*/
	const int getRemainingLoops() const;

	/**
	* Get audio decoder
	* @return	ma_decoder
	*/
	const ma_decoder* getAudioDecoder() const { return _AADecoder.getAudioDecoder();};

	/**
	* Returns if only has been loaded the header data
	* @return Has onkly header data
	*/
	bool hasOnlyHeader() const;

	/**
	* Returns if the buffer needs to be offset
	* @return Needs buffer offset
	*/
	bool needsBufferOffset() const { return false; };

	/**
	* Registers the called function when the init audio ends
	* @param callback		Function callback
	*/
	void registerOnInitAudioEnd(std::function<void(bool)> callback);

	/**
	* Returns if the async initialization thread is running
	* @return bool			Async Initialization is running
	*/
	bool isAsyncInitRunning() const { return _asyncInitRunning; };

private:

	/**
	* Decoder to load the audio info from the disk file
	*/
	AudioAnalyzerDecoder						_AADecoder;

	/**
	* Player Unit
	*/
	std::unique_ptr<AudioAnalyzerPlayerUnit>	_audioPlayerUnit;

	/**
	* Default audio device info
	*/
	AudioAnalyzerDeviceInfo						_deviceInfo;

	/**
	* Reference to the onInitAudioEnd function callback
	*/
	std::function<void(bool)>					onInitAudioEnd;

	/**
	* To know when a decoding process is running
	*/
	std::atomic<bool>							_asyncInitRunning;
};

#endif