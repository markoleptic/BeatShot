#pragma once

#include "SaveGameCustomGameMode.h"
#include "SaveGamePlayerScore.h"
#include "SaveGamePlayerSettings.h"
#include "SaveLoadInterface.generated.h"

/** Broadcast when Player Settings are changed and saved */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerSettingsChange, const FPlayerSettings&, RefreshedPlayerSettings);

/** Broadcast when AudioAnalyzer settings are changed and saved */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAASettingsChange, const FAASettingsStruct&, RefreshedAASettings);

UINTERFACE()
class USaveLoadInterface : public UInterface
{
	GENERATED_BODY()
};

class GLOBAL_API ISaveLoadInterface
{    
	GENERATED_BODY()

public:
	
	virtual FAASettingsStruct LoadAASettings() const;
	
	virtual void SaveAASettings(const FAASettingsStruct AASettingsToSave);
	
	virtual FPlayerSettings LoadPlayerSettings() const;
	
	virtual void SavePlayerSettings(const FPlayerSettings PlayerSettingsToSave);
	
	TArray<FGameModeActorStruct> LoadCustomGameModes() const;

	void SaveCustomGameMode(const TArray<FGameModeActorStruct> GameModeArrayToSave);
	
	TArray<FPlayerScore> LoadPlayerScores() const;
	
	void SavePlayerScores(const TArray<FPlayerScore> PlayerScoreArrayToSave);
};