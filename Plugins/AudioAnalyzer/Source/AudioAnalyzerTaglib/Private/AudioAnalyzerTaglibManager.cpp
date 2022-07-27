// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#include "AudioAnalyzerTaglibManager.h"

#include "tbytevector.h"
#include "tbytevectorstream.h"
#include "wavfile.h"
#include "infotag.h"
#include "mpegfile.h"
#include "id3v1tag.h"
#include "id3v2tag.h"
#include "id3v2framefactory.h"
#include "attachedpictureframe.h"
#include "oggfile.h"
#include "vorbisfile.h"
#include "xiphcomment.h"
#include "flacfile.h"
#include "flacpicture.h"

DEFINE_LOG_CATEGORY(LogParallelcubeTaglib);

AudioAnalyzerTaglibManager::MetadataArt::MetadataArt() :_extension(""), _size(0), _data(NULL)
{
}

AudioAnalyzerTaglibManager::MetadataArt::MetadataArt(const std::string& extension, size_t buffer_size, char* buffer)
{
	_extension = extension;
	_size = buffer_size;
	_data = (char*)malloc(_size);
	memmove(_data, buffer, _size);
}

AudioAnalyzerTaglibManager::MetadataArt::~MetadataArt()
{
	if (_size > 0)
		free(_data);
}

AudioAnalyzerTaglibManager::AudioAnalyzerTaglibManager()
{
	_audioMetadata.reset(new AUDIOFILE_METADATA());
}

AudioAnalyzerTaglibManager::~AudioAnalyzerTaglibManager()
{
}

void AudioAnalyzerTaglibManager::setFileInfo(const std::string fileName, const std::string fileExtension)
{
	if (_audioMetadata.get())
	{
		_audioMetadata->fileName = fileName;
		_audioMetadata->fileExtension = fileExtension;
	}
}

void AudioAnalyzerTaglibManager::loadMetadataFromMemory(char* dataPtr, unsigned int dataSize)
{
	if (_audioMetadata.get())
	{
		_audioMetadata->metatype = "";
		_audioMetadata->title = "";
		_audioMetadata->artist = "";
		_audioMetadata->album = "";
		_audioMetadata->year = "";
		_audioMetadata->genre = "";
		_audioMetadata->pictures.clear();

		if (_audioMetadata->fileExtension == "wav")
		{
			loadMetadataFromWav(dataPtr, dataSize);
		}
		else if (_audioMetadata->fileExtension == "mp3")
		{
			loadMetadataFromMp3(dataPtr, dataSize);
		}
		else if (_audioMetadata->fileExtension == "ogg" || _audioMetadata->fileExtension == "egg")
		{
			loadMetadataFromOgg(dataPtr, dataSize);
		}
		else if (_audioMetadata->fileExtension == "flac")
		{
			loadMetadataFromFlac(dataPtr, dataSize);
		}
	}
}

void AudioAnalyzerTaglibManager::loadMetadataFromWav(char* dataPtr, unsigned int dataSize)
{
	TagLib::ByteVector tl_byteVector(dataPtr, dataSize);
	TagLib::ByteVectorStream tl_iostream(tl_byteVector);
	TagLib::RIFF::WAV::File tl_wavfile(&tl_iostream, false);
	if (tl_wavfile.hasID3v2Tag())
	{
		TagLib::ID3v2::Tag* tl_id3v2tag = tl_wavfile.ID3v2Tag();
		if (tl_id3v2tag)
		{
			_audioMetadata->title = tl_id3v2tag->title().toCString();
			_audioMetadata->artist = tl_id3v2tag->artist().toCString();
			_audioMetadata->album = tl_id3v2tag->album().toCString();
			_audioMetadata->year = to_string(tl_id3v2tag->year());
			_audioMetadata->genre = tl_id3v2tag->genre().toCString();
			_audioMetadata->pictures.clear();
			_audioMetadata->metatype = "ID3_V2." + to_string(tl_id3v2tag->header()->majorVersion());
		}
	}
	else if (tl_wavfile.hasInfoTag())
	{
		TagLib::RIFF::Info::Tag* tl_infotag = tl_wavfile.InfoTag();
		if (tl_infotag)
		{
			_audioMetadata->title = tl_infotag->title().toCString();
			_audioMetadata->artist = tl_infotag->artist().toCString();
			_audioMetadata->album = tl_infotag->album().toCString();
			_audioMetadata->year = to_string(tl_infotag->year());
			_audioMetadata->genre = tl_infotag->genre().toCString();
			_audioMetadata->metatype = "INFO";
			_audioMetadata->pictures.clear();
		}
	}
}

void AudioAnalyzerTaglibManager::loadMetadataFromMp3(char* dataPtr, unsigned int dataSize)
{
	TagLib::ByteVector tl_byteVector(dataPtr, dataSize);
	TagLib::ByteVectorStream tl_iostream(tl_byteVector);
	TagLib::MPEG::File tl_mpegfile(&tl_iostream, TagLib::ID3v2::FrameFactory::instance(), false);
	if (tl_mpegfile.hasID3v2Tag())
	{
		TagLib::ID3v2::Tag* tl_id3v2tag = tl_mpegfile.ID3v2Tag();
		if (tl_id3v2tag)
		{
			_audioMetadata->title = tl_id3v2tag->title().toCString();
			_audioMetadata->artist = tl_id3v2tag->artist().toCString();
			_audioMetadata->album = tl_id3v2tag->album().toCString();
			_audioMetadata->year = to_string(tl_id3v2tag->year());
			_audioMetadata->genre = tl_id3v2tag->genre().toCString();
			_audioMetadata->pictures.clear();

			unsigned int majorVersion = tl_id3v2tag->header()->majorVersion();
			_audioMetadata->metatype = "ID3_V2." + to_string(majorVersion);

			if (majorVersion >= 3)
			{
				const TagLib::ID3v2::FrameList& tl_framelist = tl_id3v2tag->frameList("APIC");
				for (TagLib::ID3v2::FrameList::ConstIterator tl_picture_it = tl_framelist.begin();
					tl_picture_it != (tl_framelist.end());
					++tl_picture_it)
				{
					TagLib::ID3v2::AttachedPictureFrame* tl_frame = static_cast<TagLib::ID3v2::AttachedPictureFrame*>(*tl_picture_it);

					if (tl_frame)
					{
						TagLib::String tl_mimetype = tl_frame->mimeType();
						std::string img_ext("");
						bool supportedFormat = true;
						if (tl_mimetype == "image/jpeg")
						{
							img_ext = ".jpeg";
						}
						else if (tl_mimetype == "image/jpg")
						{
							img_ext = ".jpg";
						}
						else if (tl_mimetype == "image/png")
						{
							img_ext = ".png";
						}
						else
						{
							supportedFormat = false;
							//unsupported format
						}

						if (supportedFormat)
						{
							std::unique_ptr<MetadataArt> picturePtr(new MetadataArt(img_ext, tl_frame->picture().size(), tl_frame->picture().data()));
							_audioMetadata->pictures.push_back(std::move(picturePtr));
						}
					}
				}
			}
		}
		else if (tl_mpegfile.hasID3v1Tag())
		{
			TagLib::ID3v1::Tag* tl_id3v1tag = tl_mpegfile.ID3v1Tag();
			if (tl_id3v1tag)
			{
				_audioMetadata->title = tl_id3v1tag->title().toCString();
				_audioMetadata->artist = tl_id3v1tag->artist().toCString();
				_audioMetadata->album = tl_id3v1tag->album().toCString();
				_audioMetadata->year = to_string(tl_id3v1tag->year());
				_audioMetadata->genre = tl_id3v1tag->genre().toCString();
				_audioMetadata->metatype = "ID3_V1";
				_audioMetadata->pictures.clear();
			}
		}
	}
}

void AudioAnalyzerTaglibManager::loadMetadataFromOgg(char* dataPtr, unsigned int dataSize)
{
	TagLib::ByteVector tl_byteVector(dataPtr, dataSize);
	TagLib::ByteVectorStream tl_iostream(tl_byteVector);
	TagLib::Vorbis::File tl_vorbisfile(&tl_iostream);
	TagLib::Ogg::XiphComment* tl_oggtag = tl_vorbisfile.tag();

	if (tl_oggtag)
	{
		_audioMetadata->title = tl_oggtag->title().toCString();
		_audioMetadata->artist = tl_oggtag->artist().toCString();
		_audioMetadata->album = tl_oggtag->album().toCString();
		_audioMetadata->year = to_string(tl_oggtag->year());
		_audioMetadata->genre = tl_oggtag->genre().toCString();
		_audioMetadata->metatype = "XIPH_Comment";
		_audioMetadata->pictures.clear();

		TagLib::List<TagLib::FLAC::Picture*> tl_framelist(tl_oggtag->pictureList());
		for (TagLib::List<TagLib::FLAC::Picture*>::ConstIterator tl_picture_it = tl_framelist.begin();
			tl_picture_it != (tl_framelist.end());
			++tl_picture_it)
		{
			TagLib::FLAC::Picture* tl_frame = *tl_picture_it;

			if (tl_frame)
			{
				TagLib::String tl_mimetype = tl_frame->mimeType();
				std::string img_ext("");
				bool supportedFormat = true;
				if (tl_mimetype == "image/jpeg")
				{
					img_ext = ".jpeg";
				}
				else if (tl_mimetype == "image/jpg")
				{
					img_ext = ".jpg";
				}
				else if (tl_mimetype == "image/png")
				{
					img_ext = ".png";
				}
				else
				{
					supportedFormat = false;
					//unsupported format
				}

				if (supportedFormat)
				{
					std::unique_ptr<MetadataArt> picturePtr(new MetadataArt(img_ext, tl_frame->data().size(), tl_frame->data().data()));
					_audioMetadata->pictures.push_back(std::move(picturePtr));
				}
			}
		}
	}
}

void AudioAnalyzerTaglibManager::loadMetadataFromFlac(char* dataPtr, unsigned int dataSize)
{
	TagLib::ByteVector tl_byteVector(dataPtr, dataSize);
	TagLib::ByteVectorStream tl_iostream(tl_byteVector);
	TagLib::FLAC::File tl_flacfile(&tl_iostream, TagLib::ID3v2::FrameFactory::instance(), false);
	if (tl_flacfile.hasXiphComment())
	{
		TagLib::Ogg::XiphComment* tl_oggtag = tl_flacfile.xiphComment();
		if (tl_oggtag)
		{
			_audioMetadata->title = tl_oggtag->title().toCString();
			_audioMetadata->artist = tl_oggtag->artist().toCString();
			_audioMetadata->album = tl_oggtag->album().toCString();
			_audioMetadata->year = to_string(tl_oggtag->year());
			_audioMetadata->genre = tl_oggtag->genre().toCString();
			_audioMetadata->metatype = "XIPH_Comment";
			_audioMetadata->pictures.clear();

			TagLib::List<TagLib::FLAC::Picture*> tl_framelist(tl_flacfile.pictureList());
			for (TagLib::List<TagLib::FLAC::Picture*>::ConstIterator tl_picture_it = tl_framelist.begin();
				tl_picture_it != (tl_framelist.end());
				++tl_picture_it)
			{
				TagLib::FLAC::Picture* tl_frame = *tl_picture_it;

				if (tl_frame)
				{
					TagLib::String tl_mimetype = tl_frame->mimeType();
					std::string img_ext("");
					bool supportedFormat = true;
					if (tl_mimetype == "image/jpeg")
					{
						img_ext = ".jpeg";
					}
					else if (tl_mimetype == "image/jpg")
					{
						img_ext = ".jpg";
					}
					else if (tl_mimetype == "image/png")
					{
						img_ext = ".png";
					}
					else
					{
						supportedFormat = false;
						//unsupported format
					}

					if (supportedFormat)
					{
						std::unique_ptr<MetadataArt> picturePtr(new MetadataArt(img_ext, tl_frame->data().size(), tl_frame->data().data()));
						_audioMetadata->pictures.push_back(std::move(picturePtr));
					}
				}
			}
		}
		else if (tl_flacfile.hasID3v2Tag())
		{
			TagLib::ID3v2::Tag* tl_id3v2tag = tl_flacfile.ID3v2Tag();
			if (tl_id3v2tag)
			{
				_audioMetadata->title = tl_id3v2tag->title().toCString();
				_audioMetadata->artist = tl_id3v2tag->artist().toCString();
				_audioMetadata->album = tl_id3v2tag->album().toCString();
				_audioMetadata->year = to_string(tl_id3v2tag->year());
				_audioMetadata->genre = tl_id3v2tag->genre().toCString();
				_audioMetadata->pictures.clear();

				unsigned int majorVersion = tl_id3v2tag->header()->majorVersion();
				_audioMetadata->metatype = "ID3_V2." + to_string(majorVersion);

				if (majorVersion >= 3)
				{
					TagLib::ID3v2::FrameList tl_framelist = tl_id3v2tag->frameList("APIC");
					for (TagLib::ID3v2::FrameList::ConstIterator tl_picture_it = tl_framelist.begin();
						tl_picture_it != (tl_framelist.end());
						++tl_picture_it)
					{
						TagLib::ID3v2::AttachedPictureFrame* tl_frame = static_cast<TagLib::ID3v2::AttachedPictureFrame*>(*tl_picture_it);

						if (tl_frame)
						{
							TagLib::String tl_mimetype = tl_frame->mimeType();
							std::string img_ext("");
							bool supportedFormat = true;
							if (tl_mimetype == "image/jpeg")
							{
								img_ext = ".jpeg";
							}
							else if (tl_mimetype == "image/jpg")
							{
								img_ext = ".jpg";
							}
							else if (tl_mimetype == "image/png")
							{
								img_ext = ".png";
							}
							else
							{
								supportedFormat = false;
								//unsupported format
							}

							if (supportedFormat)
							{
								std::unique_ptr<MetadataArt> picturePtr(new MetadataArt(img_ext, tl_frame->picture().size(), tl_frame->picture().data()));
								_audioMetadata->pictures.push_back(std::move(picturePtr));
							}
						}
					}
				}
			}
		}
		else if (tl_flacfile.hasID3v1Tag())
		{
			TagLib::ID3v1::Tag* tl_id3v1tag = tl_flacfile.ID3v1Tag();
			if (tl_id3v1tag)
			{
				_audioMetadata->title = tl_id3v1tag->title().toCString();
				_audioMetadata->artist = tl_id3v1tag->artist().toCString();
				_audioMetadata->album = tl_id3v1tag->album().toCString();
				_audioMetadata->year = to_string(tl_id3v1tag->year());
				_audioMetadata->genre = tl_id3v1tag->genre().toCString();
				_audioMetadata->metatype = "ID3_V1";
				_audioMetadata->pictures.clear();
			}
		}
	}
}