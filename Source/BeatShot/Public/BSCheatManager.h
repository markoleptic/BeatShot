#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "BSCheatManager.generated.h"

class UGameplayAbility;

UCLASS(Blueprintable)
class BEATSHOT_API UBSCheatManager : public UCheatManager
{
	GENERATED_BODY()

protected:
	virtual void InitCheatManager() override;

	UFUNCTION(exec)
	void ToggleAimBot();

	UFUNCTION(exec)
	void ToggleRLAgentWidget() const;

	UFUNCTION(exec)
	void ToggleSpawnMemory() const;

	UFUNCTION(exec)
	void ToggleSpawnBox() const;

	UFUNCTION(exec)
	void ToggleAllTargetSpawnerDebug() const;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "BeatShot|Abilities")
	TSubclassOf<UGameplayAbility> AimBotAbility;

	bool bAimBotEnabled = false;
};
