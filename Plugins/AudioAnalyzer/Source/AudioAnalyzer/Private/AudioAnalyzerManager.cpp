// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#include "AudioAnalyzerManager.h"

#include "HAL/FileManager.h"
#include "Runtime/Core/Public/Misc/Paths.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundWaveProcedural.h"
#include "Async/Async.h"
#include "Misc/App.h"

#include <functional>
#include <math.h>

//float container
UChannelFResult::UChannelFResult()
{
}

UChannelFResult::~UChannelFResult()
{
}

const int32 UChannelFResult::GetNumChannels() const
{
	return _values.Num();
}

const TArray<float>& UChannelFResult::GetChannelResult(const int32 Channel) const
{
	return _values[Channel];
}

TArray<TArray<float> >& UChannelFResult::getContainer()
{
	return _values;
}

//int32 container
UChannelIResult::UChannelIResult()
{
}

UChannelIResult::~UChannelIResult()
{
}

const int32 UChannelIResult::GetNumChannels() const
{
	return _values.Num();
}

const TArray<int32>& UChannelIResult::GetChannelResult(const int32 Channel) const
{
	return _values[Channel];
}

TArray<TArray<int32> >& UChannelIResult::getContainer()
{
	return _values;
}

//bool
UChannelBResult::UChannelBResult()
{
}

UChannelBResult::~UChannelBResult()
{
}

const int32 UChannelBResult::GetNumChannels() const
{
	return _values.Num();
}

const TArray<bool>& UChannelBResult::GetChannelResult(const int32 Channel) const
{
	return _values[Channel];
}

TArray<TArray<bool> >& UChannelBResult::getContainer()
{
	return _values;
}


UAudioAnalyzerManager::UAudioAnalyzerManager()
{
	_AAPlayer.registerOnPlaybackEnd(std::bind(&UAudioAnalyzerManager::EBOnPlaybackFinished, this));
	_AAPlayer.registerOnPlaybackLoopEnd(std::bind(&UAudioAnalyzerManager::EBOnPlaybackLoopFinished,this));
	_AAPlayer.registerOnInitAudioEnd(std::bind(&UAudioAnalyzerManager::EBOnInitPlayerAudioFinished, this, std::placeholders::_1));

	_AAExtractor.registerOnAudioComponentPlaybackEnd(std::bind(&UAudioAnalyzerManager::EBOnAudioComponentPlaybackFinished, this));
	_AAExtractor.registerOnAudioComponentPlaybackLoopEnd(std::bind(&UAudioAnalyzerManager::EBOnAudioComponentPlaybackLoopFinished, this));
	_AAExtractor.registerOnAudioComponentStopFinished(std::bind(&UAudioAnalyzerManager::EBOnAudioComponentStopFinished, this));
	_AAExtractor.registerOnInitAudioEnd(std::bind(&UAudioAnalyzerManager::EBOnInitExtractorAudioFinished, this, std::placeholders::_1));
	_AAExtractor.registerOnConstructSoundWaveEnd(std::bind(&UAudioAnalyzerManager::EBOnConstructSoundWaveFinished, this, std::placeholders::_1));

	_AACapturer.registerOnCapturedData(std::bind(&UAudioAnalyzerManager::EBOnCapturedData, this, std::placeholders::_1));
	_AALoopback.registerOnCapturedData(std::bind(&UAudioAnalyzerManager::EBOnLoopbackCapturedData, this, std::placeholders::_1));

	_amplitudeByChannel = CreateDefaultSubobject<UChannelFResult>(TEXT("AmplitudeByChannel"));
	_frequencyByChannel = CreateDefaultSubobject<UChannelFResult>(TEXT("FrequencyByChannel"));
	_averageFrequencyByChannel = CreateDefaultSubobject<UChannelFResult>(TEXT("AvFrequencyByChannel"));
	_frequencyPeaksByChannel = CreateDefaultSubobject<UChannelFResult>(TEXT("FrequencyPeaksByChannel"));
	_frequencyPeaksEnergyByChannel = CreateDefaultSubobject<UChannelFResult>(TEXT("FrequencyPeaksEnergyByChannel"));

	_beatOnsetByChannel = CreateDefaultSubobject<UChannelBResult>(TEXT("BeatByChannel"));
	_frequencyBTByChannel = CreateDefaultSubobject<UChannelFResult>(TEXT("FrequencyBTByChannel"));
	_averageFrequencyBTByChannel = CreateDefaultSubobject<UChannelFResult>(TEXT("AvFrequencyBTByChannel"));
	_varianceFrequencyBTByChannel = CreateDefaultSubobject<UChannelFResult>(TEXT("VaFrequencyBTByChannel"));
	_BPMCurrentByChannel = CreateDefaultSubobject<UChannelIResult>(TEXT("BPMCurrentByChannel"));
	_BPMTotalByChannel = CreateDefaultSubobject<UChannelIResult>(TEXT("BPMTotalByChannel"));

	_pitchTrackingByChannel = CreateDefaultSubobject<UChannelFResult>(TEXT("PitchTrackingByChannel"));

	_soundWave = CreateDefaultSubobject<UAudioAnalyzerSoundWave>(TEXT("AASoundWave"));
}

UAudioAnalyzerManager::~UAudioAnalyzerManager()
{
}

/***************************************************/
/*                      PLAYER                     */
/***************************************************/

bool UAudioAnalyzerManager::InitPlayerAudio(const FString& FileName)
{
	return InitPlayerAudioEx(FileName, 0, 0, 0);
}

bool UAudioAnalyzerManager::InitPlayerAudioEx(const FString& FileName, int32 PeriodSizeInFrames, int32 PeriodSizeInMilliseconds, int32 Periods)
{
	if (_AAPlayer.initializeAudio(FileName, PeriodSizeInFrames, PeriodSizeInMilliseconds, Periods))
	{
		_AACore.setAudioSource(&_AAPlayer);
		return true;
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Error on Audio Player Initialization"));
		return false;
	}
}

bool UAudioAnalyzerManager::AsyncInitPlayerAudio(const FString& FileName, int32 PeriodSizeInFrames, int32 PeriodSizeInMilliseconds, int32 Periods)
{
	if (!IsAsyncInitAudioRunning())
	{
		return _AAPlayer.asyncInitializeAudio(FileName, PeriodSizeInFrames, PeriodSizeInMilliseconds, Periods);
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("An Async Initialization is already running"));
		return false;
	}
}

bool UAudioAnalyzerManager::IsAsyncInitPlayerAudioRunning() const
{
	return _AAPlayer.isAsyncInitRunning();
}

void UAudioAnalyzerManager::UnloadPlayerAudio()
{
	_AACore.setAudioSource(NULL);
	_AAPlayer.unloadAudio();
}

void UAudioAnalyzerManager::Play()
{
	_AAPlayer.play(1, 0.0);
}

void UAudioAnalyzerManager::PlayEx(int32 Loops, float StartTime)
{
	_AAPlayer.play(Loops, StartTime);
}

void UAudioAnalyzerManager::SetPaused(bool Pause)
{
	_AAPlayer.pause(Pause);
}

void UAudioAnalyzerManager::Pause()
{
	_AAPlayer.pause();
}

void UAudioAnalyzerManager::Stop()
{
	_AAPlayer.stop();
	_AACore.resetBPMCounters();
}

bool UAudioAnalyzerManager::IsPlaying() const
{
	return _AAPlayer.isPlaying();
}

bool UAudioAnalyzerManager::IsPaused() const
{
	return _AAPlayer.isPaused();
}

void UAudioAnalyzerManager::EventPlaybackBroadcast(FPlaybackDelegate& AudioAnalyzerEvent)
{
	AsyncTask(ENamedThreads::GameThread, [&]()
	{
		if (AudioAnalyzerEvent.IsBound())
		{
			AudioAnalyzerEvent.Broadcast();
		}
	});
}

void UAudioAnalyzerManager::EventCaptureBroadcast(FCaptureDelegate& AudioAnalyzerEvent, const TArray<uint8>& Buffer)
{
	AsyncTask(ENamedThreads::GameThread, [AudioAnalyzerEvent, Buffer]()
	{
		if (AudioAnalyzerEvent.IsBound())
		{
			AudioAnalyzerEvent.Broadcast(Buffer);
		}
	});
}

void UAudioAnalyzerManager::EventInitAudioBroadcast(FInitAudioDelegate& AudioAnalyzerEvent, const bool& Result)
{
	AsyncTask(ENamedThreads::GameThread, [AudioAnalyzerEvent, Result]()
	{
		if (AudioAnalyzerEvent.IsBound())
		{
			AudioAnalyzerEvent.Broadcast(Result);
		}
	});
}

void UAudioAnalyzerManager::EventConstructSoundWaveBroadcast(FConstructSoundWaveDelegate& AudioAnalyzerEvent, UAudioAnalyzerSoundWave*& SoundWave, const bool& Result)
{
	AsyncTask(ENamedThreads::GameThread, [AudioAnalyzerEvent, SoundWave, Result]()
	{
		if (AudioAnalyzerEvent.IsBound())
		{
			AudioAnalyzerEvent.Broadcast(SoundWave, Result);
		}
	});
}

void UAudioAnalyzerManager::SetPlaybackTime(float Progress)
{
	_AAPlayer.setPlaybackProgress(Progress);
}

float UAudioAnalyzerManager::GetPlaybackTime()
{
	return _AAPlayer.getPlaybackProgress();
}

float UAudioAnalyzerManager::GetPlaybackVolume()
{
	return _AAPlayer.getPlaybackVolume();
}

void UAudioAnalyzerManager::SetPlaybackVolume(float Volume)
{
	_AAPlayer.setPlaybackVolume(Volume);
}

float UAudioAnalyzerManager::GetTotalDuration() const
{
	return _AAPlayer.getTotalDuration();
}

int32 UAudioAnalyzerManager::GetRemainingLoops() const
{
	return _AAPlayer.getRemainingLoops();
}

void UAudioAnalyzerManager::GetMetadata(FString& Filename, FString& Extension, FString& MetaType, FString& Title, FString& Artist, FString& Album, FString& Year, FString& Genre)
{
	_AAPlayer.getMetadata(Filename, Extension, MetaType, Title, Artist, Album, Year, Genre);
}

void UAudioAnalyzerManager::GetMetadataArt(const FString& Prefix, const FString& Folder, int32& NumberOfPictures)
{
	_AAPlayer.getMetadataArt(Prefix, Folder, NumberOfPictures);
}

void UAudioAnalyzerManager::SetDefaultDevicePlayerAudio(const FString& AudioDeviceName)
{
	_AAPlayer.setDefaultAudioDevice(AudioDeviceName);
}

void UAudioAnalyzerManager::GetDefaultDevicePlayerAudio(FString& AudioDeviceName)
{
	_AAPlayer.getDefaultAudioDevice(AudioDeviceName);
}

void UAudioAnalyzerManager::GetPlayerDeviceAudioInfo(FString& DeviceName, int32& SampleRate, int32& NumChannels, int32& BufferSamples, EAudioFormat& Format, int32& BitDepth)
{
	const AudioAnalyzerDeviceInfo& deviceInfo = _AAPlayer.cgetAudioDeviceInfo();
	
	DeviceName = deviceInfo.name.c_str();
	SampleRate = deviceInfo.sampleRate;
	NumChannels = deviceInfo.numChannels;
	BufferSamples = deviceInfo.bufferSamples;
	Format = static_cast<EAudioFormat>(deviceInfo.format);
	BitDepth = deviceInfo.bitDepth;
}

/***************************************************/
/*                     CAPTURER                    */
/***************************************************/

bool UAudioAnalyzerManager::InitCapturerAudio()
{
	return InitCapturerAudioEx(44100, EAudioDepth::B_16, EAudioFormat::Signed_Int, 1.0, false, 0, 0, 0);
}

bool UAudioAnalyzerManager::InitCapturerAudioEx(int32 SampleRate, EAudioDepth BitDepth, EAudioFormat AudioFormat, float AudioBufferSeconds, 
	bool InitPlaybackDevice, int32 PeriodSizeInFrames, int32 PeriodSizeInMilliseconds, int32 Periods)
{
	int32 bitdepthDecoded = 16;
	if (BitDepth == EAudioDepth::B_32)
	{
		bitdepthDecoded = 32;
	}

	if (_AACapturer.initializeAudio(SampleRate, bitdepthDecoded, (uint8)AudioFormat, AudioBufferSeconds, InitPlaybackDevice, PeriodSizeInFrames, PeriodSizeInMilliseconds, Periods))
	{
		_AACore.setAudioSource(&_AACapturer);
		return true;
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Error on Audio Capturer Initialization"));
		return false;
	}
}

void UAudioAnalyzerManager::UnloadCapturerAudio()
{
	_AACore.setAudioSource(NULL);
	_AACapturer.unloadAudio();
}

void UAudioAnalyzerManager::StartCapture(bool PlaybackCapture, bool CapturedDataBroadcast)
{
	_AACapturer.start(PlaybackCapture, CapturedDataBroadcast);
}

void UAudioAnalyzerManager::StopCapture()
{
	_AACapturer.stop();
	_AACore.resetBPMCounters();
}

bool UAudioAnalyzerManager::IsCapturing() const
{
	return _AACapturer.isCapturing();
}

float UAudioAnalyzerManager::GetCaptureTime()
{
	return _AACapturer.getPlaybackProgress();
}

void UAudioAnalyzerManager::SetDefaultDevicesCapturerAudio(const FString& CapturerDeviceName, const FString& PlayerDeviceName)
{
	_AACapturer.setDefaultAudioDevices(CapturerDeviceName, PlayerDeviceName);
}

void UAudioAnalyzerManager::GetDefaultDevicesCapturerAudio(FString& CapturerDeviceName, FString& PlayerDeviceName)
{
	_AACapturer.getDefaultAudioDevices(CapturerDeviceName, PlayerDeviceName);
}

void UAudioAnalyzerManager::GetCapturerDevicesAudioInfo(FString& CapturerDeviceName, int32& CapturerSampleRate, int32& CapturerNumChannels, int32& CapturerBufferSamples, EAudioFormat& CapturerFormat, int32& CapturerBitDepth, 
														FString& PlayerDeviceName, int32& PlayerSampleRate, int32& PlayerNumChannels, int32& PlayerBufferSamples, EAudioFormat& PlayerFormat, int32& PlayerBitDepth)
{
	const AudioAnalyzerDeviceInfo& deviceInfo = _AACapturer.getAudioCapturerDeviceInfo();

	CapturerDeviceName = deviceInfo.name.c_str();
	CapturerSampleRate = deviceInfo.sampleRate;
	CapturerNumChannels = deviceInfo.numChannels;
	CapturerBufferSamples = deviceInfo.bufferSamples;
	CapturerFormat = static_cast<EAudioFormat>(deviceInfo.format);
	CapturerBitDepth = deviceInfo.bitDepth;

	const AudioAnalyzerDeviceInfo& playerDeviceInfo = _AACapturer.getAudioPlayerDeviceInfo();

	PlayerDeviceName = playerDeviceInfo.name.c_str();
	PlayerSampleRate = playerDeviceInfo.sampleRate;
	PlayerNumChannels = playerDeviceInfo.numChannels;
	PlayerBufferSamples = playerDeviceInfo.bufferSamples;
	PlayerFormat = static_cast<EAudioFormat>(playerDeviceInfo.format);
	PlayerBitDepth = playerDeviceInfo.bitDepth;
}

void UAudioAnalyzerManager::GetCaptureVolume(float& CaptureVolume, float& PlaybackVolume)
{
	CaptureVolume = _AACapturer.getCaptureVolume();
	PlaybackVolume = _AACapturer.getPlaybackVolume();
}

void UAudioAnalyzerManager::SetCaptureVolume(float CaptureVolume, float PlaybackVolume)
{
	_AACapturer.setCaptureVolume(CaptureVolume);
	_AACapturer.setPlaybackVolume(PlaybackVolume);
}

/***************************************************/
/*                     LOOPBACK                    */
/***************************************************/

bool UAudioAnalyzerManager::InitLoopbackAudio()
{
	return InitLoopbackAudioEx(2, 44100, EAudioDepth::B_16, EAudioFormat::Signed_Int, 1.0, 0, 0, 0);
}

bool UAudioAnalyzerManager::InitLoopbackAudioEx(int32 NumChannels, int32 SampleRate, EAudioDepth BitDepth, EAudioFormat AudioFormat, float AudioBufferSeconds, 
	int32 PeriodSizeInFrames, int32 PeriodSizeInMilliseconds, int32 Periods)
{
	int32 bitdepthDecoded = 16;
	if (BitDepth == EAudioDepth::B_32)
	{
		bitdepthDecoded = 32;
	}

	if (_AALoopback.initializeAudio(NumChannels, SampleRate, bitdepthDecoded, (uint8)AudioFormat, AudioBufferSeconds, PeriodSizeInFrames, PeriodSizeInMilliseconds, Periods))
	{
		_AACore.setAudioSource(&_AALoopback);
		return true;
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Error on Audio Loopback Initialization"));
		return false;
	}
}

void UAudioAnalyzerManager::UnloadLoopbackAudio()
{
	_AACore.setAudioSource(NULL);
	_AALoopback.unloadAudio();
}

void UAudioAnalyzerManager::StartLoopback(bool CapturedDataBroadcast)
{
	_AALoopback.start(CapturedDataBroadcast);
}

void UAudioAnalyzerManager::StopLoopback()
{
	_AALoopback.stop();
	_AACore.resetBPMCounters();
}

bool UAudioAnalyzerManager::IsLoopbackCapturing() const
{
	return _AALoopback.isCapturing();
}

float UAudioAnalyzerManager::GetLoopbackCaptureTime()
{
	return _AALoopback.getPlaybackProgress();
}

void UAudioAnalyzerManager::SetDefaultDeviceLoopbackAudio(const FString& CapturerDeviceName)
{
	_AALoopback.setDefaultAudioDevice(CapturerDeviceName);
}

void UAudioAnalyzerManager::GetDefaultDeviceLoopbackAudio(FString& CapturerDeviceName)
{
	_AALoopback.getDefaultAudioDevice(CapturerDeviceName);
}

void UAudioAnalyzerManager::GetLoopbackDeviceAudioInfo(FString& DeviceName, int32& SampleRate, int32& NumChannels, int32& BufferSamples, EAudioFormat& Format, int32& BitDepth)
{
	const AudioAnalyzerDeviceInfo& deviceInfo = _AALoopback.getAudioDeviceInfo();

	DeviceName = deviceInfo.name.c_str();
	SampleRate = deviceInfo.sampleRate;
	NumChannels = deviceInfo.numChannels;
	BufferSamples = deviceInfo.bufferSamples;
	Format = static_cast<EAudioFormat>(deviceInfo.format);
	BitDepth = deviceInfo.bitDepth;
}

/***************************************************/
/*                    EXTRACTOR                    */
/***************************************************/

bool UAudioAnalyzerManager::InitExtractorAudio(const FString& FileName, bool OnlyHeader)
{
	if (_AAExtractor.initializeAudio(FileName, OnlyHeader))
	{
		_AACore.setAudioSource(&_AAExtractor);
		return true;
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Error on Audio Extractor Initialization"));
		return false;
	}
}

bool UAudioAnalyzerManager::AsyncInitExtractorAudio(const FString& FileName, bool OnlyHeader)
{
	if (!IsAsyncInitAudioRunning())
	{
		return _AAExtractor.asyncInitializeAudio(FileName, OnlyHeader);
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("An Async Initialization is already running"));
		return false;
	}
}

bool UAudioAnalyzerManager::IsAsyncInitExtractorAudioRunning() const
{
	return _AAExtractor.isAsyncInitRunning();
}

bool UAudioAnalyzerManager::ConstructSoundWave(UAudioAnalyzerSoundWave*& SoundWave, const FString& FileName)
{
	if (_AAExtractor.initializeSoundWave(_soundWave, FileName))
	{
		SoundWave = _soundWave;
		_AACore.setAudioSource(&_AAExtractor);
		return true;
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Error on SoundWave Initialization"));
		return false;
	}
}

bool UAudioAnalyzerManager::AsyncConstructSoundWave(const FString& FileName)
{
	if (!IsAsyncInitAudioRunning())
	{
		return _AAExtractor.asyncInitializeSoundWave(_soundWave, FileName);
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("An Async Initialization is already running"));
		return false;
	}
}

bool UAudioAnalyzerManager::IsAsyncConstructSoundWaveRunning() const
{
	return _AAExtractor.isAsyncInitRunning();
}

void UAudioAnalyzerManager::GetSoundWave(UAudioAnalyzerSoundWave*& SoundWave) const
{
	if (_soundWave->HasParent())
	{
		SoundWave = _soundWave;
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("No valid SoundWave found. Are you missing ConstructSoundWave call?"));
	}
}

void UAudioAnalyzerManager::UnloadExtractorAudio()
{
	_AACore.setAudioSource(NULL);
	_AAExtractor.unloadAudio();
}

void UAudioAnalyzerManager::UnloadSoundWave()
{
	UnloadExtractorAudio();
}

float UAudioAnalyzerManager::GetAudioTotalDuration() const
{
	return _AAExtractor.getTotalDuration();
}

void UAudioAnalyzerManager::GetAudioMetadata(FString& Filename, FString& Extension, FString& MetaType, FString& Title, FString& Artist, FString& Album, FString& Year, FString& Genre)
{
	_AAExtractor.getMetadata(Filename, Extension, MetaType, Title, Artist, Album, Year, Genre);
}

void UAudioAnalyzerManager::GetAudioMetadataArt(const FString& Prefix, const FString& Folder, int32& NumberOfPictures)
{
	_AAExtractor.getMetadataArt(Prefix, Folder, NumberOfPictures);
}

/***************************************************/
/*                     STREAM                      */
/***************************************************/
bool UAudioAnalyzerManager::InitStreamAudio(int32 NumChannels, int32 SampleRate, EAudioDepth BitDepth, EAudioFormat AudioFormat, float AudioBufferSeconds,
	bool InitPlaybackDevice, int32 PeriodSizeInFrames, int32 PeriodSizeInMilliseconds, int32 Periods)
{
	int32 bitdepthDecoded = 16;
	if (BitDepth == EAudioDepth::B_32)
	{
		bitdepthDecoded = 32;
	}

	if (_AAStream.initializeAudio(NumChannels, SampleRate, bitdepthDecoded, (uint8)AudioFormat, AudioBufferSeconds, InitPlaybackDevice, PeriodSizeInFrames, PeriodSizeInMilliseconds, Periods))
	{
		_AACore.setAudioSource(&_AAStream);
		return true;
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Error on Audio Stream Initialization"));
		return false;
	}
}

void UAudioAnalyzerManager::UnloadStreamAudio()
{
	_AACore.setAudioSource(NULL);
	_AAStream.unloadAudio();
}

void UAudioAnalyzerManager::SetDefaultDeviceStreamAudio(const FString& PlayerDeviceName)
{
	_AAStream.setDefaultAudioDevice(PlayerDeviceName);
}

void UAudioAnalyzerManager::GetDefaultDeviceStreamAudio(FString& PlayerDeviceName)
{
	_AAStream.getDefaultAudioDevice(PlayerDeviceName);
}

void UAudioAnalyzerManager::OpenStreamCapture(bool PlaybackCapture)
{
	_AAStream.open(PlaybackCapture);
}

void UAudioAnalyzerManager::FeedStreamCapture(TArray<uint8> streamData)
{
	_AAStream.feed(streamData.GetData(), streamData.Num());
}

void UAudioAnalyzerManager::CloseStreamCapture()
{
	_AAStream.close();
}

bool UAudioAnalyzerManager::IsStreamCapturing() const
{
	return _AAStream.isCapturing();
}

float UAudioAnalyzerManager::GetStreamCaptureTime()
{
	return _AAStream.getPlaybackProgress();
}

void UAudioAnalyzerManager::GetStreamDevicesAudioInfo(FString& PlayerDeviceName, int32& PlayerSampleRate, int32& PlayerNumChannels, int32& PlayerBufferSamples, EAudioFormat& PlayerFormat, int32& PlayerBitDepth)
{
	const AudioAnalyzerDeviceInfo& playerDeviceInfo = _AAStream.getAudioPlayerDeviceInfo();

	PlayerDeviceName = playerDeviceInfo.name.c_str();
	PlayerSampleRate = playerDeviceInfo.sampleRate;
	PlayerNumChannels = playerDeviceInfo.numChannels;
	PlayerBufferSamples = playerDeviceInfo.bufferSamples;
	PlayerFormat = static_cast<EAudioFormat>(playerDeviceInfo.format);
	PlayerBitDepth = playerDeviceInfo.bitDepth;
}

/***************************************************/
/*                       OVR                       */
/***************************************************/

bool UAudioAnalyzerManager::InitOVRAudio(int32 SampleRate, EAudioFormat AudioFormat, float AudioBufferSeconds, bool InitPlaybackDevice,
	int32 PeriodSizeInFrames, int32 PeriodSizeInMilliseconds, int32 Periods)
{
	if (_AAOVR.initializeAudio(SampleRate, (uint8)AudioFormat, AudioBufferSeconds, InitPlaybackDevice, PeriodSizeInFrames, PeriodSizeInMilliseconds, Periods))
	{
		_AACore.setAudioSource(&_AAOVR);
		return true;
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Error on Audio OVR Initialization"));
		return false;
	}
}

void UAudioAnalyzerManager::UnloadOVRAudio()
{
	_AACore.setAudioSource(NULL);
	_AAOVR.unloadAudio();
}

void UAudioAnalyzerManager::StartOVRCapture(bool PlaybackCapturer)
{
	_AAOVR.start(PlaybackCapturer);
}

void UAudioAnalyzerManager::StopOVRCapture()
{
	_AAOVR.stop();
	_AACore.resetBPMCounters();
}

bool UAudioAnalyzerManager::IsOVRCapturing() const
{
	return _AAOVR.isCapturing();
}

float UAudioAnalyzerManager::GetOVRCaptureTime()
{
	return _AAOVR.getPlaybackProgress();
}

void UAudioAnalyzerManager::SetDefaultDevicesOVRAudio(const FString& CapturerDeviceName, const FString& PlayerDeviceName)
{
	_AAOVR.setDefaultAudioDevices(CapturerDeviceName, PlayerDeviceName);
}

void UAudioAnalyzerManager::GetDefaultDevicesOVRAudio(FString& CapturerDeviceName, FString& PlayerDeviceName)
{
	_AAOVR.getDefaultAudioDevices(CapturerDeviceName, PlayerDeviceName);
}

void UAudioAnalyzerManager::GetOVRDevicesAudioInfo(FString& CapturerDeviceName, int32& CapturerSampleRate, int32& CapturerNumChannels, int32& CapturerBufferSamples, EAudioFormat& CapturerFormat, int32& CapturerBitDepth,
	FString& PlayerDeviceName, int32& PlayerSampleRate, int32& PlayerNumChannels, int32& PlayerBufferSamples, EAudioFormat& PlayerFormat, int32& PlayerBitDepth)
{
	const AudioAnalyzerDeviceInfo& deviceInfo = _AAOVR.getAudioCapturerDeviceInfo();

	CapturerDeviceName = deviceInfo.name.c_str();
	CapturerSampleRate = deviceInfo.sampleRate;
	CapturerNumChannels = deviceInfo.numChannels;
	CapturerBufferSamples = deviceInfo.bufferSamples;
	CapturerFormat = static_cast<EAudioFormat>(deviceInfo.format);
	CapturerBitDepth = deviceInfo.bitDepth;

	const AudioAnalyzerDeviceInfo& playerDeviceInfo = _AAOVR.getAudioPlayerDeviceInfo();

	PlayerDeviceName = playerDeviceInfo.name.c_str();
	PlayerSampleRate = playerDeviceInfo.sampleRate;
	PlayerNumChannels = playerDeviceInfo.numChannels;
	PlayerBufferSamples = playerDeviceInfo.bufferSamples;
	PlayerFormat = static_cast<EAudioFormat>(playerDeviceInfo.format);
	PlayerBitDepth = playerDeviceInfo.bitDepth;
}

/***************************************************/
/*                 Audio Component                 */
/***************************************************/
void UAudioAnalyzerManager::LinkAudioComponent(UAudioComponent* AudioComponent)
{
	_AAExtractor.setAudioComponent(AudioComponent);
}

void UAudioAnalyzerManager::UnlinkAudioComponent()
{
	_AAExtractor.setAudioComponent(nullptr);
}

void UAudioAnalyzerManager::PlayAudioComponent(int32 Loops, float StartTime)
{
	_AAExtractor.playAudioComponent(Loops, StartTime);
}

void UAudioAnalyzerManager::StopAudioComponent()
{
	AsyncStopAudioComponent();
}

void UAudioAnalyzerManager::AsyncStopAudioComponent()
{
	_AAExtractor.asyncStopAudioComponent();
	_AACore.resetBPMCounters();
}

void UAudioAnalyzerManager::PauseAudioComponent()
{
	_AAExtractor.pauseAudioComponent();
}

void UAudioAnalyzerManager::SetPausedAudioComponent(bool Pause)
{
	_AAExtractor.pauseAudioComponent(Pause);
}

bool UAudioAnalyzerManager::IsAudioComponentPlaying() const
{
	return _AAExtractor.isAudioComponentPlaying();
}

bool UAudioAnalyzerManager::IsAudioComponentPaused() const
{
	return _AAExtractor.isAudioComponentPaused();
}

void UAudioAnalyzerManager::SetAudioComponentPlaybackTime(float Progress)
{
	_AAExtractor.setAudioComponentPlaybackProgress(Progress);
}

float UAudioAnalyzerManager::GetAudioComponentPlaybackTime()
{
	return _AAExtractor.getAudioComponentPlaybackProgress();
}

float UAudioAnalyzerManager::GetAudioComponentTotalDuration() const
{
	return _AAExtractor.getAudioComponentTotalDuration();
}

int32 UAudioAnalyzerManager::GetAudioComponentRemainingLoops() const
{
	return _AAExtractor.getAudioComponentRemainingLoops();
}

UAudioComponent* UAudioAnalyzerManager::GetLinkedAudioComponent() const
{
	return _AAExtractor.getAudioComponent();
}

/***************************************************/
/*                     ANALYSIS                    */
/***************************************************/

void UAudioAnalyzerManager::InitAmplitudeConfig(EChannelSelectionMode ChannelMode, int32 Channel, int32 NumTimeBands, float TimeWindow)
{
	if (_AACore.hasValidAudio())
	{
		_AACore.initializeAConfig(_amplitudeByChannel->getContainer(), (uint8)ChannelMode, Channel, NumTimeBands, TimeWindow);
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Missing Audio info. Initialize Source Audio (Use InitPlayerAudio or InitCapturerAudio before calling this node)"));
	}
}

void UAudioAnalyzerManager::GetAmplitude(TArray<float>& Amplitude)
{
	if (!_AACore.isAConfigSplitChannel())
	{
		if (_AACore.hasAmplitudeConfig())
		{
			_AACore.getAmplitude(_amplitudeByChannel->getContainer());
			Amplitude = _amplitudeByChannel->getContainer()[0];
		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Missing Amplitude analysis configuration info. Use InitAmplitudeConfig before this node call"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Wrong channel selection mode in InitAmplitudeConfig: Use GetAmplitudeByChannel instead of this node"));
	}
}

void UAudioAnalyzerManager::GetAmplitudeByChannel(UChannelFResult*& AmplitudeByChannel)
{
	if (_AACore.isAConfigSplitChannel())
	{
		if (_AACore.hasAmplitudeConfig() && _amplitudeByChannel)
		{
			_AACore.getAmplitude(_amplitudeByChannel->getContainer());
			AmplitudeByChannel = _amplitudeByChannel;
		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Missing Amplitude analysis configuration info. Use InitAmplitudeConfig before this node call"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Wrong channel selection mode in InitAmplitudeConfig: Use GetAmplitude instead of this node"));
	}
}

void UAudioAnalyzerManager::GetAmplitudeSection(TArray<float>& Amplitude, EChannelSelectionMode ChannelMode, int32 Channel, float StartTime, float EndTime, int Resolution)
{
	if (ChannelMode == EChannelSelectionMode::Split_all)
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Wrong channel selection mode: Use GetAmplitudeSectionByChannel instead of this node"));
	}
	else
	{
		if (Resolution > 0)
		{
			_AACore.getAmplitudeSection(_amplitudeByChannel->getContainer(), (uint8)ChannelMode, Channel, StartTime, EndTime, Resolution);
			Amplitude = _amplitudeByChannel->getContainer()[0];

		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Resolution must be greater than zero"));
		}
	}
}


void UAudioAnalyzerManager::GetAmplitudeSectionByChannel(UChannelFResult*& AmplitudeByChannel, float StartTime, float EndTime, int Resolution)
{
	if (Resolution > 0)
	{
		_AACore.getAmplitudeSection(_amplitudeByChannel->getContainer(), (uint8)EChannelSelectionMode::Split_all, 0, StartTime, EndTime, Resolution);
		AmplitudeByChannel = _amplitudeByChannel;
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Resolution must be greater than zero"));
	}
}

void UAudioAnalyzerManager::InitSpectrumConfig(ESpectrumType type, 
												EChannelSelectionMode ChannelMode, 
												int32 Channel, 
												int32 NumFreqBands, 
												float TimeWindow, 
												int32 HistorySize, 
												bool UsePeakValues, 
												int32 NumPeaks)
{
	if (_AACore.hasValidAudio())
	{
		if (type == ESpectrumType::ST_Log)
		{
			_AACore.initializeSConfig_Log(_frequencyByChannel->getContainer(), 
				_averageFrequencyByChannel->getContainer(),
				_frequencyPeaksByChannel->getContainer(), 
				_frequencyPeaksEnergyByChannel->getContainer(),
				(uint8)ChannelMode, Channel, NumFreqBands, TimeWindow, HistorySize, UsePeakValues, NumPeaks);
		}
		else
		{
			_AACore.initializeSConfig_Linear(_frequencyByChannel->getContainer(),
				_averageFrequencyByChannel->getContainer(), 
				_frequencyPeaksByChannel->getContainer(),
				_frequencyPeaksEnergyByChannel->getContainer(),
				(uint8)ChannelMode, Channel, NumFreqBands, TimeWindow, HistorySize, UsePeakValues, NumPeaks);
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Missing Audio info. Initialize Source Audio (Use InitPlayerAudio or InitCapturerAudio before calling this node)"));
	}
}

void UAudioAnalyzerManager::InitSpectrumConfigWLimits(EChannelSelectionMode ChannelMode, 
														int32 Channel, 
														const TArray<FVector2D>& BandLimits, 
														float TimeWindow,
														int32 HistorySize,
														bool UsePeakValues,
														int32 NumPeaks)
{
	if (_AACore.hasValidAudio())
	{
		_AACore.initializeSConfig_Custom(_frequencyByChannel->getContainer(), 
			_averageFrequencyByChannel->getContainer(), 
			_frequencyPeaksByChannel->getContainer(),
			_frequencyPeaksEnergyByChannel->getContainer(),
			(uint8)ChannelMode, Channel, BandLimits, TimeWindow, HistorySize, UsePeakValues, NumPeaks);
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Missing Audio info. Initialize Source Audio (Use InitPlayerAudio or InitCapturerAudio before calling this node)"));
	}
}

void UAudioAnalyzerManager::GetSpectrum(TArray<float>& Frequencies, TArray<float>& AverageFrequencies, bool CalculateAverageFrequencies)
{
	if (!_AACore.isSConfigSplitChannel())
	{
		if (_AACore.hasFrequencySpectrumConfig())
		{
			_AACore.getSpectrum(_frequencyByChannel->getContainer());
			Frequencies = _frequencyByChannel->getContainer()[0];

			if (CalculateAverageFrequencies)
			{
				_AACore.getSpectrum_Average(_averageFrequencyByChannel->getContainer());
				AverageFrequencies = _averageFrequencyByChannel->getContainer()[0];
			}
		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Missing Frequency Spectrum Configuration. (Use InitSpectrumConfig or InitSpectrumConfigWLimits before calling this node)"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Wrong channel selection mode in InitSpectrumConfig: Use GetSpectrumByChannel instead of this node"));
	}
}

void UAudioAnalyzerManager::GetSpectrumByChannel(UChannelFResult*& Frequencies, UChannelFResult*& AverageFrequencies, bool CalculateAverageFrequencies)
{
	if (_AACore.isSConfigSplitChannel())
	{
		if (_AACore.hasFrequencySpectrumConfig())
		{
			_AACore.getSpectrum(_frequencyByChannel->getContainer());
			Frequencies = _frequencyByChannel;
			if (CalculateAverageFrequencies)
			{
				_AACore.getSpectrum_Average(_averageFrequencyByChannel->getContainer());
				AverageFrequencies = _averageFrequencyByChannel;
			}
		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Missing Frequency Spectrum Configuration. (Use InitSpectrumConfig or InitSpectrumConfigWLimits before calling this node)"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Wrong channel selection mode in InitSpectrumConfig: Use GetSpectrum instead of this node"));
	}
}

void UAudioAnalyzerManager::GetSpectrumPeaks(TArray<float>& FrequencyPeaks, TArray<float>& FrequencyPeaksEnergy)
{
	if (!_AACore.isSConfigSplitChannel())
	{
		if (_AACore.hasFrequencySpectrumConfig())
		{
			_AACore.getSpectrumPeaks(_frequencyPeaksByChannel->getContainer(), _frequencyPeaksEnergyByChannel->getContainer());
			FrequencyPeaks = _frequencyPeaksByChannel->getContainer()[0];
			FrequencyPeaksEnergy = _frequencyPeaksEnergyByChannel->getContainer()[0];
		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Missing Frequency Spectrum Configuration. (Use InitSpectrumConfig or InitSpectrumConfigWLimits before calling this node)"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Wrong channel selection mode in InitSpectrumConfig: Use GetSpectrumPeaksByChannel instead of this node"));
	}
}

void UAudioAnalyzerManager::GetSpectrumPeaksByChannel(UChannelFResult*& FrequencyPeaks, UChannelFResult*& FrequencyPeaksEnergy)
{
	if (_AACore.isSConfigSplitChannel())
	{
		if (_AACore.hasFrequencySpectrumConfig())
		{
			_AACore.getSpectrumPeaks(_frequencyPeaksByChannel->getContainer(), _frequencyPeaksEnergyByChannel->getContainer());
			FrequencyPeaks = _frequencyPeaksByChannel;
			FrequencyPeaksEnergy = _frequencyPeaksEnergyByChannel;
		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Missing Frequency Spectrum Configuration. (Use InitSpectrumConfig or InitSpectrumConfigWLimits before calling this node)"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Wrong channel selection mode in InitSpectrumConfig: Use GetSpectrumPeaks instead of this node"));
	}
}


void UAudioAnalyzerManager::InitBeatTrackingConfig(EChannelSelectionMode ChannelMode, int32 Channel, float TimeWindow, int32 HistorySize, bool CalculateBPM, int32 BPMHistorySize, float Threshold)
{
	if (_AACore.hasValidAudio())
	{
		_AACore.initializeBTConfig(_beatOnsetByChannel->getContainer(), 
									_frequencyBTByChannel->getContainer(),
									_averageFrequencyBTByChannel->getContainer(),
									_varianceFrequencyBTByChannel->getContainer(),
									_BPMCurrentByChannel->getContainer(), 
									_BPMTotalByChannel->getContainer(),
									(uint8)ChannelMode,
									Channel,
									TimeWindow, 
									HistorySize, 
									Threshold,
									CalculateBPM, 
									BPMHistorySize);
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Missing Audio info. Initialize Source Audio (use InitPlayerAudio or InitCapturerAudio before calling this node )"));
	}
}

void UAudioAnalyzerManager::GetBeatTracking(bool& IsBass, bool& IsLowM, bool& IsHighM, TArray<float>& SpectrumValues, TArray<int32>& BPMCurrent, TArray<int32>& BPMTotal)
{
	if (!_AACore.isBTConfigSplitChannel())
	{
		if (_AACore.hasBeatTrackingConfig())
		{
			TArray<float> EmptyArray;
			TArray<bool> IsBassByChannel;
			TArray<bool> IsLowMByChannel;
			TArray<bool> IsHighMByChannel;

			_AACore.getBeatTracking(_beatOnsetByChannel->getContainer(), _frequencyBTByChannel->getContainer(), _BPMCurrentByChannel->getContainer(), _BPMTotalByChannel->getContainer(), EmptyArray);
			_AACore.extractBasicBeatTracking(IsBassByChannel, IsLowMByChannel, IsHighMByChannel, _beatOnsetByChannel->getContainer());

			IsBass = IsBassByChannel[0];
			IsLowM = IsLowMByChannel[0];
			IsHighM = IsHighMByChannel[0];
			SpectrumValues = _frequencyBTByChannel->getContainer()[0];
			BPMCurrent = _BPMCurrentByChannel->getContainer()[0];
			BPMTotal = _BPMTotalByChannel->getContainer()[0];
		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Missing Beat Tracking Configuration. (Use InitBeatTrackingConfig before calling this node)"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Wrong channel selection mode in InitBeatTrackingConfig: Use GetBeatTrackingByChannel instead of this node"));
	}
}

void UAudioAnalyzerManager::GetBeatTrackingWThreshold(bool& IsBass, bool& IsLowM, bool& IsHighM, TArray<float>& SpectrumValues, TArray<int32>& BPMCurrent, TArray<int32>& BPMTotal, const TArray<float>& OverrideThreshold)
{
	if (!_AACore.isBTConfigSplitChannel())
	{
		if (_AACore.hasBeatTrackingConfig())
		{
			TArray<bool> IsBassByChannel;
			TArray<bool> IsLowMByChannel;
			TArray<bool> IsHighMByChannel;

			_AACore.getBeatTracking(_beatOnsetByChannel->getContainer(), _frequencyBTByChannel->getContainer(), _BPMCurrentByChannel->getContainer(), _BPMTotalByChannel->getContainer(), OverrideThreshold);
			_AACore.extractBasicBeatTracking(IsBassByChannel, IsLowMByChannel, IsHighMByChannel, _beatOnsetByChannel->getContainer());

			IsBass = IsBassByChannel[0];
			IsLowM = IsLowMByChannel[0];
			IsHighM = IsHighMByChannel[0];
			SpectrumValues = _frequencyBTByChannel->getContainer()[0];
			BPMCurrent = _BPMCurrentByChannel->getContainer()[0];
			BPMTotal = _BPMTotalByChannel->getContainer()[0];
		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Missing Beat Tracking Configuration. (Use InitBeatTrackingConfig before calling this node)"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Wrong channel selection mode in InitBeatTrackingConfig: Use GetBeatTrackingWThresholdByChannel instead of this node"));
	}
}

void UAudioAnalyzerManager::InitBeatTrackingConfigWLimits(EChannelSelectionMode ChannelMode, int32 Channel, const TArray<FVector2D>& BandLimits, float TimeWindow, int32 HistorySize, bool CalculateBPM, int32 BPMHistorySize, float Threshold)
{
	if (_AACore.hasValidAudio())
	{
		_AACore.initializeBTConfig_Custom(_beatOnsetByChannel->getContainer(),
											_frequencyBTByChannel->getContainer(),
											_averageFrequencyBTByChannel->getContainer(),
											_varianceFrequencyBTByChannel->getContainer(),
											_BPMCurrentByChannel->getContainer(),
											_BPMTotalByChannel->getContainer(),
											(uint8)ChannelMode, 
											Channel, 
											BandLimits, 
											TimeWindow, 
											HistorySize, 
											Threshold, 
											CalculateBPM, 
											BPMHistorySize);
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Missing Audio info. Initialize Source Audio (use InitPlayerAudio or InitCapturerAudio) before calling this node"));
	}
}

void UAudioAnalyzerManager::GetBeatTrackingWLimits(TArray<bool>& Beats, TArray<float>& SpectrumValues, TArray<int32>& BPMCurrent, TArray<int32>& BPMTotal)
{
	if (!_AACore.isBTConfigSplitChannel())
	{
		if (_AACore.hasBeatTrackingConfig())
		{
			TArray<float> EmptyArray;
			_AACore.getBeatTracking(_beatOnsetByChannel->getContainer(), _frequencyBTByChannel->getContainer(), _BPMCurrentByChannel->getContainer(), _BPMTotalByChannel->getContainer(), EmptyArray);
			Beats = _beatOnsetByChannel->getContainer()[0];
			SpectrumValues = _frequencyBTByChannel->getContainer()[0];
			BPMCurrent = _BPMCurrentByChannel->getContainer()[0];
			BPMTotal = _BPMTotalByChannel->getContainer()[0];
		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Missing Beat Tracking Configuration. (Use InitBeatTrackingConfigWLimits before calling this node)"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Wrong channel selection mode in InitBeatTrackingConfigWLimits: Use GetBeatTrackingWLimitsByChannel instead of this node"));
	}
}

void UAudioAnalyzerManager::GetBeatTrackingWLimitsWThreshold(TArray<bool>& Beats, TArray<float>& SpectrumValues, TArray<int32>& BPMCurrent, TArray<int32>& BPMTotal, const TArray<float>& OverrideThreshold)
{
	if (!_AACore.isBTConfigSplitChannel())
	{
		if (_AACore.hasBeatTrackingConfig())
		{
			_AACore.getBeatTracking(_beatOnsetByChannel->getContainer(), _frequencyBTByChannel->getContainer(), _BPMCurrentByChannel->getContainer(), _BPMTotalByChannel->getContainer(), OverrideThreshold);
			Beats = _beatOnsetByChannel->getContainer()[0];
			SpectrumValues = _frequencyBTByChannel->getContainer()[0];
			BPMCurrent = _BPMCurrentByChannel->getContainer()[0];
			BPMTotal = _BPMTotalByChannel->getContainer()[0];
		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Missing Beat Tracking Configuration. (Use InitBeatTrackingConfigWLimits before calling this node)"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Wrong channel selection mode in InitBeatTrackingConfigWLimits: Use GetBeatTrackingWLimitsWThresholdByChannel instead of this node"));
	}
}

void UAudioAnalyzerManager::GetBeatTrackingAverage(TArray<float>& AverageFrequencies)
{
	if (!_AACore.isBTConfigSplitChannel())
	{
		if (_AACore.hasBeatTrackingConfig())
		{
			_AACore.getBeatTracking_Average(_averageFrequencyBTByChannel->getContainer());
			AverageFrequencies = _averageFrequencyBTByChannel->getContainer()[0];
		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Missing Beat Tracking Configuration. (Use InitBeatTrackingConfig of InitBeatTrackingConfigWLimits before calling this node)"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Wrong channel selection mode in InitBeatTrackingConfig: Use GetBeatTrackingAverageByChannel instead of this node"));
	}
}

void UAudioAnalyzerManager::GetBeatTrackingAverageAndVariance(TArray<float>& AverageFrequencies, TArray<float>& VarianceFrequencies)
{
	if (!_AACore.isBTConfigSplitChannel())
	{
		if (_AACore.hasBeatTrackingConfig())
		{
			_AACore.getBeatTracking_AverageAndVariance(_varianceFrequencyBTByChannel->getContainer(), _averageFrequencyBTByChannel->getContainer());
			AverageFrequencies = _averageFrequencyBTByChannel->getContainer()[0];
			VarianceFrequencies = _varianceFrequencyBTByChannel->getContainer()[0];
		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Missing Beat Tracking Configuration. (Use InitBeatTrackingConfig of InitBeatTrackingConfigWLimits before calling this node)"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Wrong channel selection mode in InitBeatTrackingConfig: Use GetBeatTrackingAverageAndVarianceByChannel instead of this node"));
	}
}

void UAudioAnalyzerManager::GetBeatTrackingByChannel(TArray<bool>& IsBass, TArray<bool>& IsLowM, TArray<bool>& IsHighM, UChannelFResult*& SpectrumValues, UChannelIResult*& BPMCurrent, UChannelIResult*& BPMTotal)
{
	if (_AACore.isBTConfigSplitChannel())
	{
		if (_AACore.hasBeatTrackingConfig())
		{
			TArray<float> EmptyArray;
			_AACore.getBeatTracking(_beatOnsetByChannel->getContainer(), _frequencyBTByChannel->getContainer(), _BPMCurrentByChannel->getContainer(), _BPMTotalByChannel->getContainer(), EmptyArray);
			_AACore.extractBasicBeatTracking(IsBass, IsLowM, IsHighM, _beatOnsetByChannel->getContainer());
			SpectrumValues = _frequencyBTByChannel;
			BPMCurrent = _BPMCurrentByChannel;
			BPMTotal = _BPMTotalByChannel;
		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Missing Beat Tracking Configuration. (Use InitBeatTrackingConfig before calling this node)"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Wrong channel selection mode in InitBeatTrackingConfig: Use GetBeatTracking instead of this node"));
	}
}

void UAudioAnalyzerManager::GetBeatTrackingWThresholdByChannel(TArray<bool>& IsBass, TArray<bool>& IsLowM, TArray<bool>& IsHighM, UChannelFResult*& SpectrumValues, UChannelIResult*& BPMCurrent, UChannelIResult*& BPMTotal, const TArray<float>& OverrideThreshold)
{
	if (_AACore.isBTConfigSplitChannel())
	{
		if (_AACore.hasBeatTrackingConfig())
		{
			_AACore.getBeatTracking(_beatOnsetByChannel->getContainer(), _frequencyBTByChannel->getContainer(), _BPMCurrentByChannel->getContainer(), _BPMTotalByChannel->getContainer(), OverrideThreshold);
			_AACore.extractBasicBeatTracking(IsBass, IsLowM, IsHighM, _beatOnsetByChannel->getContainer());
			SpectrumValues = _frequencyBTByChannel;
			BPMCurrent = _BPMCurrentByChannel;
			BPMTotal = _BPMTotalByChannel;
		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Missing Beat Tracking Configuration. (Use InitBeatTrackingConfig before calling this node)"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Wrong channel selection mode in InitBeatTrackingConfig: Use GetBeatTracking instead of this node"));
	}
}

void UAudioAnalyzerManager::GetBeatTrackingWLimitsByChannel(UChannelBResult*& Beats, UChannelFResult*& SpectrumValues, UChannelIResult*& BPMCurrent, UChannelIResult*& BPMTotal)
{
	if (_AACore.isBTConfigSplitChannel())
	{
		if (_AACore.hasBeatTrackingConfig())
		{
			TArray<float> EmptyArray;
			_AACore.getBeatTracking(_beatOnsetByChannel->getContainer(), _frequencyBTByChannel->getContainer(), _BPMCurrentByChannel->getContainer(), _BPMTotalByChannel->getContainer(), EmptyArray);
			Beats = _beatOnsetByChannel;
			SpectrumValues = _frequencyBTByChannel;
			BPMCurrent = _BPMCurrentByChannel;
			BPMTotal = _BPMTotalByChannel;
		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Missing Beat Tracking Configuration. (Use InitBeatTrackingConfigWLimits before calling this node)"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Wrong channel selection mode in InitBeatTrackingConfig: Use GetBeatTrackingWLimits instead of this node"));
	}
}

void UAudioAnalyzerManager::GetBeatTrackingWLimitsWThresholdByChannel(UChannelBResult*& Beats, UChannelFResult*& SpectrumValues, UChannelIResult*& BPMCurrent, UChannelIResult*& BPMTotal, const TArray<float>& OverrideThreshold)
{
	if (_AACore.isBTConfigSplitChannel())
	{
		if (_AACore.hasBeatTrackingConfig())
		{
			_AACore.getBeatTracking(_beatOnsetByChannel->getContainer(), _frequencyBTByChannel->getContainer(), _BPMCurrentByChannel->getContainer(), _BPMTotalByChannel->getContainer(), OverrideThreshold);
			Beats = _beatOnsetByChannel;
			SpectrumValues = _frequencyBTByChannel;
			BPMCurrent = _BPMCurrentByChannel;
			BPMTotal = _BPMTotalByChannel;
		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Missing Beat Tracking Configuration. (Use InitBeatTrackingConfigWLimits before calling this node)"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Wrong channel selection mode in InitBeatTrackingConfig: Use GetBeatTrackingWLimitsWThreshold instead of this node"));
	}
}

void UAudioAnalyzerManager::GetBeatTrackingAverageByChannel(UChannelFResult*& AverageFrequencies)
{
	if (!_AACore.isBTConfigSplitChannel())
	{
		if (_AACore.hasBeatTrackingConfig())
		{
			_AACore.getBeatTracking_Average(_averageFrequencyBTByChannel->getContainer());
			AverageFrequencies = _averageFrequencyBTByChannel;
		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Missing Beat Tracking Configuration. (Use InitBeatTrackingConfig of InitBeatTrackingConfigWLimits before calling this node)"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Wrong channel selection mode in InitBeatTrackingConfig: Use GetBeatTrackingAverage instead of this node"));
	}
}

void UAudioAnalyzerManager::GetBeatTrackingAverageAndVarianceByChannel(UChannelFResult*& AverageFrequencies, UChannelFResult*& VarianceFrequencies)
{
	if (_AACore.isBTConfigSplitChannel())
	{
		if (_AACore.hasBeatTrackingConfig())
		{
			_AACore.getBeatTracking_AverageAndVariance(_varianceFrequencyBTByChannel->getContainer(), _averageFrequencyBTByChannel->getContainer());
			AverageFrequencies = _averageFrequencyBTByChannel;
			VarianceFrequencies = _varianceFrequencyBTByChannel;
		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Missing Beat Tracking Configuration. (Use InitBeatTrackingConfig of InitBeatTrackingConfigWLimits before calling this node)"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Wrong channel selection mode in InitBeatTrackingConfig: Use GetBeatTrackingAverageAndVariance instead of this node"));
	}
}


void UAudioAnalyzerManager::InitPitchTrackingConfig(EChannelSelectionMode ChannelMode,
													int32 Channel,
													float TimeWindow,
													float Threshold)
{
	if (_AACore.hasValidAudio())
	{
			_AACore.initializePTConfig(_pitchTrackingByChannel->getContainer(),
										(uint8)ChannelMode, Channel, TimeWindow, Threshold);
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Missing Audio info. Initialize Source Audio (Use InitPlayerAudio or InitCapturerAudio before calling this node)"));
	}
}


void UAudioAnalyzerManager::GetPitchTracking(float& FundamentalFrequency)
{
	if (!_AACore.isPTConfigSplitChannel())
	{
		if (_AACore.hasPitchTrackingConfig())
		{
			_AACore.getPitchTracking(_pitchTrackingByChannel->getContainer());
			FundamentalFrequency = _pitchTrackingByChannel->getContainer()[0][0];
		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Missing Pitch Tracking Configuration. (Use InitPitchTrackingConfig before calling this node)"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Wrong channel selection mode in InitPitchTrackingConfig: Use GetPitchTrackingByChannel instead of this node"));
	}
}

void UAudioAnalyzerManager::GetPitchTrackingByChannel(UChannelFResult*& FundamentalFrequency)
{
	if (_AACore.isPTConfigSplitChannel())
	{
		if (_AACore.hasPitchTrackingConfig())
		{
			_AACore.getPitchTracking(_pitchTrackingByChannel->getContainer());
			FundamentalFrequency = _pitchTrackingByChannel;
		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Missing Pitch Tracking Configuration. (Use InitPitchTrackingConfig before calling this node)"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Wrong channel selection mode in InitPitchTrackingConfig: Use GetPitchTracking instead of this node"));
	}
}

/***************************************************/
/*                      UTILS                      */
/***************************************************/

FString UAudioAnalyzerManager::GetAndroidProjectFolder()
{
#if PLATFORM_ANDROID
	extern FString GFilePathBase;
	return GFilePathBase + FString("/UE4Game/") + FApp::GetProjectName() + FString("/");
#else
	return FString("");
#endif
}

bool UAudioAnalyzerManager::GetFolderFiles(const FString& FolderPath, const FString& Extension, TArray<FString>& FileNames)
{
	if (!FolderPath.IsEmpty())
	{
		FString NormFolderPath = FolderPath;
		FPaths::NormalizeDirectoryName(NormFolderPath);
		IFileManager& fileManager = IFileManager::Get();

		FString extensionFilter = "*.*";

		if (!Extension.IsEmpty())
		{
			extensionFilter = (Extension.Left(1) == ".") ? "*" + Extension : "*." + Extension;
		}

		FString finalPath = NormFolderPath + "/" + extensionFilter;
		fileManager.FindFiles(FileNames, *finalPath, true, false);
		return true;
	}

	return false;
}

bool UAudioAnalyzerManager::DeleteFolderFilesByExtension(const FString& FolderPath, const FString& Extension)
{
	TArray<FString> fileNames;
	GetFolderFiles(FolderPath, Extension, fileNames);
	return DeleteFolderFilesByFilename(FolderPath, fileNames);
}

bool UAudioAnalyzerManager::DeleteFolderFilesByFilename(const FString& FolderPath, const TArray<FString>& FileNames)
{
	if (FileNames.Num() > 0)
	{
		FString NormFolderPath = FolderPath;
		FPaths::NormalizeDirectoryName(NormFolderPath);
		IFileManager& fileManager = IFileManager::Get();

		for (int index = 0; index < FileNames.Num(); ++index)
		{
			FString filename = NormFolderPath + "/" + FileNames[index];
			fileManager.Delete(*filename);
		}

		return true;
	}

	return false;
}

bool UAudioAnalyzerManager::CreateDirectoryTree(const FString& FolderPath)
{
	FString NormFolderPath = FolderPath;
	FPaths::NormalizeDirectoryName(NormFolderPath);
	IFileManager& fileManager = IFileManager::Get();
	if (!fileManager.DirectoryExists(*NormFolderPath))
	{
		return fileManager.MakeDirectory(*FolderPath, true);
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Directory already exists %s"), *FolderPath);
		return false;
	}
}

bool UAudioAnalyzerManager::DeleteFolder(const FString& FolderPath, bool OnlyIfEmpty)
{
	FString NormFolderPath = FolderPath;
	FPaths::NormalizeDirectoryName(NormFolderPath);
	IFileManager& fileManager = IFileManager::Get();

	if (fileManager.DirectoryExists(*NormFolderPath))
	{
		FString finalPath = NormFolderPath + "/*";
		TArray<FString> FileNames;
		fileManager.FindFiles(FileNames, *finalPath, true, false);
		if (FileNames.Num() > 0)
		{
			if (OnlyIfEmpty)
			{
				UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Directory has files %s"), *FolderPath);
				return false;
			}
			else
			{
				return fileManager.DeleteDirectory(*FolderPath, true, true);
			}
		}
		else
		{
			return fileManager.DeleteDirectory(*FolderPath);
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Missing directory %s"), *FolderPath);
		return false;
	}
}

bool UAudioAnalyzerManager::FolderExists(const FString& FolderPath)
{
	FString NormFolderPath = FolderPath;
	FPaths::NormalizeDirectoryName(NormFolderPath);
	IFileManager& fileManager = IFileManager::Get();
	return fileManager.DirectoryExists(*NormFolderPath);
}

void UAudioAnalyzerManager::GetOutputAudioDevices(TArray<FString>& AudioDeviceNames)
{
	AudioAnalyzerPlayer::getOutputAudioDeviceNames(AudioDeviceNames);
}

void UAudioAnalyzerManager::GetInputAudioDevices(TArray<FString>& AudioDeviceNames)
{
	AudioAnalyzerCapturer::getInputAudioDeviceNames(AudioDeviceNames);
}

void UAudioAnalyzerManager::FrequencyToMIDI(int32& Note, int32& Cents, const float& FrequencyHz)
{
	if (FrequencyHz > 0)
	{
		float fNote = (12.0f * log10(FrequencyHz / 55.0f) * (1.0f / log10(2.0f))) + 33.0f;
		Note = (fNote + 0.5f);
		Cents = ((Note - fNote) * 100);
	}
	else
	{
		Note = 0;
		Cents = 0;
	}
}

void UAudioAnalyzerManager::AmplitudeToDB(float& DB, const float& Amplitude)
{
	if (Amplitude > 0)
	{
		DB = 20 * log10(Amplitude);
	}
	else
	{
		DB = 0.0;
	}
}

void UAudioAnalyzerManager::SetEnableMetadataLoad(bool EnableMetadataLoad)
{
	_AAPlayer.setEnableMetadataLoad(EnableMetadataLoad);
	_AAExtractor.setEnableMetadataLoad(EnableMetadataLoad);
}

void UAudioAnalyzerManager::EBOnInitPlayerAudioFinished(const bool& Result) 
{
	if (Result)
	{
		_AACore.setAudioSource(&_AAPlayer);
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Error on Async Audio Player Initialization"));
	}

	EventInitAudioBroadcast(OnInitPlayerAudioFinished, Result);
}

void UAudioAnalyzerManager::EBOnInitExtractorAudioFinished(const bool& Result)
{
	if (Result)
	{
		_AACore.setAudioSource(&_AAExtractor);
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Error on Async Audio Extractor Initialization"));
	}

	EventInitAudioBroadcast(OnInitExtractorAudioFinished, Result);
}

void UAudioAnalyzerManager::EBOnConstructSoundWaveFinished(const bool& Result)
{
	if (Result)
	{
		_AACore.setAudioSource(&_AAExtractor);
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Error on Async Construct Soundwave Initialization"));
	}

	EventConstructSoundWaveBroadcast(OnConstructSoundWaveFinished, _soundWave, Result);
}

bool UAudioAnalyzerManager::IsAsyncInitAudioRunning() const
{
	return (IsAsyncInitPlayerAudioRunning() || IsAsyncInitExtractorAudioRunning());
}