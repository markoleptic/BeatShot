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

	void CVarOnChanged_PrintDebug_SpawnAreaStateInfo(IConsoleVariable* Variable);
	void CVarOnChanged_PrintDebug_ChooseBestActionIndex(IConsoleVariable* Variable);
	void CVarOnChanged_PrintDebug_GetMaxIndex(IConsoleVariable* Variable);
	void CVarOnChanged_PrintDebug_Grid(IConsoleVariable* Variable);
	void CVarOnChanged_PrintDebug_QTableUpdate(IConsoleVariable* Variable);
	
	void CVarOnChanged_ShowDebug_TargetManager(IConsoleVariable* Variable);
	void CVarOnChanged_ShowDebug_SpawnBox(IConsoleVariable* Variable);
	void CVarOnChanged_ShowDebug_DirectionalBoxes(IConsoleVariable* Variable);
	void CVarOnChanged_ShowDebug_ReinforcementLearningWidget(IConsoleVariable* Variable);
	void CVarOnChanged_ShowDebug_AllSpawnAreas(IConsoleVariable* Variable);
	void CVarOnChanged_ShowDebug_RemovedFromExtremaChange(IConsoleVariable* Variable);
	void CVarOnChanged_ShowDebug_SpawnableSpawnAreas(IConsoleVariable* Variable);
	void CVarOnChanged_ShowDebug_ActivatableSpawnAreas(IConsoleVariable* Variable);
	void CVarOnChanged_ShowDebug_FilteredBordering(IConsoleVariable* Variable);
	void CVarOnChanged_ShowDebug_Vertices_Dynamic(IConsoleVariable* Variable);
	void CVarOnChanged_ShowDebug_Vertices(IConsoleVariable* Variable);
	
	void CVarOnChanged_ShowDebug_SpotLightFront(IConsoleVariable* Variable);

	void CVarOnChanged_SetTimeOfDay(IConsoleVariable* Variable);

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "BeatShot|Abilities")
	TSubclassOf<UGameplayAbility> AimBotAbility;

	UPROPERTY()
	FGameplayAbilitySpecHandle AimBotSpecHandle;
};
