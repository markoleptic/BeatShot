// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#include "AudioAnalyzerCapturer.h"

#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>

#include "miniaudio.h"

using namespace PCAudioAnalyzer;

AudioAnalyzerCapturer::AUDIO_DEVICE_MANAGER::AUDIO_DEVICE_MANAGER() :
	hasContext(false),
	hasDevice(false)
{
}

AudioAnalyzerCapturer::AudioAnalyzerCapturer(): _isCapturing(false),
												_initPlayBackDevice(false),
												_enablePlayBack(false), 
												_enableOnCapturedData(false),
												_audioVolumeCapture(1.0),
												_audioVolumePlayback(1.0),
												_totalBufferSize(0)
{
}

AudioAnalyzerCapturer::~AudioAnalyzerCapturer()
{
	unloadAudio();
}

bool AudioAnalyzerCapturer::initializeAudio(uint32 sampleRate, 
											int bitDepth, 
											uint8 audioFormat, 
											float audioBufferSeconds, 
											bool initPlaybackDevice, 
											uint32 periodSizeInFrames, 
											uint32 periodSizeInMilliseconds, 
											uint32 periods)
{
	unloadAudio();

	_audioInfo.reset(new PCAudioAnalyzer::AUDIOFILE_INFO);
	_audioInfo->numChannels = 1;
	_audioInfo->bitDepth = bitDepth; 
	_audioInfo->sampleRate = sampleRate;
	_audioInfo->audioFormat = (PCAudioAnalyzer::AUDIOFILE_INFO_FORMAT)audioFormat;
	_audioInfo->dataChunkSize = audioBufferSeconds * _audioInfo->sampleRate * (_audioInfo->bitDepth / 8) * _audioInfo->numChannels;

	_audioBuffer.reset(new QueueBuffer());
	_audioBuffer->initQueueBuffer<uint8>(_audioInfo->dataChunkSize);
	_audioInfo->dataChunkRawData = _audioBuffer->getBufferPtr();

	_initPlayBackDevice = initPlaybackDevice;

	return loadAudio(periodSizeInFrames, periodSizeInMilliseconds, periods);
}

void AudioAnalyzerCapturer::s_captureCallBack(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	static_cast<AudioAnalyzerCapturer*>(pDevice->pUserData)->captureCallBack(pOutput, pInput, frameCount);
}

void AudioAnalyzerCapturer::captureCallBack(void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	if (_captureDeviceManager.hasDevice)
	{
		uint32_t bytesPerSample = ma_get_bytes_per_sample(_captureDeviceManager.device.capture.format);
		if (bytesPerSample > 0)
		{
			uint32_t length = frameCount * bytesPerSample * _captureDeviceManager.device.capture.channels;

			std::unique_ptr<uint8[]> tempBuffer(new uint8[length]());
			switch (_captureDeviceManager.device.capture.format)
			{
			case ma_format_u8:
				ma_copy_and_apply_volume_factor_u8((ma_uint8*)tempBuffer.get(), (ma_uint8*)pInput, length / bytesPerSample, _audioVolumeCapture);
				break;
			case ma_format_s16:
				ma_copy_and_apply_volume_factor_s16((ma_int16*)tempBuffer.get(), (ma_int16*)pInput, length / bytesPerSample, _audioVolumeCapture);
				break;
			case ma_format_s32:
				ma_copy_and_apply_volume_factor_s32((ma_int32*)tempBuffer.get(), (ma_int32*)pInput, length / bytesPerSample, _audioVolumeCapture);
				break;
			case ma_format_f32:
				ma_copy_and_apply_volume_factor_f32((float*)tempBuffer.get(), (float*)pInput, length / bytesPerSample, _audioVolumeCapture);
				break;
			}

			if (_initPlayBackDevice)
			{
				if (_enablePlayBack)
				{
					switch (_captureDeviceManager.device.capture.format)
					{
					case ma_format_u8:
						ma_copy_and_apply_volume_factor_u8((ma_uint8*)pOutput, (ma_uint8*)pInput, length / bytesPerSample, _audioVolumePlayback);
						break;
					case ma_format_s16:
						ma_copy_and_apply_volume_factor_s16((ma_int16*)pOutput, (ma_int16*)pInput, length / bytesPerSample, _audioVolumePlayback);
						break;
					case ma_format_s32:
						ma_copy_and_apply_volume_factor_s32((ma_int32*)pOutput, (ma_int32*)pInput, length / bytesPerSample, _audioVolumePlayback);
						break;
					case ma_format_f32:
						ma_copy_and_apply_volume_factor_f32((float*)pOutput, (float*)pInput, length / bytesPerSample, _audioVolumePlayback);
						break;
					}
				}
				else
				{
					memset(pOutput, 0, length);
				}
			}

			{
				std::lock_guard<std::mutex> lock(_sourceMutex);

				_audioBuffer->insertBlock(tempBuffer.get(), length);

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

bool AudioAnalyzerCapturer::loadAudio(uint32 periodSizeInFrames, uint32 periodSizeInMilliseconds, uint32 periods)
{
	_totalBufferSize = 0;

	const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfoPtr = _audioInfo.get();
	if (audioInfoPtr)
	{
		if (_initPlayBackDevice)
		{
			_captureDeviceManager.deviceConfig = ma_device_config_init(ma_device_type_duplex);
			_captureDeviceManager.deviceConfig.playback.format = (ma_format)PCAudioAnalyzer::formatToMa_format(audioInfoPtr->audioFormat, audioInfoPtr->bitDepth);
			_captureDeviceManager.deviceConfig.playback.channels = audioInfoPtr->numChannels;
		}
		else
		{
			_captureDeviceManager.deviceConfig = ma_device_config_init(ma_device_type_capture);
		}

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
	
		_captureDeviceManager.deviceConfig.capture.shareMode = ma_share_mode_shared;
		_captureDeviceManager.deviceConfig.capture.format = (ma_format)PCAudioAnalyzer::formatToMa_format(audioInfoPtr->audioFormat, audioInfoPtr->bitDepth);
		_captureDeviceManager.deviceConfig.capture.channels = audioInfoPtr->numChannels;

		_captureDeviceManager.deviceConfig.sampleRate = audioInfoPtr->sampleRate;
		_captureDeviceManager.deviceConfig.dataCallback = AudioAnalyzerCapturer::s_captureCallBack;
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
				if (_initPlayBackDevice)
				{
					if (_devicePlayerInfo.isDefaultDevice())
					{
						_captureDeviceManager.deviceConfig.playback.pDeviceID = NULL;
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
								_captureDeviceManager.deviceConfig.playback.pDeviceID = &device_info.id;
							}
						}

						if (!deviceFound)
						{
							UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Playback Device Not found %s, using default system device"), *desiredDeviceName);
						}
					}
				}

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

					if (_initPlayBackDevice)
					{
						_devicePlayerInfo.numChannels = _captureDeviceManager.device.playback.internalChannels;
						PCAudioAnalyzer::ma_formatToFormat(_captureDeviceManager.device.playback.internalFormat, _devicePlayerInfo.format, _devicePlayerInfo.bitDepth);
						_devicePlayerInfo.bufferFrames = _captureDeviceManager.device.playback.internalPeriodSizeInFrames;
						_devicePlayerInfo.sampleRate = _captureDeviceManager.device.playback.internalSampleRate;
					}
					else
					{
						_devicePlayerInfo.numChannels = 0;
						_devicePlayerInfo.bufferFrames = 0;
						_devicePlayerInfo.sampleRate = 0;
					}

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

void AudioAnalyzerCapturer::start(bool playback, bool enableCaptureDataBroadcast)
{
	if (!_isCapturing)
	{
		_enableOnCapturedData = enableCaptureDataBroadcast;

		if (!_initPlayBackDevice && playback)
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Missing Playback device, Are you missing initialization in InitCapturerAudioEx node?"));
			_enablePlayBack = false;
		}
		else
		{
			_enablePlayBack = playback;
		}

		if (_captureDeviceManager.hasDevice)
		{
			ma_result result = ma_device_start(&_captureDeviceManager.device);
			if (result != MA_SUCCESS)
			{
				UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Start capture has failed. ErrorCode: %d"), result);
			}
			else
			{
				_isCapturing = true;
			}
		}
	}
}

void AudioAnalyzerCapturer::stop()
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

bool AudioAnalyzerCapturer::isCapturing() const 
{
	return _isCapturing;
}

void AudioAnalyzerCapturer::unloadAudio()
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

float AudioAnalyzerCapturer::getPlaybackProgress(float& bufferPosTime)
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

float AudioAnalyzerCapturer::getPlaybackProgress()
{
	float dummyValue;
	return getPlaybackProgress(dummyValue);
}

void AudioAnalyzerCapturer::getInputAudioDeviceNames(TArray<FString>& audioDeviceNames)
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
			audioDeviceNames.Init("", captureDeviceCount + 1);
			audioDeviceNames[0] = AudioAnalyzerDeviceInfo::SYSTEM_DEVICE_NAME.c_str();

			for (ma_uint32 currentDevice = 0; currentDevice < captureDeviceCount; ++currentDevice)
			{
				audioDeviceNames[currentDevice + 1] = UTF8_TO_TCHAR(pCaptureDeviceInfos[currentDevice].name);
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

void AudioAnalyzerCapturer::setDefaultAudioDevices(const FString& capturerDeviceName, const FString& playerDeviceName)
{
	_deviceCapturerInfo.name = TCHAR_TO_UTF8(*capturerDeviceName);
	_devicePlayerInfo.name = TCHAR_TO_UTF8(*playerDeviceName);
}

void AudioAnalyzerCapturer::getDefaultAudioDevices(FString& capturerDeviceName, FString& playerDeviceName)
{
	capturerDeviceName = UTF8_TO_TCHAR(_deviceCapturerInfo.name.c_str());
	playerDeviceName = UTF8_TO_TCHAR(_devicePlayerInfo.name.c_str());
}

const AudioAnalyzerDeviceInfo& AudioAnalyzerCapturer::getAudioCapturerDeviceInfo() const
{
	return _deviceCapturerInfo;
}

const AudioAnalyzerDeviceInfo& AudioAnalyzerCapturer::getAudioPlayerDeviceInfo() const
{
	return _devicePlayerInfo;
}

float AudioAnalyzerCapturer::getCaptureVolume()
{
	std::lock_guard<std::mutex> lock(_sourceMutex);
	return _audioVolumeCapture;
}

void AudioAnalyzerCapturer::setCaptureVolume(float volume)
{
	std::lock_guard<std::mutex> lock(_sourceMutex);
	if (volume < 0)
	{
		_audioVolumeCapture = 0.0;
	}
	else if (volume > 1.0)
	{
		_audioVolumeCapture = 1.0;
	}
	else
	{
		_audioVolumeCapture = volume;
	}
}

float AudioAnalyzerCapturer::getPlaybackVolume()
{
	std::lock_guard<std::mutex> lock(_sourceMutex);
	return _audioVolumePlayback;
}

void AudioAnalyzerCapturer::setPlaybackVolume(float volume)
{
	std::lock_guard<std::mutex> lock(_sourceMutex);
	if (volume < 0)
	{
		_audioVolumePlayback = 0.0;
	}
	else if (volume > 1.0)
	{
		_audioVolumePlayback = 1.0;
	}
	else
	{
		_audioVolumePlayback = volume;
	}
}

void AudioAnalyzerCapturer::registerOnCapturedData(std::function<void(TArray<uint8>)> callback)
{
	onCapturedData = callback;
}