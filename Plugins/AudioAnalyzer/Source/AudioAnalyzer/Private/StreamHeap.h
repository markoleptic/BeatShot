// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#ifndef StreamHeap_H
#define StreamHeap_H

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <memory>

/**
* @brief Stream Heap implementation
*
* Dynamic Heap implementation, max frequency values on top
* @tparam Type of the elements of the Heap
*/
template <class T>
class StreamHeap
{
public:
	/**
	* Contructor
	*/
	StreamHeap()
	{
	}

	/**
	* Contructor
	* @param heapSize	Max size of the heap
	*/
	StreamHeap(size_t heapSize)
	{
		_freqVector.reserve(heapSize);
		_indexMap.reserve(heapSize);
	}

	/**
	* Destructor
	*/
	~StreamHeap()
	{
	}

	/**
	* Disable copy constructor
	* @param x	---
	*/
	StreamHeap(StreamHeap const &x) = delete;

	/**
	* Disable copy assignment
	* @param x	---
	*/
	void operator=(StreamHeap const &x) = delete;

	/**
	* Insert a new element in the Heap and reorder
	* @param value		New heap value
	*/
	void insertElement(T value)
	{
		typename HeapIndexMap::iterator indexMap_it = _indexMap.find(value);
		if (indexMap_it == _indexMap.end())
		{
			_indexMap.insert(std::make_pair(value, _freqVector.size()));
			_freqVector.push_back(std::make_pair(value, 1));
		}
		else
		{
			int currentIndex = indexMap_it->second;
			typename HeapFreqVector::iterator freqVector_it = _freqVector.begin() + currentIndex;

			if (currentIndex == 0)
			{
				freqVector_it->second += 1;
			}
			else
			{
				typename HeapFreqVector::iterator destFreqVector_it = freqVector_it - 1;
				while (destFreqVector_it != _freqVector.begin() && ((freqVector_it->second + 1) > destFreqVector_it->second))
				{
					--destFreqVector_it;
				}

				if ((destFreqVector_it == _freqVector.begin()) && ((freqVector_it->second + 1) > destFreqVector_it->second))
				{
				}
				else
				{
					++destFreqVector_it;
				}

				// swap index
				typename HeapIndexMap::iterator indexMapDest_it = _indexMap.find(destFreqVector_it->first);
				int oldIndex = indexMapDest_it->second;
				indexMapDest_it->second = currentIndex;
				indexMap_it->second = oldIndex;

				//increment frequency and swap
				freqVector_it->second += 1;
				std::iter_swap(freqVector_it, destFreqVector_it);
			}
		}
	}

	/**
	* Returns heap top value (Max frequency)
	* @return T		Heap Top value 
	*/
	T getTop() const 
	{
		return (_freqVector.size() > 0) ? _freqVector[0].first : 0;
	}

	/**
	* Clear Heap values
	*/
	void clear()
	{
		_freqVector.clear();
		_indexMap.clear();
	}


private:

	/**
	* Buffer index container data type
	*/
	typedef std::unordered_map<T, int> HeapIndexMap;

	/**
	* Buffer frequency container data type
	*/
	typedef std::vector<std::pair<T, int> > HeapFreqVector;

	/**
	* Frequency values vector <value, freq>
	*/
	HeapFreqVector	_freqVector;

	/**
	* Index frequency vector map <value, index>
	*/
	HeapIndexMap	_indexMap;

};

/**
* @brief Stream Heap Vector implementation
*
* Vector of StreamHeap interface
* @tparam Type of the elements of the Heap
*/

template <class T>
class StreamHeapVector
{
public:
	/**
	* Constructor
	*/
	StreamHeapVector()
	{
	}

	/**
	* Constructor
	* @param vectorSize		Vector size
	*/
	StreamHeapVector(size_t vectorSize)
	{
		_container.reserve(vectorSize);
		for (size_t i = 0; i < vectorSize; ++i)
		{
			_container.emplace_back(std::make_unique<StreamHeap<T>>());
		}
	}

	/**
	* Destructor
	*/
	~StreamHeapVector()
	{
	}

	/**
	* Disable copy constructor
	* @param x	---
	*/
	StreamHeapVector(StreamHeapVector const &x) = delete;

	/**
	* Disable copy assignment
	* @param x	---
	*/
	void operator=(StreamHeapVector const &x) = delete;

	/**
	* Returns Heap top for the index
	* @param index	Index of the container
	* @return T		Heap top Value
	*/
	T getTop(int index) const 
	{
		if (index >= 0 && index < _container.size())
		{
			return _container[index]->getTop();
		}
		return 0;
	}

	/**
	* Insert element in one Heap
	* @param value		New value
	* @param index		Heap index
	*/
	void insertElement(T value, int index)
	{
		if (index >= 0 && index < _container.size())
		{
			_container[index]->insertElement(value);
		}
	}

	/**
	* Clear All heap values
	*/
	void clear()
	{
		for (int index = 0; index < _container.size(); ++index)
		{
			_container[index]->clear();
		}
	}

private:
	/**
	* Stream Heap container
	*/
	std::vector < std::unique_ptr< StreamHeap<T>> > _container;
};


#endif