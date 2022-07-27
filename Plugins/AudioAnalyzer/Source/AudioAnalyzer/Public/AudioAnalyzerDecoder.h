// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#ifndef AudioAnalyzerDecoder_H
#define AudioAnalyzerDecoder_H

#include "AudioAnalyzerCommon.h"

#include "AudioAnalyzerTaglibManager.h"

#include "miniaudio.h"

#include <memory>
#include <atomic>

/**
* @brief Audio Analyzer Player Decoder
*
* This class will be used to extract the audio info from the header of the file
*/
class AudioAnalyzerDecoder
{
public:

	/** Enum for Bigendian/LittleEndian options */
	enum class AudioFileType
	{
		None,
		WAV,
		MP3,
		OGG,
		FLAC
	};

	/**
	* Constructor
	*/
	AudioAnalyzerDecoder();

	/**
	* Destructor
	*/
	~AudioAnalyzerDecoder();

	/**
	* Disable copy constructor
	* @param x	---
	*/
	AudioAnalyzerDecoder(AudioAnalyzerDecoder const &x) = delete;

	/**
	* Disable copy assignment
	* @param x	---
	*/
	void operator=(AudioAnalyzerDecoder const &x) = delete;
	
	/**
	* Extracts audio info from a wav file
	* @param filePath		Path to the wav file
	* @param onlyHeader		Decode only header to retrieve metadata info
	* @return				Extraction execution result
	*/
	bool loadAudioInfo(const FString& filePath, bool onlyHeader=false);

	/**
	* Moves the pointer reference of the audio info to a new pointer
	* @param[out] destination		Pointer destination
	*/
	void moveAudioInfo(std::unique_ptr<PCAudioAnalyzer::AUDIOFILE_INFO>& destination);

	/**
	* Returns thirdparty decoder
	* @return		ma_decoder
	*/
	const ma_decoder* getAudioDecoder() const { return &_audioDecoder; }

	/**
	* Returns the file metadata
	* @param[out] Filename		Filename of the audio file
	* @param[out] Extension		Extension of the audio file
	* @param[out] MetaType		ID3_V1 | ID3_V2.3 | ID3_V2.4
	* @param[out] Title			Title of the song
	* @param[out] Artist		Artist 
	* @param[out] Album			Album
	* @param[out] Year			Year
	* @param[out] Genre			Genre
	*/
	void getMetadata(FString& Filename, FString& Extension, FString& MetaType, FString& Title, FString& Artist, FString& Album, FString& Year, FString& Genre);

	/**
	* Extract the AlbumArt pictures into the selected folder. Prefix1.png, Prefix2.png,...
	* @param Prefix						Prefix used to name the pictures.
	* @param Folder						Destination folder
	* @param[out] NumberOfPictures		Number of stored pictures
	*/
	void getMetadataArt(const FString& Prefix, const FString& Folder, int32& NumberOfPictures);

	/**
	* Returns if only has been loaded the header data
	* @return Has onkly header data
	*/
	bool hasOnlyHeader() const { return _onlyHeaderDecode; };

	/**
	* Enables or disables metadata load
	* @param enableMetadataLoad			Enable metadata load
	*/
	void setEnableMetadataLoad(bool enableMetadataLoad);

	/**
	* Free buffer data and metadata
	*/
	void freePCMdata();

private:

	/** Enum for Bigendian/LittleEndian options */
	enum class Endian
	{
		Little, /**< LittleEndian */
		Big /**< BigEndian */
	};

	/**
	* Indicates if the audio decode has been initializated
	*/
	std::atomic<bool>					_hasAudioDecoder;

	/**
	* Main audio decoder
	*/
	ma_decoder							_audioDecoder;

	/**
	* File raw data
	*/
	TArray<uint8>						_rawFile;

	/**
	* Audio info from a file
	*/
	std::unique_ptr<PCAudioAnalyzer::AUDIOFILE_INFO>		_audioInfo;

	/**
	* Audio metadata from a file
	*/
	std::unique_ptr<AudioAnalyzerTaglibManager> _audioMetadataManager;

	/**
	* Audio File type
	*/
	AudioFileType						_audioFileType;

	/**
	* PCM raw data buffer. _audioInfo->dataChunkSize has the size 
	*/
	std::unique_ptr<uint8[]>			_decodedPCMBuffer;

	/**
	* Only the header has been decoded (compressed audio formats)
	*/
	bool								_onlyHeaderDecode;

	/**
	* Enables metadata load
	*/
	bool								_enableMetadataLoad;

	/**
	* Decodes audio data from memory pointer (wav file)
	* @param memoryPtr		Pointer to the file data in memory
	* @param memorySize		Size of the file memory data (bytes)
	* @return				Sucessfull extraction
	*/
	bool loadAudioInfoFromWav(uint8* memoryPtr, unsigned int memorySize);

	/**
	* Decodes audio data from memory pointer (mp3 file)
	* @param memoryPtr		Pointer to the file data in memory
	* @param memorySize		Size of the file memory data (bytes)
	* @return				Sucessfull extraction
	*/
	bool loadAudioInfoFromMp3(uint8* memoryPtr, unsigned int memorySize);

	/**
	* Decodes audio data from memory pointer (ogg vorbis file)
	* @param memoryPtr		Pointer to the file data in memory
	* @param memorySize		Size of the file memory data (bytes)
	* @return				Sucessfull extraction
	*/
	bool loadAudioInfoFromOgg(uint8* memoryPtr, unsigned int memorySize);

	/**
	* Decodes audio data from memory pointer (ogg flac file)
	* @param memoryPtr		Pointer to the file data in memory
	* @param memorySize		Size of the file memory data (bytes)
	* @return				Sucessfull extraction
	*/
	bool loadAudioInfoFromFlac(uint8* memoryPtr, unsigned int memorySize);

	/**
	* Finds the first occurrence of a string into a raw data
	* @param dataPtr		Pointer to the file data in memory
	* @param dataSize		Size of the file memory data
	* @param strToSearch	String to search
	* @return				First string ocurrence position (-1 for no ocurrence)
	*/
	int getStrOffset(uint8* dataPtr, unsigned int dataSize, std::string strToSearch);

	/**
	* Converts 4 consecutive bytes into an int value
	* @param dataPtr		Pointer to the data section
	* @param dataOffset		Offset relative to dataPtr
	* @param endianness		Type of endianness (Little/Big)
	* @return				Conversion int result
	*/
	int32_t fourBytesToInt(uint8* dataPtr, int dataOffset, Endian endianness = Endian::Little);

	/**
	* Converts 2 consecutive bytes into an int value
	* @param dataPtr		Pointer to the data section
	* @param dataOffset		Offset relative to dataPtr
	* @param endianness		Type of endianness (Little/Big)
	* @return				Conversion int result
	*/
	int16_t twoBytesToInt(uint8* dataPtr, int dataOffset, Endian endianness = Endian::Little);

};

#endif