// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#include "AudioAnalyzerExtractor.h"
#include "AudioAnalyzerSoundWave.h"

#include "Async/Async.h"
#include "Components/AudioComponent.h"
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>

AudioAnalyzerExtractor::AudioAnalyzerExtractor():_soundWave(nullptr), _audioComponent(nullptr), _asyncInitRunning(false)
{
}

AudioAnalyzerExtractor::~AudioAnalyzerExtractor()
{
	unloadAudio();
}

bool AudioAnalyzerExtractor::initializeAudio(const FString& filePath, bool onlyHeader)
{
	if (!_asyncInitRunning)
	{
		unloadAudio();
		if (_AADecoder.loadAudioInfo(filePath, onlyHeader))
		{
			_AADecoder.moveAudioInfo(_audioInfo);
			return true;
		}
		else
		{
			_audioInfo = nullptr;
			UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Error loading Extractor Audio info or missing file"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Already running an async initialization"));
	}

	return false;
}

bool AudioAnalyzerExtractor::asyncInitializeAudio(const FString& filePath, bool onlyHeader)
{
	if (!_asyncInitRunning)
	{
		_asyncInitRunning = true;
		unloadAudio();
		AsyncTask(ENamedThreads::AnyThread, [&, filePath, onlyHeader]()
		{
			bool isOk = false;
			if (_AADecoder.loadAudioInfo(filePath, onlyHeader))
			{
				_AADecoder.moveAudioInfo(_audioInfo);
				isOk = true;
			}
			else
			{
				UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Error loading Extractor Audio info or missing file"));
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

bool AudioAnalyzerExtractor::initializeSoundWave(UAudioAnalyzerSoundWave* soundWave, const FString& filePath)
{
	if (!_asyncInitRunning)
	{
		unloadAudio();
		if (_AADecoder.loadAudioInfo(filePath, false))
		{
			_AADecoder.moveAudioInfo(_audioInfo);
			_soundWave = soundWave;
			if (_soundWave && _audioInfo)
			{
				_soundWave->RawPCMDataSize = _audioInfo->dataChunkSize;
				_soundWave->Duration = (_audioInfo->dataChunkSize / (float)(_audioInfo->numChannels * (_audioInfo->bitDepth / 8))) / _audioInfo->sampleRate;
				_soundWave->SetSampleRate(_audioInfo->sampleRate);
				_soundWave->NumChannels = _audioInfo->numChannels;
				if (_soundWave->NumChannels >= 4)
				{
					_soundWave->bIsAmbisonics = 1;
				}
				_soundWave->bProcedural = true;
				_soundWave->bLooping = false;
				_soundWave->InitializeSoundWave(this);
			}
			return true;
		}
		else
		{
			_audioInfo = nullptr;
			UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Error loading SoundWave Audio info or missing file"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Already running an async initialization"));
	}

	return false;
}

bool AudioAnalyzerExtractor::asyncInitializeSoundWave(UAudioAnalyzerSoundWave* soundWave, const FString& filePath)
{
	if (!_asyncInitRunning)
	{
		_asyncInitRunning = true;
		unloadAudio();
		AsyncTask(ENamedThreads::AnyThread, [&, soundWave, filePath]()
		{
			bool isOk = false;
			if (_AADecoder.loadAudioInfo(filePath, false))
			{
				_AADecoder.moveAudioInfo(_audioInfo);
				_soundWave = soundWave;
				if (_soundWave && _audioInfo)
				{
					_soundWave->RawPCMDataSize = _audioInfo->dataChunkSize;
					_soundWave->Duration = (_audioInfo->dataChunkSize / (float)(_audioInfo->numChannels * (_audioInfo->bitDepth / 8))) / _audioInfo->sampleRate;
					_soundWave->SetSampleRate(_audioInfo->sampleRate);
					_soundWave->NumChannels = _audioInfo->numChannels;
					if (_soundWave->NumChannels >= 4)
					{
						_soundWave->bIsAmbisonics = 1;
					}
					_soundWave->bProcedural = true;
					_soundWave->bLooping = false;
					_soundWave->InitializeSoundWave(this);
				}
				isOk = true;
			}
			else
			{
				UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Error loading SoundWave Audio info or missing file"));
			}

			onConstructSoundWaveEnd(isOk);
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

void AudioAnalyzerExtractor::unloadAudio()
{
	if (_soundWave)
	{
		_soundWave->InvalidateSoundWave();
	}

	_audioInfo = nullptr;
	_soundWave = nullptr;
	_audioComponent = nullptr;
	_AADecoder.freePCMdata();
}

float AudioAnalyzerExtractor::getPlaybackProgress(float& bufferPosTime)
{
	if (_audioComponent && _audioComponent->IsValidLowLevel())
	{
		return getAudioComponentPlaybackProgress(bufferPosTime);
	}
	else if (_soundWave)
	{
		return _soundWave->GetPlaybackProgress(bufferPosTime);
	}
	
	return 0;
}

float AudioAnalyzerExtractor::getTotalDuration() const
{
	if (_audioInfo)
	{
		return (_audioInfo->dataChunkSize / (float)(_audioInfo->numChannels * (_audioInfo->bitDepth / 8) )) / _audioInfo->sampleRate;
	}
	return 0;
}

void AudioAnalyzerExtractor::setEnableMetadataLoad(bool enableMetadataLoad)
{
	_AADecoder.setEnableMetadataLoad(enableMetadataLoad);
}

void AudioAnalyzerExtractor::getMetadata(FString& Filename, FString& Extension, FString& MetaType, FString& Title, FString& Artist, FString& Album, FString& Year, FString& Genre)
{
	_AADecoder.getMetadata(Filename, Extension, MetaType, Title, Artist, Album, Year, Genre);
}

void AudioAnalyzerExtractor::getMetadataArt(const FString& Prefix, const FString& Folder, int32& NumberOfPictures)
{
	_AADecoder.getMetadataArt(Prefix, Folder, NumberOfPictures);
}

bool AudioAnalyzerExtractor::hasOnlyHeader() const
{
	return _AADecoder.hasOnlyHeader();
}

void AudioAnalyzerExtractor::registerOnInitAudioEnd(std::function<void(bool)> callback)
{
	onInitAudioEnd = callback;
}

void AudioAnalyzerExtractor::registerOnConstructSoundWaveEnd(std::function<void(bool)> callback)
{
	onConstructSoundWaveEnd = callback;
}

void AudioAnalyzerExtractor::setAudioComponent(UAudioComponent* audioComponent)
{
	_audioComponent = audioComponent;
}

void AudioAnalyzerExtractor::playAudioComponent(int32 loops, float startTime)
{
	if (_audioComponent && _audioComponent->IsValidLowLevel())
	{
		UAudioAnalyzerSoundWave* soundWave = dynamic_cast<UAudioAnalyzerSoundWave*>(_audioComponent->Sound.Get());
		if (soundWave)
		{
			_audioComponent->Play(0.0);
			soundWave->Play(loops, startTime);
		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("No valid SoundWave attached to the Audio Component, only AudioAnalyzerSoundWave class allowed"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("No valid Audio Component found, are you missing LinkAudioComponent call?"));
	}
}

void AudioAnalyzerExtractor::asyncStopAudioComponent()
{
	if (_audioComponent && _audioComponent->IsValidLowLevel())
	{
		UAudioAnalyzerSoundWave* soundWave = dynamic_cast<UAudioAnalyzerSoundWave*>(_audioComponent->Sound.Get());
		if (soundWave)
		{
			_audioComponent->Stop();
			soundWave->AsyncStopSound();
		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("No valid SoundWave attached to the Audio Component, only AudioAnalyzerSoundWave class allowed"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("No valid Audio Component found, are you missing LinkAudioComponent call?"));
	}
}

void AudioAnalyzerExtractor::pauseAudioComponent(bool forcePause)
{
	if (_audioComponent && _audioComponent->IsValidLowLevel())
	{
		UAudioAnalyzerSoundWave* soundWave = dynamic_cast<UAudioAnalyzerSoundWave*>(_audioComponent->Sound.Get());
		if (soundWave)
		{
			if (isAudioComponentPlaying() && forcePause)
			{
				_audioComponent->SetPaused(true);
				soundWave->SetPaused(true);
			}
			else
			{
				if (_audioComponent->bIsPaused)
				{
					_audioComponent->SetPaused(false);
					soundWave->SetPaused(false);
				}
			}
		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("No valid SoundWave attached to the Audio Component, only AudioAnalyzerSoundWave class allowed"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("No valid Audio Component found, are you missing LinkAudioComponent call?"));
	}
}

bool AudioAnalyzerExtractor::isAudioComponentPlaying() const
{
	if (_audioComponent && _audioComponent->IsValidLowLevel())
	{
		UAudioAnalyzerSoundWave* soundWave = dynamic_cast<UAudioAnalyzerSoundWave*>(_audioComponent->Sound.Get());
		if (soundWave)
		{
			return soundWave->IsPlaying();
		}
	}

	return false;
}

bool AudioAnalyzerExtractor::isAudioComponentPaused() const
{
	if (_audioComponent && _audioComponent->IsValidLowLevel())
	{
		UAudioAnalyzerSoundWave* soundWave = dynamic_cast<UAudioAnalyzerSoundWave*>(_audioComponent->Sound.Get());
		if (soundWave)
		{
			return soundWave->IsPaused();
		}
	}

	return false;
}

float AudioAnalyzerExtractor::getAudioComponentPlaybackProgress(float& bufferPosTime)
{
	if (_audioComponent && _audioComponent->IsValidLowLevel())
	{
		UAudioAnalyzerSoundWave* soundWave = dynamic_cast<UAudioAnalyzerSoundWave*>(_audioComponent->Sound.Get());
		if (soundWave)
		{
			return soundWave->GetPlaybackProgress(bufferPosTime);
		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("No valid SoundWave attached to the Audio Component, only AudioAnalyzerSoundWave class allowed"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("No valid Audio Component found, are you missing LinkAudioComponent call?"));
	}
	return 0;
}

float AudioAnalyzerExtractor::getAudioComponentPlaybackProgress()
{
	float dummyValue;
	return getAudioComponentPlaybackProgress(dummyValue);
}

void AudioAnalyzerExtractor::setAudioComponentPlaybackProgress(float time)
{
	if (_audioComponent && _audioComponent->IsValidLowLevel())
	{
		UAudioAnalyzerSoundWave* soundWave = dynamic_cast<UAudioAnalyzerSoundWave*>(_audioComponent->Sound.Get());
		if (soundWave)
		{
			soundWave->SetPlaybackTime(time);
		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("No valid SoundWave attached to the Audio Component, only AudioAnalyzerSoundWave class allowed"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("No valid Audio Component found, are you missing LinkAudioComponent call?"));
	}
}

float AudioAnalyzerExtractor::getAudioComponentTotalDuration() const
{
	if (_audioComponent && _audioComponent->IsValidLowLevel())
	{
		UAudioAnalyzerSoundWave* soundWave = dynamic_cast<UAudioAnalyzerSoundWave*>(_audioComponent->Sound.Get());
		if (soundWave)
		{
			return soundWave->GetTotalDuration();
		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("No valid SoundWave attached to the Audio Component, only AudioAnalyzerSoundWave class allowed"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("No valid Audio Component found, are you missing LinkAudioComponent call?"));
	}
	return 0;
}

const int AudioAnalyzerExtractor::getAudioComponentRemainingLoops() const
{
	if (_audioComponent && _audioComponent->IsValidLowLevel())
	{
		UAudioAnalyzerSoundWave* soundWave = dynamic_cast<UAudioAnalyzerSoundWave*>(_audioComponent->Sound.Get());
		if (soundWave)
		{
			return soundWave->GetRemainingLoops();
		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("No valid SoundWave attached to the Audio Component, only AudioAnalyzerSoundWave class allowed"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("No valid Audio Component found, are you missing LinkAudioComponent call?"));
	}
	return 0;
}

void AudioAnalyzerExtractor::registerOnAudioComponentPlaybackEnd(std::function<void()> callback)
{
	onAudioComponentPlaybackFinished = callback;
}

void AudioAnalyzerExtractor::registerOnAudioComponentPlaybackLoopEnd(std::function<void()> callback)
{
	onAudioComponentPlaybackLoopFinished = callback;
}

void AudioAnalyzerExtractor::registerOnAudioComponentStopFinished(std::function<void()> callback)
{
	onAudioComponentStopFinished = callback;
}

