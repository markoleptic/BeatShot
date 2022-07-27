// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#include "AudioAnalyzerSoundWave.h"

#include "AudioAnalyzerExtractor.h"
#include "AudioAnalyzerCommon.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundAttenuation.h"
#include "AudioDevice.h"
#include "EngineGlobals.h"
#include "Async/Async.h"

void UAudioAnalyzerSoundWave::BeginDestroy()
{
	USoundWaveProcedural::BeginDestroy();
}

void UAudioAnalyzerSoundWave::InitializeSoundWave(AudioAnalyzerExtractor* ParentSource)
{
	_audioBufferPtr = NULL;
	_audioBufferSize = 0;

	_parentSource = ParentSource;
	_startTime = 0.0;
	_remainingLoops = 0;

	SetPlaybackTime(0.0);

	_isPlaying = false;
	_isPaused = false;
	_isPlayable = true;
}

void UAudioAnalyzerSoundWave::PlaySound2D(const UObject* WorldContextObject, float VolumeMultiplier, float PitchMultiplier, float StartTime, int32 Loops, USoundConcurrency* ConcurrencySettings, AActor* OwningActor)
{
	Play(Loops, StartTime);
	UGameplayStatics::PlaySound2D(WorldContextObject, this, VolumeMultiplier, PitchMultiplier, 0.0, ConcurrencySettings, OwningActor);
}

UAudioComponent* UAudioAnalyzerSoundWave::SpawnSound2D(const UObject* WorldContextObject, float VolumeMultiplier, float PitchMultiplier, float StartTime, int32 Loops, USoundConcurrency* ConcurrencySettings, bool bPersistAcrossLevelTransition, bool bAutoDestroy)
{
	Play(Loops, StartTime);
	UAudioComponent* audioComponent = UGameplayStatics::SpawnSound2D(WorldContextObject, this, VolumeMultiplier, PitchMultiplier, 0.0, ConcurrencySettings, bPersistAcrossLevelTransition, bAutoDestroy);
	_parentSource->setAudioComponent(audioComponent);
	return audioComponent;
}

UAudioComponent* UAudioAnalyzerSoundWave::CreateSound2D(const UObject* WorldContextObject, float VolumeMultiplier, float PitchMultiplier, float StartTime, int32 Loops, USoundConcurrency* ConcurrencySettings, bool bPersistAcrossLevelTransition, bool bAutoDestroy)
{
	Play(Loops, StartTime);
	_isPlaying = false;
	UAudioComponent* audioComponent = UGameplayStatics::CreateSound2D(WorldContextObject, this, VolumeMultiplier, PitchMultiplier, 0.0, ConcurrencySettings, bPersistAcrossLevelTransition, bAutoDestroy);
	_parentSource->setAudioComponent(audioComponent);
	return audioComponent;
}

void UAudioAnalyzerSoundWave::PlaySoundAtLocation(const UObject* WorldContextObject, FVector Location, FRotator Rotation, float VolumeMultiplier, float PitchMultiplier, float StartTime, int32 Loops, class USoundAttenuation* Attenuation_Settings, class USoundConcurrency* ConcurrencySettings, AActor* OwningActor)
{
	Play(Loops, StartTime);
	UGameplayStatics::PlaySoundAtLocation(WorldContextObject, this, Location, Rotation, VolumeMultiplier, PitchMultiplier, 0.0, Attenuation_Settings, ConcurrencySettings, OwningActor);
}

UAudioComponent* UAudioAnalyzerSoundWave::SpawnSoundAtLocation(const UObject* WorldContextObject, FVector Location, FRotator Rotation, float VolumeMultiplier, float PitchMultiplier, float StartTime, int32 Loops, class USoundAttenuation* Attenuation_Settings, USoundConcurrency* ConcurrencySettings, bool bAutoDestroy)
{
	Play(Loops, StartTime);
	UAudioComponent* audioComponent = UGameplayStatics::SpawnSoundAtLocation(WorldContextObject, this, Location, Rotation, VolumeMultiplier, PitchMultiplier, 0.0, Attenuation_Settings, ConcurrencySettings, bAutoDestroy);
	_parentSource->setAudioComponent(audioComponent);
	return audioComponent;
}

UAudioComponent* UAudioAnalyzerSoundWave::SpawnSoundAttached(USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, EAttachLocation::Type LocationType, bool bStopWhenAttachedToDestroyed, float VolumeMultiplier, float PitchMultiplier, float StartTime, int32 Loops, USoundAttenuation* Attenuation_Settings, USoundConcurrency* ConcurrencySettings, bool bAutoDestroy)
{
	Play(Loops, StartTime);
	UAudioComponent* audioComponent = UGameplayStatics::SpawnSoundAttached(this, AttachToComponent, AttachPointName, Location, Rotation, LocationType, bStopWhenAttachedToDestroyed, VolumeMultiplier, PitchMultiplier, 0.0, Attenuation_Settings, ConcurrencySettings, bAutoDestroy);
	_parentSource->setAudioComponent(audioComponent);
	return audioComponent;
}

void UAudioAnalyzerSoundWave::Play(int32 Loops, float StartTime)
{
	_startTime = FMath::Max(0.f, StartTime);
	_remainingLoops = Loops - 1;

	SetPlaybackTime(_startTime);

	_isPlaying = true;
	_isPaused = false;
	_isPlayable = true;
}

void UAudioAnalyzerSoundWave::StopSound()
{
	_isPlaying = false;
	_isPaused = false;
	_isPlayable = false;

	SetPlaybackTime(0.0);
}

void UAudioAnalyzerSoundWave::AsyncStopSound()
{
	StopSound();
}

void UAudioAnalyzerSoundWave::PauseSound()
{
	SetPaused(true);
}

void UAudioAnalyzerSoundWave::SetPaused(bool forcePause)
{
	if (IsPlaying() && forcePause)
	{
		_isPlaying = false;
		_isPaused = true;
	}
	else
	{
		if (_isPaused)
		{
			_isPlaying = true; //reactivate audio render thread
			_isPaused = false;
		}
	}
}

bool UAudioAnalyzerSoundWave::IsPlaying() const
{
	return _isPlaying;
}

bool UAudioAnalyzerSoundWave::IsPaused() const
{
	return _isPaused;
}

float UAudioAnalyzerSoundWave::GetPlaybackProgress(float& bufferPosTime)
{
	if (_parentSource)
	{
		const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfoPtr = _parentSource->getAudioInfo();
		std::lock_guard<std::mutex> lock(_parentSource->getSourceMutex());
		if (audioInfoPtr)
		{
			bufferPosTime = (audioInfoPtr->dataChunkSize - _audioBufferSize) / (float)(audioInfoPtr->numChannels * (audioInfoPtr->bitDepth / 8)) / audioInfoPtr->sampleRate;
		}
		else
		{
			bufferPosTime = 0;
		}
	}
	return bufferPosTime;
}

float UAudioAnalyzerSoundWave::GetPlaybackTime()
{
	float dummyValue;
	return GetPlaybackProgress(dummyValue);
}

void UAudioAnalyzerSoundWave::SetPlaybackTime(float Time)
{
	if (_parentSource)
	{
		const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfoPtr = _parentSource->getAudioInfo();
		std::lock_guard<std::mutex> lock(_parentSource->getSourceMutex());
		if (audioInfoPtr)
		{
			uint32 offset = Time * audioInfoPtr->sampleRate;
			offset *= (audioInfoPtr->numChannels * (audioInfoPtr->bitDepth / 8));
			offset = (offset > audioInfoPtr->dataChunkSize ? audioInfoPtr->dataChunkSize : offset);
			_audioBufferPtr = reinterpret_cast<uint8_t*>(audioInfoPtr->dataChunkRawData) + offset;
			_audioBufferSize = audioInfoPtr->dataChunkSize - offset;
		}
	}
}

float UAudioAnalyzerSoundWave::GetTotalDuration() const
{
	if (_parentSource)
	{
		const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfoPtr = _parentSource->getAudioInfo();
		if (audioInfoPtr)
		{
			return (audioInfoPtr->dataChunkSize / (float)(audioInfoPtr->numChannels * (audioInfoPtr->bitDepth / 8))) / (audioInfoPtr->sampleRate);
		}
	}
	return 0;
}

float UAudioAnalyzerSoundWave::GetDuration() const
{
	return GetTotalDuration();
}
void UAudioAnalyzerSoundWave::OnBeginGenerate()
{
	//UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("OnBeginGenerate"));
}

void UAudioAnalyzerSoundWave::OnEndGenerate()
{
	//UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("OnEndGenerate"));
	EventPlaybackBroadcast(OnStopFinished);
	if (HasAudioComponent())
	{
		_parentSource->onAudioComponentStopFinished();
	}
}

bool UAudioAnalyzerSoundWave::IsPlayable() const
{
	return _isPlayable;
}

int32 UAudioAnalyzerSoundWave::OnGeneratePCMAudio(TArray<uint8>& OutAudio, int32 NumSamples)
{
	if (!_isPlayable || !_isPlaying)
	{
		return 0;
	}

	if (_audioBufferSize == 0)
	{
		if (_remainingLoops == 0)
		{
			StopSound();
			EventPlaybackBroadcast(OnPlaybackLoopFinished);
			EventPlaybackBroadcast(OnPlaybackFinished);
			if (HasAudioComponent())
			{
				_parentSource->onAudioComponentPlaybackFinished();
				_parentSource->onAudioComponentPlaybackLoopFinished();
			}
			return 0;
		}
		else
		{
			if (_remainingLoops > 0)
			{
				--_remainingLoops;
			}
			//reset loop position
			EventPlaybackBroadcast(OnPlaybackLoopFinished);
			if (HasAudioComponent())
			{
				_parentSource->onAudioComponentPlaybackLoopFinished();
			}
			const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfoPtr = _parentSource->getAudioInfo();
			if (audioInfoPtr)
			{
				{
					std::lock_guard<std::mutex> lock(_parentSource->getSourceMutex());
					uint32 offset = _startTime * audioInfoPtr->sampleRate;
					offset *= (audioInfoPtr->numChannels * (audioInfoPtr->bitDepth / 8));
					offset = (offset > audioInfoPtr->dataChunkSize ? audioInfoPtr->dataChunkSize : offset);
					_audioBufferPtr = reinterpret_cast<uint8*>(audioInfoPtr->dataChunkRawData) + offset;
					_audioBufferSize = audioInfoPtr->dataChunkSize - offset;
				}
			}
		}
	}


	{
		std::lock_guard<std::mutex> lock(_parentSource->getSourceMutex());

		if (_parentSource)
		{
			const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfoPtr = _parentSource->getAudioInfo();
			if (audioInfoPtr)
			{

				int32 bytesToCopy = NumSamples * (audioInfoPtr->bitDepth / 8);
				bytesToCopy = (bytesToCopy > _audioBufferSize ? _audioBufferSize : bytesToCopy);

				OutAudio.Reserve(bytesToCopy);
				OutAudio = TArray<uint8>( _audioBufferPtr, bytesToCopy);

				_audioBufferPtr += bytesToCopy;
				_audioBufferSize -= bytesToCopy;

				return (bytesToCopy / (audioInfoPtr->bitDepth / 8));
			}
		}
	}

	return NumSamples;
}

Audio::EAudioMixerStreamDataFormat::Type UAudioAnalyzerSoundWave::GetGeneratedPCMDataFormat() const
{
	if (_parentSource)
	{
		const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfoPtr = _parentSource->getAudioInfo();
		if (audioInfoPtr)
		{
			if (audioInfoPtr->audioFormat == PCAudioAnalyzer::AUDIOFILE_INFO_FORMAT::AUDIOFORMAT_INT)
			{
				switch (audioInfoPtr->bitDepth)
				{
					case 16: return Audio::EAudioMixerStreamDataFormat::Type::Int16;
				}
			}
			else if (audioInfoPtr->audioFormat == PCAudioAnalyzer::AUDIOFILE_INFO_FORMAT::AUDIOFORMAT_FLOAT)
			{
				switch (audioInfoPtr->bitDepth)
				{
					case 32: return Audio::EAudioMixerStreamDataFormat::Type::Float;
				}
			}
		}
	}

	return Audio::EAudioMixerStreamDataFormat::Type::Int16;
}

void UAudioAnalyzerSoundWave::EventPlaybackBroadcast(FPlaybackDelegateSoundWave& SoundWaveEvent)
{
	AsyncTask(ENamedThreads::GameThread, [&] ()
	{
		if (SoundWaveEvent.IsBound())
		{
			SoundWaveEvent.Broadcast();
		}
	});
}

void UAudioAnalyzerSoundWave::InvalidateSoundWave()
{
	StopSound();
	_parentSource = NULL;
}

bool UAudioAnalyzerSoundWave::HasParent() const
{
	return (_parentSource != NULL);
}

bool UAudioAnalyzerSoundWave::HasAudioComponent() const
{
	if (_parentSource)
	{
		return (_parentSource->getAudioComponent() != nullptr);
	}
	else
	{
		return false;
	}
}
