// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#include "AudioAnalyzerPlayerUnitFile.h"

#include "AudioAnalyzerPlayer.h"

#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>

#include "miniaudio.h"

#include <mutex>
#include <functional>
#include <algorithm>


AudioAnalyzerPlayerUnitFile::AUDIO_DEVICE_MANAGER::AUDIO_DEVICE_MANAGER():
	hasContext(false), 
	hasDevice(false)
{
}

AudioAnalyzerPlayerUnitFile::AudioAnalyzerPlayerUnitFile():_isPlaying(false), 
_isPaused(false),
															_audioBufferPtr(NULL), 
															_audioBufferSize(0), 
															_audioVolume(0.5)
														
{
}

AudioAnalyzerPlayerUnitFile::AudioAnalyzerPlayerUnitFile(AudioAnalyzerPlayer* parentPlayer) : AudioAnalyzerPlayerUnit(parentPlayer), 
_isPlaying(false),
																							_isPaused(false),
																							_audioBufferPtr(NULL), 
																							_audioBufferSize(0), 
																							_audioVolume(0.5)
																							
																				
{
}

AudioAnalyzerPlayerUnitFile::~AudioAnalyzerPlayerUnitFile()
{
	unloadAudio();
}

void AudioAnalyzerPlayerUnitFile::s_playbackCallBack(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	static_cast<AudioAnalyzerPlayerUnitFile*>(pDevice->pUserData)->playbackCallBack(pOutput, pInput, frameCount);
}

void AudioAnalyzerPlayerUnitFile::playbackCallBack(void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	if (!_isPlaying)
	{
		return;
	}

	if (_audioBufferSize == 0)
	{
		if (_remainingLoops == 0)
		{
			_isPlaying = false;
			_parentPlayer->onPlaybackLoopEndCallBack();
			_parentPlayer->onPlaybackEndCallBack();
			return;
		}
		else
		{
			if (_remainingLoops > 0)
			{
				--_remainingLoops;
			}
			//reset loop position
			_parentPlayer->onPlaybackLoopEndCallBack();
			const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfoPtr = _parentPlayer->getAudioInfo();
			if (audioInfoPtr)
			{
				{
					std::lock_guard<std::mutex> lock(_parentPlayer->getSourceMutex());
					uint32_t offset = _startTime * audioInfoPtr->sampleRate;
					offset *= (audioInfoPtr->numChannels * (audioInfoPtr->bitDepth / 8));
					offset = (offset > audioInfoPtr->dataChunkSize ? audioInfoPtr->dataChunkSize : offset);
					_audioBufferPtr = reinterpret_cast<uint8*>(audioInfoPtr->dataChunkRawData) + offset;
					_audioBufferSize = audioInfoPtr->dataChunkSize - offset;
				}
			}
		}	
	}

	{
		std::lock_guard<std::mutex> lock(_parentPlayer->getSourceMutex());

		if (_audioDeviceManager.hasDevice)
		{
			uint32_t bytesPerSample = ma_get_bytes_per_sample(_audioDeviceManager.device.playback.format);
			if (bytesPerSample > 0)
			{
				uint32_t length = frameCount * bytesPerSample * _audioDeviceManager.device.playback.channels;
				length = (length > _audioBufferSize ? _audioBufferSize : length);

				switch (_audioDeviceManager.device.playback.format)
				{
				case ma_format_u8:
					ma_copy_and_apply_volume_factor_u8((ma_uint8*)pOutput, (ma_uint8*)_audioBufferPtr, length / bytesPerSample, _audioVolume);
					break;
				case ma_format_s16:
					ma_copy_and_apply_volume_factor_s16((ma_int16*)pOutput, (ma_int16*)_audioBufferPtr, length / bytesPerSample, _audioVolume);
					break;
				case ma_format_s32:
					ma_copy_and_apply_volume_factor_s32((ma_int32*)pOutput, (ma_int32*)_audioBufferPtr, length / bytesPerSample, _audioVolume);
					break;
				case ma_format_f32:
					ma_copy_and_apply_volume_factor_f32((float*)pOutput, (float*)_audioBufferPtr, length / bytesPerSample, _audioVolume);
					break;
				}

				_audioBufferPtr += length;
				_audioBufferSize -= length;
			}
		}
	}
}

bool AudioAnalyzerPlayerUnitFile::loadAudio(uint32 periodSizeInFrames, uint32 periodSizeInMilliseconds, uint32 periods)
{
	const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfoPtr = _parentPlayer->getAudioInfo();
	if (audioInfoPtr)
	{
		_audioBufferPtr = reinterpret_cast<uint8_t*>(audioInfoPtr->dataChunkRawData);
		_audioBufferSize = audioInfoPtr->dataChunkSize;

		const ma_decoder* decoderPtr = _parentPlayer->getAudioDecoder();
		if (decoderPtr)
		{
			_audioDeviceManager.deviceConfig = ma_device_config_init(ma_device_type_playback);
			_audioDeviceManager.deviceConfig.playback.format = decoderPtr->outputFormat;
			_audioDeviceManager.deviceConfig.playback.channels = decoderPtr->outputChannels;

			if (periodSizeInFrames > 0)
			{
				_audioDeviceManager.deviceConfig.periodSizeInFrames = periodSizeInFrames;
			}

			if (periodSizeInMilliseconds > 0)
			{
				_audioDeviceManager.deviceConfig.periodSizeInMilliseconds = periodSizeInMilliseconds;
			}

			if (periods > 0)
			{
				_audioDeviceManager.deviceConfig.periods = periods;
			}

			_audioDeviceManager.deviceConfig.sampleRate = decoderPtr->outputSampleRate;
			_audioDeviceManager.deviceConfig.dataCallback = AudioAnalyzerPlayerUnitFile::s_playbackCallBack;
			_audioDeviceManager.deviceConfig.pUserData = this;

			ma_result result = MA_ERROR;

			ma_device_info* pPlaybackDeviceInfos;
			ma_uint32 playbackDeviceCount;
			ma_device_info* pCaptureDeviceInfos;
			ma_uint32 captureDeviceCount;

			result = ma_context_init(NULL, 0, NULL, &_audioDeviceManager.context);
			if (result == MA_SUCCESS)
			{
				_audioDeviceManager.hasContext = true;
				result = ma_context_get_devices(&_audioDeviceManager.context, &pPlaybackDeviceInfos, &playbackDeviceCount, &pCaptureDeviceInfos, &captureDeviceCount);
				if (result == MA_SUCCESS)
				{
					if (_parentPlayer->cgetAudioDeviceInfo().isDefaultDevice())
					{
						_audioDeviceManager.deviceConfig.playback.pDeviceID = NULL;
					}
					else
					{
						bool deviceFound = false;
						FString desiredDeviceName(UTF8_TO_TCHAR(_parentPlayer->cgetAudioDeviceInfo().name.c_str()));
						for (ma_uint32 currentDevice = 0; !deviceFound && currentDevice < playbackDeviceCount; ++currentDevice)
						{
							const ma_device_info& device_info = pPlaybackDeviceInfos[currentDevice];
							FString currentDeviceName(UTF8_TO_TCHAR(device_info.name));
							if (currentDeviceName.Equals(desiredDeviceName))
							{
									deviceFound = true;
									_audioDeviceManager.deviceConfig.playback.pDeviceID = &device_info.id;
							}
						}

						if (!deviceFound)
						{
							UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Device Not found %s, using default system device"), *desiredDeviceName);
						}
					}

					result = ma_device_init(&_audioDeviceManager.context, &_audioDeviceManager.deviceConfig, &_audioDeviceManager.device);
					if (result == MA_SUCCESS)
					{
						_audioDeviceManager.hasDevice = true;

						AudioAnalyzerDeviceInfo& deviceInfo = _parentPlayer->getAudioDeviceInfo();
						deviceInfo.numChannels = _audioDeviceManager.device.playback.internalChannels;
						PCAudioAnalyzer::ma_formatToFormat(_audioDeviceManager.device.playback.internalFormat, deviceInfo.format, deviceInfo.bitDepth);
						deviceInfo.bufferFrames = _audioDeviceManager.device.playback.internalPeriodSizeInFrames;
						deviceInfo.sampleRate = _audioDeviceManager.device.playback.internalSampleRate;

						return true;
					}
					else
					{
						ma_context_uninit(&_audioDeviceManager.context);
						_audioDeviceManager.hasContext = false;
						UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Failed device init. ErroCode:%d"), result);
					}
				}
				else
				{
					ma_context_uninit(&_audioDeviceManager.context);
					_audioDeviceManager.hasContext = false;
					UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Failed to enumarate devices. ErroCode:%d"), result);
				}
			}
			else
			{
				UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Failed to initialize playback context. ErroCode:%d"), result);
			}
		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("No decoder has been initializated"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Invalid Audio info:"));
	}

	return false;
}

void AudioAnalyzerPlayerUnitFile::unloadAudio()
{
	_isPlaying = false;

	if (_audioDeviceManager.hasDevice)
	{
		ma_device_uninit(&_audioDeviceManager.device);
		_audioDeviceManager.hasDevice = false;
	}

	if (_audioDeviceManager.hasContext)
	{
		ma_context_uninit(&_audioDeviceManager.context);
		_audioDeviceManager.hasContext = false;
	}
}

void AudioAnalyzerPlayerUnitFile::play(int maxLoops, float startTime)
{
	if (_audioDeviceManager.hasDevice)
	{
		if (_isPaused)
		{
			if (!_isPlaying)
			{
				ma_result result = ma_device_start(&_audioDeviceManager.device);
				if (result != MA_SUCCESS)
				{
					UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Device start failed. ErrorCode: %d"), result);
					_isPlaying = false;
					_isPaused = false;
				}
				else
				{
					_isPlaying = true;
					_isPaused = false;
				}
			}
		}
		else
		{
			//reset playback progress
			const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfoPtr = _parentPlayer->getAudioInfo();
			if (audioInfoPtr)
			{
				_startTime = std::max(0.f, startTime);
				_remainingLoops = maxLoops - 1;

				{
					std::lock_guard<std::mutex> lock(_parentPlayer->getSourceMutex());
					uint32_t offset = _startTime * audioInfoPtr->sampleRate;
					offset *= (audioInfoPtr->numChannels * (audioInfoPtr->bitDepth / 8));
					offset = (offset > audioInfoPtr->dataChunkSize ? audioInfoPtr->dataChunkSize : offset);
					_audioBufferPtr = reinterpret_cast<uint8_t*>(audioInfoPtr->dataChunkRawData) + offset;
					_audioBufferSize = audioInfoPtr->dataChunkSize - offset;
				}

				if (!_isPlaying)
				{
					ma_result result = ma_device_start(&_audioDeviceManager.device);
					if (result != MA_SUCCESS)
					{
						UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Device start failed. ErrorCode: %d"), result);
						_isPlaying = false;
						_isPaused = false;
					}
					else
					{
						_isPlaying = true;
						_isPaused = false;
					}
				}
			}
		}
	}
}

void AudioAnalyzerPlayerUnitFile::stop()
{
	if (_audioDeviceManager.hasDevice)
	{
		//reset playback progress
		const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfoPtr = _parentPlayer->getAudioInfo();
		if (audioInfoPtr)
		{
			{
				std::lock_guard<std::mutex> lock(_parentPlayer->getSourceMutex());
				_audioBufferPtr = reinterpret_cast<uint8_t*>(audioInfoPtr->dataChunkRawData);
				_audioBufferSize = audioInfoPtr->dataChunkSize;
			}

			ma_device_stop(&_audioDeviceManager.device);
			_isPlaying = false;
			_isPaused = false;
		}
	}
}

void AudioAnalyzerPlayerUnitFile::pause(bool forcePause)
{
	if (_audioDeviceManager.hasDevice)
	{
		if (isPlaying() && forcePause)
		{
			ma_device_stop(&_audioDeviceManager.device);
			_isPlaying = false;
			_isPaused = true;
		}
		else
		{
			if (_isPaused)
			{
				ma_device_start(&_audioDeviceManager.device);
				_isPlaying = true;
				_isPaused = false;
			}
		}
	}
}

bool AudioAnalyzerPlayerUnitFile::isPlaying() const
{
	return _isPlaying;
}


bool AudioAnalyzerPlayerUnitFile::isPaused() const
{
	return _isPaused;
}

float AudioAnalyzerPlayerUnitFile::getPlaybackProgress(float& bufferPosTime)
{
	const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfoPtr = _parentPlayer->getAudioInfo();
	std::lock_guard<std::mutex> lock(_parentPlayer->getSourceMutex());
	if (audioInfoPtr)
	{
		bufferPosTime = (audioInfoPtr->dataChunkSize - _audioBufferSize) / (float)(audioInfoPtr->numChannels * (audioInfoPtr->bitDepth / 8)) / audioInfoPtr->sampleRate;
	}
	else
	{
		bufferPosTime = 0;
	}
	return bufferPosTime;
}

float AudioAnalyzerPlayerUnitFile::getPlaybackProgress()
{
	float dummyValue;
	return getPlaybackProgress(dummyValue);
}

void AudioAnalyzerPlayerUnitFile::setPlaybackProgress(float seconds)
{
	const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfoPtr = _parentPlayer->getAudioInfo();
	std::lock_guard<std::mutex> lock(_parentPlayer->getSourceMutex());
	if (audioInfoPtr)
	{
		uint32_t offset = seconds * audioInfoPtr->sampleRate;
		offset *= (audioInfoPtr->numChannels * (audioInfoPtr->bitDepth / 8));
		offset = (offset > audioInfoPtr->dataChunkSize ? audioInfoPtr->dataChunkSize : offset);
		_audioBufferPtr = reinterpret_cast<uint8_t*>(audioInfoPtr->dataChunkRawData) + offset;
		_audioBufferSize = audioInfoPtr->dataChunkSize - offset;
	}
}

float AudioAnalyzerPlayerUnitFile::getTotalDuration() const
{
	const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfoPtr = _parentPlayer->getAudioInfo();
	if (audioInfoPtr)
	{
		return (audioInfoPtr->dataChunkSize / (float)(audioInfoPtr->numChannels * (audioInfoPtr->bitDepth / 8) )) / audioInfoPtr->sampleRate;
	}
	return 0;
}

float AudioAnalyzerPlayerUnitFile::getPlaybackVolume()
{
	std::lock_guard<std::mutex> lock(_parentPlayer->getSourceMutex());
	return _audioVolume;
}

void AudioAnalyzerPlayerUnitFile::setPlaybackVolume(float volume)
{
	std::lock_guard<std::mutex> lock(_parentPlayer->getSourceMutex());
	if (volume < 0)
	{
		_audioVolume = 0.0;
	}
	else if(volume > 1.0)
	{
		_audioVolume = 1.0;
	}
	else
	{
		_audioVolume = volume;
	}
}

void AudioAnalyzerPlayerUnitFile::setAudioBufferSize(float seconds)
{
}