// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#include "AudioAnalyzerPlayer.h"

#include "AudioAnalyzerPlayerUnitFile.h"

#include "Async/Async.h"
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>

#include "miniaudio.h"

AudioAnalyzerPlayer::AudioAnalyzerPlayer():_asyncInitRunning(false)
{
}

AudioAnalyzerPlayer::~AudioAnalyzerPlayer()
{
	unloadAudio();
}

bool AudioAnalyzerPlayer::initializeAudio(const FString& filePath, uint32 periodSizeInFrames, uint32 periodSizeInMilliseconds, uint32 periods)
{
	if (!_asyncInitRunning)
	{
		if (_audioPlayerUnit.get())
		{
			_audioPlayerUnit->unloadAudio();
		}

		if (_AADecoder.loadAudioInfo(filePath))
		{
			_AADecoder.moveAudioInfo(_audioInfo);
			_audioPlayerUnit.reset(new AudioAnalyzerPlayerUnitFile(this));

			if (_audioPlayerUnit.get())
			{
				return _audioPlayerUnit->loadAudio(periodSizeInFrames, periodSizeInMilliseconds, periods);
			}
		}
		else
		{
			_audioInfo = nullptr;
			UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Error loading Player Audio info or missing file"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Already running an async initialization"));
	}

	return false;
}

bool AudioAnalyzerPlayer::asyncInitializeAudio(const FString& filePath, uint32 periodSizeInFrames, uint32 periodSizeInMilliseconds, uint32 periods)
{
	if (!_asyncInitRunning)
	{
		_asyncInitRunning = true;
		if (_audioPlayerUnit.get())
		{
			_audioPlayerUnit->unloadAudio();
		}

		AsyncTask(ENamedThreads::AnyThread, [&, filePath, periodSizeInFrames, periodSizeInMilliseconds, periods]()
		{
			bool isOk = false;
			if (_AADecoder.loadAudioInfo(filePath))
			{
				_AADecoder.moveAudioInfo(_audioInfo);
				_audioPlayerUnit.reset(new AudioAnalyzerPlayerUnitFile(this));

				if (_audioPlayerUnit.get())
				{
					isOk = _audioPlayerUnit->loadAudio(periodSizeInFrames, periodSizeInMilliseconds, periods);
				}
			}
			else
			{
				_audioInfo = nullptr;
				UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Error loading Player Audio info or missing file"));
			}

			onInitAudioEnd(isOk);
			_asyncInitRunning = false;
		});
		return true;
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Already running an async initialization"));
		return false;
	}
}

void AudioAnalyzerPlayer::unloadAudio()
{
	if (_audioPlayerUnit.get())
	{
		_audioPlayerUnit->unloadAudio();
	}
}

void AudioAnalyzerPlayer::play(int loops, float startTime)
{
	if (_audioPlayerUnit.get())
	{
		_audioPlayerUnit->play(loops, startTime);
	}
}

void AudioAnalyzerPlayer::stop()
{
	if (_audioPlayerUnit.get())
	{
		_audioPlayerUnit->stop();
	}
}

void AudioAnalyzerPlayer::pause(bool forcePause)
{
	if (_audioPlayerUnit.get())
	{
		_audioPlayerUnit->pause();
	}
}

bool AudioAnalyzerPlayer::isPlaying() const
{
	if (_audioPlayerUnit.get())
	{
		return _audioPlayerUnit->isPlaying();
	}
	return false;
}

bool AudioAnalyzerPlayer::isPaused() const
{
	if (_audioPlayerUnit.get())
	{
		return _audioPlayerUnit->isPaused();
	}
	return false;
}

float AudioAnalyzerPlayer::getPlaybackProgress(float& bufferPosTime)
{
	if (_audioPlayerUnit.get())
	{
		return _audioPlayerUnit->getPlaybackProgress(bufferPosTime);
	}
	bufferPosTime = 0;
	return 0;
}

float AudioAnalyzerPlayer::getPlaybackProgress()
{
	float dummyValue;
	return getPlaybackProgress(dummyValue);
}

void AudioAnalyzerPlayer::setPlaybackProgress(float seconds)
{
	if (_audioPlayerUnit.get())
	{
		_audioPlayerUnit->setPlaybackProgress(seconds);
	}
}

float AudioAnalyzerPlayer::getTotalDuration() const
{
	if (_audioPlayerUnit.get())
	{
		return _audioPlayerUnit->getTotalDuration();
	}
	return 0;
}

float AudioAnalyzerPlayer::getPlaybackVolume()
{
	if (_audioPlayerUnit.get())
	{
		return _audioPlayerUnit->getPlaybackVolume();
	}
	return 0;
}

void AudioAnalyzerPlayer::setPlaybackVolume(float volume)
{
	if (_audioPlayerUnit.get())
	{
		_audioPlayerUnit->setPlaybackVolume(volume);
	}
}

void AudioAnalyzerPlayer::registerOnPlaybackEnd(std::function<void()> callback)
{
	onPlaybackEndCallBack = callback;
}

void AudioAnalyzerPlayer::registerOnPlaybackLoopEnd(std::function<void()> callback)
{
	onPlaybackLoopEndCallBack = callback;
}

void AudioAnalyzerPlayer::adjustBufferSize(float bufferSeconds)
{
	if (_audioPlayerUnit.get())
	{
		_audioPlayerUnit->setAudioBufferSize(bufferSeconds);
	}
}

void AudioAnalyzerPlayer::setEnableMetadataLoad(bool enableMetadataLoad)
{
	_AADecoder.setEnableMetadataLoad(enableMetadataLoad);
}

void AudioAnalyzerPlayer::getMetadata(FString& Filename, FString& Extension, FString& MetaType, FString& Title, FString& Artist, FString& Album, FString& Year, FString& Genre)
{
	_AADecoder.getMetadata(Filename, Extension, MetaType, Title, Artist, Album, Year, Genre);
}

void AudioAnalyzerPlayer::getMetadataArt(const FString& Prefix, const FString& Folder, int32& NumberOfPictures)
{
	_AADecoder.getMetadataArt(Prefix, Folder, NumberOfPictures);
}

void AudioAnalyzerPlayer::getOutputAudioDeviceNames(TArray<FString>& audioDeviceNames)
{
	ma_context context;
	ma_device_info* pPlaybackDeviceInfos;
	ma_uint32 playbackDeviceCount;
	ma_device_info* pCaptureDeviceInfos;
	ma_uint32 captureDeviceCount;

	if (ma_context_init(NULL, 0, NULL, &context) == MA_SUCCESS) 
	{
		ma_result result = ma_context_get_devices(&context, &pPlaybackDeviceInfos, &playbackDeviceCount, &pCaptureDeviceInfos, &captureDeviceCount);
		if (result == MA_SUCCESS)
		{
			audioDeviceNames.Init("", playbackDeviceCount + 1);
			audioDeviceNames[0] = AudioAnalyzerDeviceInfo::SYSTEM_DEVICE_NAME.c_str();

			for (ma_uint32 currentDevice = 0; currentDevice < playbackDeviceCount; ++currentDevice)
			{
				audioDeviceNames[currentDevice + 1] = UTF8_TO_TCHAR(pPlaybackDeviceInfos[currentDevice].name);
			}
		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Failed to retrieve device information"));
		}

		ma_context_uninit(&context);
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Failed to initialize context"));
	}
}

void AudioAnalyzerPlayer::setDefaultAudioDevice(const FString& audioDeviceName)
{
	_deviceInfo.name = TCHAR_TO_UTF8(*audioDeviceName);
}

void AudioAnalyzerPlayer::getDefaultAudioDevice(FString& audioDeviceName)
{
	audioDeviceName = UTF8_TO_TCHAR(_deviceInfo.name.c_str());
}

const int AudioAnalyzerPlayer::getRemainingLoops() const
{
	if (_audioPlayerUnit.get())
	{
		return _audioPlayerUnit->getRemainingLoops();
	}
	return 0;
}

bool AudioAnalyzerPlayer::hasOnlyHeader() const
{
	return _AADecoder.hasOnlyHeader();
}

const AudioAnalyzerDeviceInfo& AudioAnalyzerPlayer::cgetAudioDeviceInfo() const
{
	return _deviceInfo;
}

AudioAnalyzerDeviceInfo& AudioAnalyzerPlayer::getAudioDeviceInfo()
{
	return _deviceInfo;
}

void AudioAnalyzerPlayer::registerOnInitAudioEnd(std::function<void(bool)> callback)
{
	onInitAudioEnd = callback;
}