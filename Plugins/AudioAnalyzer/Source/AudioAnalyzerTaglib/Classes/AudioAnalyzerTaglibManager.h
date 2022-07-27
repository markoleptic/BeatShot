// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#ifndef AudioAnalyzerTaglibManager_H
#define AudioAnalyzerTaglibManager_H

#include "CoreMinimal.h"

#include <memory>
#include <string>
#include <vector>
#include <sstream>

/**
* Plugin Log category
*/
DECLARE_LOG_CATEGORY_EXTERN(LogParallelcubeTaglib, Log, All);


/**
* @brief Audio Analyzer Player Source
*
* This class will be used to load audio from a disk file and interact with it using the most basic player options (play/stop/pause)
*/
class PARALLELCUBETAGLIB_API AudioAnalyzerTaglibManager
{
public:

	/**
	* @brief Audio art metadata structure
	*/
	class MetadataArt
	{
	protected:
		/**
		* Picture image format
		*/
		std::string		_extension;

		/**
		* Size of the image file
		*/
		std::size_t		_size;

		/**
		* Binary file
		*/
		char* _data;

	public:
		/**
		* Constructor
		*/
		MetadataArt();

		/**
		* Constructor with parameters
		* @param ext			Extension of the image file
		* @param buffer_size	Size of the image data
		* @param buffer			Image data
		*/
		MetadataArt(const std::string& ext, std::size_t buffer_size, char* buffer);

		/**
		* Destructor
		*/
		~MetadataArt();

		/**
		* Disable copy constructor
		* @param x	---
		*/
		MetadataArt(MetadataArt const& x) = delete;

		/**
		* Disable copy assignment
		* @param x	---
		*/
		void operator=(MetadataArt const& x) = delete;

		/**
		* Returns the image file extension
		* @return File extension
		*/
		inline const std::string& getExtension() const { return _extension; };

		/**
		* Returns the image buffer size
		* @return Buffer size
		*/
		inline const std::size_t& getBufferSize() const { return _size; };

		/**
		* Returns the image buffer
		* @return Buffer
		*/
		inline const char* getBuffer() const { return _data; };
	};

	/**
	* @brief Audio metadata structure
	*/
	struct AUDIOFILE_METADATA {
		std::string fileName; /**< player filename */
		std::string fileExtension; /**< player filename extension */
		std::string metatype; /**< metadata Type */
		std::string title; /**< title */
		std::string artist; /**< artist */
		std::string album; /**< album */
		std::string year; /**< year */
		std::string genre; /**< genre */
		std::vector<std::unique_ptr<MetadataArt>> pictures; /**< data picture array */
	};

	/**
	* Constructor
	*/
	AudioAnalyzerTaglibManager();

	/**
	* Destructor
	* This will unload the loaded audio too
	*/
	~AudioAnalyzerTaglibManager();

	/**
	* Disable copy constructor
	* @param x	---
	*/
	AudioAnalyzerTaglibManager(AudioAnalyzerTaglibManager const &x) = delete;

	/**
	* Disable copy assignment
	* @param x	---
	*/
	void operator=(AudioAnalyzerTaglibManager const &x) = delete;

	/**
	* Sets filename and extension
	* @param fileName			filename 
	* @param fileExtension		extension
	*/
	void setFileInfo(const std::string fileName, const std::string fileExtension);

	/**
	* Get Metadata structure
	* @return metadata structure
	*/
	const AUDIOFILE_METADATA* getMetadata() const { return _audioMetadata.get(); };

	/**
	* Extract audio metadata from memory pointer
	* @param dataPtr		Pointer to the file data in memory
	* @param dataSize		Size of the file memory data (bytes)
	*/
	void loadMetadataFromMemory(char* dataPtr, unsigned int dataSize);

private:

	/**
	* Audio metadata from a file
	*/
	std::unique_ptr<AUDIOFILE_METADATA> _audioMetadata;

	/**
	* Extract audio metadata from memory pointer (wav file)
	* @param memoryPtr		Pointer to the file data in memory
	* @param memorySize		Size of the file memory data (bytes)
	*/
	void loadMetadataFromWav(char* memoryPtr, unsigned int memorySize);

	/**
	* Extract audio metadata from memory pointer (mp3 file)
	* @param memoryPtr		Pointer to the file data in memory
	* @param memorySize		Size of the file memory data (bytes)
	*/
	void loadMetadataFromMp3(char* memoryPtr, unsigned int memorySize);

	/**
	* Extract audio metadata from memory pointer (ogg file)
	* @param memoryPtr		Pointer to the file data in memory
	* @param memorySize		Size of the file memory data (bytes)
	*/
	void loadMetadataFromOgg(char* memoryPtr, unsigned int memorySize);

	/**
	* Extract audio metadata from memory pointer (flac file)
	* @param memoryPtr		Pointer to the file data in memory
	* @param memorySize		Size of the file memory data (bytes)
	*/
	void loadMetadataFromFlac(char* memoryPtr, unsigned int memorySize);

	/**
	* to_string implementation
	* @tparam Type of the value
	* @param value		Value to be converted
	* @return value converted to string
	*/
	template <typename T>
	static std::string to_string(T value)
	{
		std::ostringstream stream;
		stream << value;
		return stream.str();
	}

};

#endif