// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#include "AudioAnalyzerStream.h"

#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>

#include "miniaudio.h"

AudioAnalyzerStream::AUDIO_DEVICE_MANAGER::AUDIO_DEVICE_MANAGER() :
	hasContext(false),
	hasDevice(false)
{
}

AudioAnalyzerStream::AudioAnalyzerStream() : _initPlayBackDevice(false),
											 _enablePlayBack(false),
											 _isOpen(false),
											 _totalBufferSize(0)
{
}

AudioAnalyzerStream::~AudioAnalyzerStream()
{
	unloadAudio();
}

bool AudioAnalyzerStream::initializeAudio(uint16 numChannels, 
											uint32 sampleRate, 
											int bitDepth, 
											uint8 audioFormat, 
											float audioBufferSeconds,
											bool initPlaybackDevice, 
											uint32 periodSizeInFrames,	
											uint32 periodSizeInMilliseconds, 
											uint32 periods)
{
	unloadAudio();

	_totalBufferSize = 0;
	_audioInfo.reset(new PCAudioAnalyzer::AUDIOFILE_INFO);
	_audioInfo->numChannels = numChannels;
	_audioInfo->bitDepth = bitDepth;
	_audioInfo->sampleRate = sampleRate;
	_audioInfo->audioFormat = (PCAudioAnalyzer::AUDIOFILE_INFO_FORMAT)audioFormat; // unsigned, signed , float
	_audioInfo->dataChunkSize = audioBufferSeconds * _audioInfo->sampleRate * (_audioInfo->bitDepth / 8) * _audioInfo->numChannels;

	_audioBuffer.reset(new QueueBuffer());
	_audioBuffer->initQueueBuffer<uint8>(_audioInfo->dataChunkSize);
	_audioInfo->dataChunkRawData = _audioBuffer->getBufferPtr();

	_feedBuffer.reset(new QueueBuffer());
	_feedBuffer->initQueueBuffer<uint8>(_audioInfo->dataChunkSize);

	_initPlayBackDevice = initPlaybackDevice;

	return loadAudio(periodSizeInFrames, periodSizeInMilliseconds, periods);
}

void AudioAnalyzerStream::s_playbackCallBack(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	static_cast<AudioAnalyzerStream*>(pDevice->pUserData)->playbackCallBack(pOutput, pInput, frameCount);
}

void AudioAnalyzerStream::playbackCallBack(void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	if (!_isOpen || !_enablePlayBack)
	{
		return;
	}
	
	if (_audioDeviceManager.hasDevice)
	{
		uint32_t bytesPerSample = ma_get_bytes_per_sample(_audioDeviceManager.device.playback.format);
		if (bytesPerSample > 0)
		{
			uint32_t length = frameCount * bytesPerSample * _audioDeviceManager.device.playback.channels;

			{
				std::lock_guard<std::mutex> lock(_sourceMutex);

				if (_feedBuffer.get())
				{
					size_t availableData = _feedBuffer->getFilledSize();

					if (availableData < length || !_enablePlayBack)
					{
						//play silence
						memset(pOutput, 0, length);
					}
					else
					{
						memcpy(pOutput, _feedBuffer->getBufferPtr(), length);
						_feedBuffer->removeBlock(length);
					}
				}
			}
		}
	}
}

bool AudioAnalyzerStream::loadAudio(uint32 periodSizeInFrames, uint32 periodSizeInMilliseconds, uint32 periods)
{
	const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfoPtr = _audioInfo.get();
	if (audioInfoPtr)
	{
		if (_initPlayBackDevice)
		{
			_audioDeviceManager.deviceConfig = ma_device_config_init(ma_device_type_playback);
			_audioDeviceManager.deviceConfig.playback.format = (ma_format)PCAudioAnalyzer::formatToMa_format(audioInfoPtr->audioFormat, audioInfoPtr->bitDepth);
			_audioDeviceManager.deviceConfig.playback.channels = audioInfoPtr->numChannels;

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

			_audioDeviceManager.deviceConfig.sampleRate = audioInfoPtr->sampleRate;
			_audioDeviceManager.deviceConfig.dataCallback = AudioAnalyzerStream::s_playbackCallBack;
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
					if (_devicePlayerInfo.isDefaultDevice())
					{
						_audioDeviceManager.deviceConfig.playback.pDeviceID = NULL;
					}
					else
					{
						bool deviceFound = false;
						FString desiredDeviceName(UTF8_TO_TCHAR(_devicePlayerInfo.name.c_str()));
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

						_totalBufferSize = 0;

						_devicePlayerInfo.numChannels = _audioDeviceManager.device.playback.internalChannels;
						PCAudioAnalyzer::ma_formatToFormat(_audioDeviceManager.device.playback.internalFormat, _devicePlayerInfo.format, _devicePlayerInfo.bitDepth);
						_devicePlayerInfo.bufferFrames = _audioDeviceManager.device.playback.internalPeriodSizeInFrames;
						_devicePlayerInfo.sampleRate = _audioDeviceManager.device.playback.internalSampleRate;

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
			_devicePlayerInfo.format = PCAudioAnalyzer::AUDIOFILE_INFO_FORMAT::AUDIOFORMAT_NONE;
			_devicePlayerInfo.numChannels = 0;
			_devicePlayerInfo.bufferFrames = 0;
			_devicePlayerInfo.sampleRate = 0;

			return true;
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Invalid Audio info:"));
	}

	return false;
}


void AudioAnalyzerStream::open(bool enablePlayback)
{
	if (!_isOpen)
	{
		if (!_initPlayBackDevice && enablePlayback)
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Missing Playback device, Are you missing initialization in InitStreamAudio node?"));
			_enablePlayBack = false;
		}
		else
		{
			_enablePlayBack = enablePlayback;
		}

		if (_audioDeviceManager.hasDevice)
		{
			ma_result result = ma_device_start(&_audioDeviceManager.device);
			if (result != MA_SUCCESS)
			{
				UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Device start failed. ErrorCode: %d"), result);
				_enablePlayBack = false;
			}
		}

		_isOpen = true;
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Stream must closed before open again"));
	}
}

void AudioAnalyzerStream::feed(unsigned char* data, size_t dataSize)
{
	if (_isOpen)
	{
		{
			std::lock_guard<std::mutex> lock(_sourceMutex);
			_audioBuffer->insertBlock(data, dataSize);
			_feedBuffer->insertBlock(data, dataSize);
			_totalBufferSize += dataSize;
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Closed stream can be feed, are you missing OpenStreamCapture node?"));
	}
}

void AudioAnalyzerStream::close()
{
	_isOpen = false;

	if (_audioDeviceManager.hasDevice)
	{
		ma_device_stop(&_audioDeviceManager.device);
	}

	{
		std::lock_guard<std::mutex> lock(_sourceMutex);
		_feedBuffer->removeBlock(_feedBuffer->getBufferSize());
		_totalBufferSize = 0;
	}
}

void AudioAnalyzerStream::unloadAudio()
{
	_totalBufferSize = 0;

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

void AudioAnalyzerStream::setDefaultAudioDevice(const FString& playerDeviceName)
{
	_devicePlayerInfo.name = TCHAR_TO_UTF8(*playerDeviceName);
}

bool AudioAnalyzerStream::isCapturing() const
{
	return _isOpen;
}

float AudioAnalyzerStream::getPlaybackProgress(float& bufferPosTime)
{
	const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfoPtr = _audioInfo.get();
	std::lock_guard<std::mutex> lock(_sourceMutex);
	if (audioInfoPtr)
	{
		bufferPosTime = (_audioBuffer->getFilledSize() / (float)(audioInfoPtr->numChannels * (audioInfoPtr->bitDepth / 8))) / audioInfoPtr->sampleRate;
		return (_totalBufferSize / (float)(audioInfoPtr->numChannels * (audioInfoPtr->bitDepth / 8))) / audioInfoPtr->sampleRate;
	}

	bufferPosTime = 0;
	return 0;
}

float AudioAnalyzerStream::getPlaybackProgress()
{
	float dummyValue;
	return getPlaybackProgress(dummyValue);
}

void AudioAnalyzerStream::getDefaultAudioDevice(FString& playerDeviceName)
{
	playerDeviceName = UTF8_TO_TCHAR(_devicePlayerInfo.name.c_str());
}

const AudioAnalyzerDeviceInfo& AudioAnalyzerStream::getAudioPlayerDeviceInfo() const
{
	return _devicePlayerInfo;
}
