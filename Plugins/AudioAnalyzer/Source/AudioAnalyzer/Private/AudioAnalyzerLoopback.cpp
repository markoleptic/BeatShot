// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#include "AudioAnalyzerLoopback.h"

#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>

using namespace PCAudioAnalyzer;

AudioAnalyzerLoopback::AUDIO_DEVICE_MANAGER::AUDIO_DEVICE_MANAGER() :
	hasContext(false),
	hasDevice(false)
{
}

AudioAnalyzerLoopback::AudioAnalyzerLoopback() : _isCapturing(false),
												_enableOnCapturedData(false),
												_totalBufferSize(0)
{
}

AudioAnalyzerLoopback::~AudioAnalyzerLoopback()
{
	unloadAudio();
}

bool AudioAnalyzerLoopback::initializeAudio(uint16 numChannels, 
											uint32 sampleRate, 
											int bitDepth, 
											uint8 audioFormat, 
											float audioBufferSeconds, 
											uint32 periodSizeInFrames, 
											uint32 periodSizeInMilliseconds, 
											uint32 periods)
{
	unloadAudio();

	_audioInfo.reset(new PCAudioAnalyzer::AUDIOFILE_INFO);
	_audioInfo->numChannels = numChannels;
	_audioInfo->bitDepth = bitDepth;
	_audioInfo->sampleRate = sampleRate;
	_audioInfo->audioFormat = (PCAudioAnalyzer::AUDIOFILE_INFO_FORMAT)audioFormat;
	_audioInfo->dataChunkSize = audioBufferSeconds * _audioInfo->sampleRate * (_audioInfo->bitDepth / 8) * _audioInfo->numChannels;

	_audioBuffer.reset(new QueueBuffer());
	_audioBuffer->initQueueBuffer<uint8>(_audioInfo->dataChunkSize);
	_audioInfo->dataChunkRawData = _audioBuffer->getBufferPtr();

	return loadAudio(periodSizeInFrames, periodSizeInMilliseconds, periods);
}

void AudioAnalyzerLoopback::s_captureCallBack(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	static_cast<AudioAnalyzerLoopback*>(pDevice->pUserData)->captureCallBack(pOutput, pInput, frameCount);
}

void AudioAnalyzerLoopback::captureCallBack(void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	if (_captureDeviceManager.hasDevice)
	{
		uint32_t bytesPerSample = ma_get_bytes_per_sample(_captureDeviceManager.device.capture.format);
		if (bytesPerSample > 0)
		{
			uint32_t length = frameCount * bytesPerSample * _captureDeviceManager.device.capture.channels;

			{
				std::lock_guard<std::mutex> lock(_sourceMutex);

				_audioBuffer->insertBlock(pInput, length);

				if (_enableOnCapturedData)
				{
					_capturedBuffer.Reserve(length);
					_capturedBuffer = TArray<uint8>((uint8*)pInput, length);
					onCapturedData(_capturedBuffer);
				}

				_totalBufferSize += length;
			}
		}
	}
}

#if PLATFORM_WINDOWS
bool AudioAnalyzerLoopback::loadAudio(uint32 periodSizeInFrames, uint32 periodSizeInMilliseconds, uint32 periods)
{
	_totalBufferSize = 0;
	const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfoPtr = _audioInfo.get();
	if (audioInfoPtr)
	{
		_captureDeviceManager.deviceConfig = ma_device_config_init(ma_device_type_loopback);
		_captureDeviceManager.deviceConfig.capture.shareMode = ma_share_mode_shared;
		_captureDeviceManager.deviceConfig.capture.format = (ma_format)PCAudioAnalyzer::formatToMa_format(audioInfoPtr->audioFormat, audioInfoPtr->bitDepth);
		_captureDeviceManager.deviceConfig.capture.channels = audioInfoPtr->numChannels;

		if (periodSizeInFrames > 0)
		{
			_captureDeviceManager.deviceConfig.periodSizeInFrames = periodSizeInFrames;
		}

		if (periodSizeInMilliseconds > 0)
		{
			_captureDeviceManager.deviceConfig.periodSizeInMilliseconds = periodSizeInMilliseconds;
		}

		if (periods > 0)
		{
			_captureDeviceManager.deviceConfig.periods = periods;
		}

		_captureDeviceManager.deviceConfig.sampleRate = audioInfoPtr->sampleRate;
		_captureDeviceManager.deviceConfig.dataCallback = AudioAnalyzerLoopback::s_captureCallBack;
		_captureDeviceManager.deviceConfig.pUserData = this;

		ma_result result = MA_ERROR;

		ma_device_info* pPlaybackDeviceInfos;
		ma_uint32 playbackDeviceCount;
		ma_device_info* pCaptureDeviceInfos;
		ma_uint32 captureDeviceCount;

		result = ma_context_init(NULL, 0, NULL, &_captureDeviceManager.context);
		if (result == MA_SUCCESS)
		{
			_captureDeviceManager.hasContext = true;
			result = ma_context_get_devices(&_captureDeviceManager.context, &pPlaybackDeviceInfos, &playbackDeviceCount, &pCaptureDeviceInfos, &captureDeviceCount);
			if (result == MA_SUCCESS)
			{
				if (_deviceCapturerInfo.isDefaultDevice())
				{
					_captureDeviceManager.deviceConfig.capture.pDeviceID = NULL;
				}
				else
				{
					bool deviceFound = false;
					FString desiredDeviceName(UTF8_TO_TCHAR(_deviceCapturerInfo.name.c_str()));
					for (ma_uint32 currentDevice = 0; !deviceFound && currentDevice < captureDeviceCount; ++currentDevice)
					{
						const ma_device_info& device_info = pCaptureDeviceInfos[currentDevice];
						FString currentDeviceName(UTF8_TO_TCHAR(device_info.name));
						if (currentDeviceName.Equals(desiredDeviceName))
						{
							deviceFound = true;
							_captureDeviceManager.deviceConfig.capture.pDeviceID = &device_info.id;
						}
					}

					if (!deviceFound)
					{
						UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Capture Device Not found %s, using default system device"), *desiredDeviceName);
					}
				}

				result = ma_device_init(&_captureDeviceManager.context, &_captureDeviceManager.deviceConfig, &_captureDeviceManager.device);
				if (result == MA_SUCCESS)
				{
					_captureDeviceManager.hasDevice = true;

					_deviceCapturerInfo.numChannels = _captureDeviceManager.device.capture.internalChannels;
					PCAudioAnalyzer::ma_formatToFormat(_captureDeviceManager.device.capture.internalFormat, _deviceCapturerInfo.format, _deviceCapturerInfo.bitDepth);
					_deviceCapturerInfo.bufferFrames = _captureDeviceManager.device.capture.internalPeriodSizeInFrames;
					_deviceCapturerInfo.sampleRate = _captureDeviceManager.device.capture.internalSampleRate;

					return true;
				}
				else
				{
					ma_context_uninit(&_captureDeviceManager.context);
					_captureDeviceManager.hasContext = false;
					UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Failed device init. ErrorCode:%d"), result);
				}
			}
			else
			{
				ma_context_uninit(&_captureDeviceManager.context);
				_captureDeviceManager.hasContext = false;
				UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Failed to retrieve device information. ErrorCode:%d"), result);
			}
		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Failed to initialize playback context. ErrorCode:%d"), result);
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Invalid Audio info:"));
	}

	return false;
}

void AudioAnalyzerLoopback::start(bool captureDataBroadcast)
{
	if (!_isCapturing)
	{
		_enableOnCapturedData = captureDataBroadcast;

		if (_captureDeviceManager.hasDevice)
		{
			ma_result result = ma_device_start(&_captureDeviceManager.device);
			if (result != MA_SUCCESS)
			{
				UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Start loopback capture has failed. ErrorCode: %d"), result);
			}
			else
			{
				_isCapturing = true;
			}
		}
	}
}

void AudioAnalyzerLoopback::stop()
{
	if (_captureDeviceManager.hasDevice)
	{
		ma_device_stop(&_captureDeviceManager.device);
		{
			std::lock_guard<std::mutex> lock(_sourceMutex);
			_totalBufferSize = 0;
		}

		_isCapturing = false;
	}
}

void AudioAnalyzerLoopback::unloadAudio()
{
	_isCapturing = false;

	if (_captureDeviceManager.hasDevice)
	{
		ma_device_uninit(&_captureDeviceManager.device);
		_captureDeviceManager.hasDevice = false;
	}

	if (_captureDeviceManager.hasContext)
	{
		ma_context_uninit(&_captureDeviceManager.context);
		_captureDeviceManager.hasContext = false;
	}
}
#else
bool AudioAnalyzerLoopback::loadAudio(uint32 periodSizeInFrames, uint32 periodSizeInMilliseconds, uint32 periods)
{
	UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Platform not supported"));
	return false;
}

void AudioAnalyzerLoopback::start(bool captureDataBroadcast)
{
}

void AudioAnalyzerLoopback::stop()
{
}

void AudioAnalyzerLoopback::unloadAudio()
{
}

#endif // PLATFORM_WINDOWS

bool AudioAnalyzerLoopback::isCapturing() const
{
	return _isCapturing;
}

float AudioAnalyzerLoopback::getPlaybackProgress(float& bufferPosTime)
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

float AudioAnalyzerLoopback::getPlaybackProgress()
{
	float dummyValue;
	return getPlaybackProgress(dummyValue);
}

void AudioAnalyzerLoopback::setDefaultAudioDevice(const FString& capturerDeviceName)
{
	_deviceCapturerInfo.name = TCHAR_TO_UTF8(*capturerDeviceName);
}

void AudioAnalyzerLoopback::getDefaultAudioDevice(FString& capturerDeviceName)
{
	capturerDeviceName = UTF8_TO_TCHAR(_deviceCapturerInfo.name.c_str());
}

const AudioAnalyzerDeviceInfo& AudioAnalyzerLoopback::getAudioDeviceInfo() const
{
	return _deviceCapturerInfo;
}

void AudioAnalyzerLoopback::registerOnCapturedData(std::function<void(TArray<uint8>)> callback)
{
	onCapturedData = callback;
}