// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "AbilitySystem/BSDamageExecCalc.h"
#include "BeatShot/BSGameplayTags.h"
#include "AbilitySystem/BSAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/BSAttributeSetBase.h"

// Declare the attributes to capture and define how we want to capture them from the Source and Target.
struct BSDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);

	BSDamageStatics()
	{
		// Snapshot happens at time of GESpec creation

		// We're not capturing anything from the Source in this example, but there could be like AttackPower attributes that you might want.

		// Capture optional Damage set on the damage GE as a CalculationModifier under the ExecutionCalculation
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBSAttributeSetBase, Damage, Source, true);

		// Capture the Target's Health. Don't snapshot.
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBSAttributeSetBase, Health, Target, false);
	}
};

static const BSDamageStatics& DamageStatics()
{
	static BSDamageStatics DStatics;
	return DStatics;
}

UBSDamageExecCalc::UBSDamageExecCalc()
{
	RelevantAttributesToCapture.Add(DamageStatics().DamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().HealthDef);
}

void UBSDamageExecCalc::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

	AActor* SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->GetAvatarActor() : nullptr;
	AActor* TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// Gather the tags from the source and target as that can affect which buffs should be used
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float Health = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().HealthDef, EvaluationParameters, Health);
	Health = FMath::Max<float>(Health, 0.0f);

	float Damage = 0.0f;
	// Capture optional damage value set on the damage GE as a CalculationModifier under the ExecutionCalculation
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageDef, EvaluationParameters, Damage);
	// Add SetByCaller damage if it exists
	Damage += FMath::Max<float>(Spec.GetSetByCallerMagnitude(FBSGameplayTags::Get().Data_Damage, true, -1.0f), 0.0f);

	float UnmitigatedDamage = Damage; // Can multiply any damage boosters here
	
	float MitigatedDamage = UnmitigatedDamage;

	if (MitigatedDamage > 0.f)
	{
		// Set the Target's damage meta attribute
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().DamageProperty, EGameplayModOp::Additive, MitigatedDamage));
	}

	// Broadcast damages to Target ASC
	UBSAbilitySystemComponent* TargetASC = Cast<UBSAbilitySystemComponent>(TargetAbilitySystemComponent);
	if (TargetASC)
	{
		UBSAbilitySystemComponent* SourceASC = Cast<UBSAbilitySystemComponent>(SourceAbilitySystemComponent);
		TargetASC->ReceiveDamage(SourceASC, UnmitigatedDamage, MitigatedDamage);
	}
}