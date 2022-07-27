// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#pragma once

#include "Sound/SoundWaveProcedural.h"

#include <atomic>

#include "AudioAnalyzerSoundWave.generated.h"

class AudioAnalyzerExtractor;
class USoundAttenuation;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlaybackDelegateSoundWave);

/**
* SoundWaveProcedural custom class 
*/
UCLASS(BlueprintType)
class PARALLELCUBEAUDIOANALYZER_API UAudioAnalyzerSoundWave : public USoundWaveProcedural
{
	GENERATED_BODY()
public:

	/**
	* Event to track the playback end
	*/
	UPROPERTY(BlueprintAssignable, Category = "AudioAnalyzerManager|SoundWave|Events")
		FPlaybackDelegateSoundWave OnPlaybackFinished;
	/**
	* Event to track the playback end
	*/
	UPROPERTY(BlueprintAssignable, Category = "AudioAnalyzerManager|SoundWave|Events")
		FPlaybackDelegateSoundWave OnPlaybackLoopFinished;
	/**
	* Event to track the playback end
	*/
	UPROPERTY(BlueprintAssignable, Category = "AudioAnalyzerManager|SoundWave|Events")
		FPlaybackDelegateSoundWave OnStopFinished;

	/**
	 * Begin Destroy override method
	 */
	virtual void BeginDestroy() override;

	/**
	* Initializes the playback parameters of the sound
	* @param Loops			Number of loops
	* @param StartTime		Starting playback position (Seconds)
	*/
	void Play(int32 Loops, float StartTime);

	/**
	* Stops playback
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|SoundWave|Playback")
		void AsyncStopSound();

	/**
	* Pauses playback if the device is in playing state
	* UnPauses playback if the device has been paused previously
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|SoundWave|Playback")
		void PauseSound();

	/**
	* Sets the playback state on pause/unpause state
	* @param Pause				New pause state
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|SoundWave|Playback")
		void SetPaused(bool Pause);

	/**
	* Returns current playback position in seconds
	* @return Playback position
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|SoundWave|Playback")
		float GetPlaybackTime();

	/**
	* Sets current playback position
	* @param Time		New playback position in seconds
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|SoundWave|Playback")
		void SetPlaybackTime(float Time);

	/**
	* Returns total player playback duration
	* @return Playback duration
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|SoundWave|Playback")
		float GetTotalDuration() const;

	/**
	* Returns total player playback duration
	* @return Playback duration
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|SoundWave|Playback")
		float GetDuration() const override;

	/**
	* Returns playback playing state
	* @return playback is playing
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|SoundWave|Playback")
		bool IsPlaying() const;

	/**
	* Returns playback pause state
	* @return playback is paused
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|SoundWave|Playback")
		bool IsPaused() const;

	/**
	* Returns remaining loops to play
	* @return remaining loops
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|SoundWave|Playback")
		const int GetRemainingLoops() const { return _remainingLoops; };

	/**
	* Plays a sound directly with no attenuation, perfect for UI sounds.
	*
	* * Fire and Forget.
	* * Not Replicated.
	* @param WorldContextObject		World Context Object
	* @param VolumeMultiplier		Multiplied with the volume to make the sound louder or softer.
	* @param PitchMultiplier		Multiplies the pitch.
	* @param StartTime				How far in to the sound to begin playback at
	* @param Loops					Number of Loops (Default: 1)
	* @param ConcurrencySettings	Override concurrency settings package to play sound with
	* @param OwningActor			The actor to use as the "owner" for concurrency settings purposes. Allows PlaySound calls to do a concurrency limit per owner.
	*/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "AudioAnalyzerManager|SoundWave|Override", meta = (WorldContext = "WorldContextObject", AdvancedDisplay = "1", UnsafeDuringActorConstruction = "true"))
		void PlaySound2D(const UObject* WorldContextObject, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, int32 Loops = 1, USoundConcurrency* ConcurrencySettings = nullptr, AActor* OwningActor = nullptr);

	/**
	* Spawns a sound with no attenuation, perfect for UI sounds.
	*
	* * Not Replicated.
	* @param WorldContextObject				World Context Object
	* @param VolumeMultiplier				Multiplied with the volume to make the sound louder or softer.
	* @param PitchMultiplier				Multiplies the pitch.
	* @param StartTime						How far in to the sound to begin playback at
	* @param Loops							Number of Loops (Default: 1)
	* @param ConcurrencySettings			Override concurrency settings package to play sound with
	* @param bPersistAcrossLevelTransition	Whether the sound should continue to play when the map it was played in is unloaded
	* @param bAutoDestroy					Whether the returned audio component will be automatically cleaned up when the sound finishes (by completing or stopping) or whether it can be reactivated
	* @return An audio component to manipulate the spawned sound
	*/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "AudioAnalyzerManager|SoundWave|Override", meta = (WorldContext = "WorldContextObject", AdvancedDisplay = "1", UnsafeDuringActorConstruction = "true", Keywords = "play"))
		UAudioComponent* SpawnSound2D(const UObject* WorldContextObject, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, int32 Loops = 1, USoundConcurrency* ConcurrencySettings = nullptr, bool bPersistAcrossLevelTransition = false, bool bAutoDestroy = true);

	/**
	* Creates a sound with no attenuation, perfect for UI sounds. This does NOT play the sound
	*
	* * Not Replicated.
	* @param WorldContextObject				World Context Object
	* @param VolumeMultiplier				Multiplied with the volume to make the sound louder or softer.
	* @param PitchMultiplier				Multiplies the pitch.
	* @param StartTime						How far in to the sound to begin playback at
	* @param Loops							Number of Loops (Default: 1)
	* @param ConcurrencySettings			Override concurrency settings package to play sound with
	* @param bPersistAcrossLevelTransition	Whether the sound should continue to play when the map it was played in is unloaded
	* @param bAutoDestroy					Whether the returned audio component will be automatically cleaned up when the sound finishes (by completing or stopping) or whether it can be reactivated
	* @return An audio component to manipulate the created sound
	*/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "AudioAnalyzerManager|SoundWave|Override", meta = (WorldContext = "WorldContextObject", AdvancedDisplay = "1", UnsafeDuringActorConstruction = "true", Keywords = "play"))
		UAudioComponent* CreateSound2D(const UObject* WorldContextObject, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, int32 Loops = 1, USoundConcurrency* ConcurrencySettings = nullptr, bool bPersistAcrossLevelTransition = false, bool bAutoDestroy = true);
	
	/**
	* Plays a sound at the given location. This is a fire and forget sound and does not travel with any actor. Replication is also not handled at this point.
	* @param WorldContextObject				World Context Object
	* @param Location						World position to play sound at
	* @param Rotation						World rotation to play sound at
	* @param VolumeMultiplier				Volume multiplier
	* @param PitchMultiplier				PitchMultiplier
	* @param StartTime						How far in to the sound to begin playback at
	* @param Loops							Number of Loops (Default: 1)
	* @param Attenuation_Settings			Override attenuation settings package to play sound with
	* @param ConcurrencySettings			Override concurrency settings package to play sound with
	* @param OwningActor					The actor to use as the "owner" for concurrency settings purposes. Allows PlaySound calls to do a concurrency limit per owner.
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|SoundWave|Override", meta = (WorldContext = "WorldContextObject", AdvancedDisplay = "2", UnsafeDuringActorConstruction = "true", Keywords = "play"))
		void PlaySoundAtLocation(const UObject* WorldContextObject, FVector Location, FRotator Rotation, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, int32 Loops = 1, class USoundAttenuation* Attenuation_Settings = nullptr, USoundConcurrency* ConcurrencySettings = nullptr, AActor* OwningActor = nullptr);

	/**
	* Spawns a sound at the given location. This does not travel with any actor. Replication is also not handled at this point.
	* @param WorldContextObject				World Context Object
	* @param Location						World position to play sound at
	* @param Rotation						World rotation to play sound at
	* @param VolumeMultiplier				Volume multiplier
	* @param PitchMultiplier				PitchMultiplier
	* @param StartTime						How far in to the sound to begin playback at
	* @param Loops							Number of Loops (Default: 1)
	* @param Attenuation_Settings			Override attenuation settings package to play sound with
	* @param ConcurrencySettings			Override concurrency settings package to play sound with
	* @param bAutoDestroy					Whether the returned audio component will be automatically cleaned up when the sound finishes (by completing or stopping) or whether it can be reactivated
	* @return An audio component to manipulate the spawned sound
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|SoundWave|Override", meta = (WorldContext = "WorldContextObject", AdvancedDisplay = "2", UnsafeDuringActorConstruction = "true", Keywords = "play"))
		UAudioComponent* SpawnSoundAtLocation(const UObject* WorldContextObject, FVector Location, FRotator Rotation = FRotator::ZeroRotator, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, int32 Loops = 1, class USoundAttenuation* Attenuation_Settings = nullptr, USoundConcurrency* ConcurrencySettings = nullptr, bool bAutoDestroy = true);

	/** Plays a sound attached to and following the specified component. This is a fire and forget sound. Replication is also not handled at this point.
	* @param AttachToComponent				Component to attach to.
	* @param AttachPointName				Optional named point within the AttachComponent to play the sound at
	* @param Location						Depending on the value of Location Type this is either a relative offset from the attach component/point or an absolute world position that will be translated to a relative offset
	* @param Rotation						Depending on the value of Location Type this is either a relative offset from the attach component/point or an absolute world rotation that will be translated to a relative offset
	* @param LocationType					Specifies whether Location is a relative offset or an absolute world position
	* @param bStopWhenAttachedToDestroyed	Specifies whether the sound should stop playing when the owner of the attach to component is destroyed.
	* @param VolumeMultiplier				Volume multiplier
	* @param PitchMultiplier				PitchMultiplier
	* @param StartTime						How far in to the sound to begin playback at
	* @param Loops							Number of Loops (Default: 1)
	* @param Attenuation_Settings			Override attenuation settings package to play sound with
	* @param ConcurrencySettings			Override concurrency settings package to play sound with
	* @param bAutoDestroy					Whether the returned audio component will be automatically cleaned up when the sound finishes (by completing or stopping) or whether it can be reactivated
	* @return An audio component to manipulate the spawned sound
	*/
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzerManager|SoundWave|Override", meta = (AdvancedDisplay = "1", UnsafeDuringActorConstruction = "true", Keywords = "play"))
		UAudioComponent* SpawnSoundAttached(USceneComponent* AttachToComponent, FName AttachPointName = NAME_None, FVector Location = FVector(ForceInit), FRotator Rotation = FRotator::ZeroRotator, EAttachLocation::Type LocationType = EAttachLocation::KeepRelativeOffset, bool bStopWhenAttachedToDestroyed = false, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, int32 Loops = 1, USoundAttenuation* Attenuation_Settings = nullptr, USoundConcurrency* ConcurrencySettings = nullptr, bool bAutoDestroy = true);

	/**
	* Returns the playback progress and buffer postion time
	* @param[out] bufferPosTime		Buffer position (same as below value)
	* @return float					Progress in seconds
	*/
	float GetPlaybackProgress(float& bufferPosTime);

	/**
	 * Callback to the send the data to the audio render thread
	 * @param OutAudio		Retrieved PCM data array
	 * @param NumSamples	Required number of samples 
	 * @return int32		Number of retrieved samples
	 */
	virtual int32 OnGeneratePCMAudio(TArray<uint8>& OutAudio, int32 NumSamples) override;

	/**
	 * Gets the format of the retrieved PCM data
	 * @return Audio format type
	 */
	virtual Audio::EAudioMixerStreamDataFormat::Type GetGeneratedPCMDataFormat() const override;

	/**
	 * Overrides SoundWave OnBeginGenerate Event
	 */
	virtual void OnBeginGenerate() override;

	/**
	 * Overrides SoundWave OnEndGenerate Event
	 */
	virtual void OnEndGenerate() override;

	/**
	 * Overrides SoundBase IsPlayable Event
	 * @return bool		Is valid to be played
	 */
	virtual bool IsPlayable() const override;

	/**
	* Initializes the soundwave object with the controller class
	* @param ParentSource	This class will be used by the Core to sync the analysis
	*/
	void InitializeSoundWave(AudioAnalyzerExtractor* ParentSource);

	/**
	* Detach parent audio extractor class
	*/
	void InvalidateSoundWave();

	/**
	* The soundwave has a valid parent audio extractor class with audio data
	* @return bool		Has a valid audio analyzer extractor pointer
	*/
	bool HasParent() const;

	/**
	* The sound is used in the audio component of the parent audio extractor class
	* @return bool		Has an audio component linked to the audio extractor
	*/
	bool HasAudioComponent() const;

private: 

	/**
	* Parent class pointer to use the source mutex and audio info
	*/
	AudioAnalyzerExtractor* _parentSource;

	/**
	* Current pointer to the buffer audio raw data
	*/
	uint8*					_audioBufferPtr;

	/**
	* Buffer audio data size
	*/
	int32					_audioBufferSize;

	/**
	*	Current Loop
	*/
	int						_remainingLoops;

	/**
	*	Start time for loops
	*/
	float					_startTime;

	/**
	* Variable used to store the pause state
	*/
	std::atomic<bool>		_isPlaying;

	/**
	* Variable used to store the pause state
	*/
	std::atomic<bool>		_isPaused;

	/**
	* Variable used to force stop OnGeneratePCMAudio
	*/
	std::atomic<bool>		_isPlayable;

	/**
	* Execute an AsyncTask to trigger the SoundWaveEvent
	* @param[in,out] SoundWaveEvent		Event to trigger
	*/
	static void EventPlaybackBroadcast(FPlaybackDelegateSoundWave& SoundWaveEvent);

	/**
	* Stop SoundWave Playback
	*/
	void StopSound();

};
