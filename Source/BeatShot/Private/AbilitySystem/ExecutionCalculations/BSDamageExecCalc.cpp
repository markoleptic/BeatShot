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
	
	BSDamageStatics()
	{
		// Capture HitDamage set on the damage GE as a CalculationModifier under the ExecutionCalculation
		HitDamageDef = FGameplayEffectAttributeCaptureDefinition(UBSAttributeSetBase::GetHitDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		TrackingDamageDef = FGameplayEffectAttributeCaptureDefinition(UBSAttributeSetBase::GetTrackingDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		TotalDamageDef = FGameplayEffectAttributeCaptureDefinition(UBSAttributeSetBase::GetTotalDamageAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
		
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
}

void UBSDamageExecCalc::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	//UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	//UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

	//AActor* SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->GetAvatarActor() : nullptr;
	//AActor* TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// Gather the tags from the source and target as that can affect which buffs should be used
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	/*float Health = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().HealthDef, EvaluationParameters, Health);
	Health = FMath::Max<float>(Health, 0.0f);*/

	float HitDamage = 0.0f;
	if (!TargetTags->HasTag(FBSGameplayTags::Get().Target_State_Immune_FireGun))
	{
		// Capture  damage value set on the damage GE as a CalculationModifier under the ExecutionCalculation
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().HitDamageDef, EvaluationParameters, HitDamage);
	}
	
	float TrackingDamage = 0.0f;
	if (!TargetTags->HasTag(FBSGameplayTags::Get().Target_State_Immune_Tracking))
	{
		// Capture  damage value set on the damage GE as a CalculationModifier under the ExecutionCalculation
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().TrackingDamageDef, EvaluationParameters, TrackingDamage);
	}
	
	UE_LOG(LogTemp, Display, TEXT("HitDamage captured: %f TrackingDamage captured: %f"), HitDamage, TrackingDamage);

	const float TotalDamage = HitDamage + TrackingDamage;
	
	// Add SetByCaller damage if it exists
	//TotalDamage += FMath::Max<float>(Spec.GetSetByCallerMagnitude(FBSGameplayTags::Get().Data_Damage, true, -1.0f), 0.0f);

	if (TotalDamage > 0.0f )
	{
		// Set the Target's total damage meta attribute, this gets turned into - health on the target
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UBSAttributeSetBase::GetTotalDamageAttribute(), EGameplayModOp::Additive, TotalDamage));
	}
}