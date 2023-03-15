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
	void SetAimBotEnabled(const bool bEnable) const;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "BeatShot|Abilities")
	TSubclassOf<UGameplayAbility> AimBotAbility;
};
