// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#pragma once

#include "AudioAnalyzerCore.h"
#include "AudioAnalyzerPlayer.h"
#include "AudioAnalyzerCapturer.h"
#include "AudioAnalyzerLoopback.h"
#include "AudioAnalyzerStream.h"
#include "AudioAnalyzerExtractor.h"
#include "AudioAnalyzerOVR.h"
#include "AudioAnalyzerSoundWave.h"

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/NoExportTypes.h"

#include "AudioAnalyzerManager.generated.h"

class USoundWaveProcedural;
class UAudioComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlaybackDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCaptureDelegate, const TArray<uint8>&, CaptureBuffer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInitAudioDelegate, bool, BoolResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FConstructSoundWaveDelegate, UAudioAnalyzerSoundWave*, SoundWaveResult, bool, BoolResult);

/** Enum for spectrum types */
UENUM(BlueprintType)
enum class ESpectrumType : uint8
{
	ST_Linear 	UMETA(DisplayName = "Linear"),
	ST_Log 		UMETA(DisplayName = "Log")
};

/** Enum for Channel selection types */
UENUM(BlueprintType)
enum class EChannelSelectionMode : uint8
{
	All_in_one 	UMETA(DisplayName = "All in one"),
	Select_one	UMETA(DisplayName = "Select one"),
	Split_all	UMETA(DisplayName = "Split all")
};

/** Enum for Audio Format */
UENUM(BlueprintType)
enum class EAudioFormat : uint8
{
	Unsigned_Int 	UMETA(DisplayName = "Unsigned Int"),
	Signed_Int		UMETA(DisplayName = "Signed Int"),
	Float			UMETA(DisplayName = "Float")
};

/** Enum for Audio Bit Depth */
UENUM(BlueprintType)
enum class EAudioDepth : uint8
{
	B_16 			UMETA(DisplayName = "16 Bits"),
	B_32			UMETA(DisplayName = "32 Bits")
};

/**
* @brief Audio Analyzer Results Container (float)
*
* Audio Analyzer float results by channel
*/
UCLASS(BlueprintType)
class PARALLELCUBEAUDIOANALYZER_API UChannelFResult : public UObject
{
	GENERATED_BODY()
public:

	/** @name Analysis Result By Channel (float)*/
	/**@{*/
	/**
	* Number of channels of the container
	* @return int32		Number of channels
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Result|ByChannel|Float")
		const int32 GetNumChannels() const;

	/**
	* Result of the channel
	* @param Channel			Number of channel
	* @return TArray<float>		Channel results
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Result|ByChannel|Float")
		const TArray<float>& GetChannelResult(const int32 Channel) const;
	/**@}*/

	/**
	* Constructor
	*/
	UChannelFResult();

	/**
	* Destructor
	*/
	~UChannelFResult();

	/**
	* Returns non const reference to reuslts by channel container 
	* @return TArray<TArray<float> >		Results by channel container
	*/
	TArray<TArray<float> >& getContainer();
private:

	/**
	* Results by channel
	*/
	TArray<TArray<float> > _values;
};

/**
* @brief Audio Analyzer Results Container (int32)
*
* Audio Analyzer int32 results by channel
*/
UCLASS(BlueprintType)
class PARALLELCUBEAUDIOANALYZER_API UChannelIResult : public UObject
{
	GENERATED_BODY()
public:
	/** @name Analysis Result By Channel (int32)*/
	/**@{*/
	/**
	* Number of channels of the container
	* @return int32		Number of channels
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Result|ByChannel|Int32")
		const int32 GetNumChannels() const;

	/**
	* Result of the channel
	* @param Channel			Number of channel
	* @return TArray<float>		Channel results
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Result|ByChannel|Int32")
		const TArray<int32>& GetChannelResult(const int32 Channel) const;
	/**@}*/

	/**
	* Constructor
	*/
	UChannelIResult();

	/**
	* Destructor
	*/
	~UChannelIResult();

	/**
	* Returns non const reference to reuslts by channel container
	* @return TArray<TArray<int32> >		Results by channel container
	*/
	TArray<TArray<int32> >& getContainer();
private:

	/**
	* Results by channel
	*/
	TArray<TArray<int32> > _values;
};

/**
* @brief Audio Analyzer Results Container (bool)
*
* Audio Analyzer bool results by channel
*/
UCLASS(BlueprintType)
class PARALLELCUBEAUDIOANALYZER_API UChannelBResult : public UObject
{
	GENERATED_BODY()
public:

	/** @name Analysis Result By Channel (Bool)*/
	/**@{*/
	/**
	* Number of channels of the container
	* @return int32		Number of channels
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Result|ByChannel|Bool")
		const int32 GetNumChannels() const;

	/**
	* Result of the channel
	* @param Channel			Number of channel
	* @return TArray<float>		Channel results
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Result|ByChannel|Bool")
		const TArray<bool>& GetChannelResult(const int32 Channel) const;
	/**@}*/

	/**
	* Constructor
	*/
	UChannelBResult();

	/**
	* Destructor
	*/
	~UChannelBResult();

	/**
	* Returns non const reference to reuslts by channel container
	* @return TArray<TArray<bool> >		Results by channel container
	*/
	TArray<TArray<bool> >& getContainer();
private:

	/**
	* Results by channel
	*/
	TArray<TArray<bool> > _values;
};

/**
* @brief Audio Analyzer Manager
*
* Audio Analyzer blueprint interface 
*/
UCLASS(Blueprintable, BlueprintType)
class PARALLELCUBEAUDIOANALYZER_API UAudioAnalyzerManager : public UObject
{
	GENERATED_BODY()
public:

	/** @name Player Initialization	*/
	/**@{*/
	/**
	* Initializes the player audio source instance
	* @param FileName	Path to the audio file
	* @return			Initialization result
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Player|Initialization")
		bool InitPlayerAudio(const FString& FileName);

	/**
	* Initializes the player audio source instance (Extended version)
	* @param FileName					Path to the audio file
	* @param PeriodSizeInFrames			Device audio buffer size in frames (power of 2), 0 to use default value
	* @param PeriodSizeInMilliseconds	Device audio buffer size in milliseconds, 0 to use default value
	* @param Periods					Device audio buffer size multiplier, 0 to use default value
	* @return							Initialization result
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Player|Initialization", meta = (AdvancedDisplay = "1"))
		bool InitPlayerAudioEx(const FString& FileName, int32 PeriodSizeInFrames=0, int32 PeriodSizeInMilliseconds=0, int32 Periods=0);

	/**
	* Initializes the player audio source instance (Async version)
	* @param FileName					Path to the audio file
	* @param PeriodSizeInFrames			Device audio buffer size in frames (power of 2), 0 to use default value
	* @param PeriodSizeInMilliseconds	Device audio buffer size in milliseconds, 0 to use default value
	* @param Periods					Device audio buffer size multiplier, 0 to use default value
	* @return							Async Player initialization has been scheduled
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Player|Initialization", meta = (AdvancedDisplay = "1"))
		bool AsyncInitPlayerAudio(const FString& FileName, int32 PeriodSizeInFrames = 0, int32 PeriodSizeInMilliseconds = 0, int32 Periods = 0);

	/**
	* Check if there is an async player initialization running
	* @return bool 						An async player initialization is running
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Player|Initialization")
		bool IsAsyncInitPlayerAudioRunning() const;

	/**
	* Closes the player audio device
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Player|Initialization")
		void UnloadPlayerAudio();

	/**
	* Sets default player device 
	* @param AudioDeviceName	Name of audio device (GetOutputAudioDeviceNames output)
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Player|Initialization")
		void SetDefaultDevicePlayerAudio(const FString& AudioDeviceName);

	/**
	* Returns the default player device name (use after InitPlayerAudio)
	* @param[out] PlayerDeviceName		Name of audio device
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Player|Initialization")
		void GetDefaultDevicePlayerAudio(FString& PlayerDeviceName);
	/**@}*/

	/** @name Capturer Initialization */
	/**@{*/
	/**
	* Initializes the capturer audio source instance
	* @return			Initialization result
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Capturer|Initialization")
		bool InitCapturerAudio();

	/**
	* Initializes the capturer audio source instance (Extended version)
	* @param SampleRate					Samples per second of the capture
	* @param BitDepth					Bits per sample
	* @param AudioFormat				Sample number format
	* @param AudioBufferSeconds			Number of seconds of capture history
	* @param InitPlaybackDevice			Initializes a playback device
	* @param PeriodSizeInFrames			Device audio buffer size in frames (power of 2), 0 to use default value
	* @param PeriodSizeInMilliseconds	Device audio buffer size in milliseconds, 0 to use default value
	* @param Periods					Device audio buffer size multiplier, 0 to use default value
	* @return							Initialization result
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Capturer|Initialization", meta = (AdvancedDisplay = "5"))
		bool InitCapturerAudioEx(int32 SampleRate=44100, EAudioDepth BitDepth=EAudioDepth::B_16, EAudioFormat AudioFormat=EAudioFormat::Signed_Int, float AudioBufferSeconds =1.f,
			bool InitPlaybackDevice=false, int32 PeriodSizeInFrames=0, int32 PeriodSizeInMilliseconds=0, int32 Periods=0);

	/**
	* Closes the capturer audio device
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Capturer|Initialization")
		void UnloadCapturerAudio();

	/**
	* Sets default capturer devices
	* @param CapturerDeviceName		Name of audio capturer device (GetInputAudioDeviceNames output)
	* @param PlayerDeviceName		Name of audio player device (GetOutputAudioDeviceNames output)
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Capturer|Initialization")
		void SetDefaultDevicesCapturerAudio(const FString& CapturerDeviceName, const FString& PlayerDeviceName);

	/**
	* Returns the default capturer device names (use after InitCapturerAudio)
	* @param[out] CapturerDeviceName	Name of audio captuter device
	* @param[out] PlayerDeviceName		Name of audio player device
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Capturer|Initialization")
		void GetDefaultDevicesCapturerAudio(FString& CapturerDeviceName, FString& PlayerDeviceName);
	/**@}*/

	/** @name Loopback Initialization */
	/**@{*/
	/**
	* Initializes the loopback audio source instance
	* @return			Initialization result
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Loopback|Initialization")
		bool InitLoopbackAudio();

	/**
	* Initializes the loopback audio source instance (Extended version)
	* @param NumChannels				Number of channels
	* @param SampleRate					Samples per second
	* @param BitDepth					Bits per sample
	* @param AudioFormat				Sample number format
	* @param AudioBufferSeconds			Number of seconds of capture history
	* @param PeriodSizeInFrames			Device audio buffer size in frames (power of 2), 0 to use default value
	* @param PeriodSizeInMilliseconds	Device audio buffer size in milliseconds, 0 to use default value
	* @param Periods					Device audio buffer size multiplier, 0 to use default value
	* @return							Initialization result
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Loopback|Initialization", meta = (AdvancedDisplay = "5"))
		bool InitLoopbackAudioEx(int32 NumChannels = 2, int32 SampleRate = 44100, EAudioDepth BitDepth = EAudioDepth::B_16, EAudioFormat AudioFormat = EAudioFormat::Signed_Int, 
			float AudioBufferSeconds = 1.f, int32 PeriodSizeInFrames = 0, int32 PeriodSizeInMilliseconds = 0, int32 Periods = 0);

	/**
	* Closes the capturer audio device
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Loopback|Initialization")
		void UnloadLoopbackAudio();

	/**
	* Sets default loopback devices
	* @param SourceDeviceName		Friendly Name of audio source device (GetOutputAudioDeviceNames output)
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Loopback|Initialization")
		void SetDefaultDeviceLoopbackAudio(const FString& SourceDeviceName);

	/**
	* Returns the default loopback device names (use after InitLoopbackAudio)
	* @param[out] SourceDeviceName	 Friendly Name of audio source device
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Loopback|Initialization")
		void GetDefaultDeviceLoopbackAudio(FString& SourceDeviceName);
	/**@}*/

	/** @name Stream Initialization */
	/**@{*/
	/**
	* Initializes the stream audio source instance
	* @param NumChannels				Number of channels
	* @param SampleRate					Samples per second
	* @param BitDepth					Bits per sample
	* @param AudioFormat				Sample number format
	* @param AudioBufferSeconds			Number of seconds of capture history
	* @param InitPlaybackDevice			Initialize device to playback received data
	* @param PeriodSizeInFrames			Device audio buffer size in frames (power of 2), 0 to use default value
	* @param PeriodSizeInMilliseconds	Device audio buffer size in milliseconds, 0 to use default value
	* @param Periods					Device audio buffer size multiplier, 0 to use default value
	* @return							Initialization result
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Stream|Initialization", meta = (AdvancedDisplay = "6"))
		bool InitStreamAudio(int32 NumChannels=2, int32 SampleRate=44100, EAudioDepth BitDepth=EAudioDepth::B_16, EAudioFormat AudioFormat=EAudioFormat::Signed_Int,
			float AudioBufferSeconds = 1.0f, bool InitPlaybackDevice = false, int32 PeriodSizeInFrames = 0, int32 PeriodSizeInMilliseconds = 0, int32 Periods = 0);

	/**
	* Closes the stream listener audio device
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Stream|Initialization")
		void UnloadStreamAudio();

	/**
	* Sets default stream playback device.
	* @param PlayerDeviceName		Friendly Name of audio source device (GetOutputAudioDeviceNames output)
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Stream|Initialization")
		void SetDefaultDeviceStreamAudio(const FString& PlayerDeviceName);

	/**
	* Returns the default stream playback device names (use after InitLoopbackAudio)
	* @param[out] PlayerDeviceName	 Friendly Name of audio source device
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Stream|Initialization")
		void GetDefaultDeviceStreamAudio(FString& PlayerDeviceName);
	/**@}*/

	/** @name Extractor Initialization	*/
	/**@{*/
	/**
	* Initializes the extractor audio source instance
	* @param FileName		Path to the audio file
	* @param OnlyHeader		Load only header info, audio data will not be uncompressed
	* @return				Initialization result
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Extractor|Initialization")
		bool InitExtractorAudio(const FString& FileName, bool OnlyHeader);

	/**
	* Initializes the extractor audio source instance
	* @param FileName		Path to the audio file
	* @param OnlyHeader		Load only header info, audio data will not be uncompressed
	* @return				Async extrator initialization scheduled
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Extractor|Initialization")
		bool AsyncInitExtractorAudio(const FString& FileName, bool OnlyHeader);

	/**
	* Check if there is an async extractor initialization running
	* @return bool 						An async extractor initialization is running
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Extractor|Initialization")
		bool IsAsyncInitExtractorAudioRunning() const;

	/**
	* Construct a SoundWave object for the audio file at runtime
	* @param[out] SoundWave		SoundWave object
	* @param FileName			Path to the audio file
	* @return					Initialization result
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|SoundWave|Initialization")
		bool ConstructSoundWave(UAudioAnalyzerSoundWave*& SoundWave, const FString& FileName);

	/**
	* Construct a SoundWave object for the audio file at runtime. Use 
	* @param FileName			Path to the audio file
	* @return					Initialization scheduled
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|SoundWave|Initialization")
		bool AsyncConstructSoundWave(const FString& FileName);

	/**
	* Check if there is an async construct SoundWave running
	* @return bool 						An async construct SoundWave is running
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Extractor|Initialization")
		bool IsAsyncConstructSoundWaveRunning() const;

	/**
	* Get previuos constructed SoundWave object
	* @param[out] SoundWave		SoundWave object
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|SoundWave|Utils")
		void GetSoundWave(UAudioAnalyzerSoundWave*& SoundWave) const;

	/**
	* Closes the extractor audio source
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Extractor|Initialization")
		void UnloadExtractorAudio();

	/**
	* Closes the extractor audio source and free the SoundWave object
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|SoundWave|Initialization")
		void UnloadSoundWave();

	/**@}*/

	/** @name OVR Initialization	*/
	/**@{*/
	/**
	* Initializes the OVR audio source instance
	* @param SampleRate					Samples per second of the capture
	* @param AudioFormat				Sample format (Float/Fixed)
	* @param AudioBufferSeconds			Number of seconds of capture history
	* @param InitPlaybackDevice			Initialize device to playback received data
	* @param PeriodSizeInFrames			Device audio buffer size in frames (power of 2), 0 to use default value
	* @param PeriodSizeInMilliseconds	Device audio buffer size in milliseconds, 0 to use default value
	* @param Periods					Device audio buffer size multiplier, 0 to use default value
	* @return							Initialization result
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|OVR|Initialization", meta = (AdvancedDisplay = "4"))
		bool InitOVRAudio(int32 SampleRate = 48000, EAudioFormat AudioFormat = EAudioFormat::Signed_Int, float AudioBufferSeconds = 1.f, bool InitPlaybackDevice = false,
			int32 PeriodSizeInFrames = 0, int32 PeriodSizeInMilliseconds = 0, int32 Periods = 0);

	/**
	* Closes the OVR audio source
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|OVR|Initialization")
		void UnloadOVRAudio();

	/**
	* Sets default OVR devices
	* @param CapturerDeviceName		Name of audio capturer device (GetInputAudioDeviceNames output)
	* @param PlayerDeviceName		Name of audio player device (GetOutputAudioDeviceNames output)
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|OVR|Initialization")
		void SetDefaultDevicesOVRAudio(const FString& CapturerDeviceName, const FString& PlayerDeviceName);

	/**
	* Returns the default OVR device names (use after InitOVRAudio)
	* @param[out] CapturerDeviceName	Name of audio captuter device
	* @param[out] PlayerDeviceName		Name of audio player device
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|OVR|Initialization")
		void GetDefaultDevicesOVRAudio(FString& CapturerDeviceName, FString& PlayerDeviceName);

	/**@}*/

	/** @name Audio Component Initialization	*/
	/**@{*/
	/**
	* Add an Audio Component pointer to the audio manager
	* @param AudioComponent	Path to the audio file
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|AudioComponent|Initialization")
		void LinkAudioComponent(UAudioComponent* AudioComponent);

	/**
	* Removes the pointer of the Audio Component of the audio manager
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|AudioComponent|Initialization")
		void UnlinkAudioComponent();

	/**@}*/

	/** @name Player Playback */
	/**@{*/
	/**
	* Starts player playback, 1 loop from 0.0 seconds
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Player|Playback")
		void Play();

	/**
	* Starts player playback, n loops from StartTime seconds
	* @param Loops			Number of loops, (0 Infinite)
	* @param StartTime		Starting position in seconds
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Player|Playback")
		void PlayEx(int32 Loops=1, float StartTime=0.f);
	/**
	* Stops player playback
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Player|Playback")
		void Stop();

	/**
	* Pauses player playback if the device is in playing state
	* UnPauses player playback if the device has been paused previously
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Player|Playback")
		void Pause();

	/**
	* Sets the player state on pause/unpause state
	* @param Pause				New pause state
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Player|Playback")
		void SetPaused(bool Pause);

	/**
	* Returns current player playback position
	* @return Playback position
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Player|Playback")
		float GetPlaybackTime();

	/**
	* Sets current player playback position
	* @param Progress		New playback position in seconds
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Player|Playback")
		void SetPlaybackTime(float Progress);

	/**
	* Returns current player playback volume
	* @return Playback volume
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Player|Playback")
		float GetPlaybackVolume();

	/**
	* Sets a new volume in range 0.0 - 1.0
	* @param Volume		New volume in range 0.0 - 1.0
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Player|Playback")
		void SetPlaybackVolume(float Volume=1.f);

	/**
	* Returns total player playback duration
	* @return Playback duration
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Player|Playback")
		float GetTotalDuration() const;

	/**
	* Returns player playing state
	* @return Player is playing
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Player|Playback")
		bool IsPlaying() const;

	/**
	* Returns player pause state
	* @return Player is paused
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Player|Playback")
		bool IsPaused() const;

	/**
	* Returns remaining loops to play
	* @return remaining loops
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Player|Playback")
		int32 GetRemainingLoops() const;

	/**@}*/


	/** @name Capturer Playback */
	/**@{*/
	/**
	* Starts audio capture (and playback)
	* @param PlaybackCapture			Starts playback too
	* @param CapturedDataBroadcast		Returns the captured audio buffer using OnCapturedData Event
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Capturer|Playback")
		void StartCapture(bool PlaybackCapture=false, bool CapturedDataBroadcast=false);

	/**
	* Stops audio capture
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Capturer|Playback")
		void StopCapture();

	/**
	* Returns capture state
	* @return Source is capturing audio
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Capturer|Playback")
		bool IsCapturing() const;

	/**
	* Returns current total capture time
	* @return Elapsed capture time
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Capturer|Playback")
		float GetCaptureTime();

	/**
	* Returns current capture volume
	* @param[out] CaptureVolume		Current capture volume
	* @param[out] PlaybackVolume	Playback capture volume
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Capturer|Playback")
		void GetCaptureVolume(float& CaptureVolume, float& PlaybackVolume);

	/**
	* Sets a new volume for microphone capture and playback in range 0.0 - 1.0
	* @param CaptureVolume		New capture volume in range 0.0 - 1.0
	* @param PlaybackVolume		New playback volume in range 0.0 - 1.0
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Capturer|Playback")
		void SetCaptureVolume(float CaptureVolume=1.f, float PlaybackVolume = 1.f);

	/**@}*/

	/** @name Loopback Playback */
	/**@{*/
	/**
	* Starts audio loopback capture
	* @param CapturedDataBroadcast		Returns the captured audio buffer using OnLoopbackCapturedData event
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Loopback|Playback")
		void StartLoopback(bool CapturedDataBroadcast);

	/**
	* Stops audio loopback capture
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Loopback|Playback")
		void StopLoopback();

	/**
	* Returns player playing state
	* @return Player is playing
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Loopback|Playback")
		bool IsLoopbackCapturing() const;

	/**
	* Returns current loopback total capture time
	* @return Elapsed capture time
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Loopback|Playback")
		float GetLoopbackCaptureTime();

	/**@}*/

	/** @name Extractor Playback */
	/**@{*/
	/**
	* Returns total audio duration
	* @return Audio duration
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Extractor|Playback")
		float GetAudioTotalDuration() const;
	/**@}*/

	/** @name Stream Playback */
	/**@{*/

	/**
	* Opens audio loopback capture (and starts playback)
	* @param PlaybackCapture	Enables the playback of the stream audio pcm data
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Stream|Playback")
		void OpenStreamCapture(bool PlaybackCapture);

	/**
	* Sends the stream audio data to the analyzer
	* @param StreamData		Stream data array
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Stream|Playback")
		void FeedStreamCapture(TArray<uint8> StreamData);

	/**
	* Closes audio loopback capture and stops the playback
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Stream|Playback")
		void CloseStreamCapture();

	/**
	* Returns player playing state
	* @return Player is playing
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Stream|Playback")
		bool IsStreamCapturing() const;

	/**
	* Returns current loopback total capture time
	* @return Elapsed capture time
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Stream|Playback")
		float GetStreamCaptureTime();

	/**@}*/

	/** @name OVR Capture */
	/**@{*/
	/**
	* Starts OVR audio capture (and playback)
	* @param PlaybackCapturer		Starts playback too
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|OVR|Playback")
		void StartOVRCapture(bool PlaybackCapturer);

	/**
	* Stops OVR audio capture
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|OVR|Playback")
		void StopOVRCapture();

	/**
	* Returns OVR Capture state
	* @return OVR is capturing audio
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|OVR|Playback")
		bool IsOVRCapturing() const;

	/**
	* Returns total OVR capture time
	* @return Elapsed OVR capture time
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|OVR|Playback")
		float GetOVRCaptureTime();

	/**@}*/

	/** @name Audio Component wrapper */
	/**@{*/
	/**
	* Starts Audio component playback, n loops from StartTime seconds
	* @param Loops			Number of loops, (0 Infinite)
	* @param StartTime		Starting position in seconds
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|AudioComponent|Playback")
		void PlayAudioComponent(int32 Loops = 1, float StartTime = 0.f);

	/**
	* Stops Audio component playback
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|AudioComponent|Playback")
		void StopAudioComponent();

	/**
	* Stops Audio component playback, the event OnAudioComponentStopFinished will be triggered after stop process is done
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|AudioComponent|Playback")
		void AsyncStopAudioComponent();

	/**
	* Pauses audio component playback if the device is in playing state
	* UnPauses audio component playback if the device has been paused previously
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|AudioComponent|Playback")
		void PauseAudioComponent();

	/**
	* Sets the audio component state on pause/unpause state
	* @param Pause				New pause state
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|AudioComponent|Playback")
		void SetPausedAudioComponent(bool Pause);

	/**
	* Sets current audio component playback position
	* @param Progress		New playback position in seconds
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|AudioComponent|Playback")
		void SetAudioComponentPlaybackTime(float Progress);

	/**
	* Returns current audio component playback position
	* @return Playback position
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|AudioComponent|Playback")
		float GetAudioComponentPlaybackTime();

	/**
	* Returns total audio component playback duration
	* @return Playback duration
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|AudioComponent|Playback")
		float GetAudioComponentTotalDuration() const;

	/**
	* Returns audio component playing state
	* @return Audio component is playing
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|AudioComponent|Playback")
		bool IsAudioComponentPlaying() const;

	/**
	* Returns audio component pause state
	* @return Audio component is paused
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|AudioComponent|Playback")
		bool IsAudioComponentPaused() const;

	/**
	* Returns remaining loops to play
	* @return remaining loops
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|AudioComponent|Playback")
		int32 GetAudioComponentRemainingLoops() const;

	/**
	* Returns a pointer to the linked audio component
	* @return Pointer to Audio Component
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|AudioComponent|Utils")
		UAudioComponent* GetLinkedAudioComponent() const;


	/** @name Player Utils */
	/**@{*/
	/** 
	* Retrieves metadata info
	* @param[out] Filename		Filename of the audio file
	* @param[out] Extension		Extension of the audio file
	* @param[out] MetaType		ID3_V1 | ID3_V2.3 | ID3_V2.4
	* @param[out] Title			Title of the song
	* @param[out] Artist		Artist 
	* @param[out] Album			Album
	* @param[out] Year			Year
	* @param[out] Genre			Genre
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Player|Utils")
		void GetMetadata(FString& Filename, FString& Extension, FString& MetaType, FString& Title, FString& Artist, FString& Album, FString& Year, FString& Genre);

	/** 
	* Extract the AlbumArt pictures into the selected folder. Prefix1.png, Prefix2.png,...
	* @param Prefix						Prefix used to name the pictures.
	* @param Folder						Destination folder, must exists
	* @param[out] NumberOfPictures		Number of stored pictures
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Player|Utils")
		void GetMetadataArt(const FString& Prefix, const FString& Folder, int32& NumberOfPictures);

	/** 
	* Returns the parameters used to configure the player device
	* @param[out] DeviceName		Name of the device in the system devices list
	* @param[out] SampleRate		Number of samples per second
	* @param[out] NumChannels		Number of channels
	* @param[out] BufferSamples		Number of samples of the playback buffer
	* @param[out] Format			Format of the values that represent each sample (Fixed/Float)
	* @param[out] BitDepth			Number of bits used for each sample
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Player|Utils")
		void GetPlayerDeviceAudioInfo(FString& DeviceName, int32& SampleRate, int32& NumChannels, int32& BufferSamples, EAudioFormat& Format, int32& BitDepth);

	/**@}*/

	/** @name Capturer Utils */
	/**@{*/

	/** 
	* Retrieves the capture/playback audio device info 
	* @param[out] CapturerDeviceName		Name of the capturer device in the system devices list
	* @param[out] CapturerSampleRate		Number of samples per second
	* @param[out] CapturerNumChannels		Number of channels
	* @param[out] CapturerBufferSamples		Number of samples of the playback buffer
	* @param[out] CapturerFormat			Format of the values that represent each sample (Fixed/Float)
	* @param[out] CapturerBitDepth			Number of bits used for each sample
	* @param[out] PlayerDeviceName			Name of the player device in the system devices list
	* @param[out] PlayerSampleRate			Number of samples per second
	* @param[out] PlayerNumChannels			Number of channels
	* @param[out] PlayerBufferSamples		Number of samples of the playback buffer
	* @param[out] PlayerFormat				Format of the values that represent each sample (Fixed/Float)
	* @param[out] PlayerBitDepth			Number of bits used for each sample
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Capturer|Utils")
		void GetCapturerDevicesAudioInfo(FString& CapturerDeviceName, int32& CapturerSampleRate, int32& CapturerNumChannels, int32& CapturerBufferSamples, EAudioFormat& CapturerFormat, int32& CapturerBitDepth,
			FString& PlayerDeviceName, int32& PlayerSampleRate, int32& PlayerNumChannels, int32& PlayerBufferSamples, EAudioFormat& PlayerFormat, int32& PlayerBitDepth);

	/**@}*/

	/** @name Loopback Utils */
	/**@{*/
	/** 
	* Retrieves audio device info
	* @param[out] DeviceName		Name of the device in the system devices list
	* @param[out] SampleRate		Number of samples per second
	* @param[out] NumChannels		Number of channels
	* @param[out] BufferSamples		Number of samples of the playback buffer
	* @param[out] Format			Format of the values that represent each sample (Fixed/Float)
	* @param[out] BitDepth			Number of bits used for each sample
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Loopback|Utils")
		void GetLoopbackDeviceAudioInfo(FString& DeviceName, int32& SampleRate, int32& NumChannels, int32& BufferSamples, EAudioFormat& Format, int32& BitDepth);

	/**@}*/

	/** @name Extractor Utils */
	/**@{*/
	/** 
	* Retrieves metadata info for Extractor Source Object
	* @param[out] Filename		Filename of the audio file
	* @param[out] Extension		Extension of the audio file
	* @param[out] MetaType		ID3_V1 | ID3_V2.3 | ID3_V2.4
	* @param[out] Title			Title of the song
	* @param[out] Artist		Artist
	* @param[out] Album			Album
	* @param[out] Year			Year
	* @param[out] Genre			Genre
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Extractor|Utils")
		void GetAudioMetadata(FString& Filename, FString& Extension, FString& MetaType, FString& Title, FString& Artist, FString& Album, FString& Year, FString& Genre);

	/**
	* Extract the AlbumArt pictures into the selected folder.Prefix1.png, Prefix2.png, ...
	* @param Prefix						Prefix used to name the pictures.
	* @param Folder						Destination folder, must exists
	* @param[out] NumberOfPictures		Number of stored pictures
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Extractor|Utils")
		void GetAudioMetadataArt(const FString& Prefix, const FString& Folder, int32& NumberOfPictures);

	/**@}*/

	/** @name Stream Utils */
	/**@{*/

	/** 
	* Retrieves the stream playback audio device info
	* @param[out] PlayerDeviceName		Name of the device in the system devices list
	* @param[out] PlayerSampleRate		Number of samples per second
	* @param[out] PlayerNumChannels		Number of channels
	* @param[out] PlayerBufferSamples	Number of samples of the playback buffer
	* @param[out] PlayerFormat			Format of the values that represent each sample (Fixed/Float)
	* @param[out] PlayerBitDepth		Number of bits used for each sample
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Stream|Utils")
		void GetStreamDevicesAudioInfo(FString& PlayerDeviceName, int32& PlayerSampleRate, int32& PlayerNumChannels, int32& PlayerBufferSamples, EAudioFormat& PlayerFormat, int32& PlayerBitDepth);

	/**@}*/

	/** @name OVR Utils */
	/**@{*/

	/** 
	* Retrieves the audio device info
	* @param[out] CapturerDeviceName		Name of the capturer device in the system devices list
	* @param[out] CapturerSampleRate		Number of samples per second
	* @param[out] CapturerNumChannels		Number of channels
	* @param[out] CapturerBufferSamples		Number of samples of the playback buffer
	* @param[out] CapturerFormat			Format of the values that represent each sample (Fixed/Float)
	* @param[out] CapturerBitDepth			Number of bits used for each sample
	* @param[out] PlayerDeviceName			Name of the player device in the system devices list
	* @param[out] PlayerSampleRate			Number of samples per second
	* @param[out] PlayerNumChannels			Number of channels
	* @param[out] PlayerBufferSamples		Number of samples of the playback buffer
	* @param[out] PlayerFormat				Format of the values that represent each sample (Fixed/Float)
	* @param[out] PlayerBitDepth			Number of bits used for each sample
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|OVR|Utils")
		void GetOVRDevicesAudioInfo(FString& CapturerDeviceName, int32& CapturerSampleRate, int32& CapturerNumChannels, int32& CapturerBufferSamples, EAudioFormat& CapturerFormat, int32& CapturerBitDepth,
			FString& PlayerDeviceName, int32& PlayerSampleRate, int32& PlayerNumChannels, int32& PlayerBufferSamples, EAudioFormat& PlayerFormat, int32& PlayerBitDepth);

	/**@}*/

	/** @name Player Events */
	/**@{*/

	/**
	* Event to track the async player initialization end
	*/
	UPROPERTY(BlueprintAssignable, Category = "AudioAnalyzerManager|Player|Events")
		FInitAudioDelegate OnInitPlayerAudioFinished;

	/**
	* Event to track the player playback end
	*/
	UPROPERTY(BlueprintAssignable, Category = "AudioAnalyzerManager|Player|Events")
		FPlaybackDelegate OnPlaybackFinished;

	/**
	* Event to track the player loop playback end
	*/
	UPROPERTY(BlueprintAssignable, Category = "AudioAnalyzerManager|Player|Events")
		FPlaybackDelegate OnPlaybackLoopFinished;
	/**@}*/

	/** @name Capture Events */
	/**@{*/
	/**
	* Event to track the player playback end
	*/
	UPROPERTY(BlueprintAssignable, Category = "AudioAnalyzerManager|Capturer|Events")
		FCaptureDelegate OnCapturedData;
	/**@}*/

	/** @name Loopback Events */
	/**@{*/
	/**
	* Event to track the player playback end
	*/
	UPROPERTY(BlueprintAssignable, Category = "AudioAnalyzerManager|Loopback|Events")
		FCaptureDelegate OnLoopbackCapturedData;
	/**@}*/

	/** @name Extractor Events */
	/**@{*/
	/**
	* Event to track the async extractor initialization end
	*/
	UPROPERTY(BlueprintAssignable, Category = "AudioAnalyzerManager|Extractor|Events")
		FInitAudioDelegate OnInitExtractorAudioFinished;

	/**
	* Event to track the async construct soundwave end
	*/
	UPROPERTY(BlueprintAssignable, Category = "AudioAnalyzerManager|Extractor|Events")
		FConstructSoundWaveDelegate OnConstructSoundWaveFinished;

	/**@}*/

	/** @name AudioComponent Events */
	/**@{*/
	/**
	* Event to track the audio component playback end
	*/
	UPROPERTY(BlueprintAssignable, Category = "AudioAnalyzerManager|AudioComponent|Events")
		FPlaybackDelegate OnAudioComponentPlaybackFinished;

	/**
	* Event to track the audio component loop playback end
	*/
	UPROPERTY(BlueprintAssignable, Category = "AudioAnalyzerManager|AudioComponent|Events")
		FPlaybackDelegate OnAudioComponentPlaybackLoopFinished;

	/**
	* Event to track the audio component stop playback 
	*/
	UPROPERTY(BlueprintAssignable, Category = "AudioAnalyzerManager|AudioComponent|Events")
		FPlaybackDelegate OnAudioComponentStopFinished;
	/**@}*/

	/** @name Analysis Initialization*/
	/**@{*/

	/** Initializes the amplitude analyzer for a window of time (seconds) centered on current playback position and group it into bars
	/ @param ChannelMode			Channel selection mode
	* @param Channel				Number of channel (only for Select_one channel selection mode)
	* @param NumTimeBands			Number of bars
	* @param TimeWindow				The duration of the window to calculate the amplitude
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Initialization")
		void InitAmplitudeConfig(EChannelSelectionMode ChannelMode, int32 Channel, int32 NumTimeBands=1, float TimeWindow=0.02f);

	/** Initializes the frequency spectrum analyzer for a window of time
	* @param Type					Select the display spectrum format Linear/Log
	* @param ChannelMode			Channel selection mode
	* @param Channel				The channel of the sound to calculate (only for Select_one channel selection mode)
	* @param NumFreqBands			Number of bands of spectrum
	* @param TimeWindow				The duration of the window to calculate the spectrum of
	* @param HistorySize			History size to calculate average frequency spectrum
	* @param UsePeakValues			Returns peak value of the band instead of average value
	* @param NumPeaks				Number of peaks for GetSpectrumPeaks output
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Initialization")
		void InitSpectrumConfig(ESpectrumType Type, EChannelSelectionMode ChannelMode, int32 Channel, int32 NumFreqBands=1, float TimeWindow=0.02f, int32 HistorySize=30, bool UsePeakValues=true, int32 NumPeaks=1);

	/** Initializes the frequency spectrum analyzer for a window of time
	* @param ChannelMode			Channel selection mode
	* @param Channel				The channel of the sound to calculate (only for Select one channel selection mode)
	* @param BandLimits				Array to define the frequency bounds for each spectrum bar (no overlap)
	* @param TimeWindow				The duration of the window to calculate the spectrum of
	* @param HistorySize			History size to calculate average frequency spectrum
	* @param UsePeakValues			Returns peak value of the band instead of average value
	* @param NumPeaks				Number of peaks for GetSpectrumPeaks output
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Initialization")
		void InitSpectrumConfigWLimits(EChannelSelectionMode ChannelMode, int32 Channel, const TArray<FVector2D>& BandLimits, float TimeWindow=0.02f, int32 HistorySize=30, bool UsePeakValues=true, int32 NumPeaks=1);

	/** Initializes the beat tracking analyzer for a window of time
	* @param ChannelMode	Channel selection mode
	* @param Channel		The channel of the sound to calculate (only for Select one channel mode)
	* @param TimeWindow		The duration of the window to calculate the beat tracking
	* @param HistorySize	History size to calculate the average spectrum
	* @param CalculateBPM	Enable BPM calculator
	* @param BPMHistorySize History size to calculate the average BPM
	* @param Threshold		Beat detection threshold
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Initialization")
		void InitBeatTrackingConfig(EChannelSelectionMode ChannelMode, int32 Channel, float TimeWindow=0.02f, int32 HistorySize=30, bool CalculateBPM=false, int32 BPMHistorySize=100, float Threshold = 2.1);

	/** Initializes the beat tracking analyzer for a window of time using custom frequency bounds
	* @param ChannelMode	Channel selection mode
	* @param Channel		The channel of the sound to calculate (only for Select_one channel mode)
	* @param BandLimits		Frequency bound for each beat tracking output (no overlap)
	* @param TimeWindow		The duration of the window to calculate the beat tracking
	* @param HistorySize	History size to calculate the average spectrum
	* @param CalculateBPM	Enable BPM calculator
	* @param BPMHistorySize History size to calculate the average BPM
	* @param Threshold		Beat detection threshold
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Initialization")
		void InitBeatTrackingConfigWLimits(EChannelSelectionMode ChannelMode, int32 Channel, const TArray<FVector2D>& BandLimits, float TimeWindow=0.02f, int32 HistorySize=30, bool CalculateBPM=false, int32 BPMHistorySize=100, float Threshold = 2.1);
	
	
	/** Initializes the pitch tracking analyzer for a window of time
	* @param ChannelMode			Channel selection mode
	* @param Channel				Number of channel (only for Select_one channel selection mode)
	* @param TimeWindow				The duration of the window to calculate the amplitude
	* @param Threshold				YIN threshold
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Initialization")
		void InitPitchTrackingConfig(EChannelSelectionMode ChannelMode, int32 Channel, float TimeWindow=0.02f, float Threshold=0.19f);

	/**@}*/

	/** @name Analysis Results*/
	/**@{*/

	/** Returns amplitude using the amplitude configuration
	* @param[out] Amplitude		Amplitude array result
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Result")
		void GetAmplitude(TArray<float>& Amplitude);


	/** Calculates the frequency spectrum using the spectrum configuration
	* @param[out] Frequencies				The frequency spectrum result
	* @param[out] AverageFrequencies		The average frequency spectrum result
	* @param CalculateAverageFrequencies	Calculate history spectrum average
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Result")
		void GetSpectrum(TArray<float>& Frequencies, TArray<float>& AverageFrequencies, bool CalculateAverageFrequencies);

	/** Calculates the frequency spectrum peaks using the spectrum configuration
	* @param[out] FrequencyPeaks		The frequency peaks result
	* @param[out] FrequencyPeaksEnergy	The frequency peaks energy result
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Result")
		void GetSpectrumPeaks(TArray<float>& FrequencyPeaks, TArray<float>& FrequencyPeaksEnergy);

	/** Returns the default beat tracking result
	* @param[out] IsBass			Beat result for bass frequency range
	* @param[out] IsLowM			Beat result for low frequency mid range
	* @param[out] IsHighM			Beat result for high frequency mid range
	* @param[out] SpectrumValues	Current frequency spectrum values
	* @param[out] BPMCurrent		Average BPM (Partial)
	* @param[out] BPMTotal			Average BPM (Total)
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Result")
		void GetBeatTracking(bool& IsBass, bool& IsLowM, bool& IsHighM, TArray<float>& SpectrumValues, TArray<int32>& BPMCurrent, TArray<int32>& BPMTotal);

	/** Returns the default beat tracking result
	* @param[out] IsBass			Beat result for bass frequency range
	* @param[out] IsLowM			Beat result for low frequency mid range
	* @param[out] IsHighM			Beat result for high frequency mid range
	* @param[out] SpectrumValues	Current frequency spectrum values
	* @param[out] BPMCurrent		Average BPM (Partial)
	* @param[out] BPMTotal			Average BPM (Total)
	* @param OverrideThreshold		Overrides beat configuration threshold for each band
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Result")
		void GetBeatTrackingWThreshold(bool& IsBass, bool& IsLowM, bool& IsHighM, TArray<float>& SpectrumValues, TArray<int32>& BPMCurrent, TArray<int32>& BPMTotal, const TArray<float>& OverrideThreshold);

	/** Returns the beat tracking result for the InitBeatTrackingConfigWLimits BandLimits
	* @param[out] Beats				Beat result for BandLimits frequency ranges
	* @param[out] SpectrumValues	Current frequency spectrum values
	* @param[out] BPMCurrent		Average BPM (Partial)
	* @param[out] BPMTotal			Average BPM (Total)
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Result")
		void GetBeatTrackingWLimits(TArray<bool>& Beats, TArray<float>& SpectrumValues, TArray<int32>& BPMCurrent, TArray<int32>& BPMTotal);

	/** Returns the beat tracking result for the InitBeatTrackingConfigWLimits BandLimits
	* @param[out] Beats				Beat result for BandLimits frequency ranges
	* @param[out] SpectrumValues	Current frequency spectrum values
	* @param[out] BPMCurrent		Average BPM (Partial)
	* @param[out] BPMTotal			Average BPM (Total)
	* @param OverrideThreshold		Overrides beat configuration threshold
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Result")
		void GetBeatTrackingWLimitsWThreshold(TArray<bool>& Beats, TArray<float>& SpectrumValues, TArray<int32>& BPMCurrent, TArray<int32>& BPMTotal, const TArray<float>& OverrideThreshold);

	/** Returns the average frequency spectrum for the beat tracking history
	* @param[out] AverageFrequencies	Average frequency spectrum result
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Result")
		void GetBeatTrackingAverage(TArray<float>& AverageFrequencies);

	/** Returns the variance and average frequency spectrum for the beat tracking history
	* @param[out] VarianceFrequencies		Variance frequency spectrum result
	* @param[out] AverageFrequencies		Average frequency spectrum result
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Result")
		void GetBeatTrackingAverageAndVariance(TArray<float>& AverageFrequencies, TArray<float>& VarianceFrequencies);

	/** Calculates and obtains the pitch tracking analysis result for the current spectrum
	* @param[out] FundamentalFrequency	Fundamental frequency (F0), Pitch tracking must be enabled (InitSpectrumConfig)
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Result")
		void GetPitchTracking(float& FundamentalFrequency);
	/**@}*/

	/** @name Analysis Results By Channel*/
	/**@{*/

	/** Returns amplitude using the amplitude configuration. Only for channel selection mode Split_all
	* @param[out] AmplitudeByChannel	Amplitude array result by channel
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Result|ByChannel")
		void GetAmplitudeByChannel(UChannelFResult*& AmplitudeByChannel);

	/** Calculates the frequency spectrum using the spectrum configuration. Only for channel selection mode Split_all
	* @param[out] Frequencies				The frequency spectrum result for each channel
	* @param[out] AverageFrequencies		The average frequency spectrum result for each channel
	* @param CalculateAverageFrequencies	Calculate history spectrum average
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Result|ByChannel")
		void GetSpectrumByChannel(UChannelFResult*& Frequencies, UChannelFResult*& AverageFrequencies, bool CalculateAverageFrequencies);

	/** Calculates the frequency spectrum peaks using the spectrum configuration. Only for channel selection mode Split_all
	* @param[out] FrequencyPeaks			The frequency peaks result for each channel
	* @param[out] FrequencyPeaksEnergy		The frequency peaks energy result for each channel
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Result|ByChannel")
		void GetSpectrumPeaksByChannel(UChannelFResult*& FrequencyPeaks, UChannelFResult*& FrequencyPeaksEnergy);

	/** Returns the default beat tracking result. Only for channel selection mode Split_all
	* @param[out] IsBass			Beat result for bass frequency range
	* @param[out] IsLowM			Beat result for low frequency mid range
	* @param[out] IsHighM			Beat result for high frequency mid range
	* @param[out] SpectrumValues	Current frequency spectrum values
	* @param[out] BPMCurrent		Average BPM (Partial)
	* @param[out] BPMTotal			Average BPM (Total)
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Result|ByChannel")
		void GetBeatTrackingByChannel(TArray<bool>& IsBass, TArray<bool>& IsLowM, TArray<bool>& IsHighM, UChannelFResult*& SpectrumValues, UChannelIResult*& BPMCurrent, UChannelIResult*& BPMTotal);

	/** Returns the default beat tracking result. Only for channel selection mode Split_all
	* @param[out] IsBass			Beat result for bass frequency range
	* @param[out] IsLowM			Beat result for low frequency mid range
	* @param[out] IsHighM			Beat result for high frequency mid range
	* @param[out] SpectrumValues	Current frequency spectrum values
	* @param[out] BPMCurrent		Average BPM (Partial)
	* @param[out] BPMTotal			Average BPM (Total)
	* @param OverrideThreshold		Overrides beat configuration threshold for each band
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Result|ByChannel")
		void GetBeatTrackingWThresholdByChannel(TArray<bool>& IsBass, TArray<bool>& IsLowM, TArray<bool>& IsHighM, UChannelFResult*& SpectrumValues, UChannelIResult*& BPMCurrent, UChannelIResult*& BPMTotal, const TArray<float>& OverrideThreshold);

	/** Returns the beat tracking result for the InitBeatTrackingConfigWLimits BandLimits. Only for channel selection mode Split_all
	* @param[out] Beats				Beat result for BandLimits frequency ranges
	* @param[out] SpectrumValues	Current frequency spectrum values
	* @param[out] BPMCurrent		Average BPM (Partial)
	* @param[out] BPMTotal			Average BPM (Total)
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Result|ByChannel")
		void GetBeatTrackingWLimitsByChannel(UChannelBResult*& Beats, UChannelFResult*& SpectrumValues, UChannelIResult*& BPMCurrent, UChannelIResult*& BPMTotal);

	/** Returns the beat tracking result for the InitBeatTrackingConfigWLimits BandLimits. Only for channel selection mode Split_all
	* @param[out] Beats				Beat result for BandLimits frequency ranges
	* @param[out] SpectrumValues	Current frequency spectrum values
	* @param[out] BPMCurrent		Average BPM (Partial)
	* @param[out] BPMTotal			Average BPM (Total)
	* @param OverrideThreshold		Overrides beat configuration threshold
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Result|ByChannel")
		void GetBeatTrackingWLimitsWThresholdByChannel(UChannelBResult*& Beats, UChannelFResult*& SpectrumValues, UChannelIResult*& BPMCurrent, UChannelIResult*& BPMTotal, const TArray<float>& OverrideThreshold);

	/** Returns the average frequency spectrum for the beat tracking history. Only for channel selection mode Split_all
	* @param[out] AverageFrequencies	Average frequency spectrum result by channel
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Result|ByChannel")
		void GetBeatTrackingAverageByChannel(UChannelFResult*& AverageFrequencies);

	/** Returns the variance and average frequency spectrum for the beat tracking history. Only for channel selection mode Split_all
	* @param[out] VarianceFrequencies	Variance frequency spectrum result by channel
	* @param[out] AverageFrequencies	Average frequency spectrum result by channel
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Result|ByChannel")
		void GetBeatTrackingAverageAndVarianceByChannel(UChannelFResult*& AverageFrequencies, UChannelFResult*& VarianceFrequencies);

	/** Calculates and obtains the pitch tracking analysis result for the current spectrum. Only for channel selection mode Split_all
	* @param[out] FundamentalFrequency	Fundamental frequency (F0), Pitch tracking must be enabled (InitSpectrumConfig)
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Result|ByChannel")
		void GetPitchTrackingByChannel(UChannelFResult*& FundamentalFrequency);
	/**@}*/

	/** @name Extractor Analysis results */
	/**@{*/

	/** Returns amplitude result for an specific section analysis of the sound
	* @param[out] Amplitude		Amplitude array result
	* @param ChannelMode		Channel selection mode
	* @param Channel			Number of channel (only for Select_one channel selection mode)
	* @param StartTime			Start position of the section (seconds)
	* @param EndTime			End position of the section (seconds)
	* @param Resolution			Number of samples by bar
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Extractor|Result")
		void GetAmplitudeSection(TArray<float>& Amplitude, EChannelSelectionMode ChannelMode, int32 Channel, float StartTime, float EndTime, int Resolution=1);

	/** Returns amplitude result for an specific section analysis of the sound
	* @param[out] Amplitude		Amplitude array result by channels
	* @param StartTime			Start position of the section (seconds)
	* @param EndTime			End position of the section (seconds)
	* @param Resolution			Number of samples by bar
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Analysis|Extractor|Result|ByChannel")
		void GetAmplitudeSectionByChannel(UChannelFResult*& Amplitude, float StartTime, float EndTime, int Resolution = 1);
	/**@}*/

	/** @name Utils */
	/**@{*/

	/**
	* Returns installation android project folder path
	* @return Project folder
	*/
	UFUNCTION(BlueprintPure, Category = "AudioAnalyzerManager|Utils")
		static FString GetAndroidProjectFolder();

	/**
	* Returns the names of files in the specified directory.
	* @param FolderPath				Directory path
	* @param Extension				Extension file filter
	* @param[out] FileNames			List of files
	* @return						Operation result
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Utils")
		static bool GetFolderFiles(const FString& FolderPath, const FString& Extension, TArray<FString>& FileNames);

	/**
	* Delete all files from a folder that match the extension
	* @param FolderPath				Directory path
	* @param Extension				Extension file filter
	* @return						Operation result
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Utils")
		static bool DeleteFolderFilesByExtension(const FString& FolderPath, const FString& Extension);

	/**
	* Delete all files from a folder that match the filenames list
	* @param FolderPath			Directory path
	* @param FileNames			List of files
	* @return					Operation result
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Utils")
		static bool DeleteFolderFilesByFilename(const FString& FolderPath,const TArray<FString>& FileNames);

	/**
	* Create folder directory if not exists
	* @param FolderPath				Directory path
	* @return						Operation result, if already exists return false
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Utils")
		static bool CreateDirectoryTree(const FString& FolderPath);

	/**
	* Delete folder if empty. Disabling OnlyIfEmpty will delete the folder recursively
	* @param FolderPath				Directory path
	* @param OnlyIfEmpty			Delete only if the directory is empty
	* @return						Operation result
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Utils")
		static bool DeleteFolder(const FString& FolderPath, bool OnlyIfEmpty = true);

	/**
	* Checks if folder exists
	* @param FolderPath				Directory path
	* @return						Operation result
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Utils")
		static bool FolderExists(const FString& FolderPath);

	/**
	* Returns a list with the output audio device friendly names
	* @param[out] AudioDeviceNames		Audio device names
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Utils")
		void GetOutputAudioDevices(TArray<FString>& AudioDeviceNames);

	/**
	* Returns a list with the input audio device friendly names
	* @param[out] AudioDeviceNames		Audio device names
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Utils")
		void GetInputAudioDevices(TArray<FString>& AudioDeviceNames);

	/**
	* Converts a frequency value in Hz into MIDI note value
	* @param[out] Note		MIDI note value
	* @param[out] Cents		Note dispersion
	* @param FrequencyHz	Frequency value in Hz
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Utils")
		static void FrequencyToMIDI(int32& Note, int32& Cents, const float& FrequencyHz);

	/**
	* Converts amplitude values to decibel scale
	* @param[out] DB		Decibel value
	* @param Amplitude		Amplitude normalized value
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Utils")
		static void AmplitudeToDB(float& DB, const float& Amplitude);
	/**@}*/

	/** @name Decoder config */
	/**@{*/
	/**
	* Enables or disables metadata load
	* @param EnableMetadataLoad				Enable metadata load
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Decoder")
		void SetEnableMetadataLoad(bool EnableMetadataLoad);

	/**
	* Check if there is an async initialization running
	* @return bool 							An async initialization is running
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|Decoder")
		bool IsAsyncInitAudioRunning() const;

	/**@}*/

	/**
	* Construcrtor
	*/
	UAudioAnalyzerManager();

	/**
	* Destructor
	*/
	~UAudioAnalyzerManager();

	/**
	* Local event broadcast OnInitPlayerAudioFinished
	* @param Result			Initialization result
	*/
	void EBOnInitPlayerAudioFinished(const bool& Result);

	/**
	* Local event broadcast OnInitExtractorAudioFinished
	* @param Result			Initialization result
	*/
	void EBOnInitExtractorAudioFinished(const bool& Result);

	/**
	* Local event broadcast OnConstructSoundWaveFinished
	* @param Result			Initialization result
	*/
	void EBOnConstructSoundWaveFinished(const bool& Result);

	/**
	* Local event broadcast OnPlaybackFinished
	*/
	void EBOnPlaybackFinished() { EventPlaybackBroadcast(OnPlaybackFinished); }

	/**
	* Local event broadcast OnPlaybackLoopFinished
	*/
	void EBOnPlaybackLoopFinished() { EventPlaybackBroadcast(OnPlaybackLoopFinished); }

	/**
	* Local event broadcast OnAudioComponentPlaybackFinished
	*/
	void EBOnAudioComponentPlaybackFinished() { EventPlaybackBroadcast(OnAudioComponentPlaybackFinished); }

	/**
	* Local event broadcast OnAudioComponentPlaybackLoopFinished
	*/
	void EBOnAudioComponentPlaybackLoopFinished() { EventPlaybackBroadcast(OnAudioComponentPlaybackLoopFinished); }

	/**
	* Local event broadcast OnAudioComponentStopFinished
	*/
	void EBOnAudioComponentStopFinished() { EventPlaybackBroadcast(OnAudioComponentStopFinished); }

	/**
	* Local event broadcast OnCapturedData
	* @param Buffer		Output pcm audio buffer
	*/
	void EBOnCapturedData(const TArray<uint8>& Buffer) { EventCaptureBroadcast(OnCapturedData, Buffer); }

	/**
	* Local event broadcast OnLoopbackCapturedData
	* @param Buffer		Output pcm audio buffer
	*/
	void EBOnLoopbackCapturedData(const TArray<uint8>& Buffer) { EventCaptureBroadcast(OnLoopbackCapturedData, Buffer); }

private:

	/**
	* Audio Analyzer Core instance
	*/
	AudioAnalyzerCore		_AACore;
	/**
	* Audio Analyzer Player instance
	*/
	AudioAnalyzerPlayer		_AAPlayer;
	/**
	* Audio Analyzer Capturer instance
	*/
	AudioAnalyzerCapturer	_AACapturer;
	/**
	* Audio Analyzer Loopback instance
	*/
	AudioAnalyzerLoopback	_AALoopback;
	/**
	* Audio Analyzer Stream instance
	*/
	AudioAnalyzerStream		_AAStream;
	/**
	* Audio Analyzer Extractor instance
	*/
	AudioAnalyzerExtractor	_AAExtractor;
	/**
	* Audio Analyzer OVR Capturer instance
	*/
	AudioAnalyzerOVR		_AAOVR;

	/**
	* Amplitude container result by channel
	*/
	UPROPERTY()
		UChannelFResult* _amplitudeByChannel;

	/**
	* Frequency container result by channel
	*/
	UPROPERTY()
		UChannelFResult* _frequencyByChannel;
	/**
	* Average Ffequency container result by channel
	*/
	UPROPERTY()
		UChannelFResult* _averageFrequencyByChannel;
	/**
	* Frequency peak container result by channel
	*/
	UPROPERTY()
		UChannelFResult* _frequencyPeaksByChannel;
	/**
	* Frequency peak energy container result by channel
	*/
	UPROPERTY()
		UChannelFResult* _frequencyPeaksEnergyByChannel;

	/**
	* Beat container result by channel
	*/
	UPROPERTY()
		UChannelBResult* _beatOnsetByChannel;

	/**
	* Frequency container result by channel (BT analysis)
	*/
	UPROPERTY()
		UChannelFResult* _frequencyBTByChannel;
	/**
	* Average Frequency container result by channel (BT analysis)
	*/
	UPROPERTY()
		UChannelFResult* _averageFrequencyBTByChannel;
	/**
	* Variance Frequency container result by channel (BT analysis)
	*/
	UPROPERTY()
		UChannelFResult* _varianceFrequencyBTByChannel;

	/**
	* BPM partial results by Channel
	*/
	UPROPERTY()
		UChannelIResult* _BPMCurrentByChannel;
	/**
	* BPM total results by Channel
	*/
	UPROPERTY()
		UChannelIResult* _BPMTotalByChannel;

	/**
	* Pitch Tracking result by channel
	*/
	UPROPERTY()
		UChannelFResult* _pitchTrackingByChannel;

	/**
	* Sound Wave object
	*/
	UPROPERTY()
		UAudioAnalyzerSoundWave* _soundWave;

	/**
	* 
	* @param AudioAnalyzerEvent		Delegate event
	*/
	static void EventPlaybackBroadcast(FPlaybackDelegate& AudioAnalyzerEvent);

	/**
	*
	* @param AudioAnalyzerEvent		Delegate event
	* @param Buffer					Event buffer parameter
	*/
	static void EventCaptureBroadcast(FCaptureDelegate& AudioAnalyzerEvent, const TArray<uint8>& Buffer);

	/**
	*
	* @param AudioAnalyzerEvent		Delegate event
	* @param InitResult				Event success result
	*/
	static void EventInitAudioBroadcast(FInitAudioDelegate& AudioAnalyzerEvent, const bool& InitResult);

	/**
	*
	* @param AudioAnalyzerEvent		Delegate event
	* @param SoundWave				Event SoundWave result
	* @param ConstructResult		Event success result
	*/
	static void EventConstructSoundWaveBroadcast(FConstructSoundWaveDelegate& AudioAnalyzerEvent, UAudioAnalyzerSoundWave*& SoundWave, const bool& ConstructResult);

};