// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#ifndef AudioAnalyzerCore_H
#define AudioAnalyzerCore_H

#include "AudioAnalyzerCommon.h"
#include "HistoryManager.h"
#include "StreamHeap.h"
#include "kiss_fftr.h"

#include <memory>
#include <vector>
#include <functional>

/**
* Forward declaration
*/
class AudioAnalyzerSource;

/**
* @brief Audio Analyzer Core
*
* This class uses raw wave data to apply an amplitude, spectrum, and/or beat tracking analysis
*/
class AudioAnalyzerCore
{

public:

	/**
	* Bynary search function for TArray container
	* @tparam T					Type of the buffer elements
	* @tparam Compare			Function to compare order
	* @param ArrayToSearch		TArray reference to be used 
	* @param FirstIndex			Index of the low bound 
	* @param LastIndex			Index of the high bound
	* @param Value				Value to search
	* @param Comp				Function to compare
	* @return					Index of the found element, if not present returns the first index
	*/
	template<class T, class Compare>
	int32 binarySearch(const TArray<T>& ArrayToSearch, int32 FirstIndex, int32 LastIndex, const T& Value, Compare Comp)
	{
		int currentIndex = 0;
		int count = (LastIndex - FirstIndex) + 1;
		int step = 0;

		int resultIndex = FirstIndex;

		while (count > 0)
		{
			step = count / 2;
			currentIndex = resultIndex + step;
			if (!Comp(Value, ArrayToSearch[currentIndex]))
			{
				resultIndex = currentIndex + 1;
				count -= step + 1;
			}
			else
			{
				count = step;
			}
		}

		return resultIndex;
	}

	/**
	* TArray Channel Container (float) type
	*/
	typedef TArray<TArray<float> > FChannelTArray;

	/**
	* TArray Channel Container (int32) type
	*/
	typedef TArray<TArray<int32> > IChannelTArray;

	/**
	* TArray Channel Container (bool) type
	*/
	typedef TArray<TArray<bool> > BChannelTArray;

	/**
	* Constructor
	*/
	AudioAnalyzerCore();

	/**
	* Destructor
	*/
	~AudioAnalyzerCore();

	/**
	* Disable copy constructor
	* @param x	---
	*/
	AudioAnalyzerCore(AudioAnalyzerCore const &x) = delete;

	/**
	* Disable copy assignment
	* @param x	---
	*/
	void operator=(AudioAnalyzerCore const &x) = delete;

	/**
	* Sets the audio input source 
	* @param audioSourcePtr		A pointer to an AudioAnalyzerSource instance
	*/
	void setAudioSource(AudioAnalyzerSource* audioSourcePtr);

	/**
	* Returns if the core has an audio source with a valid loaded audio
	* @return Core has a valid audio info
	*/
	bool hasValidAudio() const { return _hasValidAudioInfo; };

	/**
	* Returns if the core has been correctly initializated to calculate frequency spectrum
	* @return Core has a valid frequency spectrum configuration
	*/
	bool hasAmplitudeConfig() const { return _hasValidAConfig; };

	/**
	* Returns if the core has been correctly initializated to calculate frequency spectrum
	* @return Core has a valid frequency spectrum configuration
	*/
	bool hasFrequencySpectrumConfig() const { return _hasValidSConfig; };

	/**
	* Returns if the core has been correctly initializated to calculate beat tracking
	* @return Core has a valid beat tracking configuration
	*/
	bool hasBeatTrackingConfig() const { return _hasValidBTConfig; };

	/**
	* Returns if the core has been correctly initializated to calculate pitch tracking
	* @return Core has a valid pitch tracking configuration
	*/
	bool hasPitchTrackingConfig() const { return _hasValidPTConfig; };

	/**
	* Return if amplitude configuration is set on Split mode
	* @return Amplitude configuration is set on Split mode
	*/
	bool isAConfigSplitChannel() const;

	/**
	* Return if spectrum configuration is set on Split mode
	* @return Spectrum configuration is set on Split mode
	*/
	bool isSConfigSplitChannel() const;

	/**
	* Return if beat tracking configuration is set on Split mode
	* @return Beat tracking configuration is set on Split mode
	*/
	bool isBTConfigSplitChannel() const;

	/**
	* Return if pitch tracking configuration is set on Split mode
	* @return Pitch tracking configuration is set on Split mode
	*/
	bool isPTConfigSplitChannel() const;

	/**
	* Initializes the amplitude analysis configuration
	* @param[out] outputValues		Output container
	* @param channelMode			Channel selection mode
	* @param channel				Number of channel
	* @param numTimeBands			Number of bars
	* @param timeWindow				Window to calculate the spectrum (in seconds)
	*/
	void initializeAConfig(FChannelTArray& outputValues, int channelMode, int channel, int numTimeBands, float timeWindow);

	/**
	* Initializes the linear spectrum configuration
	* @param[out] outputValues				Output container
	* @param[out] outputAverageValues		Output average container
	* @param[out] outputPeaksValues			Output peak container
	* @param[out] outputPeaksEnergyValues	Output peak energy container
	* @param channelMode					Channel selection mode
	* @param channel						Number of channel
	* @param numFreqBands					Number of bars 
	* @param timeWindow						Window to calculate the spectrum (in seconds)
	* @param historySize					History size
	* @param usePeakValues					Returns peak value of the band instead of average values
	* @param numPeaks						Number of peaks to be calculated
	*/
	void initializeSConfig_Linear(FChannelTArray& outputValues, 
									FChannelTArray& outputAverageValues,
									FChannelTArray& outputPeaksValues,
									FChannelTArray& outputPeaksEnergyValues,
									int channelMode,
									int channel, 
									int numFreqBands, 
									float timeWindow, 
									int historySize, 
									bool usePeakValues,
									int numPeaks);

	/**
	* Initializes the logarithmic spectrum configuration
	* @param[out] outputValues				Output container
	* @param[out] outputAverageValues		Output average container
	* @param[out] outputPeaksValues			Output peak container
	* @param[out] outputPeaksEnergyValues	Output peak energy container
	* @param channelMode					Channel selection mode
	* @param channel						Number of channel
	* @param numFreqBands					Number of bars
	* @param timeWindow						Window to calculate the spectrum (in seconds)
	* @param historySize					History size
	* @param usePeakValues					Returns peak value of the band instead of average values
	* @param numPeaks						Number of peaks to be calculated
	*/
	void initializeSConfig_Log(FChannelTArray& outputValues, 
								FChannelTArray& outputAverageValues, 
								FChannelTArray& outputPeaksValues, 
								FChannelTArray& outputPeaksEnergyValues,
								int channelMode, 
								int channel, 
								int numFreqBands, 
								float timeWindow, 
								int historySize, 
								bool usePeakValues, 
								int numPeaks);

	/**
	* Initializes the custom limits spectrum configuration
	* @param[out] outputValues				Output container
	* @param[out] outputAverageValues		Output average container
	* @param[out] outputPeaksValues			Output peak container
	* @param[out] outputPeaksEnergyValues	Output peak energy container
	* @param channelMode					Channel selection mode
	* @param channel						Number of channel
	* @param bandLimits						TArray of frequency bounds for each bar
	* @param timeWindow						Window to calculate the spectrum (in seconds)
	* @param historySize					History size
	* @param usePeakValues					Returns peak value of the band instead of average values
	* @param numPeaks						Number of peaks to be calculated
	*/
	void initializeSConfig_Custom(FChannelTArray& outputValues,
									FChannelTArray& outputAverageValues, 
									FChannelTArray& outputPeaksValues, 
									FChannelTArray& outputPeaksEnergyValues,
									int channelMode,
									int channel, 
									const TArray<FVector2D>& bandLimits, 
									float timeWindow, 
									int historySize, 
									bool usePeakValues, 
									int numPeaks);

	/**
	* Initializes the defaut beat tracking configuration
	* @param[out] beatOnSet					Output beat states container
	* @param[out] frequencyValues			Output beat frequency container
	* @param[out] averageFrequencyValues	Output beat average frequency container
	* @param[out] varianceFrequencyValues	Output beat variance frequency container
	* @param[out] bpmCurrentValues			Output bpm current container
	* @param[out] bpmTotalValues			Output bpm total container
	* @param channelMode					Channel selection mode
	* @param channel						Number of channel
	* @param timeWindow						Window to calculate the spectrum (in seconds)
	* @param historySize					Spectrum History size
	* @param threshold						Beat detection threshold
	* @param calculateBPM					Enable BPM calculator
	* @param BPMHistorySize					BPM History size
	*/
	void initializeBTConfig(BChannelTArray& beatOnSet,
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
							int BPMHistorySize);

	/**
	* Initializes the custom limits beat tracking configuration
	* @param[out] beatOnSet					Output beat states container
	* @param[out] frequencyValues			Output beat frequency container
	* @param[out] averageFrequencyValues	Output beat average frequency container
	* @param[out] varianceFrequencyValues	Output beat variance frequency container
	* @param[out] bpmCurrentValues			Output bpm current container
	* @param[out] bpmTotalValues			Output bpm total container
	* @param channelMode					Channel selection mode
	* @param channel						Number of channel
	* @param bandLimits						TArray of frequency bounds for each bar
	* @param timeWindow						Window to calculate the spectrum (in seconds)
	* @param historySize					Spectrum History size
	* @param threshold						Beat detection threshold
	* @param calculateBPM					Enable BPM calculator
	* @param BPMHistorySize					BPM History size
	*/
	void initializeBTConfig_Custom(BChannelTArray& beatOnSet,
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
									int BPMHistorySize);


	/**
	* Initializes the pitch tracking configuration
	* @param[out] outputPitchTrackingValues	Output pitch tracking container
	* @param channelMode					Channel selection mode
	* @param channel						Number of channel
	* @param timeWindow						Window to calculate the spectrum (in seconds)
	* @param threshold						Yin threshold
	*/
	void initializePTConfig(FChannelTArray& outputPitchTrackingValues,
							int channelMode,
							int channel,
							float timeWindow,
							float threshold);

	/**
	* Returns amplitude for a sample for a window of time (seconds) centered on current playback position and group it into bars
	* @param[out] amplitude		Amplitude result by channel
	*/
	void getAmplitude(FChannelTArray& amplitude);
	
	/**
	* Returns frequency spectrum data centered on current playback position using the configuration spectrum parameters
	* @param[out] frequencySpectrum	 Spectrum data array by channel
	*/
	void getSpectrum(FChannelTArray& frequencySpectrum);

	/**
	* Returns average data for history spectrum using the configuration spectrum parameters
	* @param[out] averageSpectrum	Average spectrum by channel
	*/
	void getSpectrum_Average(FChannelTArray& averageSpectrum);

	/**
	* Returns frequency spectrum peak data centered on current playback position using the configuration spectrum parameters
	* @param[out] frequencyPeaks		Frequency peaks array by channel
	* @param[out] frequencyPeaksEnergy	Frequency peaks energy array by channel
	*/
	void getSpectrumPeaks(FChannelTArray& frequencyPeaks, FChannelTArray& frequencyPeaksEnergy);

	/**
	* Returns the beat tracking analysis result for current playback position
	* @param[out] beatResult		Beat result for custom bands by channel
	* @param[out] spectrumValues	Current frequency spectrum values by channel
	* @param[out] averageBPM		Return BPM partial result by channel
	* @param[out] totalBPM			Return BPM total result by channel
	* @param overrideThreshold		Overrides Beat detection configuration threshold for each band
	*/
	void getBeatTracking(BChannelTArray& beatResult, FChannelTArray& spectrumValues, IChannelTArray& averageBPM, IChannelTArray& totalBPM, const TArray<float>& overrideThreshold);

	/**
	* Returns average data for history frequency spectrum using the configuration beat tracking parameters
	* @param[out] averageSpectrum	Average frequency spectrum data array
	*/
	void getBeatTracking_Average(FChannelTArray& averageSpectrum);

	/**
	* Returns variance data for history frequency spectrum using the configuration beat tracking parameters
	* @param[out] varianceSpectrum		Variance frequency spectrum data array
	* @param[out] averageBeatTracking	Average frequency spectrum data array
	*/
	void getBeatTracking_AverageAndVariance(FChannelTArray& averageBeatTracking, FChannelTArray& varianceSpectrum);

	/**
	* Extract basic beat tracking analysis results (Bass/LowM/HighM)
	* @param[out] IsBass	Beat results on Bass band by channel
	* @param[out] IsLowM	Beat results on Low Mid band by channel
	* @param[out] IsHighM	Beat results on High Mid band by channel
	* @param beatResult		Beat results by channel 
	*/
	void extractBasicBeatTracking(TArray<bool>& IsBass, TArray<bool>& IsLowM, TArray<bool>& IsHighM, const BChannelTArray& beatResult);

	/**
	* Returns amplitude for a sample for all the sound
	* @param[out] amplitude		Amplitude result by channel
	*/
	void getTotalAmplitude(FChannelTArray& amplitude);

	/**
	* Reset BPM counters
	*/
	void resetBPMCounters();

	/**
	* Returns pitch tracking result (fundamental frequency) by channel
	* @param[out] frequencyF0			Fundamental frequency data by channel
	*/
	void getPitchTracking(FChannelTArray& frequencyF0);

	/**
	* Returns the amplitude analysis result of a section of the audio file
	* @param[out] samplesByTimeBand		Amplitude results
	* @param channelMode				Channel selection mode
	* @param channel					Number of channel
	* @param startTime					Start time of the audio section
	* @param endTime					End time of the audio section
	* @param timeBandSize				Number of samples accumulated per result
	*/
	void getAmplitudeSection(FChannelTArray& samplesByTimeBand, int channelMode, int channel, float startTime, float endTime, int timeBandSize);

private:

	/** Enum for spectrum types */
	enum SType {
		S_Linear, /**< Linear bars */
		S_Log, /**< Logarithmic bars */
		S_Custom /**< Custom limit bars */
	};

	/** Enum for beat tracking types */
	enum BTType {
		BT_Default, /**< Basic BASS/LowM/HighM bars */
		BT_Custom /**< Custom limit bars */
	};

	/** Enum for beat tracking types */
	enum ChannelSelectMode {
		All_in_one, /**<Mix all channels in one */
		Select_one,  /**< Select only one channel */
		Split_All  /**< Process all channels separately */
	};

	/** Enum for beat tracking types */
	enum PitchType {
		PT_Yin /**<YIN pitch algorithm */
	};

	/** Amplitude configuration structure */
	struct AudioAnalyzerAConfig
	{
		ChannelSelectMode channelMode; /**<  Channel selection mode */
		int channel; /**< number of channel when channel mode is Select_one*/
		int numTimeBands; /**< number of time bands */
		float timeWindow; /**< window to calculate the spectrum (in seconds) */
	};

	/** Frequency spectrum configuration structure */
	struct AudioAnalyzerSConfig
	{
		SType spectrumType; /**< Type of spectrum */
		ChannelSelectMode channelMode; /**<  Channel selection mode */
		int channel; /**< number of channel when channel mode is Select_one*/
		int numFreqBands; /**< number of bars */
		float timeWindow; /**< window to calculate the spectrum (in seconds) */
		int sampleWindow; /**< number of samples to calculate the spectrum FFT (power of 2) */
		int historySize; /**< history size (we need at least 1 to store the current result) */
		bool usePeakValues; /**< return peak value of the band instead of average values*/
		int numPeaks; /**< number of peaks to calculate*/		
	};

	/** Beat tracking configuration structure */
	struct AudioAnalyzerBTConfig
	{
		BTType beatTrackingType; /**< Type of beatTracking */
		ChannelSelectMode channelMode; /**<  Channel selection mode */
		int channel; /**< number of channel when channel mode is Select_one*/
		int numFreqBands; /**< number of bars */
		float timeWindow; /**< window to calculate the spectrum (in seconds) */
		int sampleWindow; /**< number of samples to calculate the spectrum FFT (power of 2) */
		int historySize; /**< history size (to store spectrum results, we need at least 1 to store the current result) */
		float threshold; /**< beat detection threshold */
		bool calculateBPM; /**< enable BPM calculation */
		int BPMHistorySize; /**< BPM history size (samples)*/
	};

	/** Pitch Tracking configuration structure */
	struct AudioAnalyzerPTConfig
	{
		ChannelSelectMode channelMode; /**<  Channel selection mode */
		int channel; /**< number of channel when channel mode is Select_one*/
		float timeWindow; /**< window to calculate the spectrum (in seconds) */
		int sampleWindow; /**< number of samples to calculate the spectrum FFT (power of 2) */
		PitchType pitchTrackingType; /**< pitch tracking type*/
		float pitchThreshold; /**< yin pitch tracking threshold */
	};

	/**
	* Has a source with a valid audio info
	*/
	bool _hasValidAudioInfo;

	/**
	* Has a valid amplitude configuration
	*/
	bool _hasValidAConfig;

	/**
	* Has a valid frequency spectrum configuration
	*/
	bool _hasValidSConfig;

	/**
	* Has a valid beat tracking configuration
	*/
	bool _hasValidBTConfig;

	/**
	* Has a valid pitch tracking configuration
	*/
	bool _hasValidPTConfig;

	/**
	* Current audio source pointer (player/microphone...)
	*/
	AudioAnalyzerSource* _AACurrentSource;

	/**
	* Amplitude analysis configuration
	*/
	AudioAnalyzerAConfig _aConfig;

	/**
	* Spectrum analysis configuration
	*/
	AudioAnalyzerSConfig _sConfig;

	/**
	* Spectrum input by channel
	*/
	FChannelTArray _samplesByTimeBand;

	/**
	* Spectrum input by channel for peak analysis
	*/
	FChannelTArray _samplesByPeak;

	/**
	* Spectrum input by channel for pitch analysis
	*/
	FChannelTArray _samplesByPitch;

	/**
	* Number of samples to take for each spectrum bar
	*/
	std::vector<int> _numSamplesByBand;

	/**
	* Weight factor for pitch detection
	*/
	std::vector<int> _weightByBand;

	/**
	* Spectrum analysis result by channel
	*/
	std::vector<std::unique_ptr<HistoryManager<float> > > _hmSamplesByFrequencyByChannel;

	/**
	* Frequency peak analysis result by channel
	*/
	std::vector<std::unique_ptr<HistoryManager<float> > > _hmFrequencyPeaksByChannel;

	/**
	* Frequency peak energy analysis result by channel
	*/
	std::vector<std::unique_ptr<HistoryManager<float> > > _hmFrequencyPeaksEnergyByChannel;

	/**
	* Beat tracking analysis configuration
	*/
	AudioAnalyzerBTConfig _btConfig;

	/**
	* Spectrum input by channel (BT Analysis)
	*/
	FChannelTArray _samplesByTimeBand_BT;

	/**
	* Beat tracking analysis result history by channel
	*/
	std::vector<std::unique_ptr<HistoryManager<float> > > _hmSamplesByFrequencyByChannel_BT;

	/**
	* Number of samples to take for each beat tracking bar
	*/
	std::vector<int> _numSamplesByBand_BT;

	/**
	* Beat tracking result history by channel
	*/
	std::vector<std::unique_ptr<HistoryManager<int> > > _beatHistory_BPM;

	/**
	* Beat tracking distance (seconds) result history by channel
	*/
	std::vector<std::unique_ptr<HistoryManager<float> > > _beatDistanceHistory_BPM;

	/**
	* Average seconds per beat 
	*/
	FChannelTArray _averageSPB_BPM;

	/**
	* Num beats used into the average seconds per beat
	*/
	IChannelTArray _numBeatsSPB_BPM;

	/**
	* BPM container by channel
	*/
	std::vector<std::unique_ptr<StreamHeapVector<int> > > _bpmHeaps_BPM;

	/**
	* Pitch Tracking analysis configuration
	*/
	AudioAnalyzerPTConfig _ptConfig;

	/**
	* Pitch Tracking analysis result by channel
	*/
	std::vector<std::unique_ptr<HistoryManager<float> > > _hmPitchTrackingByChannel;

	/**
	* Returns amplitude data for a sample for a window of time (seconds) centered on a playback position and group it into bars
	* @param[out] amplitude		Amplitude result by channel
	* @param channelMode		Channel selection mode
	* @param channel			Number of channel
	* @param numTimeBands		Number of bars
	* @param timeWindow			Window to calculate the spectrum (in seconds)
	* @param currentTime		Audio playback position
	*/
	void getAmplitude(FChannelTArray& amplitude, ChannelSelectMode channelMode, int channel, int numTimeBands, float timeWindow, float currentTime);

	/**
	* Returns spectrum data centered on a playback position using the configuration spectrum parameters
	* @param[out] frequencySpectrum		Spectrum data by channel
	* @param currentTime				Audio playback position
	* @param timestamp					Buffer Position timestamp
	*/
	void getSpectrum(FChannelTArray& frequencySpectrum, float currentTime, float timestamp);

	/**
	* Initializes spectrum configuration structure and calculates the near window power of 2 to fit the time window
	* @param channelMode			Channel selection mode
	* @param channel				Number of channel
	* @param numFreqBands			Number of bars
	* @param timeWindow				Window to calculate the spectrum (in seconds)
	* @param historySize			History size
	* @param usePeakValues			Return peak value of the band instead of average value
	* @param numPeaks				Number of peaks for the output
	* @return bool					Pre initialization result
	*/
	bool initializeSPreConfig(ChannelSelectMode channelMode, int channel, int numFreqBands, float timeWindow, int historySize, bool usePeakValues, int numPeaks);

	/**
	* Initializes beat tracking configuration structure and calculates the near window power of 2 to fit the time window
	* @param channelMode	Channel selection mode
	* @param channel		Number of channel
	* @param numFreqBands	Number of Bands
	* @param timeWindow		Window to calculate the spectrum (in seconds)
	* @param historySize	History size
	* @param threshold		Beat detection threshold
	* @return bool			Pre initialization result
	*/
	bool initializeBTPreConfig(ChannelSelectMode channelMode, int channel, int numFreqBands, float timeWindow, int historySize, float threshold);
	
	/**
	* Initializes history buffer for spectrum analysis and output containers
	* @param[out] frequencyValues				Frequency results container
	* @param[out] averageFrequencyValues		Frequency average results container
	* @param[out] frequencyPeaksValues			Frequency peak results container
	* @param[out] frequencyPeaksEnergyValues	Frequency peak energy results container
	* @return bool								Sucessfully post initialization
	*/
	bool initializeSPostConfig(FChannelTArray& frequencyValues, 
								FChannelTArray& averageFrequencyValues,
								FChannelTArray& frequencyPeaksValues, 
								FChannelTArray& frequencyPeaksEnergyValues);
	
	/**
	* Initializes history buffer for beat tracking analysis
	* @param[out] beatOnSet					Beat states container
	* @param[out] frequencyValues			Beat frequency container
	* @param[out] averageFrequencyValues	Beat average frequency container
	* @param[out] varianceFrequencyValues	Beat variance frequency container
	* @param[out] bpmCurrentValues			BPM current container
	* @param[out] bpmTotalValues			BPM total container
	* @return bool							Sucessfully post initialization
	*/
	bool initializeBTPostConfig(BChannelTArray& beatOnSet,
								FChannelTArray& frequencyValues,
								FChannelTArray& averageFrequencyValues,
								FChannelTArray& varianceFrequencyValues,
								IChannelTArray& bpmCurrentValues,
								IChannelTArray& bpmTotalValues);

	/**
	* Gets the amplitude analysis for the sample window
	* @param[out] samplesByTimeBand			Amplitude results by channel
	* @param[out] numSamplesByTimeband		Number of samples for each amplitude bar
	* @param channelMode					Channel selection mode
	* @param channel						Number of channel
	* @param timeWindow						Window to be analyzed (in seconds)
	* @param sampleWindow					Number of samples (power of 2)
	* @param currentTime					Audio playback position
	*/
	void getWaveDataWindowed(FChannelTArray& samplesByTimeBand, TArray<int32>& numSamplesByTimeband, ChannelSelectMode channelMode, int channel, float timeWindow, int sampleWindow, float currentTime);
	
	/**
	* Calculates FFT analysis
	* @param[out] fftOutput			Output FFT data
	* @param samplesByTimeband		Input FFT data
	* @return						FFT analysis execution result
	*/
	static bool getFFT(std::vector<kiss_fft_cpx>& fftOutput, const TArray<float>* samplesByTimeband);

	/**
	* Gets the spectrum analysis
	* @param[out] samplesByFrequency	Spectrum results by channel
	* @param[out] samplesByTimeBand		Amplitude results by channel
	* @param[out] numSamplesByBand		Number of samples for each spectrum bar
	* @param channelMode				Channel selection mode
	* @param channel					Number of channel
	* @param numFreqBands				Number of spectrum bars
	* @param timeWindow					Window to be analyzed (in seconds)
	* @param sampleWindow				Number of samples (power of 2)
	* @param hasCustomLimits			Has custom bounds for each bar
	* @param usePeakValues				Return peak values in the band instead of average values
	* @param currentTime				Audio playback position
	*/
	void getSpectrum(FChannelTArray& samplesByFrequency, 
					 FChannelTArray& samplesByTimeBand, 
					 std::vector<int>& numSamplesByBand, ChannelSelectMode channelMode, int channel, int numFreqBands, float timeWindow, int sampleWindow, bool hasCustomLimits, bool usePeakValues, float currentTime);
	
	/**
	* Gets the spectrum analysis
	* @param[out] frequencySpectrum		Spectrum results by channel
	* @param channelMode				Channel selection mode
	* @param channel					Number of channel
	* @param numFreqBands				Number of spectrum bars
	* @param timeWindow					Window to be analyzed (in seconds)
	* @param sampleWindow				Number of samples (power of 2)
	* @param hasCustomLimits			Has custom bounds for each bar
	* @param usePeakValues				Return peak values in the band instead of average values
	* @param currentTime				Audio playback position
	* @param timestamp					Buffer Position timestamp
	*/
	void getSpectrum(FChannelTArray& frequencySpectrum, ChannelSelectMode channelMode, int channel, int numFreqBands, float timeWindow, int sampleWindow, bool hasCustomLimits, bool usePeakValues, float currentTime, float timestamp);

	/**
	* Gets the spectrum peaks analysis
	* @param[out] frecuencyPeaks		Frequency Peaks results by channel
	* @param[out] frecuencyPeaksEnergy	Frequency Peaks Energy results by channel
	* @param[out] samplesByTimeBand		Amplitude results by channel
	* @param channelMode				Channel selection mode
	* @param channel					Number of channel
	* @param numPeaks					Number of peaks to calculate
	* @param timeWindow					Window to be analyzed (in seconds)
	* @param sampleWindow				Number of samples (power of 2)
	* @param currentTime				Audio playback position
	*/
	void getSpectrumPeaks(FChannelTArray& frecuencyPeaks, 
						  FChannelTArray& frecuencyPeaksEnergy, 
						  FChannelTArray& samplesByTimeBand, 
						  ChannelSelectMode channelMode, int channel, int numPeaks, float timeWindow, int sampleWindow, float currentTime);

	/**
	* Gets the spectrum peaks analysis
	* @param[out] frecuencyPeaks		Frequency Peaks results by channel
	* @param[out] frecuencyPeaksEnergy	Frequency Peaks Energy results by channel
	* @param channelMode				Channel selection mode
	* @param channel					Number of channel
	* @param numPeaks					Number of peaks to calculate
	* @param timeWindow					Window to be analyzed (in seconds)
	* @param sampleWindow				Number of samples (power of 2)
	* @param currentTime				Audio playback position
	* @param timestamp					Buffer Position timestamp
	*/
	void getSpectrumPeaks(FChannelTArray& frecuencyPeaks, 
						  FChannelTArray& frecuencyPeaksEnergy, 
						  ChannelSelectMode channelMode, int channel, int numPeaks, float timeWindow, int sampleWindow, float currentTime, float timestamp);
	/**
	* Returns the beat tracking analysis result for a playback position
	* @param[out] beatResult		Beat result for custom frequency bands by channel
	* @param[out] spectrumValues	Current frequency spectrum values by channel
	* @param[out] averageBPM		Return BPM partial result by channel
	* @param[out] totalBPM			Return BPM total result by channel
	* @param overrideThreshold		Overrides Beat detection configuration threshold for each band
	* @param currentTime			Audio playback position
	* @param timestamp				Buffer Position timestamp
	*/
	void getBeatTracking(BChannelTArray& beatResult, FChannelTArray& spectrumValues, IChannelTArray& averageBPM, IChannelTArray& totalBPM, const TArray<float>& overrideThreshold, float currentTime, float timestamp);

	/**
	* Gets the spectrum peaks analysis
	* @param[out] frecuencyF0			Fundamental Frequency results by channel
	* @param[out] samplesByTimeBand		Amplitude results by channel
	* @param channelMode				Channel selection mode
	* @param channel					Number of channel
	* @param pitchTrackingType			Pitch tracking analysis type
	* @param threshold					Pitch tracking threshold
	* @param timeWindow					Window to be analyzed (in seconds)
	* @param sampleWindow				Number of samples (power of 2)
	* @param currentTime				Audio playback position
	*/
	void getPitchTracking(FChannelTArray& frecuencyF0,
							FChannelTArray& samplesByTimeBand, ChannelSelectMode channelMode, int channel, PitchType pitchTrackingType, float threshold, float timeWindow, int sampleWindow, float currentTime);

	/**
	* Gets the spectrum peaks analysis
	* @param[out] frecuencyF0			Fundamental Frequency results by channel
	* @param channelMode				Channel selection mode
	* @param channel					Number of channel
	* @param pitchTrackingType			Pitch tracking analysis type
	* @param threshold					Pitch tracking threshold
	* @param timeWindow					Window to be analyzed (in seconds)
	* @param sampleWindow				Number of samples (power of 2)
	* @param currentTime				Audio playback position
	* @param timestamp					Buffer Position timestamp
	*/
	void getPitchTracking(FChannelTArray& frecuencyF0, ChannelSelectMode channelMode, int channel, PitchType pitchTrackingType, float threshold, float timeWindow, int sampleWindow, float currentTime, float timestamp);

	/**
	* Reads the wave data using the wav info, returns the amplitude analysis input
	* @param[out] samplesByTimeBand			Signal result by timeband and channel
	* @param[out] numSamplesByTimeband		Number of samples per timeband
	* @param[in,out] firstSample			First position to read
	* @param[in,out] lastSample				Last position to read
	* @param audioInfo						Wave info structure
	* @param timeWindow						Seconds to read
	* @param sampleIncrement				Sample read increment
	* @param timeBandSize					Number of samples per timeband
	* @param numTimeBands					Number of timebands
	* @param readData						Function to read the wave raw data
	*/
	template<typename WavType>
	void waveDataExtractor(FChannelTArray& samplesByTimeBand, TArray<int32>& numSamplesByTimeband, int32& firstSample, int32& lastSample, const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfo, const float timeWindow, const uint16& sampleIncrement, const uint32& timeBandSize, const int numTimeBands, std::function<float(void*, int32)> readData);
	
	/**
	* Reads the wave data using the wav info, returns the spectrum analysis input
	* @param[out] samplesByTimeBand			Signal result by timeband and channel
	* @param[out] numSamplesByTimeband		Number of samples per timeband
	* @param[in,out] firstSample			First position to read
	* @param[in,out] lastSample				Last position to read
	* @param audioInfo						Wave info structure
	* @param timeWindow						Seconds to read
	* @param sampleIncrement				Sample read increment
	* @param sampleWindow					Number of samples to read
	* @param readData						Function to read the wave raw data
	*/
	template<typename WavType>
	void waveDataExtractorSpectrum(FChannelTArray& samplesByTimeBand, TArray<int32>& numSamplesByTimeband, int32& firstSample, int32& lastSample, const PCAudioAnalyzer::AUDIOFILE_INFO* audioInfo, const float timeWindow, const uint16& sampleIncrement, const int sampleWindow, std::function<float(void*, int32)> readData);

	/**
	* Set values of channel result container
	* @param arrayRef	Array container reference
	* @param value		Value to initialize the container
	*/
	template<typename ArrayType>
	void zeroedChannelTArray(TArray<TArray< ArrayType> >& arrayRef, ArrayType value);

	/**
	* Apply a weight to each value of channel result container
	* @param arrayRef			Array container reference
	* @param arrayWeightsRef	Array of weights container reference
	* @param zeroWeight			Value to set on zero weight
	*/
	template<typename ArrayType, typename ArrayWeightType>
	void applyWeightChannelTArray(TArray<TArray<ArrayType> >& arrayRef, TArray<ArrayWeightType>& arrayWeightsRef, ArrayWeightType zeroWeight);
};

#endif