#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "BSCheatManager.generated.h"

UCLASS(Blueprintable)
class BEATSHOT_API UBSCheatManager : public UCheatManager
{
	GENERATED_BODY()

protected:
	virtual void InitCheatManager() override;

	UFUNCTION(exec)
	void SetAimBotEnabled(const bool bEnable) const;
};
