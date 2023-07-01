// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Player/BSCheatManager.h"
#include "AbilitySystemComponent.h"
#include "Character/BSCharacter.h"
#include "BSGameMode.h"
#include "Player/BSPlayerController.h"
#include "Target/TargetManager.h"
#include "BeatShot/BSGameplayTags.h"
#include "AbilitySystem/BSAbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"

namespace BeatShotConsoleVariables
{
	static TAutoConsoleVariable CVarShowDebugSpawnBox(TEXT("bs.showdebug.targetmanager.spawnbox"), 0, TEXT("Draw debug boxes for the spawn box areas. orange: full size, green: open, red: blocked"));
	static TAutoConsoleVariable CVarEnableAimBot(TEXT("bs.cheat.aimbot"), 0, TEXT("Enable Aim Bot"));
	static TAutoConsoleVariable CVarShowDebugReinforcementLearningWidget(TEXT("bs.showdebug.targetmanager.rl"), 0, TEXT("Show the reinforcement learning widget"));
	static TAutoConsoleVariable CVarShowDebugSpawnMemory(TEXT("bs.showdebug.targetmanager.spawnmemory"), 0, TEXT("Show the recent target locations that are being tracked"));
	static TAutoConsoleVariable CVarShowDebugAllTargetManager(TEXT("bs.showdebug.targetmanager"), 0, TEXT("Show all target manager debug"));
	static TAutoConsoleVariable CVarShowDebugOverlappingVertices(TEXT("bs.showdebug.overlap"), 0, TEXT("Show overlapping vertices"));
}

void UBSCheatManager::InitCheatManager()
{
	ReceiveInitCheatManager();

	FConsoleVariableDelegate CVarShowDebugSpawnBoxDelegate;
	CVarShowDebugSpawnBoxDelegate.BindUObject(this, &UBSCheatManager::CVarOnChanged_ShowDebugSpawnBox);
	BeatShotConsoleVariables::CVarShowDebugSpawnBox.AsVariable()->SetOnChangedCallback(CVarShowDebugSpawnBoxDelegate);
	
	FConsoleVariableDelegate CVarEnableAimBotDelegate;
	CVarEnableAimBotDelegate.BindUObject(this, &UBSCheatManager::CVarOnChanged_EnableAimBot);
	BeatShotConsoleVariables::CVarEnableAimBot.AsVariable()->SetOnChangedCallback(CVarEnableAimBotDelegate);

	FConsoleVariableDelegate CVarShowDebugReinforcementLearningWidgetDelegate;
	CVarShowDebugReinforcementLearningWidgetDelegate.BindUObject(this, &UBSCheatManager::CVarOnChanged_ShowDebugReinforcementLearningWidget);
	BeatShotConsoleVariables::CVarShowDebugReinforcementLearningWidget.AsVariable()->SetOnChangedCallback(CVarShowDebugReinforcementLearningWidgetDelegate);

	FConsoleVariableDelegate CVarShowDebugSpawnMemoryDelegate;
	CVarShowDebugSpawnMemoryDelegate.BindUObject(this, &UBSCheatManager::CVarOnChanged_ShowDebugSpawnMemory);
	BeatShotConsoleVariables::CVarShowDebugSpawnMemory.AsVariable()->SetOnChangedCallback(CVarShowDebugSpawnMemoryDelegate);

	FConsoleVariableDelegate CVarShowDebugAllTargetManagerDelegate;
	CVarShowDebugAllTargetManagerDelegate.BindUObject(this, &UBSCheatManager::CVarOnChanged_ShowDebugAllTargetManager);
	BeatShotConsoleVariables::CVarShowDebugAllTargetManager.AsVariable()->SetOnChangedCallback(CVarShowDebugAllTargetManagerDelegate);

	FConsoleVariableDelegate CVarShowDebugOverlappingVerticesDelegate;
	CVarShowDebugOverlappingVerticesDelegate.BindUObject(this, &UBSCheatManager::CVarOnChanged_ShowDebugOverlappingVertices);
	BeatShotConsoleVariables::CVarShowDebugOverlappingVertices.AsVariable()->SetOnChangedCallback(CVarShowDebugOverlappingVerticesDelegate);
}

void UBSCheatManager::CVarOnChanged_EnableAimBot(IConsoleVariable* Variable)
{
	const ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	ABSCharacter* Character = Cast<ABSCharacter>(Cast<ABSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->GetPawn());
	
	if (!GameMode || !Character)
	{
		return;
	}

	if (Variable->GetBool())
	{
		if (!GameMode->GetTargetManager()->OnTargetActivated_AimBot.IsBoundToObject(Character))
		{
			GameMode->GetTargetManager()->OnTargetActivated_AimBot.AddUObject(Character, &ABSCharacter::OnTargetSpawned_AimBot);
		}
		UBSGameplayAbility* AbilityCDO = AimBotAbility->GetDefaultObject<UBSGameplayAbility>();
		if (const FGameplayAbilitySpec AbilitySpec(AbilityCDO, 1); Character->GetBSAbilitySystemComponent()->GiveAbility(AbilitySpec).IsValid())
		{
			UE_LOG(LogTemp, Display, TEXT("AimBot activated."));
		}
	}
	else
	{
		if (GameMode->GetTargetManager()->OnTargetActivated_AimBot.IsBoundToObject(Character))
		{
			GameMode->GetTargetManager()->OnTargetActivated_AimBot.RemoveAll(Character);
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

void UBSCheatManager::CVarOnChanged_ShowDebugReinforcementLearningWidget(IConsoleVariable* Variable)
{
	Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->GetTargetManager()->ShowDebug_ReinforcementLearningWidget(Variable->GetBool());
}

void UBSCheatManager::CVarOnChanged_ShowDebugSpawnMemory(IConsoleVariable* Variable)
{
	Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->GetTargetManager()->ShowDebug_SpawnMemory(Variable->GetBool());
}

void UBSCheatManager::CVarOnChanged_ShowDebugSpawnBox(IConsoleVariable* Variable)
{
	Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->GetTargetManager()->ShowDebug_SpawnBox(Variable->GetBool());
}

void UBSCheatManager::CVarOnChanged_ShowDebugAllTargetManager(IConsoleVariable* Variable)
{
	CVarOnChanged_ShowDebugSpawnBox(Variable);
	CVarOnChanged_ShowDebugReinforcementLearningWidget(Variable);
	CVarOnChanged_ShowDebugSpawnMemory(Variable);
}

void UBSCheatManager::CVarOnChanged_ShowDebugOverlappingVertices(IConsoleVariable* Variable)
{
	Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->GetTargetManager()->ShowDebug_OverlappingVertices(Variable->GetBool());
}
