// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#ifndef QueueBuffer_H
#define QueueBuffer_H

/**
* @brief Queue buffer implementation
*
* Basic queue buffer implementation
* @tparam Type of the buffer elements
*/

class QueueBuffer
{
public:
	/**
	* Contructor
	*/
	QueueBuffer() : _bufferData(NULL), _bufferSize(0), _filledSize(0)
	{
	}

	/**
	* Buffer Memory Allocation
	* @param bufferSize		Number of elements of the buffer
	*/
	template <class T>
	void initQueueBuffer(size_t bufferSize)
	{
		_bufferSize = bufferSize;
		_bufferData = calloc(bufferSize, sizeof(T));
		_filledSize = 0;
	}

	/**
	* Destructor
	*/
	~QueueBuffer()
	{
		freeQueueBuffer();
	}

	/**
	* Deallocate Buffer memory
	*/
	void freeQueueBuffer()
	{
		if (_bufferData != NULL)
		{
			free(_bufferData);
			_bufferData = NULL;
			_filledSize = 0;
		}
	}


	/**
	* Disable copy constructor
	* @param x	---
	*/
	QueueBuffer(QueueBuffer const &x) = delete;

	/**
	* Disable copy assignment
	* @param x	---
	*/
	void operator=(QueueBuffer const &x) = delete;

	/**
	* Move the data of buffer and copy a new block data by the end.
	* @param newBlockPtr	Pointer to the data to copy
	* @param blockSize		Size of the block (bytes)
	*/
	void insertBlock(const void* newBlockPtr, size_t blockSize)
	{
		int availableSpace = _bufferSize - _filledSize;
		if (availableSpace >= blockSize)
		{
			//insert block 
			memcpy(reinterpret_cast<unsigned char*>(_bufferData) + _filledSize, newBlockPtr, blockSize);
			_filledSize += blockSize;
		}
		else
		{
			//move data
			int sizeToMove = _bufferSize < blockSize ? _bufferSize : blockSize;
			memmove(_bufferData, reinterpret_cast<unsigned char*>(_bufferData) + sizeToMove, _bufferSize - sizeToMove);
			memmove(reinterpret_cast<unsigned char*>(_bufferData) + (_bufferSize - sizeToMove), newBlockPtr, sizeToMove);
			_filledSize = _bufferSize;
		}
	}

	/**
	* Move the data of buffer and copy a new zero block data by the end.
	* @param blockSize		Size of the block (bytes)
	*/
	void insertZeroBlock(size_t blockSize)
	{
		int availableSpace = _bufferSize - _filledSize;
		if (availableSpace >= blockSize)
		{
			//insert block 
			memset(reinterpret_cast<unsigned char*>(_bufferData) + _filledSize, 0, blockSize);
			_filledSize += blockSize;
		}
		else
		{
			//move data
			int sizeToMove = _bufferSize < blockSize ? _bufferSize : blockSize;
			memmove(_bufferData, reinterpret_cast<unsigned char*>(_bufferData) + sizeToMove, _bufferSize - sizeToMove);
			memset(reinterpret_cast<unsigned char*>(_bufferData) + (_bufferSize - sizeToMove), 0, sizeToMove);
		}
	}

	/**
	* Remove data from buffer 
	* @param blockSize		Size of the block (bytes)
	*/
	void removeBlock(size_t blockSize)
	{
		if (blockSize < _filledSize)
		{
			memmove(_bufferData, reinterpret_cast<unsigned char*>(_bufferData) + blockSize, _bufferSize - blockSize);
			_filledSize -= blockSize;
		}
		else
		{
			//memset 0?
			_filledSize = 0;
		}
	}

	/**
	* Returns filled buffer size (elements)
	* @return Buffer size
	*/
	size_t getFilledSize() const
	{
		return _filledSize;
	}

	/**
	* Returns total buffer size (elements)
	* @return Buffer size
	*/
	size_t getBufferSize() const 
	{
		return _bufferSize;
	}

	/**
	* Returns the pointer to the raw data
	* @return Pointer to raw data
	*/
	void* getBufferPtr() const
	{
		return _bufferData;
	}

private:
	/**
	* Pointer to the queue buffer raw data
	*/
	void*					_bufferData;

	/**
	* Total queue buffer size (bytes)
	*/
	size_t					_bufferSize;

	/**
	* Filled queue buffer size (bytes)
	*/
	size_t					_filledSize;
};

#endif