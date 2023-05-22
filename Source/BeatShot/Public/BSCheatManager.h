#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "BSCheatManager.generated.h"

class UGameplayAbility;

/** Base CheatManager for this game */
UCLASS(Blueprintable)
class BEATSHOT_API UBSCheatManager : public UCheatManager
{
	GENERATED_BODY()

protected:
	virtual void InitCheatManager() override;

	UFUNCTION(exec)
	void ToggleAimBot(const bool bEnable);

	UFUNCTION(exec)
	void ToggleRLAgentWidget(const bool bShow) const;

	UFUNCTION(exec)
	void ToggleSpawnMemory(const bool bShow) const;

	UFUNCTION(exec)
	void ToggleSpawnBox(const bool bShow) const;

	UFUNCTION(exec)
	void ToggleAllTargetSpawnerDebug(const bool bShow) const;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "BeatShot|Abilities")
	TSubclassOf<UGameplayAbility> AimBotAbility;

	bool bAimBotEnabled = false;
};
