// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Player/BSCheatManager.h"
#include "AbilitySystemComponent.h"
#include "Character/BSCharacter.h"
#include "BSGameMode.h"
#include "Player/BSPlayerController.h"
#include "Target/TargetManager.h"
#include "AbilitySystem/BSAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/BSGA_AimBot.h"
#include "Kismet/GameplayStatics.h"
#include "RangeActors/TimeOfDayManager.h"

namespace BeatShotConsoleVariables
{
	static TAutoConsoleVariable CVarShowDebugSpawnBox(TEXT("bs.showdebug.targetmanager.spawnbox"), 0, TEXT("Draw debug boxes for the spawn box areas. orange: full size, green: open, red: blocked"));
	static TAutoConsoleVariable CVarEnableAimBot(TEXT("bs.cheat.aimbot"), FString(), TEXT("Enable Aim Bot"));
	static TAutoConsoleVariable CVarShowDebugReinforcementLearningWidget(TEXT("bs.showdebug.targetmanager.rl"), 0, TEXT("Show the reinforcement learning widget"));
	static TAutoConsoleVariable CVarShowDebugSpawnMemory(TEXT("bs.showdebug.targetmanager.spawnmemory"), 0, TEXT("Show the recent target locations that are being tracked"));
	static TAutoConsoleVariable CVarShowDebugAllTargetManager(TEXT("bs.showdebug.targetmanager"), 0, TEXT("Show all target manager debug"));
	static TAutoConsoleVariable CVarShowDebugOverlappingVertices(TEXT("bs.showdebug.overlap"), 0, TEXT("Show overlapping vertices"));
	static TAutoConsoleVariable CVarShowSpotLightFront(TEXT("bs.showspotlightfront"), 0, TEXT("Show SpotLight Front"));
	static TAutoConsoleVariable CVarSetNightMode(TEXT("bs.setnightmode"), 0, TEXT("Set the time of day to night"));
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

	FConsoleVariableDelegate CVarShowSpotLightFrontDelegate;
	CVarShowSpotLightFrontDelegate.BindUObject(this, &UBSCheatManager::CVarOnChanged_ShowSpotLightFront);
	BeatShotConsoleVariables::CVarShowSpotLightFront.AsVariable()->SetOnChangedCallback(CVarShowSpotLightFrontDelegate);

	FConsoleVariableDelegate CVarSetNightModeDelegate;
	CVarSetNightModeDelegate.BindUObject(this, &UBSCheatManager::CVarOnChanged_SetNightMode);
	BeatShotConsoleVariables::CVarSetNightMode.AsVariable()->SetOnChangedCallback(CVarSetNightModeDelegate);
}

void UBSCheatManager::CVarOnChanged_EnableAimBot(IConsoleVariable* Variable)
{
	const ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	const ABSCharacter* Character = Cast<ABSCharacter>(Cast<ABSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->GetPawn());

	if (!GameMode || !Character)
	{
		return;
	}
	
	UBSAbilitySystemComponent* ASC = Character->GetBSAbilitySystemComponent();
	if (!ASC) return;
	
	const FString StringVariable = Variable->GetString();

	if (StringVariable.Equals("0"))
	{
		if (const FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromHandle(AimBotSpecHandle))
		{
			ASC->ClearAbility(Spec->Handle);
		}
		
		UE_LOG(LogTemp, Display, TEXT("AimBot deactivated."));
		return;
	}
	
	const FGameplayAbilitySpec AbilitySpec(AimBotAbility, 1);
	AimBotSpecHandle = ASC->GiveAbility(AbilitySpec);
	if (!AimBotSpecHandle.IsValid()) return;
		
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromHandle(AimBotSpecHandle);
	if (!Spec) return;
		
	UGameplayAbility* Ability = Spec->GetPrimaryInstance();
	if (!Ability) return;
		
	UBSGA_AimBot* AbilityInstance = Cast<UBSGA_AimBot>(Ability);
	if (!AbilityInstance) return;
		
	GameMode->GetTargetManager()->OnTargetActivated_AimBot.AddUObject(AbilityInstance, &UBSGA_AimBot::OnTargetActivated);

	FVector IgnoreStartLocation = FVector::ZeroVector;
	if (StringVariable.Contains("X=") || StringVariable.Contains("Y=") || StringVariable.Contains("Z="))
	{
		IgnoreStartLocation.InitFromCompactString(StringVariable);
	}
	AbilityInstance->SetIgnoreStartLocation(IgnoreStartLocation);
	ASC->MarkAbilitySpecDirty(*Spec);
		
	UE_LOG(LogTemp, Display, TEXT("AimBot activated."));
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

void UBSCheatManager::CVarOnChanged_ShowSpotLightFront(IConsoleVariable* Variable)
{
	UBSGameInstance* GI = Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GI->TimeOfDayManager)
	{
		GI->TimeOfDayManager->SetSpotLightFrontEnabledState(Variable->GetBool());
	}
}

void UBSCheatManager::CVarOnChanged_SetNightMode(IConsoleVariable* Variable)
{
	UBSGameInstance* GI = Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GI->TimeOfDayManager)
	{
		if (Variable->GetBool() == true)
		{
			GI->TimeOfDayManager->SetTimeOfDay(ETimeOfDay::Night);
		}
		else if (Variable->GetBool() == false)
		{
			GI->TimeOfDayManager->SetTimeOfDay(ETimeOfDay::Day);
		}
	}
}
