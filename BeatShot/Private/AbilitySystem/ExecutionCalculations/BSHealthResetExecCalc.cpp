// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "AbilitySystem/ExecutionCalculations/BSHealthResetExecCalc.h"
#include "AbilitySystem/Globals/BSAttributeSetBase.h"


// Declare the attributes to capture and define how we want to capture them from the Source and Target.
struct FBSHealthStatics
{
	FGameplayEffectAttributeCaptureDefinition HealthDef;
	FGameplayEffectAttributeCaptureDefinition MaxHealthDef;

	FBSHealthStatics()
	{
		HealthDef = FGameplayEffectAttributeCaptureDefinition(UBSAttributeSetBase::GetHealthAttribute(),
		                                                      EGameplayEffectAttributeCaptureSource::Target, false);
		MaxHealthDef = FGameplayEffectAttributeCaptureDefinition(UBSAttributeSetBase::GetMaxHealthAttribute(),
		                                                         EGameplayEffectAttributeCaptureSource::Target, false);
	}
};

static const FBSHealthStatics& HealthStatics()
{
	static FBSHealthStatics HStatics;
	return HStatics;
}

UBSHealthResetExecCalc::UBSHealthResetExecCalc()
{
	RelevantAttributesToCapture.Add(HealthStatics().HealthDef);
	RelevantAttributesToCapture.Add(HealthStatics().MaxHealthDef);
}

void UBSHealthResetExecCalc::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// Gather the tags from the source and target
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float CurrentHealth = 0.f;
	float NewHealth = 0.f;

	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealthStatics().HealthDef, EvaluationParameters,
	                                                           CurrentHealth);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealthStatics().MaxHealthDef, EvaluationParameters,
	                                                           NewHealth);

	if (NewHealth > CurrentHealth)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UBSAttributeSetBase::GetHealthAttribute(),
		                                                                    EGameplayModOp::Additive,
		                                                                    NewHealth - CurrentHealth));
	}
}
