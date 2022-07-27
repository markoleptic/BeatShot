// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#include "AudioAnalyzerCore.h"

#include "AudioAnalyzerSource.h"

#include "kiss_fft.h"
#include "kiss_fftnd.h"
#include "kiss_fftr.h"

#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>

#include <algorithm> 

#include "StreamHeap.h"

AudioAnalyzerCore::AudioAnalyzerCore() :_hasValidAudioInfo(false),
										_hasValidAConfig(false),
										_hasValidSConfig(false),
										_hasValidBTConfig(false),
										_hasValidPTConfig(false),
										_AACurrentSource(NULL)
{
}

AudioAnalyzerCore::~AudioAnalyzerCore()
{
	
}

void AudioAnalyzerCore::setAudioSource(AudioAnalyzerSource* audioSourcePtr)
{
	_hasValidAudioInfo = false;
	_AACurrentSource = audioSourcePtr;

	if (_AACurrentSource && _AACurrentSource->getAudioInfo())
	{
		_hasValidAudioInfo = true;
	}

	_hasValidAConfig = false;
	_hasValidSConfig = false;
	_hasValidBTConfig = false;
	_hasValidPTConfig = false;
}

template<typename ArrayType>
void AudioAnalyzerCore::zeroedChannelTArray(TArray<TArray<ArrayType> >& arrayRef, ArrayType value)
{
	for (typename TArray<TArray<ArrayType> >::TIterator Iter(arrayRef); Iter; ++Iter)
	{
		TArray<ArrayType>& channelArray = *Iter; //TODO check copy
		for (typename TArray<ArrayType>::TIterator IterSub(channelArray); IterSub; ++IterSub)
		{
			*IterSub = value;
		}
	}
}

template<typename ArrayType, typename ArrayWeightType>
void AudioAnalyzerCore::applyWeightChannelTArray(TArray<TArray<ArrayType> >& arrayRef, TArray<ArrayWeightType>& arrayWeightsRef, ArrayWeightType zeroWeight)
{
	for (typename TArray<TArray<ArrayType> >::TIterator Iter(arrayRef); Iter; ++Iter)
	{
		TArray<ArrayType>& channelArray = *Iter;
		typename TArray<ArrayWeightType>::TIterator IterWeightSub(arrayWeightsRef);
		for (typename TArray<ArrayType>::TIterator IterSub(channelArray); IterSub; ++IterSub, ++IterWeightSub)
		{
			ArrayWeightType weightFixed = (*IterWeightSub) / arrayRef.Num();
			*IterSub = (weightFixed > zeroWeight) ? (*IterSub / weightFixed) : zeroWeight;
		}
	}
}

float fourBytesToFloat(void* dataPtr, int32 dataOffset)
{
	uint8* resultPtr = reinterpret_cast<uint8*>(dataPtr);
	int32 result;
	result = (resultPtr[dataOffset + 3] << 24) |
		(resultPtr[dataOffset + 2] << 16) |
		(resultPtr[dataOffset + 1] << 8) |
		(resultPtr[dataOffset]);

	return result / 2147483648.0f;
}

float twoBytesToFloat(void* dataPtr, int32 dataOffset)
{
	uint8* resultPtr = reinterpret_cast<uint8*>(dataPtr);
	int16 result;
	result = (resultPtr[dataOffset + 1] << 8) |
		(resultPtr[dataOffset]);
	return result / 32768.0f;
}

float readFloat(void* dataPtr, int32 dataOffset)
{
	float* resultPtr = reinterpret_cast<float*>(reinterpret_cast<uint8*>(dataPtr) + dataOffset);
	return *resultPtr;
}



/***************************************************/
/*                    AMPLITUDE                    */
/***************************************************/

bool AudioAnalyzerCore::isAConfigSplitChannel() const
{
	return (_aConfig.channelMode == ChannelSelectMode::Split_All);
}

void AudioAnalyzerCore::getAmplitude(FChannelTArray& amplitude)
{
	if (_AACurrentSource)
	{
		float currentTime;
		_AACurrentSource->getPlaybackProgress(currentTime);
		getAmplitude(amplitude, _aConfig.channelMode, _aConfig.channel, _aConfig.numTimeBands, _aConfig.timeWindow, currentTime);
	}
}

void AudioAnalyzerCore::initializeAConfig(FChannelTArray& outputValues, int channelMode, int channel, int numTimeBands, float timeWindow)
{
	_aConfig.channelMode = static_cast<ChannelSelectMode>(channelMode);
	_aConfig.channel = channel;
	_aConfig.numTimeBands = numTimeBands;
	_aConfig.timeWindow = timeWindow;

	outputValues.Empty();

	if (_AACurrentSource)
	{
		int maxChannels = 1;

		if (isAConfigSplitChannel())
		{
			const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfo = _AACurrentSource->getAudioInfo();
			if (audioInfo)
			{
				maxChannels = audioInfo->numChannels;
			}
		}

		for (int numChannel = 0; numChannel < maxChannels; ++numChannel)
		{
			TArray<float> channelValues;
			outputValues.Add(channelValues);
			outputValues[numChannel].Init(0.0, numTimeBands);
		}
		_hasValidAConfig = true;

	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Invalid Audio Info, Are you missing the InitPlayerAudio/InitCapturerAudio nodes?"));
	}
}

void AudioAnalyzerCore::getAmplitude(FChannelTArray& samplesByTimeBand, ChannelSelectMode channelMode, int channel, int numTimeBands, float timeWindow, float currentTime)
{
	zeroedChannelTArray<float>(samplesByTimeBand, 0.0);

	if (numTimeBands > 0 && timeWindow > 0)
	{
		TArray<int32> numSamplesByTimeband;
		numSamplesByTimeband.Init(0, numTimeBands);
		if (_AACurrentSource)
		{
			const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfo = _AACurrentSource->getAudioInfo();

			if (audioInfo)
			{
				uint32 timeBandSize = (audioInfo->sampleRate * timeWindow) / numTimeBands;

				if (timeBandSize > 0)
				{
					uint16 sampleIncrement = 1;
					int numSelChannel = 0;

					if (channelMode == ChannelSelectMode::Select_one)
					{
						sampleIncrement = audioInfo->numChannels;
						numSelChannel = channel;
					}

					int32 numSample = audioInfo->sampleRate * currentTime;
					int32 firstSample = (numSample * audioInfo->numChannels) + numSelChannel;
					int32 lastSample = 0;//(audioInfo->sampleRate * (currentTime + timeWindow) * audioInfo->numChannels) + numSelChannel;
					{
						std::lock_guard<std::mutex> lock(_AACurrentSource->getSourceMutex());
						if (audioInfo->audioFormat == PCAudioAnalyzer::AUDIOFILE_INFO_FORMAT::AUDIOFORMAT_INT) 	// PCM_CODE        0x0001
						{
							switch (audioInfo->bitDepth)
							{
							case 16: waveDataExtractor<int16>(samplesByTimeBand, numSamplesByTimeband, firstSample, lastSample, audioInfo, timeWindow, sampleIncrement, timeBandSize, numTimeBands, twoBytesToFloat); break;
							case 32: waveDataExtractor<int32>(samplesByTimeBand, numSamplesByTimeband, firstSample, lastSample, audioInfo, timeWindow, sampleIncrement, timeBandSize, numTimeBands, fourBytesToFloat); break;
							}
						}
						else if (audioInfo->audioFormat == PCAudioAnalyzer::AUDIOFILE_INFO_FORMAT::AUDIOFORMAT_FLOAT) 	// IEEE_FLOAT_CODE 0x0003
						{
							switch (audioInfo->bitDepth)
							{
							case 32: waveDataExtractor<float>(samplesByTimeBand, numSamplesByTimeband, firstSample, lastSample, audioInfo, timeWindow, sampleIncrement, timeBandSize, numTimeBands, readFloat); break;
							}
						}
					}

					applyWeightChannelTArray<float, int32>(samplesByTimeBand, numSamplesByTimeband, 0);
				}
			}
			else
			{
				UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Invalid Sound Info"));
			}
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("NumTimeBands and TimeWindow must be greater than zero"));
	}
}

template<typename WavType>
void AudioAnalyzerCore::waveDataExtractor(FChannelTArray& samplesByTimeBand, TArray<int32>& numSamplesByTimeband, int32& firstSample, int32& lastSample, const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfo, const float timeWindow, const uint16& sampleIncrement, const uint32& timeBandSize, const int numTimeBands, std::function<float(void*, int32)> readData)
{
	int32 maxSampleOffest = audioInfo->dataChunkSize / (audioInfo->bitDepth / 8);

	int32 numWindowSamples = audioInfo->sampleRate * audioInfo->numChannels * timeWindow;
	if (numWindowSamples > 0)
	{
		// Center the wave
		firstSample -= (numWindowSamples / 2);
		lastSample = firstSample + numWindowSamples;
	}

	int32 currentChannelIndex = 0;
	int32 numChannels = samplesByTimeBand.Num();
	TArray<float>* currentChannelValues = &(samplesByTimeBand[currentChannelIndex]);

	for (int32 sampleOffset = firstSample;
		sampleOffset < lastSample;
		sampleOffset += sampleIncrement)
	{
		int32 numTimeBand = (sampleOffset - firstSample) / (timeBandSize * audioInfo->numChannels);
		if (numTimeBand < numTimeBands)
		{
			if (sampleOffset >= 0 && sampleOffset < maxSampleOffest)
			{
				(*currentChannelValues)[numTimeBand] += FMath::Abs(readData(audioInfo->dataChunkRawData, sampleOffset * sizeof(WavType)));
				numSamplesByTimeband[numTimeBand] += 1;
			}
		}

		if (numChannels > 1)
		{
			currentChannelIndex = ((currentChannelIndex + 1) % numChannels);
			currentChannelValues = &(samplesByTimeBand[currentChannelIndex]);
		}
	}
}

/***************************************************/
/*               FREQUENCY SPECTRUM                */
/***************************************************/

bool AudioAnalyzerCore::isSConfigSplitChannel() const
{
	return (_sConfig.channelMode == ChannelSelectMode::Split_All);
}

void splitBands(std::vector<int>& input, int& remainingDivs)
{
	if (remainingDivs > 0)
	{
		bool canBeSplitted = false;
		for (int index = 0; index < input.size() && !canBeSplitted; ++index)
		{
			canBeSplitted = (input[index] > 1);
		}

		if (canBeSplitted)
		{
			std::vector<int> output;
			for (int index = 0; index < input.size(); ++index)
			{
				if (remainingDivs > 0)
				{
					if (input[index] > 1)
					{
						output.push_back(input[index] / 2);
						output.push_back(input[index] / 2);
						--remainingDivs;
					}
					else
					{
						output.push_back(1);
					}
				}
				else
				{
					output.push_back(input[index]);
				}
			}

			splitBands(output, remainingDivs);
			input = output;
		}
	}
}

bool AudioAnalyzerCore::initializeSPreConfig(ChannelSelectMode channelMode, int channel, int numFreqBands, float timeWindow, int historySize, bool usePeakValues, int numPeaks)
{
	_sConfig.channelMode = channelMode;
	_sConfig.channel = channel;
	_sConfig.numFreqBands = numFreqBands;
	_sConfig.timeWindow = timeWindow;
	_sConfig.sampleWindow = 2;
	_sConfig.historySize = (historySize > 0) ? historySize : 1; // we need at least 1 to store the current result
	_sConfig.usePeakValues = usePeakValues;
	_sConfig.numPeaks = numPeaks;

	if (_AACurrentSource)
	{
		const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfo = _AACurrentSource->getAudioInfo();
		if (audioInfo)
		{
			int32 numSamples = audioInfo->sampleRate * timeWindow;
			if (numSamples > 0)
			{
				// Shift the window enough so that we get a power of 2
				while (numSamples > _sConfig.sampleWindow)
				{
					_sConfig.sampleWindow *= 2;
				}
			}
			return true;
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Invalid Audio Info, Are you missing the InitPlayerAudio/InitCapturerAudio nodes?"));
	}
	return false;
}

bool AudioAnalyzerCore::initializeSPostConfig(FChannelTArray& frequencyValues, FChannelTArray& averageFrequencyValues, FChannelTArray& frequencyPeaksValues, FChannelTArray& frequencyPeaksEnergyValues)
{
	frequencyValues.Empty();
	averageFrequencyValues.Empty();
	frequencyPeaksValues.Empty();
	frequencyPeaksEnergyValues.Empty();
	_samplesByTimeBand.Empty();
	_samplesByPeak.Empty();

	_hmSamplesByFrequencyByChannel.clear();
	_hmFrequencyPeaksByChannel.clear();
	_hmFrequencyPeaksEnergyByChannel.clear();

	int maxChannels = 1;
	if (isSConfigSplitChannel())
	{
		const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfo = _AACurrentSource->getAudioInfo();
		if (audioInfo)
		{
			maxChannels = audioInfo->numChannels;
		}
	}

	for (int numChannel = 0; numChannel < maxChannels; ++numChannel)
	{
		TArray<float> channelValues;
		frequencyValues.Add(channelValues);
		frequencyValues[numChannel].Init(0.0, _sConfig.numFreqBands);

		averageFrequencyValues.Add(channelValues);
		averageFrequencyValues[numChannel].Init(0.0, _sConfig.numFreqBands);

		frequencyPeaksValues.Add(channelValues);
		frequencyPeaksValues[numChannel].Init(0.0, _sConfig.numPeaks);

		frequencyPeaksEnergyValues.Add(channelValues);
		frequencyPeaksEnergyValues[numChannel].Init(0.0, _sConfig.numPeaks);

		_samplesByTimeBand.Add(channelValues);
		_samplesByTimeBand[numChannel].Init(0.0, _sConfig.sampleWindow);

		_samplesByPeak.Add(channelValues);
		_samplesByPeak[numChannel].Init(0.0, _sConfig.sampleWindow);

		//historySize is at least 1
		_hmSamplesByFrequencyByChannel.push_back(std::make_unique<HistoryManager<float>>(_sConfig.historySize, _sConfig.numFreqBands));
		_hmFrequencyPeaksByChannel.push_back(std::make_unique<HistoryManager<float>>(_sConfig.historySize, _sConfig.numPeaks));
		_hmFrequencyPeaksEnergyByChannel.push_back(std::make_unique<HistoryManager<float>>(_sConfig.historySize, _sConfig.numPeaks));
	}
	
	_hasValidSConfig = true;
	return true;
}

void AudioAnalyzerCore::initializeSConfig_Linear(FChannelTArray& outputValues, 
												FChannelTArray& outputAverageValues, 
												FChannelTArray& outputPeaksValues, 
												FChannelTArray& outputPeaksEnergyValues,
												int channelMode, int channel, int numFreqBands, float timeWindow, int historySize, bool usePeakValues, int numPeaks)
{
	_sConfig.spectrumType = SType::S_Linear;
	if (initializeSPreConfig(static_cast<ChannelSelectMode>(channelMode), channel, numFreqBands, timeWindow, historySize, usePeakValues, numPeaks))
	{
		int remainingSamples = _sConfig.sampleWindow / 2;
		_numSamplesByBand.clear();
		_numSamplesByBand.resize(_sConfig.numFreqBands, 0);
		while (remainingSamples > 0)
		{
			for (int i = 0; i < _sConfig.numFreqBands && remainingSamples > 0; ++i)
			{
				++_numSamplesByBand[i];
				--remainingSamples;
			}
		}

		initializeSPostConfig(outputValues, outputAverageValues, outputPeaksValues, outputPeaksEnergyValues);
	}
}

void AudioAnalyzerCore::initializeSConfig_Log(FChannelTArray& outputValues, 
												FChannelTArray& outputAverageValues, 
												FChannelTArray& outputPeaksValues, 
												FChannelTArray& outputPeaksEnergyValues,
												int channelMode, int channel, int numFreqBands, float timeWindow, int historySize, bool usePeakValues, int numPeaks)
{
	_sConfig.spectrumType = SType::S_Log;
	if (initializeSPreConfig(static_cast<ChannelSelectMode>(channelMode), channel, numFreqBands, timeWindow, historySize, usePeakValues, numPeaks))
	{
		//calculates num fft samples per bar
		_numSamplesByBand.clear();
		int currentSize = _sConfig.sampleWindow / 2;
		while (currentSize > 1 && _numSamplesByBand.size() < (_sConfig.numFreqBands - 1))
		{
			currentSize /= 2;
			_numSamplesByBand.push_back(currentSize);
		}

		if (_numSamplesByBand.size() < _sConfig.numFreqBands)
		{
			_numSamplesByBand.push_back(1);
		}
		std::reverse(std::begin(_numSamplesByBand), std::end(_numSamplesByBand));
		int numRemainingBands = _sConfig.numFreqBands - _numSamplesByBand.size();
		splitBands(_numSamplesByBand, numRemainingBands);

		initializeSPostConfig(outputValues, outputAverageValues, outputPeaksValues, outputPeaksEnergyValues);
	}
}

void AudioAnalyzerCore::initializeSConfig_Custom(FChannelTArray& outputValues, 
												FChannelTArray& outputAverageValues,
												FChannelTArray& outputPeaksValues,
												FChannelTArray& outputPeaksEnergyValues,
												int channelMode, int channel, const TArray<FVector2D>& bandLimits, float timeWindow, int historySize, bool usePeakValues, int numPeaks)
{
	_sConfig.spectrumType = SType::S_Custom;
	if (initializeSPreConfig(static_cast<ChannelSelectMode>(channelMode), channel, bandLimits.Num(), timeWindow, historySize, usePeakValues, numPeaks))
	{
		_numSamplesByBand.clear();
		int remainingSamples = _sConfig.sampleWindow / 2;
		if (_AACurrentSource)
		{
			const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfo = _AACurrentSource->getAudioInfo();
			int freqPerSample = audioInfo->sampleRate / _sConfig.sampleWindow;

			if (freqPerSample > 0)
			{

				uint32 limitFreqLow = 0;
				uint32 limitFreqHigh = 0;
				for (int bandIndex = 0; bandIndex < bandLimits.Num(); ++bandIndex)
				{
					for (int limitIndex = 0; limitIndex < 2; ++limitIndex)
					{
						limitFreqHigh = (limitIndex == 0) ? bandLimits[bandIndex].X : bandLimits[bandIndex].Y;

						if (limitFreqHigh < limitFreqLow)
						{
							UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Invalid band limits"));
							limitFreqHigh = limitFreqLow;
						}

						if (limitFreqHigh < (audioInfo->sampleRate / 2))
						{
							int bandSamples = (limitFreqHigh - limitFreqLow) / freqPerSample;
							if (bandSamples < remainingSamples)
							{
								remainingSamples -= bandSamples;
								_numSamplesByBand.push_back(bandSamples);
							}
							else
							{
								_numSamplesByBand.push_back(remainingSamples);
								remainingSamples = 0;
							}
						}
						else
						{
							if (remainingSamples)
							{
								_numSamplesByBand.push_back(remainingSamples);
								remainingSamples = 0;
							}
							else
							{
								_numSamplesByBand.push_back(0);
							}
						}
						limitFreqLow = limitFreqHigh + 1;
					}
				}
			}
			else
			{
				UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Time Window too high for this samplerate"));
			}
		}

		initializeSPostConfig(outputValues, outputAverageValues, outputPeaksValues, outputPeaksEnergyValues);
	}
}

void AudioAnalyzerCore::getSpectrum(FChannelTArray& frequencySpectrum)
{
	if (_AACurrentSource)
	{
		float currentTime;
		float timestamp = _AACurrentSource->getPlaybackProgress(currentTime);
		getSpectrum(frequencySpectrum, currentTime, timestamp);
	}
}

void AudioAnalyzerCore::getSpectrum(FChannelTArray& frequencySpectrum, float currentTime, float timestamp)
{
	getSpectrum(frequencySpectrum, _sConfig.channelMode, _sConfig.channel, _sConfig.numFreqBands, _sConfig.timeWindow, _sConfig.sampleWindow, _sConfig.spectrumType == SType::S_Custom, _sConfig.usePeakValues, currentTime, timestamp);
}


void AudioAnalyzerCore::getSpectrum_Average(FChannelTArray& averageSpectrum)
{
	zeroedChannelTArray<float>(averageSpectrum, 0.0);
	for (int numChannel = 0; numChannel < averageSpectrum.Num(); ++numChannel)
	{
		_hmSamplesByFrequencyByChannel[numChannel]->getAverageHistory(averageSpectrum[numChannel].GetData());
	}
}

float kiss_fft_cpx_magnitude(const kiss_fft_cpx& complexValue)
{
	return sqrtf(complexValue.r * complexValue.r + complexValue.i * complexValue.i);
}

template<typename WavType>
void AudioAnalyzerCore::waveDataExtractorSpectrum(FChannelTArray& samplesByTimeBand, TArray<int32>& numSamplesByTimeband, int32& firstSample, int32& lastSample, const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfo, const float timeWindow, const uint16& sampleIncrement, const int sampleWindow, std::function<float(void*, int32)> readData)
{
	int32 maxSampleOffest = audioInfo->dataChunkSize / (audioInfo->bitDepth / 8);

	int32 maxNumWindowSamples = audioInfo->sampleRate * timeWindow * audioInfo->numChannels;
	int32 analysisSamples = sampleWindow * audioInfo->numChannels;

	int32 realWindow = (maxNumWindowSamples < analysisSamples) ? maxNumWindowSamples : analysisSamples;
	if (maxNumWindowSamples > 0)
	{
		firstSample -= (realWindow / 2);
		lastSample = firstSample + (realWindow);
	}

	int32 currentChannelIndex = 0;
	int32 numChannels = samplesByTimeBand.Num();
	TArray<float>* currentChannelValues = &(samplesByTimeBand[currentChannelIndex]);

	for (int32 sampleOffset = firstSample;
		sampleOffset < lastSample;
		sampleOffset += sampleIncrement)
	{
		int32 sampleIndex = (sampleOffset - firstSample) / audioInfo->numChannels;
		if (sampleOffset >= 0 && sampleOffset < maxSampleOffest)
		{
			(*currentChannelValues)[sampleIndex] += readData(audioInfo->dataChunkRawData, sampleOffset * sizeof(WavType));
			numSamplesByTimeband[sampleIndex] += 1;
		}

		if (numChannels > 1)
		{
			currentChannelIndex = ((currentChannelIndex + 1) % numChannels);
			currentChannelValues = &(samplesByTimeBand[currentChannelIndex]);
		}
	}
}

void AudioAnalyzerCore::getWaveDataWindowed(FChannelTArray& samplesByTimeBand, TArray<int32>& numSamplesByTimeband, ChannelSelectMode channelMode, int channel, float timeWindow, int sampleWindow, float currentTime)
{
	zeroedChannelTArray<float>(samplesByTimeBand, 0.0);
	numSamplesByTimeband.Init(0, sampleWindow);

	if (_AACurrentSource)
	{
		const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfo = _AACurrentSource->getAudioInfo();

		if (audioInfo)
		{
			uint16 sampleIncrement = 1;
			int numSelChannel = 0;

			if (channelMode == ChannelSelectMode::Select_one)
			{
				sampleIncrement = audioInfo->numChannels;
				numSelChannel = channel;
			}

			int32 numSample = audioInfo->sampleRate * currentTime;
			int32 firstSample = (numSample * audioInfo->numChannels) + numSelChannel;
			int32 lastSample = 0;
			
			{
				std::lock_guard<std::mutex> lock(_AACurrentSource->getSourceMutex());

				if (audioInfo->audioFormat == PCAudioAnalyzer::AUDIOFILE_INFO_FORMAT::AUDIOFORMAT_INT) 	// PCM_CODE        0x0001
				{
					switch (audioInfo->bitDepth)
					{
					case 16: waveDataExtractorSpectrum<int16>(samplesByTimeBand, numSamplesByTimeband, firstSample, lastSample, audioInfo, timeWindow, sampleIncrement, sampleWindow, twoBytesToFloat); break;
					case 32: waveDataExtractorSpectrum<int32>(samplesByTimeBand, numSamplesByTimeband, firstSample, lastSample, audioInfo, timeWindow, sampleIncrement, sampleWindow, fourBytesToFloat); break;
					}
				}
				else if (audioInfo->audioFormat == PCAudioAnalyzer::AUDIOFILE_INFO_FORMAT::AUDIOFORMAT_FLOAT) 	// IEEE_FLOAT_CODE 0x0003
				{
					switch (audioInfo->bitDepth)
					{
					case 32: waveDataExtractorSpectrum<float>(samplesByTimeBand, numSamplesByTimeband, firstSample, lastSample, audioInfo, timeWindow, sampleIncrement, sampleWindow, readFloat); break;
					}
				}
			}
		}

		applyWeightChannelTArray<float, int32>(samplesByTimeBand, numSamplesByTimeband, 0);
	}
}

bool AudioAnalyzerCore::getFFT(std::vector<kiss_fft_cpx>& fftOutput, const TArray<float>* samplesByTimeband)
{
	kiss_fftr_cfg fftCfg;
	if ((fftCfg = kiss_fftr_alloc(samplesByTimeband->Num(), 0, NULL, NULL)) != NULL)
	{
		std::vector<kiss_fft_scalar> fftInput;
		fftInput.reserve(samplesByTimeband->Num());

		for (int sampleIndex = 0; sampleIndex < samplesByTimeband->Num(); ++sampleIndex)
		{
			// Apply the Hann window
			fftInput.push_back(((float)(*samplesByTimeband)[sampleIndex]) * 0.5f * (1 - FMath::Cos(2 * PI * sampleIndex / (samplesByTimeband->Num() - 1))));
		}

		fftOutput.resize((fftInput.size() / 2) + 1);
		kiss_fftr(fftCfg, fftInput.data(), fftOutput.data());

		KISS_FFT_FREE(fftCfg);

		return true;
	}
	
	//TODO: DEBUG 
	//kiss_fftnd_cfg fftCfg;
	//int32 Dims[1] = { samplesByTimeband.Num() };
	//if ((fftCfg = kiss_fftnd_alloc(Dims, 1, 0, NULL, NULL)) != NULL)
	//{
	//	std::vector<kiss_fft_cpx> fftInput;
	//	fftInput.resize(samplesByTimeband.Num());

	//	for (int sampleIndex = 0; sampleIndex < samplesByTimeband.Num(); ++sampleIndex)
	//	{
	//		// Apply the Hann window
	//		fftInput[sampleIndex].r = (((float)samplesByTimeband[sampleIndex]) * 0.5f * (1 - FMath::Cos(2 * PI * sampleIndex / (samplesByTimeband.Num() - 1))));
	//		fftInput[sampleIndex].i = 0.f;
	//	}

	//	fftOutput.resize((fftInput.size() / 2) + 1);
	//	kiss_fftnd(fftCfg, fftInput.data(), fftOutput.data());

	//	KISS_FFT_FREE(fftCfg);

	//	return true;
	//}

	return false;
}

void AudioAnalyzerCore::getSpectrum(FChannelTArray& frequencySpectrum, ChannelSelectMode channelMode, int channel, int numFreqBands, float timeWindow, int sampleWindow, bool hasCustomLimits, bool usePeakValues, float currentTime, float timestamp)
{
	bool duplicated = _hmSamplesByFrequencyByChannel[0]->getLastElement(frequencySpectrum[0], timestamp);

	if (channelMode == ChannelSelectMode::Split_All)
	{
		for (int numChannel = 1; numChannel < frequencySpectrum.Num(); ++numChannel)
		{
			_hmSamplesByFrequencyByChannel[numChannel]->getLastElement(frequencySpectrum[numChannel], timestamp);
		}
	}

	if (!duplicated)
	{
		getSpectrum(frequencySpectrum, _samplesByTimeBand, _numSamplesByBand, channelMode, channel, numFreqBands, timeWindow, sampleWindow, hasCustomLimits, usePeakValues, currentTime);

		_hmSamplesByFrequencyByChannel[0]->pushElement(frequencySpectrum[0], timestamp);
		if (channelMode == ChannelSelectMode::Split_All)
		{
			for (int numChannel = 1; numChannel < frequencySpectrum.Num(); ++numChannel)
			{
				_hmSamplesByFrequencyByChannel[numChannel]->pushElement(frequencySpectrum[numChannel], timestamp);
			}
		}
	}
}

void AudioAnalyzerCore::getSpectrum(FChannelTArray& samplesByFrequency, FChannelTArray& samplesByTimeBand, std::vector<int>& numSamplesByBand, ChannelSelectMode channelMode, int channel, int numFreqBands, float timeWindow, int sampleWindow, bool hasCustomLimits, bool usePeakValues, float currentTime)
{
	zeroedChannelTArray<float>(samplesByFrequency, 0.0);
	if (numFreqBands > 0 && timeWindow > 0)
	{
		TArray<int32> numSamplesByTimeband;
		getWaveDataWindowed(samplesByTimeBand, numSamplesByTimeband, channelMode, channel, timeWindow, sampleWindow, currentTime);
		
		for (int currentChannel = 0; currentChannel < samplesByFrequency.Num(); ++currentChannel)
		{
			const TArray<float>* currentSamplesByTimeband = &samplesByTimeBand[currentChannel];
			TArray<float>* currentSamplesByFrequency = &samplesByFrequency[currentChannel];

			std::vector<kiss_fft_cpx> fftResult;
			if (getFFT(fftResult, currentSamplesByTimeband))
			{
				//TODO: DEBUG 
				//std::vector<float> fftMagnitude;
				//fftMagnitude.resize(fftResult.size());
				//for (int index = 0; index < fftResult.size(); ++index)
				//{
				//	fftMagnitude[index] = (kiss_fft_cpx_magnitude(fftResult[index])) * (4.0f / samplesByTimeband.Num());
				//}

				std::vector<kiss_fft_cpx>::const_iterator fftResult_it = fftResult.cbegin();
				for (int frecBandIndex = 0; frecBandIndex < numSamplesByBand.size(); ++frecBandIndex)
				{
					int numFreqBandSamples = numSamplesByBand[frecBandIndex];
					if (hasCustomLimits && ((frecBandIndex % 2) == 0))
					{
						fftResult_it += numFreqBandSamples;
					}
					else
					{
						if (numFreqBandSamples > 0)
						{
							if (usePeakValues)
							{
								int frecBandIndexCorrection = hasCustomLimits ? frecBandIndex / 2 : frecBandIndex;
								(*currentSamplesByFrequency)[frecBandIndexCorrection] = (kiss_fft_cpx_magnitude(*fftResult_it) * (4.0f / currentSamplesByTimeband->Num()));
								++fftResult_it;
								for (int sampleIndex = 1; sampleIndex < numFreqBandSamples; ++sampleIndex)
								{
									float fftMagnitude = (kiss_fft_cpx_magnitude(*fftResult_it) * (4.0f / currentSamplesByTimeband->Num()));
									if ((*currentSamplesByFrequency)[frecBandIndexCorrection] < fftMagnitude)
									{
										(*currentSamplesByFrequency)[frecBandIndexCorrection] = fftMagnitude;
									}
									++fftResult_it;
								}
							}
							else
							{
								//use average values
								for (int sampleIndex = 0; sampleIndex < numFreqBandSamples; ++sampleIndex)
								{
									(*currentSamplesByFrequency)[hasCustomLimits ? frecBandIndex / 2 : frecBandIndex] += (kiss_fft_cpx_magnitude(*fftResult_it) * (4.0f / currentSamplesByTimeband->Num())); //2/N kissfftr normalization * 2 Hann Window
									++fftResult_it;
								}
								(*currentSamplesByFrequency)[hasCustomLimits ? frecBandIndex / 2 : frecBandIndex] /= numFreqBandSamples;
							}
						}
					}
				}
			}
			else
			{
				UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("FFT Error"));
			}
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("NumTimeBands and TimeWindow must be greater than zero"));
	}
}

void AudioAnalyzerCore::getSpectrumPeaks(FChannelTArray& frequencyPeaks, FChannelTArray& frequencyPeaksEnergy)
{
	if (_AACurrentSource)
	{
		float currentTime;
		float timestamp = _AACurrentSource->getPlaybackProgress(currentTime);
		getSpectrumPeaks(frequencyPeaks, frequencyPeaksEnergy, _sConfig.channelMode, _sConfig.channel, _sConfig.numPeaks, _sConfig.timeWindow, _sConfig.sampleWindow, currentTime, timestamp);
	}
}

void AudioAnalyzerCore::getSpectrumPeaks(FChannelTArray& frequencyPeaks, FChannelTArray& frequencyPeaksEnergy, ChannelSelectMode channelMode, int channel, int numPeaks, float timeWindow, int sampleWindow, float currentTime, float timestamp)
{
	bool duplicated = _hmFrequencyPeaksByChannel[0]->getLastElement(frequencyPeaks[0], timestamp);

	if (channelMode == ChannelSelectMode::Split_All)
	{
		for (int numChannel = 1; numChannel < frequencyPeaks.Num(); ++numChannel)
		{
			_hmFrequencyPeaksByChannel[numChannel]->getLastElement(frequencyPeaks[numChannel], timestamp);
			_hmFrequencyPeaksEnergyByChannel[numChannel]->getLastElement(frequencyPeaksEnergy[numChannel], timestamp);
		}
	}

	if (!duplicated)
	{
		getSpectrumPeaks(frequencyPeaks, frequencyPeaksEnergy, _samplesByPeak, channelMode, channel, numPeaks, timeWindow, sampleWindow, currentTime);

		_hmFrequencyPeaksByChannel[0]->pushElement(frequencyPeaks[0], timestamp);
		_hmFrequencyPeaksEnergyByChannel[0]->pushElement(frequencyPeaksEnergy[0], timestamp);
		if (channelMode == ChannelSelectMode::Split_All)
		{
			for (int numChannel = 1; numChannel < frequencyPeaks.Num(); ++numChannel)
			{
				_hmFrequencyPeaksByChannel[numChannel]->pushElement(frequencyPeaks[numChannel], timestamp);
				_hmFrequencyPeaksEnergyByChannel[numChannel]->pushElement(frequencyPeaksEnergy[numChannel], timestamp);
			}
		}
	}
}

void AudioAnalyzerCore::getSpectrumPeaks(FChannelTArray& frequencyPeaks, FChannelTArray& frequencyPeaksEnergy, FChannelTArray& samplesByTimeband, ChannelSelectMode channelMode, int channel, int numPeaks, float timeWindow, int sampleWindow, float currentTime)
{
	zeroedChannelTArray<float>(frequencyPeaks, 0.0);
	zeroedChannelTArray<float>(frequencyPeaksEnergy, 0.0);
	if (numPeaks > 0 && timeWindow > 0)
	{
		float sampleHz = 0.0;
		TArray<int32> numSamplesByTimeband;
		getWaveDataWindowed(samplesByTimeband, numSamplesByTimeband, channelMode, channel, timeWindow, sampleWindow, currentTime);

		if (_AACurrentSource)
		{
			const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfo = _AACurrentSource->getAudioInfo();

			if (audioInfo)
			{
				sampleHz = (audioInfo->sampleRate / (float)sampleWindow);
			}
		}

		for (int currentChannel = 0; currentChannel < frequencyPeaks.Num(); ++currentChannel)
		{
			const TArray<float>* currentSamplesByTimeband = &samplesByTimeband[currentChannel];
			TArray<float>* currentFrequencyPeaks = &frequencyPeaks[currentChannel];
			TArray<float>* currentFrequencyPeaksEnergy = &frequencyPeaksEnergy[currentChannel];

			std::vector<kiss_fft_cpx> fftOutput;

			kiss_fftr_cfg fftCfg;
			if ((fftCfg = kiss_fftr_alloc(currentSamplesByTimeband->Num(), 0, NULL, NULL)) != NULL)
			{
				std::vector<kiss_fft_scalar> fftInput;
				fftInput.reserve(currentSamplesByTimeband->Num());

				for (int sampleIndex = 0; sampleIndex < currentSamplesByTimeband->Num(); ++sampleIndex)
				{
					// Apply the Hann window
					fftInput.push_back(((float)(*currentSamplesByTimeband)[sampleIndex]) * 0.5f * (1 - FMath::Cos(2 * PI * sampleIndex / (currentSamplesByTimeband->Num() - 1))));
				}

				fftOutput.resize((fftInput.size() / 2) + 1);
				kiss_fftr(fftCfg, fftInput.data(), fftOutput.data());

				KISS_FFT_FREE(fftCfg);

				std::vector<kiss_fft_cpx>::const_iterator fftResult_it = fftOutput.cbegin();

				//Fill first N values
				float frequencyHz = 0;
				int32 sampleIndex = 0;

				float peakEnergy = 0.0;
				float peakHz = 0.0;

				if (currentFrequencyPeaks->Num() > 0)
				{
					(*currentFrequencyPeaksEnergy)[0] = (kiss_fft_cpx_magnitude(*fftResult_it) * (4.0f / currentSamplesByTimeband->Num()));
					(*currentFrequencyPeaks)[0] = sampleHz;
					++fftResult_it;
					++sampleIndex;
				}

				//search for max and keep n peaks
				for (; fftResult_it != fftOutput.cend(); ++fftResult_it)
				{
					peakEnergy = (kiss_fft_cpx_magnitude(*fftResult_it) * (4.0f / currentSamplesByTimeband->Num()));
					peakHz += sampleHz;

					int32 foundIndex = binarySearch(*currentFrequencyPeaksEnergy, 0, currentFrequencyPeaksEnergy->Num() - 1, peakEnergy, std::greater<float>());

					if (foundIndex < currentFrequencyPeaksEnergy->Num())
					{
						int position = foundIndex;
						for (int movePosition = currentFrequencyPeaks->Num() - 1; movePosition > position; --movePosition)
						{
							(*currentFrequencyPeaksEnergy)[movePosition] = (*currentFrequencyPeaksEnergy)[movePosition - 1];
							(*currentFrequencyPeaks)[movePosition] = (*currentFrequencyPeaks)[movePosition - 1];
						}

						(*currentFrequencyPeaksEnergy)[position] = peakEnergy;
						(*currentFrequencyPeaks)[position] = peakHz;
					}

					++sampleIndex;
				}
			}
			else
			{
				UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("FFT Error"));
			}
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("NumPeaks and TimeWindow must be greater than zero"));
	}
}

/***************************************************/
/*                  BEAT TRACKING                  */
/***************************************************/

bool AudioAnalyzerCore::isBTConfigSplitChannel() const
{
	return (_btConfig.channelMode == ChannelSelectMode::Split_All);
}

bool AudioAnalyzerCore::initializeBTPreConfig(ChannelSelectMode channelMode, int channel, int numFreqBands, float timeWindow, int historySize, float threshold)
{
	_btConfig.channelMode = channelMode;
	_btConfig.channel = channel;
	_btConfig.numFreqBands = numFreqBands;
	_btConfig.timeWindow = timeWindow;
	_btConfig.sampleWindow = 2;
	_btConfig.historySize = (historySize > 0) ? historySize : 1; //we need at least 1 to store the current result
	_btConfig.threshold = threshold;

	if (_AACurrentSource)
	{
		const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfo = _AACurrentSource->getAudioInfo();
		if (audioInfo)
		{
			int32 numSamples = audioInfo->sampleRate * timeWindow;
			if (numSamples > 0)
			{
				// Shift the window enough so that we get a power of 2
				while (numSamples > _btConfig.sampleWindow)
				{
					_btConfig.sampleWindow *= 2;
				}
			}
			return true;
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Invalid Audio Info, Are you missing the InitPlayerAudio/InitCapturerAudio nodes?"));
	}
	return false;
}

bool AudioAnalyzerCore::initializeBTPostConfig(BChannelTArray& beatOnSet, 
												FChannelTArray& frequencyValues, 
												FChannelTArray& averageFrequencyValues, 
												FChannelTArray& varianceFrequencyValues, 
												IChannelTArray& bpmCurrentValues,
												IChannelTArray& bpmTotalValues)
{
	beatOnSet.Empty();
	frequencyValues.Empty();
	averageFrequencyValues.Empty();
	varianceFrequencyValues.Empty();
	bpmCurrentValues.Empty();
	bpmTotalValues.Empty();

	_samplesByTimeBand_BT.Empty();
	_averageSPB_BPM.Empty();
	_numBeatsSPB_BPM.Empty();
	_hmSamplesByFrequencyByChannel_BT.clear();
	_beatHistory_BPM.clear();
	_beatDistanceHistory_BPM.clear();
	_bpmHeaps_BPM.clear();

	int maxChannels = 1;
	if (isBTConfigSplitChannel())
	{
		const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfo = _AACurrentSource->getAudioInfo();
		if (audioInfo)
		{
			maxChannels = audioInfo->numChannels;
		}
	}

	for (int numChannel = 0; numChannel < maxChannels; ++numChannel)
	{
		TArray<float> channelFValues;
		TArray<int32> channelIValues;
		TArray<bool> channelBValues;

		beatOnSet.Add(channelBValues);
		beatOnSet[numChannel].Init(false, _btConfig.numFreqBands);

		frequencyValues.Add(channelFValues);
		frequencyValues[numChannel].Init(0.0, _btConfig.numFreqBands);

		averageFrequencyValues.Add(channelFValues);
		averageFrequencyValues[numChannel].Init(0.0, _btConfig.numFreqBands);

		varianceFrequencyValues.Add(channelFValues);
		varianceFrequencyValues[numChannel].Init(0.0, _btConfig.numFreqBands);

		bpmCurrentValues.Add(channelIValues);
		bpmCurrentValues[numChannel].Init(0, _btConfig.numFreqBands);

		bpmTotalValues.Add(channelIValues);
		bpmTotalValues[numChannel].Init(0, _btConfig.numFreqBands);

		_samplesByTimeBand_BT.Add(channelFValues);
		_samplesByTimeBand_BT[numChannel].Init(0.0, _btConfig.sampleWindow);

		_averageSPB_BPM.Add(channelFValues);
		_averageSPB_BPM[numChannel].Init(0.0, _btConfig.numFreqBands);

		_numBeatsSPB_BPM.Add(channelIValues);
		_numBeatsSPB_BPM[numChannel].Init(0, _btConfig.numFreqBands);

		//historySize is at least 1
		_hmSamplesByFrequencyByChannel_BT.push_back(std::make_unique<HistoryManager<float>>(_btConfig.historySize, _btConfig.numFreqBands));

		if (_btConfig.calculateBPM)
		{
			_beatHistory_BPM.push_back(std::make_unique<HistoryManager<int>>(_btConfig.BPMHistorySize, _btConfig.numFreqBands));
			_beatDistanceHistory_BPM.push_back(std::make_unique<HistoryManager<float>>(_btConfig.BPMHistorySize, _btConfig.numFreqBands));

			_bpmHeaps_BPM.push_back(std::make_unique<StreamHeapVector<int>>(_btConfig.numFreqBands));
		}
	}

	_hasValidBTConfig = true;
	return true;
}

void AudioAnalyzerCore::initializeBTConfig(BChannelTArray& beatOnSet,
											FChannelTArray& frequencyValues,
											FChannelTArray& averageFrequencyValues,
											FChannelTArray& varianceFrequencyValues,
											IChannelTArray& bpmCurrentValues,
											IChannelTArray& bpmTotalValues,
											int channelMode, 
											int channel,
											float timeWindow,
											int historySize, 
											float threshold, 
											bool calculateBPM, 
											int BPMHistorySize)
{
	TArray<FVector2D> bandLimits;
	bandLimits.Init(FVector2D(0,0), 3);

	bandLimits[0] = FVector2D(0, 87);
	bandLimits[1] = FVector2D(500, 700);
	bandLimits[2] = FVector2D(5000, 12000);

	initializeBTConfig_Custom(beatOnSet, frequencyValues, averageFrequencyValues, varianceFrequencyValues, bpmCurrentValues, bpmTotalValues, channelMode, channel, bandLimits, timeWindow, historySize, threshold, calculateBPM, BPMHistorySize);
	_btConfig.beatTrackingType = BTType::BT_Default;
}

void AudioAnalyzerCore::initializeBTConfig_Custom(BChannelTArray& beatOnSet,
													FChannelTArray& frequencyValues,
													FChannelTArray& averageFrequencyValues,
													FChannelTArray& varianceFrequencyValues,
													IChannelTArray& bpmCurrentValues,
													IChannelTArray& bpmTotalValues,
													int channelMode, 
													int channel,
													const TArray<FVector2D>& bandLimits,
													float timeWindow,
													int historySize, 
													float threshold,
													bool calculateBPM, 
													int BPMHistorySize)
{
	_btConfig.beatTrackingType = BTType::BT_Custom;
	_btConfig.calculateBPM = calculateBPM;
	_btConfig.BPMHistorySize = BPMHistorySize;

	if (initializeBTPreConfig(static_cast<ChannelSelectMode>(channelMode), channel, bandLimits.Num(), timeWindow, historySize, threshold))
	{
		_numSamplesByBand_BT.clear();
		int remainingSamples = _btConfig.sampleWindow / 2;
		if (_AACurrentSource)
		{
			const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfo = _AACurrentSource->getAudioInfo();
			int freqPerSample = audioInfo->sampleRate / _btConfig.sampleWindow;

			uint32 limitFreqLow = 0;
			uint32 limitFreqHigh = 0;
			for (int bandIndex = 0; bandIndex < bandLimits.Num(); ++bandIndex)
			{
				for (int limitIndex = 0; limitIndex < 2; ++limitIndex)
				{
					limitFreqHigh = (limitIndex == 0) ? bandLimits[bandIndex].X : bandLimits[bandIndex].Y;

					if (limitFreqHigh < limitFreqLow)
					{
						UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("Invalid band limits"));
						limitFreqHigh = limitFreqLow;
					}

					if (limitFreqHigh < (audioInfo->sampleRate / 2))
					{
						int bandSamples = (limitFreqHigh - limitFreqLow) / freqPerSample;
						if (bandSamples < remainingSamples)
						{
							remainingSamples -= bandSamples;
							_numSamplesByBand_BT.push_back(bandSamples);
						}
						else
						{
							_numSamplesByBand_BT.push_back(remainingSamples);
							remainingSamples = 0;
						}
					}
					else
					{
						if (remainingSamples)
						{
							_numSamplesByBand_BT.push_back(remainingSamples);
							remainingSamples = 0;
						}
						else
						{
							_numSamplesByBand_BT.push_back(0);
						}
					}
					limitFreqLow = limitFreqHigh + 1;
				}
			}
		}

		initializeBTPostConfig(beatOnSet, frequencyValues, averageFrequencyValues, varianceFrequencyValues, bpmCurrentValues, bpmTotalValues);
	}
}

void AudioAnalyzerCore::getBeatTracking(BChannelTArray& beatResult, FChannelTArray& spectrumValues, IChannelTArray& averageBPM, IChannelTArray& totalBPM, const TArray<float>& overrideThreshold)
{
	if (_AACurrentSource)
	{
		float currentTime;
		float timestamp = _AACurrentSource->getPlaybackProgress(currentTime);

		if (_AACurrentSource->needsBufferOffset())
		{
			const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfo = _AACurrentSource->getAudioInfo();
			float offsetTime = _btConfig.timeWindow / 2.0;
			if (audioInfo && (currentTime > offsetTime))
			{
				currentTime -= offsetTime;
			}
		}
		getBeatTracking(beatResult, spectrumValues, averageBPM, totalBPM, overrideThreshold, currentTime, timestamp);
	}
}

void AudioAnalyzerCore::getBeatTracking(BChannelTArray& beatResult, FChannelTArray& spectrumValues, IChannelTArray& averageBPM, IChannelTArray& totalBPM, const TArray<float>& overrideThreshold, float currentTime, float timestamp)
{

	bool duplicated = _hmSamplesByFrequencyByChannel_BT[0]->getLastElement(spectrumValues[0], timestamp);

	if (isBTConfigSplitChannel())
	{
		for (int numChannel = 1; numChannel < spectrumValues.Num(); ++numChannel)
		{
			_hmSamplesByFrequencyByChannel_BT[numChannel]->getLastElement(spectrumValues[numChannel], timestamp);
		}
	}

	if (!duplicated)
	{
		zeroedChannelTArray<bool>(beatResult, false);
		zeroedChannelTArray<int32>(averageBPM, 0);
		zeroedChannelTArray<int32>(totalBPM, 0);
		getSpectrum(spectrumValues, _samplesByTimeBand_BT, _numSamplesByBand_BT, _btConfig.channelMode, _btConfig.channel, _btConfig.numFreqBands, _btConfig.timeWindow, _btConfig.sampleWindow, true, true, currentTime);

		for (int currentChannel = 0; currentChannel < spectrumValues.Num(); ++currentChannel)
		{
			const TArray<float>* currentChannelSpectrum = &spectrumValues[currentChannel];
			HistoryManager<float>* currentChannelHistorySpectrum = _hmSamplesByFrequencyByChannel_BT[currentChannel].get();

			std::vector<float> averageSpectrum(_btConfig.numFreqBands, 0);
			currentChannelHistorySpectrum->getAverageHistory(averageSpectrum.data());

			for (int index = 0; index < currentChannelSpectrum->Num(); ++index)
			{
				float threshold = (index < overrideThreshold.Num()) ? overrideThreshold[index] : _btConfig.threshold;
				beatResult[currentChannel][index] = ((*currentChannelSpectrum)[index] > (threshold * averageSpectrum[index]));
			}

			if (_btConfig.calculateBPM)
			{
				_beatHistory_BPM[currentChannel]->pushBeatElement(*(_beatDistanceHistory_BPM[currentChannel]), _averageSPB_BPM[currentChannel].GetData(), _numBeatsSPB_BPM[currentChannel].GetData(), beatResult[currentChannel], timestamp);
			
				TArray<int32>* currentAverageBPM = &averageBPM[currentChannel];
				TArray<int32>* totalAverageBPM = &totalBPM[currentChannel];

				for (int index = 0; index < currentAverageBPM->Num(); ++index)
				{
					int newBPM = (_averageSPB_BPM[currentChannel][index] > 0.f) ? (60 / _averageSPB_BPM[currentChannel][index]) : 0;
					(*currentAverageBPM)[index] = newBPM;
					_bpmHeaps_BPM[currentChannel]->insertElement(newBPM, index);
					(*totalAverageBPM)[index] = _bpmHeaps_BPM[currentChannel]->getTop(index);
				}
			}
		}


		for (int numChannel = 0; numChannel < spectrumValues.Num(); ++numChannel)
		{
			_hmSamplesByFrequencyByChannel_BT[numChannel]->pushElement(spectrumValues[numChannel], timestamp);
		}
	}
}

void AudioAnalyzerCore::getBeatTracking_Average(FChannelTArray& averageSpectrum)
{
	zeroedChannelTArray<float>(averageSpectrum, 0.0);
	for (int numChannel = 0; numChannel < averageSpectrum.Num(); ++numChannel)
	{
		_hmSamplesByFrequencyByChannel_BT[numChannel]->getAverageHistory(averageSpectrum[numChannel].GetData());
	}
}

void AudioAnalyzerCore::getBeatTracking_AverageAndVariance(FChannelTArray& averageSpectrum, FChannelTArray& varianceSpectrum)
{
	zeroedChannelTArray<float>(averageSpectrum, 0.0);
	zeroedChannelTArray<float>(varianceSpectrum, 0.0);

	for (int numChannel = 0; numChannel < averageSpectrum.Num(); ++numChannel)
	{
		_hmSamplesByFrequencyByChannel_BT[numChannel]->getAverageHistory(averageSpectrum[numChannel].GetData());
		_hmSamplesByFrequencyByChannel_BT[numChannel]->getVarianceHistory(varianceSpectrum[numChannel].GetData(), averageSpectrum[numChannel].GetData());
	}
}

void AudioAnalyzerCore::extractBasicBeatTracking(TArray<bool>& IsBass, TArray<bool>& IsLowM, TArray<bool>& IsHighM, const BChannelTArray& beatResult)
{
	IsBass.Init(false, beatResult.Num());
	IsLowM.Init(false, beatResult.Num());
	IsHighM.Init(false, beatResult.Num());

	for (int numChannel = 0; numChannel < beatResult.Num(); ++numChannel)
	{
		IsBass[numChannel] = beatResult[numChannel][0];
		IsLowM[numChannel] = beatResult[numChannel][1];
		IsHighM[numChannel] = beatResult[numChannel][2];
	}
}

void AudioAnalyzerCore::resetBPMCounters()
{
	_beatHistory_BPM.clear();
	_beatDistanceHistory_BPM.clear();
	_averageSPB_BPM.Empty();
	_numBeatsSPB_BPM.Empty();
	_bpmHeaps_BPM.clear();

	int maxChannels = 1;
	if (isBTConfigSplitChannel())
	{
		const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfo = _AACurrentSource->getAudioInfo();
		if (audioInfo)
		{
			maxChannels = audioInfo->numChannels;
		}
	}

	for (int numChannel = 0; numChannel < maxChannels; ++numChannel)
	{
		TArray<float> channelFValues;
		TArray<int32> channelIValues;
		TArray<bool> channelBValues;
		_averageSPB_BPM.Add(channelFValues);
		_averageSPB_BPM[numChannel].Init(0.0, _btConfig.numFreqBands);

		_numBeatsSPB_BPM.Add(channelIValues);
		_numBeatsSPB_BPM[numChannel].Init(0, _btConfig.numFreqBands);

		if (_btConfig.calculateBPM)
		{
			_beatHistory_BPM.push_back(std::make_unique<HistoryManager<int>>(_btConfig.BPMHistorySize, _btConfig.numFreqBands));
			_beatDistanceHistory_BPM.push_back(std::make_unique<HistoryManager<float>>(_btConfig.BPMHistorySize, _btConfig.numFreqBands));

			_bpmHeaps_BPM.push_back(std::make_unique<StreamHeapVector<int>>(_btConfig.numFreqBands));
		}
	}
}


/***************************************************/
/*               PITCH TRACKING                    */
/***************************************************/

bool AudioAnalyzerCore::isPTConfigSplitChannel() const
{
	return (_ptConfig.channelMode == ChannelSelectMode::Split_All);
}

void AudioAnalyzerCore::initializePTConfig(FChannelTArray& pitchTrackingyValues, int channelMode, int channel, float timeWindow, float threshold)
{
	_ptConfig.channelMode = static_cast<ChannelSelectMode>(channelMode);
	_ptConfig.channel = channel;
	_ptConfig.timeWindow = timeWindow;
	_ptConfig.sampleWindow = 2;
	_ptConfig.pitchThreshold = threshold;
	_ptConfig.pitchTrackingType = PitchType::PT_Yin;

	if (_AACurrentSource)
	{
		const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfo = _AACurrentSource->getAudioInfo();
		int maxChannels = 1;
		if (audioInfo)
		{
			int32 numSamples = audioInfo->sampleRate * timeWindow;
			if (numSamples > 0)
			{
				// Shift the window enough so that we get a power of 2
				while (numSamples > _ptConfig.sampleWindow)
				{
					_ptConfig.sampleWindow *= 2;
				}
			}

			if (isPTConfigSplitChannel())
			{
				maxChannels = audioInfo->numChannels;
			}
		}

		pitchTrackingyValues.Empty();

		for (int numChannel = 0; numChannel < maxChannels; ++numChannel)
		{
			TArray<float> channelValues;
			_samplesByPitch.Empty();

			_samplesByPitch.Add(channelValues);
			_samplesByPitch[numChannel].Init(0.0, _ptConfig.sampleWindow);

			pitchTrackingyValues.Add(channelValues);
			pitchTrackingyValues[numChannel].Init(0.0, 1);

			_hmPitchTrackingByChannel.push_back(std::make_unique<HistoryManager<float>>(1, 1));
		}

		_hasValidPTConfig = true;
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Invalid Audio Info, Are you missing the InitPlayerAudio/InitCapturerAudio nodes?"));
	}
}


void AudioAnalyzerCore::getPitchTracking(FChannelTArray& frequencyF0)
{
	if (_AACurrentSource)
	{
		float currentTime;
		float timestamp = _AACurrentSource->getPlaybackProgress(currentTime);
		getPitchTracking(frequencyF0, _ptConfig.channelMode, _ptConfig.channel, _ptConfig.pitchTrackingType, _ptConfig.pitchThreshold, _ptConfig.timeWindow, _ptConfig.sampleWindow, currentTime, timestamp);
	}
}

void AudioAnalyzerCore::getPitchTracking(FChannelTArray& frequencyF0, ChannelSelectMode channelMode, int channel, PitchType pitchTrackingType, float pitchThreshold, float timeWindow, int sampleWindow, float currentTime, float timestamp)
{
	bool duplicated = _hmPitchTrackingByChannel[0]->getLastElement(frequencyF0[0], timestamp);

	if (channelMode == ChannelSelectMode::Split_All)
	{
		for (int numChannel = 1; numChannel < frequencyF0.Num(); ++numChannel)
		{
			_hmPitchTrackingByChannel[numChannel]->getLastElement(frequencyF0[numChannel], timestamp);
		}
	}

	if (!duplicated)
	{
		getPitchTracking(frequencyF0, _samplesByPitch, channelMode, channel, pitchTrackingType, pitchThreshold, timeWindow, sampleWindow, currentTime);
		_hmPitchTrackingByChannel[0]->pushElement(frequencyF0[0], timestamp);
		if (channelMode == ChannelSelectMode::Split_All)
		{
			for (int numChannel = 1; numChannel < frequencyF0.Num(); ++numChannel)
			{
				_hmPitchTrackingByChannel[numChannel]->pushElement(frequencyF0[numChannel], timestamp);
			}
		}
	}
}

void AudioAnalyzerCore::getPitchTracking(FChannelTArray& frequencyF0, FChannelTArray& samplesByTimeband, ChannelSelectMode channelMode, int channel, PitchType pitchTrackingType, float pitchThreshold, float timeWindow, int sampleWindow, float currentTime)
{
	zeroedChannelTArray<float>(frequencyF0, 0.0);

	if (timeWindow > 0)
	{
		int sampleRate = 0;
		TArray<int32> numSamplesByTimeband;
		getWaveDataWindowed(samplesByTimeband, numSamplesByTimeband, channelMode, channel, timeWindow, sampleWindow, currentTime);

		if (_AACurrentSource)
		{
			const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfo = _AACurrentSource->getAudioInfo();

			if (audioInfo)
			{
				sampleRate = audioInfo->sampleRate;
			}
		}

		//YIN algorithm
		for (int currentChannel = 0; currentChannel < frequencyF0.Num(); ++currentChannel)
		{
			const TArray<float>* currentSamplesByTimeband = &samplesByTimeband[currentChannel];
			TArray<float>* currentFrequencyF0 = &frequencyF0[currentChannel];

			//diff
			std::vector<float> diffData;
			diffData.reserve(currentSamplesByTimeband->Num() / 2);
			diffData.resize(currentSamplesByTimeband->Num() / 2, 0.0);
			float delta = 0.0;
			for (int tau = 0; tau < diffData.size(); ++tau)
			{
				for (int i = 0; i < diffData.size(); ++i)
				{
					delta = (*currentSamplesByTimeband)[i] - (*currentSamplesByTimeband)[i + tau];
					diffData[tau] += delta * delta;
				}
			}


			//cum
			std::vector<float> cumData;
			cumData.reserve(diffData.size());
			float cumSum = 0.0;
			cumData.push_back(1.0);
			for (int tau = 1; tau < diffData.size(); ++tau)
			{
				cumSum += diffData[tau];
				cumData.push_back(diffData[tau] * (float)tau / cumSum);
			}

			//absoluteThreshold
			int tau = 0;
			for (tau = 2; tau < cumData.size(); ++tau)
			{
				if (cumData[tau] < pitchThreshold)
				{
					while (tau + 1 < cumData.size() && cumData[tau + 1] < cumData[tau])
					{
						++tau;
					}
					break;
				}
			}

			if (tau == cumData.size() || cumData[tau] >= pitchThreshold)
			{
				// no pitch found
				(*currentFrequencyF0)[0] = 0.0;
			}
			else
			{
				//parabolic interpolation
				float bestTau;
				int x0 = tau - 1;
				int x2 = (tau < (cumData.size() - 1)) ? tau - 1 : x2 = tau;

				if (x0 == tau)
				{
					if (cumData[tau] <= cumData[x2]) 
					{
						bestTau = tau;
					}
					else
					{
						bestTau = x2;
					}
				}
				else if (x2 == tau)
				{
					if (cumData[tau] <= cumData[x0])
					{
						bestTau = tau;
					}
					else 
					{
						bestTau = x0;
					}
				}
				else
				{
					float s0, s1, s2;
					s0 = cumData[x0];
					s1 = cumData[tau];
					s2 = cumData[x2];

					bestTau = tau + (s2 - s0) / (2 * (2 * s1 - s2 - s0));
				}

				(*currentFrequencyF0)[0] = (bestTau > 0.0) ? sampleRate / bestTau : 0.0;
			}
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("TimeWindow must be greater than zero"));
	}
}

/***************************************************/
/*                 FULL ANALYSIS                   */
/***************************************************/

void AudioAnalyzerCore::getAmplitudeSection(FChannelTArray& samplesByTimeBand, int channelModeValue, int channel, float startTime, float endTime, int timeBandSize)
{
	samplesByTimeBand.Empty();
	if (_AACurrentSource)
	{
		if (timeBandSize > 0)
		{
			const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfo = _AACurrentSource->getAudioInfo();
			if (audioInfo)
			{
				ChannelSelectMode channelMode = static_cast<ChannelSelectMode>(channelModeValue);
				int maxChannels = 1;
				if (channelMode == ChannelSelectMode::Split_All)
				{
					maxChannels = audioInfo->numChannels;
				}

				float timeWindow = endTime - startTime;

				int numBars = (timeWindow * audioInfo->sampleRate) / (float) timeBandSize;

				for (int numChannel = 0; numChannel < maxChannels; ++numChannel)
				{
					TArray<float> channelValues;
					samplesByTimeBand.Add(channelValues);
					samplesByTimeBand[numChannel].Init(0.0, numBars);
				}

				//zeroedChannelTArray<float>(amplitude, 0.0);

				if (!_AACurrentSource->hasOnlyHeader())
				{
					TArray<int32> numSamplesByTimeband;
					numSamplesByTimeband.Init(0, numBars);

					uint16 sampleIncrement = 1;
					int numSelChannel = 0;

					if (channelMode == ChannelSelectMode::Select_one)
					{
						sampleIncrement = audioInfo->numChannels;
						numSelChannel = channel;
					}

					int32 firstSample = (audioInfo->sampleRate * startTime * audioInfo->numChannels) + numSelChannel;
					firstSample += ((audioInfo->sampleRate * audioInfo->numChannels * timeWindow) / 2); //unshift windoww before wavedataExtractor
					int32 lastSample = 0;//(audioInfo->sampleRate * (currentTime + timeWindow) * audioInfo->numChannels) + numSelChannel;
					{
						std::lock_guard<std::mutex> lock(_AACurrentSource->getSourceMutex());
						if (audioInfo->audioFormat == PCAudioAnalyzer::AUDIOFILE_INFO_FORMAT::AUDIOFORMAT_INT) 	// PCM_CODE        0x0001
						{
							switch (audioInfo->bitDepth)
							{
							case 16: waveDataExtractor<int16>(samplesByTimeBand, numSamplesByTimeband, firstSample, lastSample, audioInfo, timeWindow, sampleIncrement, timeBandSize, numBars, twoBytesToFloat); break;
							case 32: waveDataExtractor<int32>(samplesByTimeBand, numSamplesByTimeband, firstSample, lastSample, audioInfo, timeWindow, sampleIncrement, timeBandSize, numBars, fourBytesToFloat); break;
							}
						}
						else if (audioInfo->audioFormat == PCAudioAnalyzer::AUDIOFILE_INFO_FORMAT::AUDIOFORMAT_FLOAT) 	// IEEE_FLOAT_CODE 0x0003
						{
							switch (audioInfo->bitDepth)
							{
							case 32: waveDataExtractor<float>(samplesByTimeBand, numSamplesByTimeband, firstSample, lastSample, audioInfo, timeWindow, sampleIncrement, timeBandSize, numBars, readFloat); break;
							}
						}
					}

					applyWeightChannelTArray<float, int32>(samplesByTimeBand, numSamplesByTimeband, 0);
				}
			}
			else
			{
				UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Invalid Sound Info"));
			}
		}
		else
		{
			UE_LOG(LogParallelcubeAudioAnalyzer, Warning, TEXT("TimeBandSize must be greater than zero"));
		}
	}
	else
	{
		UE_LOG(LogParallelcubeAudioAnalyzer, Error, TEXT("Invalid Audio Info"));
	}
}