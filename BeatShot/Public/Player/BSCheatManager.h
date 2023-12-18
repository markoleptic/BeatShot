#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "HAL/IConsoleManager.h"
#include "GameFramework/CheatManager.h"
#include "BSCheatManager.generated.h"

class UGameplayAbility;

/** Base CheatManager for this game */
UCLASS(Blueprintable)
class BEATSHOT_API UBSCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	virtual void InitCheatManager() override;

	void CVarOnChanged_Cheat_AimBot(IConsoleVariable* Variable);

	void CVarOnChanged_ClearDebug(IConsoleVariable* Variable);

	void CVarOnChanged_PrintDebug_NumRecentNumActive(IConsoleVariable* Variable);
	void CVarOnChanged_PrintDebug_ChooseBestActionIndex(IConsoleVariable* Variable);
	void CVarOnChanged_PrintDebug_GetMaxIndex(IConsoleVariable* Variable);
	void CVarOnChanged_PrintDebug_Grid(IConsoleVariable* Variable);
	void CVarOnChanged_PrintDebug_QTableUpdate(IConsoleVariable* Variable);
	
	void CVarOnChanged_ShowDebug_TargetManager(IConsoleVariable* Variable);
	void CVarOnChanged_ShowDebug_SpawnBox(IConsoleVariable* Variable);
	void CVarOnChanged_ShowDebug_DirectionalBoxes(IConsoleVariable* Variable);
	void CVarOnChanged_ShowDebug_ReinforcementLearningWidget(IConsoleVariable* Variable);
	
	void CVarOnChanged_ShowDebug_AllSpawnAreas(IConsoleVariable* Variable);
	void CVarOnChanged_ShowDebug_ValidSpawnAreas(IConsoleVariable* Variable);
	void CVarOnChanged_ShowDebug_RemovedSpawnAreas(IConsoleVariable* Variable);
	void CVarOnChanged_ShowDebug_FilteredRecent(IConsoleVariable* Variable);
	void CVarOnChanged_ShowDebug_FilteredActivated(IConsoleVariable* Variable);
	void CVarOnChanged_ShowDebug_FilteredManaged(IConsoleVariable* Variable);
	void CVarOnChanged_ShowDebug_FilteredBordering(IConsoleVariable* Variable);
	
	void CVarOnChanged_ShowDebug_OverlappingVertices_Dynamic(IConsoleVariable* Variable);
	void CVarOnChanged_ShowDebug_OverlappingVertices_OnFlaggedManaged(IConsoleVariable* Variable);
	
	void CVarOnChanged_ShowDebug_SpotLightFront(IConsoleVariable* Variable);

	void CVarOnChanged_SetTimeOfDay(IConsoleVariable* Variable);

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "BeatShot|Abilities")
	TSubclassOf<UGameplayAbility> AimBotAbility;

	UPROPERTY()
	FGameplayAbilitySpecHandle AimBotSpecHandle;
};
