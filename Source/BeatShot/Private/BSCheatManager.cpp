#include "BSCheatManager.h"
#include "AbilitySystemComponent.h"
#include "BSCharacter.h"
#include "BSGameMode.h"
#include "BSPlayerController.h"
#include "TargetSpawner.h"
#include "BeatShot/BSGameplayTags.h"
#include "GameplayAbility/BSAbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"

void UBSCheatManager::InitCheatManager()
{
	ReceiveInitCheatManager();
}

void UBSCheatManager::ToggleAimBot(const bool bEnable)
{
	const ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	ABSCharacter* Character = Cast<ABSCharacter>(Cast<ABSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->GetPawn());
	
	if (!GameMode || !Character)
	{
		return;
	}
	if (bEnable)
	{
		if (!GameMode->GetTargetSpawner()->OnTargetSpawned_AimBot.IsBoundToObject(Character))
		{
			GameMode->GetTargetSpawner()->OnTargetSpawned_AimBot.AddUObject(Character, &ABSCharacter::OnTargetSpawned_AimBot);
		}
		UBSGameplayAbility* AbilityCDO = AimBotAbility->GetDefaultObject<UBSGameplayAbility>();
		if (const FGameplayAbilitySpec AbilitySpec(AbilityCDO, 1); Character->GetBSAbilitySystemComponent()->GiveAbility(AbilitySpec).IsValid())
		{
			UE_LOG(LogTemp, Display, TEXT("AimBot activated."));
		}
	}
	else
	{
		if (GameMode->GetTargetSpawner()->OnTargetSpawned_AimBot.IsBoundToObject(Character))
		{
			GameMode->GetTargetSpawner()->OnTargetSpawned_AimBot.RemoveAll(Character);
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
				UE_LOG(LogTemp, Display, TEXT("AimBot deactivated."));
			}
		}
	}
}

void UBSCheatManager::ToggleRLAgentWidget(const bool bShow) const
{
	Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->GetTargetSpawner()->ToggleRLAgentWidget(bShow);
}

void UBSCheatManager::ToggleSpawnMemory(const bool bShow) const
{
	Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->GetTargetSpawner()->ToggleDebug_SpawnMemory(bShow);
}

void UBSCheatManager::ToggleSpawnBox(const bool bShow) const
{
	Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->GetTargetSpawner()->ToggleDebug_SpawnBox(bShow);
}

void UBSCheatManager::ToggleAllTargetSpawnerDebug(const bool bShow) const
{
	const ATargetSpawner* TargetSpawner = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->GetTargetSpawner();
	const bool bBoxActive = TargetSpawner->IsDebug_SpawnBoxActive();
	const bool bWidgetActive = TargetSpawner->IsDebug_RLAgentWidgetActive();
	const bool bSpawnMemoryActive = TargetSpawner->IsDebug_SpawnMemoryActive();
	if (!bBoxActive || !bWidgetActive || !bSpawnMemoryActive)
	{
		if (!bBoxActive)
		{
			ToggleSpawnBox(bShow);
		}
		if (!bWidgetActive)
		{
			ToggleRLAgentWidget(bShow);
		}
		if (!bSpawnMemoryActive)
		{
			ToggleSpawnMemory(bShow);
		}
	}
	else
	{
		if (bBoxActive)
		{
			ToggleSpawnBox(bShow);
		}
		if (bWidgetActive)
		{
			ToggleRLAgentWidget(bShow);
		}
		if (bSpawnMemoryActive)
		{
			ToggleSpawnMemory(bShow);
		}
	}
}
