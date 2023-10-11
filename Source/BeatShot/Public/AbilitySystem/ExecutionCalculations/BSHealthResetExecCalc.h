// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "BSHealthResetExecCalc.generated.h"


UCLASS()
class BEATSHOT_API UBSHealthResetExecCalc : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UBSHealthResetExecCalc();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
