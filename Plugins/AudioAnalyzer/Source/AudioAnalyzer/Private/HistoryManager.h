// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#ifndef HistoryManager_H
#define HistoryManager_H

#include <mutex>
#include <deque>
#include "CoreMinimal.h"
#include <vector>

/**
* @brief History data manager
*
* Basic buffer history manager implementation using a deque
* @tparam Type of the buffer elements
*/
template<class T>
class HistoryManager
{

public:
	/**
	* Buffer element type
	*/
	class HistoryElement
	{
	public:
		/**
		* Buffer element data type
		*/
		typedef TArray<T> DataContainer;

		/**
		* Constructor
		* @param rawData	Buffer element data (copy)
		* @param timestamp	Buffer element timestamp
		*/
		HistoryElement(const DataContainer& rawData, float timestamp): _rawData(rawData), _timestamp(timestamp) 
		{
		};

		/**
		* Destructor
		*/
		~HistoryElement()
		{
		};

		/**
		* Disable copy constructor
		* @param x	---
		*/
		HistoryElement(HistoryElement const &x) = delete;

		/**
		* Disable copy assignment
		* @param x	---
		*/
		void operator=(HistoryElement const &x) = delete;

		/**
		* Gets the reference to the buffer container
		* @return Buffer container
		*/
		const DataContainer& get_rawData() const { return _rawData; };

		/**
		* Set the value for an specific buffer position
		* @param index		Element Position
		* @param value		Element value
		*/
		void set_rawDataValue(const int index, const T& value) { _rawData[index] = value; };

		/**
		* Get the timestamp for this history element
		* @return Timestamp of the container
		*/
		const float& get_timestamp() const { return _timestamp; };

		/**
		* Set the timestamp for this history element 
		* @param timestamp		New timestamp of the container
		*/
		void set_timestamp(const float timestamp) { _timestamp = timestamp; };

	private:
		/**
		* Buffer element data 
		*/
		DataContainer	_rawData;
		/**
		* Buffer element timestamp
		*/
		float			_timestamp;
	};

	/** 
	* Buffer type
	*/
	typedef std::deque<HistoryElement> HistoryContainer;

	/**
	* Constructor
	* @param maxCapacity	Max total size for the buffer
	* @param elementSize	Max total size size for the element buffer
	*/
	HistoryManager(int maxCapacity, int elementSize) :_maxCapacity(maxCapacity), _elementSize(elementSize)
	{
	};

	/**
	* Destructor
	*/
	~HistoryManager()
	{
	};

	/**
	* Disable copy constructor
	* @param x	---
	*/
	HistoryManager(HistoryManager const &x) = delete;

	/**
	* Disable copy assignment
	* @param x	---
	*/
	void operator=(HistoryManager const &x) = delete;

	/**
	* Gets the max buffer capacity
	* @return Max buffer capacity
	*/
	const int getMaxCapacity() const { return _maxCapacity; }

	/**
	* Gets the max buffer element capacity
	* @return Max buffer element capacity
	*/
	const int getElementSize() const { return _elementSize; }

	/**
	* Gets the reference to the buffer container
	* @return Buffer container
	*/
	const HistoryContainer& getHistoryContainer() const { return _historyContainer;}

	/**
	* Gets the reference to the buffer container
	* @return Buffer container
	*/
	HistoryContainer& getHistoryContainerNC() { return _historyContainer; }

	/**
	* Sets the max buffer capacity
	* @param maxCapacity	Max buffer capacity
	*/
	void setMaxCapacity(int maxCapacity) { _maxCapacity = maxCapacity; };
	
	/**
	* Inserts a new element (copy) into the buffer
	* @param data			New Data container reference
	* @param timestamp		timestamp for new data
	*/
	void pushElement(const typename HistoryElement::DataContainer& data, const float timestamp)
	{
		if (_maxCapacity > 0)
		{
			{
				std::lock_guard<std::mutex> lock(_containerMutex);
				int exceededElements = _historyContainer.size() - _maxCapacity + 1;
				for (int i = 0; i < exceededElements; ++i)
				{
					_historyContainer.pop_front();
				}

				_historyContainer.emplace_back(data, timestamp);
			}
		}
	};

	/**
	* Inserts a new element (copy) into the buffer (BPM optimization)
	* @param beatDistanceHistory	Beat Distance History Container
	* @param beatDistanceAverage	Beat Distance Average Container pointer
	* @param numBeatAverage			Num of beats of the history Container pointer
	* @param data					Current Beat data Container
	* @param timestamp				Current beat timestamp
	*/
	void pushBeatElement(HistoryManager<float>& beatDistanceHistory, float* beatDistanceAverage, int* numBeatAverage, const TArray<bool>& data, const float timestamp)
	{
		if (_maxCapacity > 0)
		{
			{
				std::lock_guard<std::mutex> lock(_containerMutex);

				int exceededElements = _historyContainer.size() - _maxCapacity + 1;

				HistoryManager<float>::HistoryContainer& historyDistanceContainer = beatDistanceHistory.getHistoryContainerNC();
				if(exceededElements > 0)
				{
					const HistoryElement& exceededElement = _historyContainer.front();
					const HistoryManager<float>::HistoryElement& exceededDistanceElement = historyDistanceContainer.front();

					//adjust average, remove element
					for (int index = 0; index < exceededElement.get_rawData().Num(); ++index)
					{
						if (exceededElement.get_rawData()[index] == 3) // first beat onset
						{
							if (numBeatAverage[index] > 0)
							{
								numBeatAverage[index]--;
								beatDistanceAverage[index] = numBeatAverage[index] > 0 ? (beatDistanceAverage[index] - ((exceededDistanceElement.get_rawData()[index] - beatDistanceAverage[index]) / (numBeatAverage[index]))) : 0;
							}
						}
					}

					//remove element from history container
					_historyContainer.pop_front();
					historyDistanceContainer.pop_front();
				}

				typename HistoryElement::DataContainer fixedContainer;
				fixedContainer.Init(0, _elementSize);
				HistoryManager<float>::HistoryElement::DataContainer distanceContainer;
				distanceContainer.Init(0.f, _elementSize);
				if (_historyContainer.size() > 0)
				{
					const HistoryElement& lastElement = _historyContainer.back();
					for (int index = 0; index < lastElement.get_rawData().Num(); ++index)
					{
						if (data[index])
						{
							
							////TODO: Allow small gaps
							//int maxGap = 3;
							//int prevValue = 0;

							//HistoryElement* currentElement = &_historyContainer.back();
							//if (_historyContainer.size() > 1 && maxGap > 0)
							//{
							//	typename HistoryContainer::reverse_iterator history_it = _historyContainer.rbegin();
							//	++history_it;
							//	while ((history_it != _historyContainer.rend()) && (maxGap > 0) && (currentElement->get_rawData()[index] == 0))
							//	{
							//		HistoryElement* prevElement = &(*history_it);
							//		if (prevElement->get_rawData()[index] > 0)
							//		{
							//			currentElement->set_rawDataValue(index, 1);
							//			--maxGap;
							//			currentElement = prevElement;
							//		}
							//		else
							//		{
							//			break;
							//		}

							//		++history_it;
							//	}
							//}


							if (lastElement.get_rawData()[index] > 0)
							{
								fixedContainer[index] = 1;
							}
							else
							{
								//mark first beat onset
								fixedContainer[index] = 3;

								//search previous beat and calculate distance
								typename HistoryManager<float>::HistoryContainer::reverse_iterator historyDistance_it = historyDistanceContainer.rbegin();
								for (typename HistoryContainer::const_reverse_iterator history_it = _historyContainer.crbegin();
									history_it != (_historyContainer.crend());
									++history_it, ++historyDistance_it)
								{
									const HistoryElement& historyElement = *history_it;
									HistoryManager<float>::HistoryElement& historyDistanceElement = *historyDistance_it;
									if (historyElement.get_rawData()[index] > 1)
									{
										float newDistance = (timestamp - historyElement.get_timestamp());
										historyDistanceElement.set_rawDataValue(index, newDistance);
										//update average
										numBeatAverage[index]++;
										beatDistanceAverage[index] = numBeatAverage[index] > 0 ? (beatDistanceAverage[index] + ((newDistance - beatDistanceAverage[index]) / (numBeatAverage[index]))) : 0;
										break;
									}
								}
							}
						}
						else
						{
							fixedContainer[index] = 0;
						}
					}
				}


				_historyContainer.emplace_back(fixedContainer, timestamp);
				historyDistanceContainer.emplace_back(distanceContainer, timestamp);

				////TODO: DEBUG 
				//debugVector->Init(0, _historyContainer.size());
				//debugVector3->Init(0.0, _historyContainer.size());
				//int debindex = 0;
				//typename HistoryContainer::const_iterator history_it = _historyContainer.cbegin();
				//typename HistoryManager<float>::HistoryContainer::const_iterator historyDistance_it = historyDistanceContainer.cbegin();
				//for (;
				//	history_it != (_historyContainer.cend());
				//	++history_it, ++historyDistance_it)
				//{
				//	const HistoryElement* currentHistoryElement = &*history_it;
				//	const HistoryManager<float>::HistoryElement* historyDistanceElement = &*historyDistance_it;

				//	(*debugVector)[debindex] = static_cast<int>(currentHistoryElement->get_rawData()[0]);
				//	(*debugVector3)[debindex] = static_cast<float>(currentHistoryElement->get_timestamp());
				//	debindex++;
				//}
			}
		}
	};

	/**
	* Check and return the last element of the buffer
	* @param[out] data		Element data output
	* @param timestamp		Element timestamp
	* @return bool			Element already in the last buffer position
	*/
	bool getLastElement(typename HistoryElement::DataContainer& data, const float timestamp)
	{
		if (_maxCapacity > 0)
		{
			{
				std::lock_guard<std::mutex> lock(_containerMutex);

				if (_historyContainer.size() > 0)
				{
					HistoryElement& historyElement = _historyContainer.back();
					if (historyElement.get_timestamp() == timestamp)
					{
						//copy
						data = historyElement.get_rawData();
						return true;
					}
				}
			}
		}
		return false;
	};

	/**
	* Clears the history buffer
	*/
	void clearHistory()
	{
		_historyContainer.clear();
	};

	/**
	* Calculates the average of the current buffer elements
	* @param[out] averageSpectrum		Output average results
	*/
	void getAverageHistory(T* averageSpectrum)
	{
		int numElements = 0;

		{
			std::lock_guard<std::mutex> lock(_containerMutex);
			numElements = _historyContainer.size();
			for (typename HistoryContainer::const_iterator history_it = _historyContainer.cbegin();
				history_it != (_historyContainer.cend());
				++history_it)
			{
				const HistoryElement& historyElement = *history_it;
				const typename HistoryElement::DataContainer& datacontainer = historyElement.get_rawData();
				for (int index = 0; index < datacontainer.Num(); ++index)
				{
					averageSpectrum[index] += datacontainer[index];
				}
			}
		}

		if (numElements > 0)
		{
			for (int index = 0; index < _elementSize; ++index)
			{
				averageSpectrum[index] /= numElements;
			}
		}
	}

	/**
	* Calculates the variance of the current buffer elements with their average results
	* @param[out] varianceSpectrum		Output variance results
	* @param[in] averageSpectrum		Buffer average results
	*/
	void getVarianceHistory(T* varianceSpectrum, const T* averageSpectrum)
	{
		int numElements = 0;

		{
			std::lock_guard<std::mutex> lock(_containerMutex);
			numElements = _historyContainer.size();
			for (typename HistoryContainer::const_iterator history_it = _historyContainer.cbegin();
				history_it != _historyContainer.cend();
				++history_it)
			{
				const HistoryElement& historyElement = *history_it;
				const typename HistoryElement::DataContainer& datacontainer = historyElement.get_rawData();
				for (int index = 0; index < datacontainer.Num(); ++index)
				{
					varianceSpectrum[index] += (datacontainer[index] - averageSpectrum[index]) * (datacontainer[index] - averageSpectrum[index]);
				}
			}
		}

		if (numElements > 0)
		{
			for (int index = 0; index < _elementSize; ++index)
			{
				varianceSpectrum[index] /= numElements;
			}
		}
	}

	/**
	* Returns a Tarray with the history data and timestamps
	* @param[out] timestampTArray	History data timestamps
	* @param[out] dataTArray		History data
	* @param[in] dataIndex			History index
	*/
	void getAsTArray(TArray<T>* timestampTArray, TArray<T>* dataTArray, int dataIndex)
	{
		timestampTArray->Init(0.0, _maxCapacity);
		dataTArray->Init(0.0, _maxCapacity);
		{
			std::lock_guard<std::mutex> lock(_containerMutex);
			int index = 0;
			for (typename HistoryContainer::const_iterator history_it = _historyContainer.cbegin();
				history_it != (_historyContainer.cend());
				++history_it)
			{
				const HistoryElement& historyElement = *history_it;
				const typename HistoryElement::DataContainer& datacontainer = historyElement.get_rawData();
				(*timestampTArray)[index] = historyElement.get_timestamp();
				(*dataTArray)[index] = datacontainer[dataIndex];
				++index;
			}
		}
	}

private:
	
	/**
	* Buffer container
	*/
	HistoryContainer _historyContainer;

	/**
	* Max buffer size
	*/
	int _maxCapacity;

	/**
	* Max buffer element size
	*/
	int _elementSize;

	/**
	* History Container mutex
	*/
	std::mutex	_containerMutex;
};

#endif