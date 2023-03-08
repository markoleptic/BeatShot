// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "GameplayAbility/BSGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "BSCharacter.h"
#include "BSPlayerController.h"
#include "GameplayAbility/BSAbilitySystemComponent.h"

#define ENSURE_ABILITY_IS_INSTANTIATED_OR_RETURN(FunctionName, ReturnValue)																				\
{																																						\
	if (!ensure(IsInstantiated()))																														\
	{																																					\
		ABILITY_LOG(Error, TEXT("%s: " #FunctionName " cannot be called on a non-instanced ability. Check the instancing policy."), *GetPathName());	\
		return ReturnValue;																																\
	}																																					\
}

UBSGameplayAbility::UBSGameplayAbility()
{
	// Default to Instance Per Actor
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ActivationPolicy = EBSAbilityActivationPolicy::OnInputTriggered;
	ActivationGroup = EBSAbilityActivationGroup::Independent;
}

UBSAbilitySystemComponent* UBSGameplayAbility::GetBSAbilitySystemComponentFromActorInfo() const
{
	return (CurrentActorInfo
		        ? Cast<UBSAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get())
		        : nullptr);
}

ABSPlayerController* UBSGameplayAbility::GetBSPlayerControllerFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<ABSPlayerController>(CurrentActorInfo->PlayerController.Get()) : nullptr);
}

AController* UBSGameplayAbility::GetControllerFromActorInfo() const
{
	if (CurrentActorInfo)
	{
		if (AController* PC = CurrentActorInfo->PlayerController.Get())
		{
			return PC;
		}

		// Look for a player controller or pawn in the owner chain.
		AActor* TestActor = CurrentActorInfo->OwnerActor.Get();
		while (TestActor)
		{
			if (AController* C = Cast<AController>(TestActor))
			{
				return C;
			}

			if (APawn* Pawn = Cast<APawn>(TestActor))
			{
				return Pawn->GetController();
			}

			TestActor = TestActor->GetOwner();
		}
	}

	return nullptr;
}

ABSCharacter* UBSGameplayAbility::GetBSCharacterFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<ABSCharacter>(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}

void UBSGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (ActivateAbilityOnGranted)
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}
