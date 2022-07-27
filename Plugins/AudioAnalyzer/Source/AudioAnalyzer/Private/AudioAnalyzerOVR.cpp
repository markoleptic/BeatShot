// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#include "AudioAnalyzerOVR.h"
#include "AudioAnalyzerConfig.h"

#include "miniaudio.h"

#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#if AUDIOANALYZER_OVR
	#include <ThirdParty/Oculus/LibOVRPlatform/LibOVRPlatform/include/OVR_Platform.h>
#endif

AudioAnalyzerOVR::AUDIO_DEVICE_MANAGER::AUDIO_DEVICE_MANAGER() :
	hasContext(false),
	hasDevice(false)
{
}

AudioAnalyzerOVR::AudioAnalyzerOVR():   _initPlayBackDevice(false),
										_enablePlayBack(false), 
										_audioVolume(1.0),
										_captureDevice(NULL),
										_totalBufferSize(0),
										_dllHandler(NULL)
{
#if AUDIOANALYZER_OVR
#if PLATFORM_WINDOWS
	_dllHandler = FPlatformProcess::GetDllHandle(*FString("LibOVRPlatform64_1.dll"));
#endif
#endif
}

AudioAnalyzerOVR::~AudioAnalyzerOVR()
{
	unloadAudio();

#if AUDIOANALYZER_OVR
#if PLATFORM_WINDOWS
	if (_dllHandler)
	{
		FPlatformProcess::FreeDllHandle(_dllHandler);
	}
#endif
#endif
}

bool AudioAnalyzerOVR::initializeAudio(uint32 sampleRate, 
										uint8 audioFormat, 
										float audioBufferSeconds, 
										bool initPlaybackDevice, 
										uint32 periodSizeInFrames,
										uint32 periodSizeInMilliseconds,
										uint32 periods)
{
	unloadAudio();

	_audioInfo.reset(new PCAudioAnalyzer::AUDIOFILE_INFO);

	if (audioFormat == PCAudioAnalyzer::AUDIOFILE_INFO_FORMAT::AUDIOFORMAT_FLOAT)
	{
		_audioInfo->bitDepth = 32; 
		_audioInfo->audioFormat = PCAudioAnalyzer::AUDIOFILE_INFO_FORMAT::AUDIOFORMAT_FLOAT;
	}
	else
	{
		_audioInfo->bitDepth = 16; 
		_audioInfo->audioFormat = PCAudioAnalyzer::AUDIOFILE_INFO_FORMAT::AUDIOFORMAT_INT;
	}

	_audioInfo->numChannels = 1;

	if (sampleRate != 48000)
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Invalid sample rate, a sample rate of 48000 will be used"));
	}
	_audioInfo->sampleRate = 48000;
	_audioInfo->dataChunkSize = audioBufferSeconds * _audioInfo->sampleRate * (_audioInfo->bitDepth / 8) * _audioInfo->numChannels;

	_audioBuffer.reset(new QueueBuffer());
	_audioBuffer->initQueueBuffer<uint8>(_audioInfo->dataChunkSize);
	_audioInfo->dataChunkRawData = _audioBuffer->getBufferPtr();

	_feedBuffer.reset(new QueueBuffer());
	_feedBuffer->initQueueBuffer<uint8>(_audioInfo->dataChunkSize);

	_initPlayBackDevice = initPlaybackDevice;

	return loadAudio(periodSizeInFrames, periodSizeInMilliseconds, periods);
}

#if AUDIOANALYZER_OVR
void AudioAnalyzerOVR::playbackCallBack(void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	if (!_enablePlayBack)
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

				if (_feedBuffer)
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

void AudioAnalyzerOVR::captureCallBack()
{
	const size_t samplesAvailable = ovr_Microphone_GetNumSamplesAvailable(_captureDevice);
	if (samplesAvailable > 0)
	{
		if (_deviceCapturerInfo.format == PCAudioAnalyzer::AUDIOFILE_INFO_FORMAT::AUDIOFORMAT_FLOAT)
		{
			std::unique_ptr<float[]> audioBuffer = std::unique_ptr<float[]>(new float[samplesAvailable]);
			const size_t samples = ovr_Microphone_GetPCMFloat(_captureDevice, audioBuffer.get(), samplesAvailable);

			{
				std::lock_guard<std::mutex> lock(_sourceMutex);
				_audioBuffer->insertBlock(audioBuffer.get(), samples * 4);
				_feedBuffer->insertBlock(audioBuffer.get(), samples * 4);
				_totalBufferSize += (samples * 4);
			}
		}
		else
		{
			std::unique_ptr<int16[]> audioBuffer = std::unique_ptr<int16[]>(new int16[samplesAvailable]);
			const size_t samples = ovr_Microphone_GetPCM(_captureDevice, audioBuffer.get(), samplesAvailable);

			{
				std::lock_guard<std::mutex> lock(_sourceMutex);
				_audioBuffer->insertBlock(audioBuffer.get(), samples * 2);
				_feedBuffer->insertBlock(audioBuffer.get(), samples * 2);
				_totalBufferSize += (samples * 2);
			}
		}
	}
}
#else
void AudioAnalyzerOVR::playbackCallBack(void* pOutput, const void* pInput, ma_uint32 frameCount)
{
}

void AudioAnalyzerOVR::captureCallBack()
{
}
#endif

void AudioAnalyzerOVR::s_captureCallBack(void *userdata)
{
	static_cast<AudioAnalyzerOVR*>(userdata)->captureCallBack();
}

void AudioAnalyzerOVR::s_playbackCallBack(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	static_cast<AudioAnalyzerOVR*>(pDevice->pUserData)->playbackCallBack(pOutput, pInput, frameCount);
}

#if AUDIOANALYZER_OVR
bool AudioAnalyzerOVR::loadAudio(uint32 periodSizeInFrames, uint32 periodSizeInMilliseconds, uint32 periods)
{

#if PLATFORM_WINDOWS
	if (!_dllHandler)
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Missing LibOVRPlatform64_1.dll"));
		return false;
	}
#endif

	const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfoPtr = _audioInfo.get();
	if (audioInfoPtr)
	{
		_captureDevice = ovr_Microphone_Create();
		ovr_Microphone_SetAudioDataAvailableCallback(_captureDevice, &AudioAnalyzerOVR::s_captureCallBack, this);

		_deviceCapturerInfo.numChannels = audioInfoPtr->numChannels;
		_deviceCapturerInfo.bitDepth = audioInfoPtr->bitDepth;
		_deviceCapturerInfo.format = audioInfoPtr->audioFormat;
		_deviceCapturerInfo.sampleRate = audioInfoPtr->sampleRate;

		if (_captureDevice == 0)
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Failed to open audio: could not create OVR Microphone"));
			return false;
		}
		else
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
				_audioDeviceManager.deviceConfig.dataCallback = AudioAnalyzerOVR::s_playbackCallBack;
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
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Invalid Audio info:"));
	}

	return false;
}
#else
bool AudioAnalyzerOVR::loadAudio(uint32 periodSizeInFrames, uint32 periodSizeInMilliseconds, uint32 periods)
{
	UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Platform not supported by LibOVRPlatform"));
	return false;
}
#endif

#if AUDIOANALYZER_OVR
void AudioAnalyzerOVR::start(bool playback)
{
#if PLATFORM_WINDOWS
	if (!_dllHandler)
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Missing LibOVRPlatform64_1.dll"));
		return;
	}
#endif

	if (_captureDevice)
	{
		ovr_Microphone_Start(_captureDevice);
		_isCapturing = true;
	}

	if (_audioDeviceManager.hasDevice)
	{
		ma_result result = ma_device_start(&_audioDeviceManager.device);
		if (result != MA_SUCCESS)
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Device start failed. ErrorCode: %d"), result);
			_enablePlayBack = false;
		}
		else
		{
			_enablePlayBack = playback;
		}
	}
}
#else
void AudioAnalyzerOVR::start(bool playback)
{
	UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Platform not supported by LibOVRPlatform"));
}
#endif

#if AUDIOANALYZER_OVR
void AudioAnalyzerOVR::stop()
{
#if PLATFORM_WINDOWS
	if (!_dllHandler)
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Missing LibOVRPlatform64_1.dll"));
		return;
	}
#endif

	if (_initPlayBackDevice && _audioDeviceManager.hasDevice)
	{
		ma_device_stop(&_audioDeviceManager.device);
	}

	if (_captureDevice)
	{
		_isCapturing = false;
		ovr_Microphone_Stop(_captureDevice);
		{
			std::lock_guard<std::mutex> lock(_sourceMutex);
			_totalBufferSize = 0;
		}
	}
}
#else
void AudioAnalyzerOVR::stop()
{
	UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Platform not supported by LibOVRPlatform"));
}
#endif

bool AudioAnalyzerOVR::isCapturing() const 
{
	if (_captureDevice)
	{
		return _isCapturing;
	}
	return false;
}

#if AUDIOANALYZER_OVR
void AudioAnalyzerOVR::unloadAudio()
{
#if PLATFORM_WINDOWS
	if (!_dllHandler)
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Log, TEXT("Missing LibOVRPlatform64_1.dll"));
		return;
	}
#endif

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

	if (_captureDevice)
	{
		_isCapturing = false;
		ovr_Microphone_Destroy(_captureDevice);
		_captureDevice = 0;
	}
}
#else
void AudioAnalyzerOVR::unloadAudio()
{
}
#endif

float AudioAnalyzerOVR::getPlaybackProgress(float& bufferPosTime)
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

float AudioAnalyzerOVR::getPlaybackProgress()
{
	float dummyValue;
	return getPlaybackProgress(dummyValue);
}

void AudioAnalyzerOVR::setDefaultAudioDevices(const FString& capturerDeviceName, const FString& playerDeviceName)
{
	_deviceCapturerInfo.name = TCHAR_TO_UTF8(*capturerDeviceName);
	_devicePlayerInfo.name = TCHAR_TO_UTF8(*playerDeviceName);
}

void AudioAnalyzerOVR::getDefaultAudioDevices(FString& capturerDeviceName, FString& playerDeviceName)
{
	capturerDeviceName = UTF8_TO_TCHAR(_deviceCapturerInfo.name.c_str());
	playerDeviceName = UTF8_TO_TCHAR(_devicePlayerInfo.name.c_str());
}

const AudioAnalyzerDeviceInfo& AudioAnalyzerOVR::getAudioCapturerDeviceInfo() const
{
	return _deviceCapturerInfo;
}

const AudioAnalyzerDeviceInfo& AudioAnalyzerOVR::getAudioPlayerDeviceInfo() const
{
	return _devicePlayerInfo;
}

float AudioAnalyzerOVR::getCaptureVolume()
{
	std::lock_guard<std::mutex> lock(_sourceMutex);
	return _audioVolume;
}

void AudioAnalyzerOVR::setCaptureVolume(float volume)
{
	std::lock_guard<std::mutex> lock(_sourceMutex);
	if (volume < 0)
	{
		_audioVolume = 0.0;
	}
	else if (volume > 1.0)
	{
		_audioVolume = 1.0;
	}
	else
	{
		_audioVolume = volume;
	}
}