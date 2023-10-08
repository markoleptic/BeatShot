// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "AbilitySystem/ExecutionCalculations/BSDamageExecCalc.h"
#include "BeatShot/BSGameplayTags.h"
#include "AbilitySystem/Globals/BSAttributeSetBase.h"

// Declare the attributes to capture and define how we want to capture them from the Source and Target.
struct BSDamageStatics
{
	FGameplayEffectAttributeCaptureDefinition HitDamageDef;
	FGameplayEffectAttributeCaptureDefinition TrackingDamageDef;
	FGameplayEffectAttributeCaptureDefinition TotalDamageDef;
	//FGameplayEffectAttributeCaptureDefinition HealthDef;

	BSDamageStatics()
	{
		HitDamageDef = FGameplayEffectAttributeCaptureDefinition(UBSAttributeSetBase::GetHitDamageAttribute(),
			EGameplayEffectAttributeCaptureSource::Source, true);
		TrackingDamageDef = FGameplayEffectAttributeCaptureDefinition(UBSAttributeSetBase::GetTrackingDamageAttribute(),
			EGameplayEffectAttributeCaptureSource::Source, true);
		TotalDamageDef = FGameplayEffectAttributeCaptureDefinition(UBSAttributeSetBase::GetTotalDamageAttribute(),
			EGameplayEffectAttributeCaptureSource::Target, false);

		// Capture the Target's Health. Don't snapshot.
		//HealthDef = FGameplayEffectAttributeCaptureDefinition(UBSAttributeSetBase::GetHealthAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
	}
};

static const BSDamageStatics& DamageStatics()
{
	static BSDamageStatics DStatics;
	return DStatics;
}

UBSDamageExecCalc::UBSDamageExecCalc()
{
	RelevantAttributesToCapture.Add(DamageStatics().HitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().TrackingDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().TotalDamageDef);
	//RelevantAttributesToCapture.Add(DamageStatics().HealthDef);
}

void UBSDamageExecCalc::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// Gather the tags from the source and target
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float HitDamage = 0.0f;
	if (!TargetTags->HasTag(FBSGameplayTags::Get().Target_State_Immune_HitDamage))
	{
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().HitDamageDef, EvaluationParameters,
			HitDamage);
	}

	float TrackingDamage = 0.0f;
	if (!TargetTags->HasTag(FBSGameplayTags::Get().Target_State_Immune_TrackingDamage))
	{
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().TrackingDamageDef,
			EvaluationParameters, TrackingDamage);
	}

	const float TotalDamage = HitDamage + TrackingDamage;

	if (TotalDamage > 0.0f)
	{
		// Set the Target's total damage meta attribute, this gets turned into - health on the target
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(UBSAttributeSetBase::GetTotalDamageAttribute(), EGameplayModOp::Additive,
				TotalDamage));
		//OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UBSAttributeSetBase::GetHealthAttribute(), EGameplayModOp::Additive, -TotalDamage));
	}
}
