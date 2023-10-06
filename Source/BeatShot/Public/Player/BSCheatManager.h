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
	
	void CVarOnChanged_EnableAimBot(IConsoleVariable* Variable);
	
	void CVarOnChanged_ShowDebugReinforcementLearningWidget(IConsoleVariable* Variable);
	
	void CVarOnChanged_ShowDebugSpawnMemory(IConsoleVariable* Variable);

	void CVarOnChanged_ShowDebugAllSpawnAreas(IConsoleVariable* Variable);
	
	void CVarOnChanged_ShowDebugSpawnBox(IConsoleVariable* Variable);
	
	void CVarOnChanged_ShowDebugAllTargetManager(IConsoleVariable* Variable);

	void CVarOnChanged_ShowDebugOverlappingVertices(IConsoleVariable* Variable);

	void CVarOnChanged_ShowSpotLightFront(IConsoleVariable* Variable);

	void CVarOnChanged_SetNightMode(IConsoleVariable* Variable);

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "BeatShot|Abilities")
	TSubclassOf<UGameplayAbility> AimBotAbility;

	bool bAimBotEnabled = false;
	
	UPROPERTY()
	FGameplayAbilitySpecHandle AimBotSpecHandle;
};
