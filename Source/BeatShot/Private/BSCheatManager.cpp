#include "BSCheatManager.h"
#include "AbilitySystemComponent.h"
#include "BSCharacter.h"
#include "BSGameMode.h"
#include "BSPlayerController.h"
#include "BeatShot/BSGameplayTags.h"
#include "GameplayAbility/BSAbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"

void UBSCheatManager::InitCheatManager()
{
	ReceiveInitCheatManager();
}

void UBSCheatManager::SetAimBotEnabled(const bool bEnable) const
{
	ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	ABSCharacter* Character = Cast<ABSCharacter>(Cast<ABSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->GetPawn());
	
	if (!GameMode || !Character)
	{
		return;
	}
	if (bEnable)
	{
		if (!GameMode->OnTargetSpawned.IsBoundToObject(Character))
		{
			GameMode->OnTargetSpawned.AddUFunction(Character, FName("OnTargetSpawned_AimBot"));
		}
		UBSGameplayAbility* AbilityCDO = AimBotAbility->GetDefaultObject<UBSGameplayAbility>();
		const FGameplayAbilitySpec AbilitySpec(AbilityCDO, 1);
		Character->GetBSAbilitySystemComponent()->GiveAbility(AbilitySpec);
	}
	else
	{
		if (GameMode->OnTargetSpawned.IsBoundToObject(Character))
		{
			GameMode->OnTargetSpawned.RemoveAll(Character);
		}
		FGameplayTagContainer Container;
		TArray<FGameplayAbilitySpec*> Activatable;
		Container.AddTag(FBSGameplayTags::Get().Cheat_AimBot);
		Character->GetBSAbilitySystemComponent()->GetActivatableGameplayAbilitySpecsByAllMatchingTags(Container,Activatable);
		if (!Activatable.IsEmpty())
		{
			for (const FGameplayAbilitySpec* Spec : Activatable)
			{
				Character->GetBSAbilitySystemComponent()->ClearAbility(Spec->Handle);
			}
		}
	}
}
