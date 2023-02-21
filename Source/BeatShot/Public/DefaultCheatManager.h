#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "DefaultCheatManager.generated.h"

UCLASS(Blueprintable)
class BEATSHOT_API UDefaultCheatManager : public UCheatManager
{
	GENERATED_BODY()

protected:

	virtual void InitCheatManager() override;

	UFUNCTION(exec)
	void SetAimBotEnabled(const bool bEnable);
};
