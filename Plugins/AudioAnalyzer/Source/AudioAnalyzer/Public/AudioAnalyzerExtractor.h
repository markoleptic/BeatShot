// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#ifndef AudioAnalyzerExtractor_H
#define AudioAnalyzerExtractor_H

#include "AudioAnalyzerSource.h"
#include "AudioAnalyzerCommon.h"
#include "AudioAnalyzerDecoder.h"

#include <atomic>
#include <functional>

class UAudioComponent;
class UAudioAnalyzerSoundWave;
/**
* @brief Audio Analyzer Extractor 
*
* This class will be used to load audio from a disk file and do offline analysis
*/
class AudioAnalyzerExtractor : public AudioAnalyzerSource
{
public:

	/**
	* Constructor
	*/
	AudioAnalyzerExtractor();

	/**
	* Destructor
	* This will unload the loaded audio too
	*/
	~AudioAnalyzerExtractor();

	/**
	* Disable copy constructor
	* @param x	---
	*/
	AudioAnalyzerExtractor(AudioAnalyzerExtractor const &x) = delete;

	/**
	* Disable copy assignment
	* @param x	---
	*/
	void operator=(AudioAnalyzerExtractor const &x) = delete;

	/**
	* Decodes the PCM audio data and opens the audio extractor device
	* @param filePath				Path to the audio file
	* @param onlyHeader				Load only header info, audio data will not be uncompressed
	* @return						Audio initilization execution result
	*/
	bool initializeAudio(const FString& filePath, bool onlyHeader);

	/**
	* Decodes the PCM audio data and opens the audio extractor device
	* @param filePath				Path to the audio file
	* @param onlyHeader				Load only header info, audio data will not be uncompressed
	* @return						Audio initilization scheduled
	*/
	bool asyncInitializeAudio(const FString& filePath, bool onlyHeader);

	/**
	* Decodes the PCM audio data and constructs a sound wave object
	* @param[out] soundWave SoundWave pointer
	* @param filePath		Path to the audio file
	* @return				SoundWave initialization result
	*/
	bool initializeSoundWave(UAudioAnalyzerSoundWave* soundWave, const FString& filePath);

	/**
	* Decodes the PCM audio data and constructs a sound wave object
	* @param[out] soundWave SoundWave pointer
	* @param filePath		Path to the audio file
	* @return				SoundWave initilization scheduled
	*/
	bool asyncInitializeSoundWave(UAudioAnalyzerSoundWave* soundWave, const FString& filePath);

	/**
	* Closes audio player device
	*/
	void unloadAudio();
	
	/**
	* Returns the playback progress and buffer postion time
	* @param[out] bufferPosTime		Buffer position (same as below value)
	* @return float					Progress in seconds
	*/
	float getPlaybackProgress(float& bufferPosTime);

	/**
	* Returns the total audio duration
	* @return Duration in seconds
	*/
	float getTotalDuration() const;

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
	* Get audio format
	* @return	AudioAnalyzerDecoder::AudioFileType
	*/
	const AudioAnalyzerDecoder::AudioFileType getAudioFileFormat() const {	return AudioAnalyzerDecoder::AudioFileType::WAV;/*TODO esto*/};

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
	* Registers the called function when the init audio ends
	* @param callback		Function callback
	*/
	void registerOnConstructSoundWaveEnd(std::function<void(bool)> callback);

	/**
	* Returns if the async initialization thread is running
	* @return bool			Async Initialization is running
	*/
	bool isAsyncInitRunning() const { return _asyncInitRunning; };

	/**
	* Link an audio component to the analyzer
	* @param audioComponent		Audio Component pointer
	*/
	void setAudioComponent(UAudioComponent* audioComponent);

	/**
	* Reference to the audio component onPlaybackEnd function callback
	*/
	std::function<void()> onAudioComponentPlaybackFinished;

	/**
	* Reference to the audio component onPlaybackLoopEnd function callback
	*/
	std::function<void()> onAudioComponentPlaybackLoopFinished;

	/**
	* Reference to the audio component onStopFinished function callback
	*/
	std::function<void()> onAudioComponentStopFinished;

	/**
	* Registers the called function when the audio playback reaches its end
	* @param callback		Function callback
	*/
	void registerOnAudioComponentPlaybackEnd(std::function<void()> callback);

	/**
	* Registers the called function when one loop of the audio playback reaches its end
	* @param callback		Function callback
	*/
	void registerOnAudioComponentPlaybackLoopEnd(std::function<void()> callback);

	/**
	* Registers the called function when one loop of the audio playback reaches its end
	* @param callback		Function callback
	*/
	void registerOnAudioComponentStopFinished(std::function<void()> callback);

	/**
	* Returns the audio component pointer
	* @return Audio Component pointer
	*/
	UAudioComponent* getAudioComponent() const { return _audioComponent; };

	/**
	* Starts the audio component playback
	* @param loops			Number of loops
	* @param startTime		Starting position seconds
	*/
	void playAudioComponent(int32 loops, float startTime);

	/**
	* Stops audio playback
	*/
	void asyncStopAudioComponent();

	/**
	* Pauses audio component playback if the device is in playing state
	* UnPauses audio component playback if the device has been paused previously
	* @param forcePause		Set the pause to this state
	*/
	void pauseAudioComponent(bool forcePause = true);

	/**
	* Returns if the device is currently playing audio
	* @return Device is playing
	*/
	bool isAudioComponentPlaying() const;

	/**
	* Returns if the audio component device is currently paused
	* @return Device is paused
	*/
	bool isAudioComponentPaused() const;

	/**
	* Returns the audio component  progress and buffer postion time
	* @param[out] bufferPosTime		Buffer position (same as below value)
	* @return float					Progress in seconds
	*/
	 float getAudioComponentPlaybackProgress(float& bufferPosTime);

	/**
	* Returns the audio component playback progress
	* @return float					Progress in seconds
	*/
	float getAudioComponentPlaybackProgress();

	/**
	* Sets the audio component playback progress
	* @param time		New playback position in seconds
	*/
	void setAudioComponentPlaybackProgress(float time);

	/**
	* Returns the total audio duration
	* @return Duration in seconds
	*/
	float getAudioComponentTotalDuration() const;

	/**
	* Get remaining loops of the Audio Component
	* @return remainingLoops
	*/
	const int getAudioComponentRemainingLoops() const;


private:

	/**
	* Decoder to load the audio info from the disk file
	*/
	AudioAnalyzerDecoder			_AADecoder;

	/**
	* Core can use the SoundWave playback info to generate analysis results
	*/
	bool							_hasSoundWave;

	/**
	* Pointer to the soundwave
	*/
	UAudioAnalyzerSoundWave*		_soundWave;

	/**
	* Pointer to an Audio Component
	*/
	UAudioComponent*				_audioComponent;

	/**
	* Reference to the onInitAudioEnd function callback
	*/
	std::function<void(bool)>		onInitAudioEnd;

	/**
	* Reference to the onConstructSoundWaveEnd function callback
	*/
	std::function<void(bool)>		onConstructSoundWaveEnd;

	/**
	* To know when a decoding process is running
	*/
	std::atomic<bool>				_asyncInitRunning;

};

#endif