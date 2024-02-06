// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


// ReSharper disable CppMemberFunctionMayBeConst
// ReSharper disable CppParameterMayBeConstPtrOrRef
#include "Player/BSCheatManager.h"
#if !UE_BUILD_SHIPPING
#include "AbilitySystemComponent.h"
#include "BSGameInstance.h"
#include "Character/BSCharacterBase.h"
#include "BSGameMode.h"
#include "Player/BSPlayerController.h"
#include "Target/TargetManager.h"
#include "AbilitySystem/BSAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/BSGA_AimBot.h"
#include "Kismet/GameplayStatics.h"
#include "RangeActors/TimeOfDayManager.h"

namespace BeatShotCVars
{
	static TAutoConsoleVariable CVarCheat_AimBot(TEXT("bs_cheat.aimbot"), FString(),
		TEXT("Enable Aim Bot\n")
		TEXT("1: Enables for all locations\n")
		TEXT("0: Disables and removes from player\n")
		TEXT("X={} Y={} Z={}: Only destroy targets at locations that are >= the value if all positive,\n")
		TEXT("or <= if negative. If either Y or Z is left blank, only the other value is considered.\n"));

	
	static TAutoConsoleVariable CVarShowDebug(TEXT("bs_showdebug"),
		0,
		TEXT("Shows/clears all visual and print debug options."));
	

	static TAutoConsoleVariable CVarPrintDebug_SpawnAreaStateInfo(TEXT("bs_printdebug.spawnareastateinfo"),
		0,
		TEXT("Toggles printing the number managed, activated, and recent Spawn Areas."));


	static TAutoConsoleVariable CVarPrintDebug_ChooseBestActionIndex(TEXT("bs_printdebug.choosebestidx"),
		0,
		TEXT("Toggles printing info about the Reinforcement Learning Component choosing the Best Action Index."));


	static TAutoConsoleVariable CVarPrintDebug_GetMaxIndex(TEXT("bs_printdebug.getmaxindex"),
		0,
		TEXT("Toggles printing the Reinforcement Learning Component's intermediate values when finding the max "
		"action index."));

	
	static TAutoConsoleVariable CVarPrintDebug_ActiveTargetPairs(TEXT("bs_pringdebug.activetargetpairs"),
		0,
		TEXT("Toggles printing the target pairs that are added to the RL Component's ActiveTargetPairs."));

	
	static TAutoConsoleVariable CVarPrintDebug_QTableInit(TEXT("bs_pringdebug.qtableinit"),
		0,
		TEXT("Toggles printing initialization info about the QTable in the RL Component."));

	
	static TAutoConsoleVariable CVarPrintDebug_QTableUpdate(TEXT("bs_printdebug.qtableupdate"),
		0,
		TEXT("Toggles printing the old and new QTable value each time the Reinforcement Learning Component")
		TEXT("makes an update to the QTable."));


	static TAutoConsoleVariable CVarPrintDebug_Grid(TEXT("bs_printdebug.grid"),
		0,
		TEXT("Toggles printing various grid-distribution related info."));
	

	static TAutoConsoleVariable CVarShowDebug_ReinforcementLearningWidget(TEXT("bs_showdebug.rlwidget"),
		FString("avg"),
		TEXT("Toggles showing the reinforcement learning widget.\n")
		TEXT("\"avg\": Shows the average values across each QTable column (destination indices).\n")
		TEXT("\"max\": Shows the max values across each QTable column (destination indices).\n")
		TEXT("Can also use 0/1 and it will use max values."));


	static TAutoConsoleVariable CVarShowDebug_SpawnBox(TEXT("bs_showdebug.spawnbox"),
		0,
		TEXT("Draw debug boxes for the SpawnBox and the StaticExtentsBox."));

	
	static TAutoConsoleVariable CVarShowDebug_SpawnVolume(TEXT("bs_showdebug.spawnvolume"),
		0,
		TEXT("Draw debug box for the SpawnVolume."));


	static TAutoConsoleVariable CVarShowDebug_DirectionalBoxes(TEXT("bs_showdebug.dirboxes"),
		0,
		TEXT("Draw debug boxes for the 6 directional boxes.\n")
	    TEXT("X: Red\n")
	    TEXT("Y: Green\n")
	    TEXT("Z: Blue\n"));


	static TAutoConsoleVariable CVarShowDebug_AllSpawnAreas(TEXT("bs_showdebug.allspawnareas"),
		0,
		TEXT("Toggles showing debug boxes for all Spawn Areas."));


	static TAutoConsoleVariable CVarShowDebug_RemovedFromExtremaChange(TEXT("bs_showdebug.removedextrema"),
		0,
		TEXT("Toggles showing debug boxes for removed Spawn Areas due to BoxBounds."));

	
	static TAutoConsoleVariable CVarShowDebug_ValidInvalid(TEXT("bs_showdebug.validinvalid"),
		0,
		TEXT("Toggles showing debug boxes for all spawn areas removed due to overlap."));

	
	static TAutoConsoleVariable CVarShowDebug_SpawnableSpawnAreas(TEXT("bs_showdebug.spawnable"),
		0,
		TEXT("Toggles showing debug boxes for valid spawnable spawn areas."));

	
	static TAutoConsoleVariable CVarShowDebug_ActivatableSpawnAreas(TEXT("bs_showdebug.activatable"),
		0,
		TEXT("Toggles showing debug boxes for valid activatable spawn areas."));

	
	static TAutoConsoleVariable CVarShowDebug_ActivatedSpawnAreas(TEXT("bs_showdebug.activated"),
		0,
		TEXT("Toggles showing debug boxes for activated spawn areas."));

	
	static TAutoConsoleVariable CVarShowDebug_DeactivatedSpawnAreas(TEXT("bs_showdebug.deactivated"),
		0,
		TEXT("Toggles showing debug boxes for deactivated spawn areas."));

	
	static TAutoConsoleVariable CVarShowDebug_RecentSpawnAreas(TEXT("bs_showdebug.recent"),
		0,
		TEXT("Toggles showing debug boxes for recent spawn areas."));
	
	
	static TAutoConsoleVariable CVarShowDebug_NonAdjacent(TEXT("bs_showdebug.nonadjacent"),
		0,
		TEXT("Toggles debug boxes for spawn areas removed due to being non adjacent."));
	
	
	static TAutoConsoleVariable CVarShowDebug_Vertices(TEXT("bs_showdebug.verts"),
		0,
		TEXT("Toggles showing debug points and a debug sphere for overlapping vertices of activated spawn areas."));


	static TAutoConsoleVariable CVarShowDebug_FrontSpotLight(TEXT("bs_showdebug.frontspotlight"),
		0,
		TEXT("If night, shows toggles showing the front spotlight."));


	static TAutoConsoleVariable CVarSetTimeOfDay(TEXT("bs_settimeofday"),
		FString("day"),
		TEXT("Set the time of day to either \"day\" or \"night\"."));
}

using namespace BeatShotCVars;

void UBSCheatManager::InitCheatManager()
{
	ReceiveInitCheatManager();

	BIND_BS_CVAR(CVarCheat_AimBot, this, &UBSCheatManager::Cheat_AimBot);
	BIND_BS_CVAR(CVarShowDebug, this, &UBSCheatManager::ShowDebug);
	BIND_BS_CVAR(CVarShowDebug_ReinforcementLearningWidget, this, &UBSCheatManager::ShowDebug_ReinforcementLearningWidget);
	BIND_BS_CVAR(CVarShowDebug_SpawnBox, this, &UBSCheatManager::ShowDebug_SpawnBox);
	BIND_BS_CVAR(CVarShowDebug_SpawnVolume, this, &UBSCheatManager::ShowDebug_SpawnVolume);
	BIND_BS_CVAR(CVarShowDebug_DirectionalBoxes, this, &UBSCheatManager::ShowDebug_DirectionalBoxes);
	BIND_BS_CVAR(CVarShowDebug_FrontSpotLight, this, &UBSCheatManager::ShowDebug_SpotLightFront);
	BIND_BS_CVAR(CVarSetTimeOfDay, this, &UBSCheatManager::SetTimeOfDay);
	
	BIND_BS_COMP_CVAR(CVarShowDebug_AllSpawnAreas, this, SetComponentDebugBool, &USpawnAreaManagerComponent::bShowDebug_AllSpawnAreas);
	BIND_BS_COMP_CVAR(CVarShowDebug_ValidInvalid, this, SetComponentDebugBool, &USpawnAreaManagerComponent::bShowDebug_ValidInvalidSpawnAreas);
	BIND_BS_COMP_CVAR(CVarShowDebug_SpawnableSpawnAreas, this, SetComponentDebugBool, &USpawnAreaManagerComponent::bShowDebug_SpawnableSpawnAreas);
	BIND_BS_COMP_CVAR(CVarShowDebug_ActivatableSpawnAreas, this, SetComponentDebugBool, &USpawnAreaManagerComponent::bShowDebug_ActivatableSpawnAreas);
	BIND_BS_COMP_CVAR(CVarShowDebug_ActivatedSpawnAreas, this, SetComponentDebugBool, &USpawnAreaManagerComponent::bShowDebug_ActivatedSpawnAreas);
	BIND_BS_COMP_CVAR(CVarShowDebug_DeactivatedSpawnAreas, this, SetComponentDebugBool, &USpawnAreaManagerComponent::bShowDebug_DeactivatedSpawnAreas);
	BIND_BS_COMP_CVAR(CVarShowDebug_RecentSpawnAreas, this, SetComponentDebugBool, &USpawnAreaManagerComponent::bShowDebug_RecentSpawnAreas);
	BIND_BS_COMP_CVAR(CVarShowDebug_RemovedFromExtremaChange, this, SetComponentDebugBool, &USpawnAreaManagerComponent::bShowDebug_RemovedFromExtremaChange);
	BIND_BS_COMP_CVAR(CVarShowDebug_NonAdjacent, this, SetComponentDebugBool, &USpawnAreaManagerComponent::bShowDebug_NonAdjacent);
	BIND_BS_COMP_CVAR(CVarShowDebug_Vertices, this, SetComponentDebugBool, &USpawnAreaManagerComponent::bShowDebug_Vertices);
	BIND_BS_COMP_CVAR(CVarPrintDebug_SpawnAreaStateInfo, this, SetComponentDebugBool, &USpawnAreaManagerComponent::bPrintDebug_SpawnAreaStateInfo);
	BIND_BS_COMP_CVAR(CVarPrintDebug_Grid, this, SetComponentDebugBool, &USpawnAreaManagerComponent::bPrintDebug_Grid);
	
	BIND_BS_COMP_CVAR(CVarPrintDebug_ChooseBestActionIndex, this, SetComponentDebugBool, &UReinforcementLearningComponent::bPrintDebug_ChooseActionIndex);
	BIND_BS_COMP_CVAR(CVarPrintDebug_GetMaxIndex, this, SetComponentDebugBool, &UReinforcementLearningComponent::bPrintDebug_GetMaxIndex);
	BIND_BS_COMP_CVAR(CVarPrintDebug_QTableUpdate, this, SetComponentDebugBool, &UReinforcementLearningComponent::bPrintDebug_QTableUpdate);
	BIND_BS_COMP_CVAR(CVarPrintDebug_QTableInit, this, SetComponentDebugBool, &UReinforcementLearningComponent::bPrintDebug_QTableInit);
	BIND_BS_COMP_CVAR(CVarPrintDebug_ActiveTargetPairs, this, SetComponentDebugBool, &UReinforcementLearningComponent::bPrintDebug_ActiveTargetPairs);
}

void UBSCheatManager::Cheat_AimBot(IConsoleVariable* Variable)
{

	const ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	const ABSCharacterBase* Character = Cast<ABSCharacterBase>(
		Cast<ABSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->GetPawn());

	if (!GameMode || !Character)
	{
		return;
	}

	UBSAbilitySystemComponent* ASC = Character->GetBSAbilitySystemComponent();
	if (!ASC) return;

	const FString StringVariable = Variable->GetString();

	// Remove AimBot
	if (StringVariable.Equals("0"))
	{
		if (AimBotSpecHandle.IsValid())
		{
			ASC->ClearAbility(AimBotSpecHandle);
			UE_LOG(LogTemp, Display, TEXT("AimBot deactivated."));
		}
		return;
	}

	// Give AimBot
	if (!ASC->FindAbilitySpecFromHandle(AimBotSpecHandle))
	{
		const FGameplayAbilitySpec AbilitySpec(AimBotAbility, 1);
		AimBotSpecHandle = ASC->GiveAbility(AbilitySpec);
		UE_LOG(LogTemp, Display, TEXT("AimBot activated."));
	}
	
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromHandle(AimBotSpecHandle);
	if (!Spec) return;

	UGameplayAbility* Ability = Spec->GetPrimaryInstance();
	if (!Ability) return;

	UBSGA_AimBot* AbilityInstance = Cast<UBSGA_AimBot>(Ability);
	if (!AbilityInstance) return;

	if (!GameMode->GetTargetManager()->OnTargetActivated_AimBot.IsBoundToObject(AbilityInstance))
	{
		GameMode->GetTargetManager()->OnTargetActivated_AimBot.
		  AddUObject(AbilityInstance, &UBSGA_AimBot::OnTargetActivated);
	}
	
	FVector IgnoreStartLocation = FVector::ZeroVector;
	if (StringVariable.Contains("X=") || StringVariable.Contains("Y=") || StringVariable.Contains("Z="))
	{
		IgnoreStartLocation.InitFromCompactString(StringVariable);
	}
	AbilityInstance->SetIgnoreStartLocation(IgnoreStartLocation);
	ASC->MarkAbilitySpecDirty(*Spec);
	UE_LOG(LogTemp, Display, TEXT("AimBot modified."));
}

void UBSCheatManager::ShowDebug(IConsoleVariable* Variable)
{
	ShowDebug_ReinforcementLearningWidget(Variable);
	ShowDebug_SpawnBox(Variable);
	ShowDebug_SpawnVolume(Variable);
	ShowDebug_DirectionalBoxes(Variable);

	SetComponentDebugBool(Variable, &USpawnAreaManagerComponent::bShowDebug_AllSpawnAreas);
	SetComponentDebugBool(Variable, &USpawnAreaManagerComponent::bShowDebug_ValidInvalidSpawnAreas);
	SetComponentDebugBool(Variable, &USpawnAreaManagerComponent::bShowDebug_SpawnableSpawnAreas);
	SetComponentDebugBool(Variable, &USpawnAreaManagerComponent::bShowDebug_ActivatableSpawnAreas);
	SetComponentDebugBool(Variable, &USpawnAreaManagerComponent::bShowDebug_ActivatedSpawnAreas);
	SetComponentDebugBool(Variable, &USpawnAreaManagerComponent::bShowDebug_DeactivatedSpawnAreas);
	SetComponentDebugBool(Variable, &USpawnAreaManagerComponent::bShowDebug_RecentSpawnAreas);
	SetComponentDebugBool(Variable, &USpawnAreaManagerComponent::bShowDebug_RemovedFromExtremaChange);
	SetComponentDebugBool(Variable, &USpawnAreaManagerComponent::bShowDebug_NonAdjacent);
	SetComponentDebugBool(Variable, &USpawnAreaManagerComponent::bShowDebug_Vertices);
	SetComponentDebugBool(Variable, &USpawnAreaManagerComponent::bShowDebug_NonAdjacent);
	SetComponentDebugBool(Variable, &USpawnAreaManagerComponent::bPrintDebug_SpawnAreaStateInfo);
	SetComponentDebugBool(Variable, &USpawnAreaManagerComponent::bPrintDebug_Grid);

	SetComponentDebugBool(Variable, &UReinforcementLearningComponent::bPrintDebug_ChooseActionIndex);
	SetComponentDebugBool(Variable, &UReinforcementLearningComponent::bPrintDebug_GetMaxIndex);
	SetComponentDebugBool(Variable, &UReinforcementLearningComponent::bPrintDebug_QTableUpdate);
	SetComponentDebugBool(Variable, &UReinforcementLearningComponent::bPrintDebug_QTableInit);
	SetComponentDebugBool(Variable, &UReinforcementLearningComponent::bPrintDebug_ActiveTargetPairs);
}

void UBSCheatManager::ShowDebug_ReinforcementLearningWidget(IConsoleVariable* Variable)
{
	const ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode) return;

	const ATargetManager* TargetManager = GameMode->GetTargetManager();
	if (!TargetManager || !TargetManager->RLComponent) return;

	ABSPlayerController* Controller = Cast<ABSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!Controller) return;

	const FString VariableString = Variable->GetString();
	bool bShowWidget = false;
	bool bAvg = true;

	if (VariableString.Contains("avg", ESearchCase::IgnoreCase))
	{
		TargetManager->RLComponent->bBroadcastAverageOnQTableUpdate = true;
		bShowWidget = true;
		bAvg = true;
	}
	else if (VariableString.Contains("max", ESearchCase::IgnoreCase))
	{
		TargetManager->RLComponent->bBroadcastAverageOnQTableUpdate = false;
		bShowWidget = true;
		bAvg = false;
	}
	else if (Variable->GetBool())
	{
		bShowWidget = true;
		bAvg = true;
	}

	if (bShowWidget)
	{
		if (bAvg)
		{
			Controller->ShowRLAgentWidget(TargetManager->RLComponent->OnQTableUpdate, 5, 5,
				TargetManager->RLComponent->GetTArray_FromNdArray_QTableAvg());
		}
		else
		{
			Controller->ShowRLAgentWidget(TargetManager->RLComponent->OnQTableUpdate, 5, 5,
				TargetManager->RLComponent->GetTArray_FromNdArray_QTableMax());
		}
	}
	else
	{
		Controller->HideRLAgentWidget();
	}
}

void UBSCheatManager::ShowDebug_SpawnBox(IConsoleVariable* Variable)
{
	const ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode) return;

	const ATargetManager* TargetManager = GameMode->GetTargetManager();
	if (!TargetManager) return;

	if (Variable->GetBool())
	{
		TargetManager->SpawnBox->SetHiddenInGame(false);
		TargetManager->SpawnBox->ShapeColor = FColor::Blue;
		TargetManager->SpawnBox->SetLineThickness(6.f);
		TargetManager->SpawnBox->SetVisibility(true);
		TargetManager->SpawnBox->MarkRenderStateDirty();

		TargetManager->StaticExtentsBox->SetHiddenInGame(false);
		TargetManager->StaticExtentsBox->ShapeColor = FColor::Orange;
		TargetManager->StaticExtentsBox->SetLineThickness(6.f);
		TargetManager->StaticExtentsBox->SetVisibility(true);
		TargetManager->StaticExtentsBox->MarkRenderStateDirty();
	}
	else
	{
		TargetManager->SpawnBox->SetHiddenInGame(true);
		TargetManager->SpawnBox->SetVisibility(false);
		TargetManager->SpawnBox->MarkRenderStateDirty();

		TargetManager->StaticExtentsBox->SetHiddenInGame(true);
		TargetManager->StaticExtentsBox->SetVisibility(false);
		TargetManager->StaticExtentsBox->MarkRenderStateDirty();
	}
}

void UBSCheatManager::ShowDebug_SpawnVolume(IConsoleVariable* Variable)
{
	const ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode) return;

	const ATargetManager* TargetManager = GameMode->GetTargetManager();
	if (!TargetManager) return;

	if (Variable->GetBool())
	{
		TargetManager->SpawnVolume->SetHiddenInGame(false);
		TargetManager->SpawnVolume->ShapeColor = FColor::Orange;
		TargetManager->SpawnVolume->SetLineThickness(6.f);
		TargetManager->SpawnVolume->SetVisibility(true);
		TargetManager->SpawnVolume->MarkRenderStateDirty();
	}
	else
	{
		TargetManager->SpawnVolume->SetHiddenInGame(true);
		TargetManager->SpawnVolume->SetVisibility(false);
		TargetManager->SpawnVolume->MarkRenderStateDirty();

	}
}

void UBSCheatManager::ShowDebug_DirectionalBoxes(IConsoleVariable* Variable)
{
	const ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode) return;

	const ATargetManager* TargetManager = GameMode->GetTargetManager();
	if (!TargetManager) return;

	if (Variable->GetBool())
	{
		TargetManager->TopBox->SetHiddenInGame(false);
		TargetManager->BottomBox->SetHiddenInGame(false);
		TargetManager->LeftBox->SetHiddenInGame(false);
		TargetManager->RightBox->SetHiddenInGame(false);
		TargetManager->ForwardBox->SetHiddenInGame(false);
		TargetManager->BackwardBox->SetHiddenInGame(false);

		TargetManager->TopBox->SetVisibility(true);
		TargetManager->BottomBox->SetVisibility(true);
		TargetManager->LeftBox->SetVisibility(true);
		TargetManager->RightBox->SetVisibility(true);
		TargetManager->ForwardBox->SetVisibility(true);
		TargetManager->BackwardBox->SetVisibility(true);

		TargetManager->TopBox->ShapeColor = FColor::Blue;
		TargetManager->BottomBox->ShapeColor = FColor::Blue;
		TargetManager->LeftBox->ShapeColor = FColor::Green;
		TargetManager->RightBox->ShapeColor = FColor::Green;
		TargetManager->ForwardBox->ShapeColor = FColor::Red;
		TargetManager->BackwardBox->ShapeColor = FColor::Red;

		TargetManager->TopBox->SetLineThickness(4.f);
		TargetManager->BottomBox->SetLineThickness(4.f);
		TargetManager->LeftBox->SetLineThickness(4.f);
		TargetManager->RightBox->SetLineThickness(4.f);
		TargetManager->ForwardBox->SetLineThickness(4.f);
		TargetManager->BackwardBox->SetLineThickness(4.f);
		
		TargetManager->TopBox->MarkRenderStateDirty();
		TargetManager->BottomBox->MarkRenderStateDirty();
		TargetManager->LeftBox->MarkRenderStateDirty();
		TargetManager->RightBox->MarkRenderStateDirty();
		TargetManager->ForwardBox->MarkRenderStateDirty();
		TargetManager->BackwardBox->MarkRenderStateDirty();
	}
	else
	{
		TargetManager->TopBox->SetHiddenInGame(true);
		TargetManager->BottomBox->SetHiddenInGame(true);
		TargetManager->LeftBox->SetHiddenInGame(true);
		TargetManager->RightBox->SetHiddenInGame(true);
		TargetManager->ForwardBox->SetHiddenInGame(true);
		TargetManager->BackwardBox->SetHiddenInGame(true);

		TargetManager->TopBox->SetVisibility(false);
		TargetManager->BottomBox->SetVisibility(false);
		TargetManager->LeftBox->SetVisibility(false);
		TargetManager->RightBox->SetVisibility(false);
		TargetManager->ForwardBox->SetVisibility(false);
		TargetManager->BackwardBox->SetVisibility(false);
		
		TargetManager->TopBox->MarkRenderStateDirty();
		TargetManager->BottomBox->MarkRenderStateDirty();
		TargetManager->LeftBox->MarkRenderStateDirty();
		TargetManager->RightBox->MarkRenderStateDirty();
		TargetManager->ForwardBox->MarkRenderStateDirty();
		TargetManager->BackwardBox->MarkRenderStateDirty();
	}
}

void UBSCheatManager::ShowDebug_SpotLightFront(IConsoleVariable* Variable)
{
	UBSGameInstance* GI = Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GI->GetTimeOfDayManager())
	{
		GI->GetTimeOfDayManager()->SetSpotLightFrontEnabledState(Variable->GetBool());
	}
}

void UBSCheatManager::SetTimeOfDay(IConsoleVariable* Variable)
{
	UBSGameInstance* GI = Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GI->GetTimeOfDayManager())
	{
		if (Variable->GetString().Equals("day", ESearchCase::IgnoreCase))
		{
			GI->GetTimeOfDayManager()->SetTimeOfDay(ETimeOfDay::Day);
		}
		if (Variable->GetString().Equals("night", ESearchCase::IgnoreCase))
		{
			GI->GetTimeOfDayManager()->SetTimeOfDay(ETimeOfDay::Night);
		}
	}
}

template<typename T, class ComponentClass>
void UBSCheatManager::SetComponentDebugBool(IConsoleVariable* Variable, T ComponentClass::*DebugBool)
{
	const ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode) return;

	const ATargetManager* TargetManager = GameMode->GetTargetManager();
	if (!TargetManager) return;

	ComponentClass* Comp = TargetManager->GetComponentByClass<ComponentClass>();
	if (!Comp) return;

	Comp->*DebugBool = Variable->GetBool();
}

#endif
