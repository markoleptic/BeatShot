// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.

#include "AudioAnalyzerDecoder.h"
#include "AudioAnalyzerConfig.h"

#include "Misc/FileHelper.h"
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>

#include "dr_wav.h"
#include "dr_mp3.h"
#include "dr_flac.h"
#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"
#include "miniaudio.h"

#include <sstream>

/**
* @brief Memory File
*
* Class to manage files loaded into memory
*/
struct MemoryFile
{
	PCAudioAnalyzer::AUDIOFILE_INFO*	header; /**< Pointer to audio file info */
	uint8*								currentPtr; /**< Reading pointer */
	uint8*								bufferPtr; /**< Raw Audio Data start*/
	uint32_t							bufferSize; /**< Raw Audio Data size*/
};

AudioAnalyzerDecoder::AudioAnalyzerDecoder():_hasAudioDecoder(false),
											 _audioFileType(AudioFileType::None),
											 _onlyHeaderDecode(false),
											 _enableMetadataLoad(true)
{
	_audioMetadataManager.reset(new AudioAnalyzerTaglibManager());
}

AudioAnalyzerDecoder::~AudioAnalyzerDecoder()
{
	if (_hasAudioDecoder)
	{
		ma_decoder_uninit(&_audioDecoder);
		_hasAudioDecoder = false;
	}
}

bool AudioAnalyzerDecoder::loadAudioInfo(const FString& filePath, bool onlyHeader)
{
	if (_hasAudioDecoder)
	{
		ma_decoder_uninit(&_audioDecoder);
		_hasAudioDecoder = false;
	}

	_onlyHeaderDecode = onlyHeader;
	if (!FFileHelper::LoadFileToArray(_rawFile, *filePath))
	{
		return false;
	}
	uint8* data = _rawFile.GetData();
	unsigned int dataLength = 0;
	dataLength = _rawFile.Num() * sizeof(uint8);

	bool validHeader = false;
	//get extension
	std::string filepathStr = std::string(TCHAR_TO_UTF8(*filePath));
	std::string::size_type lastSlashPos = filepathStr.rfind('/');

	std::string fileName = "";
	if (lastSlashPos != std::string::npos)
	{
		fileName = filepathStr.substr(lastSlashPos + 1);
	}
	else
	{
		fileName = filepathStr;
	}

	std::string::size_type extensionPos = fileName.rfind('.');
	if (extensionPos != std::string::npos)
	{
		if (_audioMetadataManager.get())
		{
			_audioMetadataManager->setFileInfo(fileName.substr(0, extensionPos), fileName.substr(extensionPos + 1));
			if (_enableMetadataLoad)
			{
				_audioMetadataManager->loadMetadataFromMemory(reinterpret_cast<char*>(data), dataLength);
			}

			const AudioAnalyzerTaglibManager::AUDIOFILE_METADATA* audioMetadata = _audioMetadataManager->getMetadata();
			if (audioMetadata)
			{
				if (audioMetadata->fileExtension == "wav")
				{
					UE_LOG(LogParallelcubeAudioAnalyzer, Log, TEXT("Starting audio header decode: %s"), *filePath);
					validHeader = loadAudioInfoFromWav(data, dataLength);
					UE_LOG(LogParallelcubeAudioAnalyzer, Log, TEXT("Finished audio header decode"));
				}
				else if (audioMetadata->fileExtension == "mp3")
				{
					UE_LOG(LogParallelcubeAudioAnalyzer, Log, TEXT("Starting audio header decode: %s"), *filePath);
					validHeader = loadAudioInfoFromMp3(data, dataLength);
					UE_LOG(LogParallelcubeAudioAnalyzer, Log, TEXT("Finished audio header decode"));
				}
				else if (audioMetadata->fileExtension == "ogg" || audioMetadata->fileExtension == "egg")
				{
					UE_LOG(LogParallelcubeAudioAnalyzer, Log, TEXT("Starting audio header decode: %s"), *filePath);
					validHeader = loadAudioInfoFromOgg(data, dataLength);
					UE_LOG(LogParallelcubeAudioAnalyzer, Log, TEXT("Finished audio header decode"));
				}
				else if (audioMetadata->fileExtension == "flac")
				{
					UE_LOG(LogParallelcubeAudioAnalyzer, Log, TEXT("Starting audio header decode: %s"), *filePath);
					validHeader = loadAudioInfoFromFlac(data, dataLength);
					UE_LOG(LogParallelcubeAudioAnalyzer, Log, TEXT("Finished audio header decode"));
				}
			}
		}
	}
	else
	{
		_audioMetadataManager->setFileInfo(fileName, "");
	}

	if (!validHeader)
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Audio file format not supported"));
		return false;
	}
	else
	{
		return true;
	}
}

int AudioAnalyzerDecoder::getStrOffset(uint8* dataPtr, unsigned int dataSize, std::string strToSearch)
{
	int strOffset = -1;
	int strLength = (int)strToSearch.length();

	for (unsigned int dataOffset = 0; dataOffset < (dataSize - strLength); ++dataOffset)
	{
		std::string section(dataPtr + dataOffset, dataPtr + dataOffset + strLength);

		if (section == strToSearch)
		{
			strOffset = dataOffset;
			break;
		}
	}

	return strOffset;
}

int32_t AudioAnalyzerDecoder::fourBytesToInt(uint8* dataPtr, int dataOffset, Endian endianness)
{
	int32_t result;
	if (endianness == Endian::Little)
	{
		result = (dataPtr[dataOffset + 3] << 24) | 
				(dataPtr[dataOffset + 2] << 16) | 
				(dataPtr[dataOffset + 1] << 8) | 
				(dataPtr[dataOffset]);
	}
	else
	{
		result = (dataPtr[dataOffset] << 24) |
				(dataPtr[dataOffset + 1] << 16) | 
				(dataPtr[dataOffset + 2] << 8) | 
				(dataPtr[dataOffset + 3]);
	}

	return result;
}

int16_t AudioAnalyzerDecoder::twoBytesToInt(uint8* dataPtr, int dataOffset, Endian endianness)
{
	int16_t result;
	if (endianness == Endian::Little)
	{
		result = (dataPtr[dataOffset + 1] << 8) |
				(dataPtr[dataOffset]);
	}
	else
	{
		result = (dataPtr[dataOffset] << 8) | 
				(dataPtr[dataOffset + 1]);
	}

	return result;
}

bool AudioAnalyzerDecoder::loadAudioInfoFromMp3(uint8* dataPtr, unsigned int dataSize)
{
	_audioFileType = AudioFileType::MP3;

	ma_result result = ma_decoder_init_memory_mp3(dataPtr, dataSize, NULL, &_audioDecoder);
	if (result == MA_SUCCESS)
	{
		_audioInfo.reset(new PCAudioAnalyzer::AUDIOFILE_INFO);

		_audioInfo->numChannels = _audioDecoder.outputChannels;
		_audioInfo->sampleRate = _audioDecoder.outputSampleRate;

		ma_formatToFormat(_audioDecoder.outputFormat, _audioInfo->audioFormat, _audioInfo->bitDepth);
		ma_uint64 totalFrameCount = ma_decoder_get_length_in_pcm_frames(&_audioDecoder);
		_audioInfo->dataChunkSize = totalFrameCount * ma_get_bytes_per_frame(_audioDecoder.outputFormat, _audioDecoder.outputChannels);
		_decodedPCMBuffer.reset(new ma_uint8[_audioInfo->dataChunkSize]);
		ma_decoder_read_pcm_frames(&_audioDecoder, _decodedPCMBuffer.get(), totalFrameCount);
		_audioInfo->dataChunkRawData = _decodedPCMBuffer.get();

		_hasAudioDecoder = true;

		return true;
}

	return false;
}

bool AudioAnalyzerDecoder::loadAudioInfoFromOgg(uint8* dataPtr, unsigned int dataSize)
{
	_audioFileType = AudioFileType::OGG;

	//vorbis_fix
	ma_uint64 totalFrameCount = 0;
	int errorCode = 0;
	stb_vorbis* vorbisFile = stb_vorbis_open_memory(dataPtr, dataSize, &errorCode, NULL);
	if (vorbisFile)
	{
		totalFrameCount = stb_vorbis_stream_length_in_samples(vorbisFile);
		stb_vorbis_close(vorbisFile);
	}


	ma_result result = ma_decoder_init_memory_vorbis(dataPtr, dataSize, NULL, &_audioDecoder);
	if (result == MA_SUCCESS)
	{
		_audioInfo.reset(new PCAudioAnalyzer::AUDIOFILE_INFO);

		_audioInfo->numChannels = _audioDecoder.outputChannels;
		_audioInfo->sampleRate = _audioDecoder.outputSampleRate;

		ma_formatToFormat(_audioDecoder.outputFormat, _audioInfo->audioFormat, _audioInfo->bitDepth);
		//ma_uint64 totalFrameCount = ma_decoder_get_length_in_pcm_frames(&_audioDecoder);
		_audioInfo->dataChunkSize = totalFrameCount * ma_get_bytes_per_frame(_audioDecoder.outputFormat, _audioDecoder.outputChannels);
		_decodedPCMBuffer.reset(new ma_uint8[_audioInfo->dataChunkSize]);
		ma_decoder_read_pcm_frames(&_audioDecoder, _decodedPCMBuffer.get(), totalFrameCount);
		_audioInfo->dataChunkRawData = _decodedPCMBuffer.get();

		_hasAudioDecoder = true;

		return true;
	}

	return false;
}

bool AudioAnalyzerDecoder::loadAudioInfoFromFlac(uint8* dataPtr, unsigned int dataSize)
{
	_audioFileType = AudioFileType::FLAC;

	ma_result result = ma_decoder_init_memory_flac(dataPtr, dataSize, NULL, &_audioDecoder);
	if (result == MA_SUCCESS)
	{
		_audioInfo.reset(new PCAudioAnalyzer::AUDIOFILE_INFO);

		_audioInfo->numChannels = _audioDecoder.outputChannels;
		_audioInfo->sampleRate = _audioDecoder.outputSampleRate;

		ma_formatToFormat(_audioDecoder.outputFormat, _audioInfo->audioFormat, _audioInfo->bitDepth);
		ma_uint64 totalFrameCount = ma_decoder_get_length_in_pcm_frames(&_audioDecoder);
		_audioInfo->dataChunkSize = totalFrameCount * ma_get_bytes_per_frame(_audioDecoder.outputFormat, _audioDecoder.outputChannels);
		_decodedPCMBuffer.reset(new ma_uint8[_audioInfo->dataChunkSize]);
		ma_decoder_read_pcm_frames(&_audioDecoder, _decodedPCMBuffer.get(), totalFrameCount);
		_audioInfo->dataChunkRawData = _decodedPCMBuffer.get();

		_hasAudioDecoder = true;

		return true;
	}

	return false;
}

bool AudioAnalyzerDecoder::loadAudioInfoFromWav(uint8* dataPtr, unsigned int dataSize)
{
	std::string headerChunkID(dataPtr, dataPtr + 4);
	std::string format(dataPtr + 8, dataPtr + 12);

	if (headerChunkID == "RIFF" || format == "WAVE")
	{
		_audioFileType = AudioFileType::WAV;
		ma_result result = ma_decoder_init_memory_wav(dataPtr, dataSize, NULL, &_audioDecoder);
		if (result == MA_SUCCESS)
		{
			_audioInfo.reset(new PCAudioAnalyzer::AUDIOFILE_INFO);

			_audioInfo->numChannels = _audioDecoder.outputChannels;
			_audioInfo->sampleRate = _audioDecoder.outputSampleRate;

			ma_formatToFormat(_audioDecoder.outputFormat, _audioInfo->audioFormat, _audioInfo->bitDepth);
			ma_uint64 totalFrameCount = ma_decoder_get_length_in_pcm_frames(&_audioDecoder);
			_audioInfo->dataChunkSize = totalFrameCount * ma_get_bytes_per_frame(_audioDecoder.outputFormat, _audioDecoder.outputChannels);
			_decodedPCMBuffer.reset(new ma_uint8[_audioInfo->dataChunkSize]);
			ma_decoder_read_pcm_frames(&_audioDecoder, _decodedPCMBuffer.get(), totalFrameCount);
			_audioInfo->dataChunkRawData = _decodedPCMBuffer.get();

			_hasAudioDecoder = true;

			return true;
		}
	}

	return false;
}

void AudioAnalyzerDecoder::moveAudioInfo(std::unique_ptr<PCAudioAnalyzer::AUDIOFILE_INFO>& destination)
{
	destination = std::move(_audioInfo);
}

void AudioAnalyzerDecoder::getMetadata(FString& Filename, FString& Extension, FString& MetaType, FString& Title, FString& Artist, FString& Album, FString& Year, FString& Genre)
{
	if (_audioMetadataManager.get())
	{
		const AudioAnalyzerTaglibManager::AUDIOFILE_METADATA* audioMetadata = _audioMetadataManager->getMetadata();
		if (audioMetadata)
		{
			Filename = audioMetadata->fileName.c_str();
			Extension = audioMetadata->fileExtension.c_str();
			MetaType = audioMetadata->metatype.c_str();
			Title = audioMetadata->title.c_str();
			Artist = audioMetadata->artist.c_str();
			Album = audioMetadata->album.c_str();
			Year = audioMetadata->year.c_str();
			Genre = audioMetadata->genre.c_str();
		}
	}
}

void AudioAnalyzerDecoder::getMetadataArt(const FString& Prefix, const FString& Folder, int32& NumberOfPictures)
{
	if (_audioMetadataManager.get())
	{
		const AudioAnalyzerTaglibManager::AUDIOFILE_METADATA* audioMetadata = _audioMetadataManager->getMetadata();
		if (audioMetadata)
		{
			int numPicture = 0;
			NumberOfPictures = audioMetadata->pictures.size();
			for (std::vector<std::unique_ptr<AudioAnalyzerTaglibManager::MetadataArt>>::const_iterator picture_it = audioMetadata->pictures.cbegin();
				picture_it != (audioMetadata->pictures.cend());
				++picture_it)
			{
				const AudioAnalyzerTaglibManager::MetadataArt* pictureData = picture_it->get();
				std::ostringstream numPictureStr;
				numPictureStr << numPicture;
				const std::string filename = std::string(TCHAR_TO_UTF8(*Folder)) + "/" + std::string(TCHAR_TO_UTF8(*Prefix)) + numPictureStr.str() + pictureData->getExtension();
				FILE* pFile;
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4996)
#endif 
				pFile = fopen(filename.c_str(), "wb");
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
				if (pFile)
				{
					fwrite(pictureData->getBuffer(), 1, pictureData->getBufferSize(), pFile);
					fclose(pFile);
				}
				else
				{
					UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Missing folder to store media art:%s"), *Folder);
				}
				++numPicture;
			}
		}
	}
}

void AudioAnalyzerDecoder::setEnableMetadataLoad(bool enableMetadataLoad)
{
	_enableMetadataLoad = enableMetadataLoad;
}

void AudioAnalyzerDecoder::freePCMdata()
{
	_audioInfo.reset();
	_decodedPCMBuffer.reset();
	_rawFile.Empty();

	if (_hasAudioDecoder)
	{
		ma_decoder_uninit(&_audioDecoder);
		_hasAudioDecoder = false;
	}
}
