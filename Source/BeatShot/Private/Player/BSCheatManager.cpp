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
	static TAutoConsoleVariable CVarCheat_AimBot(TEXT("bs_cheat.aimbot"), FString(), TEXT(
		"Enable Aim Bot \n" "1: Enables for all locations \n" "0: Disables and removes from player \n\n"
		"X={} Y={} Z={}: Only destroy targets at locations that are >= the value if all positive, \n"
		"or <= if negative. If either Y or Z is left blank, only the other value is considered."));


	static TAutoConsoleVariable CVarPrintDebug_NumRecentNumActive(TEXT("bs_printdebug.numrecentnumactive"), 0,
		TEXT("Toggles printing the number of currently activated and recent targets."));


	static TAutoConsoleVariable CVarPrintDebug_ChooseBestActionIndex(TEXT("bs_printdebug.choosebestidx"), 0,
		TEXT("Toggles printing info about the Reinforcement Learning Component choosing the Best Action Index."));


	static TAutoConsoleVariable CVarPrintDebug_GetMaxIndex(TEXT("bs_printdebug.getmaxindex"), 0,
		TEXT(
			"Toggles printing the Reinforcement Learning Component's intermediate values when finding the max action index."));


	static TAutoConsoleVariable CVarPrintDebug_QTableUpdate(TEXT("bs_printdebug.qtableupdate"), 0,
		TEXT(
			"Toggles printing the old and new QTable value each time the Reinforcement Learning Component makes an update to the QTable."));


	static TAutoConsoleVariable CVarShowDebug_TargetManager(TEXT("bs_showdebug.targetmanager"), 0,
		TEXT("Toggles showing the SpawnBox, SpawnVolume, Valid Spawn Areas, and Removed Spawn Areas.\n"
			"Blue Box: SpawnBox\n" "Orange Box: SpawnVolume\n" "Green Boxes: Valid Spawn Areas\n"
			"Red Boxes: Removed Spawn Areas"));


	static TAutoConsoleVariable CVarShowDebug_ReinforcementLearningWidget(TEXT("bs_showdebug.rlwidget"), 0,
		TEXT("Toggles showing the reinforcement learning widget.\n"
			"\"avg\": Shows the average values across each QTable column (destination indices).\n"
			"\"max\": Shows the max values across each QTable column (destination indices).\n"
			"Can also use 0/1 and it will use max values."));


	static TAutoConsoleVariable CVarShowDebug_SpawnBox(TEXT("bs_showdebug.spawnbox"), 0,
		TEXT("Draw debug boxes for the SpawnBox and SpawnVolume.\n" "Blue: SpawnBox\n" "Orange: SpawnVolume"));


	static TAutoConsoleVariable CVarShowDebug_AllSpawnAreas(TEXT("bs_showdebug.allspawnareas"), 0,
		TEXT(
			"Toggles showing Cyan debug boxes for all Spawn Areas at the end of the GetValidSpawnLocations function. Persistant boxes."));


	static TAutoConsoleVariable CVarShowDebug_ValidSpawnAreas(TEXT("bs_showdebug.validspawnareas"), 0,
		TEXT(
			"Toggles showing Green debug boxes for valid Spawn Areas at the end of the GetValidSpawnLocations function."));


	static TAutoConsoleVariable CVarShowDebug_RemovedSpawnAreas(TEXT("bs_showdebug.removedspawnareas"), 0,
		TEXT("Toggles showing Red debug boxes for removed Spawn Areas."));


	static TAutoConsoleVariable CVarShowDebug_FilteredRecent(TEXT("bs_showdebug.filteredrecent"), 0,
		TEXT(
			"Toggles showing Turquoise debug boxes for filtered recent Spawn Areas at the end of the FilterRecentIndices function."));


	static TAutoConsoleVariable CVarShowDebug_FilteredActivated(TEXT("bs_showdebug.filteredactivated"), 0,
		TEXT(
			"Toggles showing Cyan debug boxes for filtered activated Spawn Areas at the end of the FilterActivatedIndices function."));


	static TAutoConsoleVariable CVarShowDebug_FilteredManaged(TEXT("bs_showdebug.filtermanaged"), 0,
		TEXT(
			"Toggles showing Blue debug boxes for filtered managed Spawn Areas at the end of the FilterManagedIndices function."));


	static TAutoConsoleVariable CVarShowDebug_OverlappingVertices_Dynamic(TEXT("bs_showdebug.overlappingverts_dynamic"),
		0, TEXT(
			"Shows the overlapping vertices generated during RemoveOverlappingSpawnLocations function.\n"
			"Draws a Magenta Debug Sphere showing the target that was used to generate the overlapping points.\n"
			"Draws Red Debug Boxes for the removed overlapping vertices."));


	static TAutoConsoleVariable CVarShowDebug_OverlappingVertices_OnFlaggedManaged(
		TEXT("bs_showdebug.overlappingverts_onmanaged"), 0, TEXT(
			"Shows the overlapping vertices generated when Spawn Area was flagged as Managed.\n"
			"Draws a Magenta Debug Sphere showing the target that was used to generate the overlapping points.\n"
			"Draws Red Debug Boxes for the removed overlapping vertices.\n" "Draws Green Debug Boxes for valid."));


	static TAutoConsoleVariable CVarShowDebug_FrontSpotLight(TEXT("bs_showdebug.frontspotlight"), 0,
		TEXT("If night, shows toggles showing the front spotlight."));


	static TAutoConsoleVariable CVarSetTimeOfDay(TEXT("bs_settimeofday"), FString("day"),
		TEXT("Set the time of day to either \"day\" or \"night\"."));
}

void UBSCheatManager::InitCheatManager()
{
	ReceiveInitCheatManager();

	FConsoleVariableDelegate CVarEnableAimBotDelegate;
	CVarEnableAimBotDelegate.BindUObject(this, &UBSCheatManager::CVarOnChanged_Cheat_AimBot);
	BeatShotConsoleVariables::CVarCheat_AimBot.AsVariable()->SetOnChangedCallback(CVarEnableAimBotDelegate);

	FConsoleVariableDelegate CVarPrintDebugNumRecentNumActiveDelegate;
	CVarPrintDebugNumRecentNumActiveDelegate.BindUObject(this,
		&UBSCheatManager::CVarOnChanged_PrintDebug_NumRecentNumActive);
	BeatShotConsoleVariables::CVarPrintDebug_NumRecentNumActive.AsVariable()->SetOnChangedCallback(
		CVarPrintDebugNumRecentNumActiveDelegate);

	FConsoleVariableDelegate CVarPrintDebugChooseBestActionIndexDelegate;
	CVarPrintDebugChooseBestActionIndexDelegate.BindUObject(this,
		&UBSCheatManager::CVarOnChanged_PrintDebug_ChooseBestActionIndex);
	BeatShotConsoleVariables::CVarPrintDebug_ChooseBestActionIndex.AsVariable()->SetOnChangedCallback(
		CVarPrintDebugChooseBestActionIndexDelegate);

	FConsoleVariableDelegate CVarPrintDebugGetMaxIndexDelegate;
	CVarPrintDebugGetMaxIndexDelegate.BindUObject(this, &UBSCheatManager::CVarOnChanged_PrintDebug_GetMaxIndex);
	BeatShotConsoleVariables::CVarPrintDebug_GetMaxIndex.AsVariable()->SetOnChangedCallback(
		CVarPrintDebugGetMaxIndexDelegate);

	FConsoleVariableDelegate CVarPrintDebugQTableUpdateDelegate;
	CVarPrintDebugQTableUpdateDelegate.BindUObject(this, &UBSCheatManager::CVarOnChanged_PrintDebug_QTableUpdate);
	BeatShotConsoleVariables::CVarPrintDebug_QTableUpdate.AsVariable()->SetOnChangedCallback(
		CVarPrintDebugQTableUpdateDelegate);

	FConsoleVariableDelegate CVarShowDebugReinforcementLearningWidgetDelegate;
	CVarShowDebugReinforcementLearningWidgetDelegate.BindUObject(this,
		&UBSCheatManager::CVarOnChanged_ShowDebug_ReinforcementLearningWidget);
	BeatShotConsoleVariables::CVarShowDebug_ReinforcementLearningWidget.AsVariable()->SetOnChangedCallback(
		CVarShowDebugReinforcementLearningWidgetDelegate);

	FConsoleVariableDelegate CVarShowDebugSpawnBoxDelegate;
	CVarShowDebugSpawnBoxDelegate.BindUObject(this, &UBSCheatManager::CVarOnChanged_ShowDebug_SpawnBox);
	BeatShotConsoleVariables::CVarShowDebug_SpawnBox.AsVariable()->SetOnChangedCallback(CVarShowDebugSpawnBoxDelegate);

	FConsoleVariableDelegate CVarShowDebugAllSpawnAreas;
	CVarShowDebugAllSpawnAreas.BindUObject(this, &UBSCheatManager::CVarOnChanged_ShowDebug_AllSpawnAreas);
	BeatShotConsoleVariables::CVarShowDebug_AllSpawnAreas.AsVariable()->
	                                                      SetOnChangedCallback(CVarShowDebugAllSpawnAreas);

	FConsoleVariableDelegate CVarShowDebugValidSpawnAreas;
	CVarShowDebugValidSpawnAreas.BindUObject(this, &UBSCheatManager::CVarOnChanged_ShowDebug_ValidSpawnAreas);
	BeatShotConsoleVariables::CVarShowDebug_ValidSpawnAreas.AsVariable()->SetOnChangedCallback(
		CVarShowDebugValidSpawnAreas);

	FConsoleVariableDelegate CVarShowDebugRemovedSpawnAreas;
	CVarShowDebugRemovedSpawnAreas.BindUObject(this, &UBSCheatManager::CVarOnChanged_ShowDebug_RemovedSpawnAreas);
	BeatShotConsoleVariables::CVarShowDebug_RemovedSpawnAreas.AsVariable()->SetOnChangedCallback(
		CVarShowDebugRemovedSpawnAreas);

	FConsoleVariableDelegate CVarShowDebugFilteredRecent;
	CVarShowDebugFilteredRecent.BindUObject(this, &UBSCheatManager::CVarOnChanged_ShowDebug_FilteredRecent);
	BeatShotConsoleVariables::CVarShowDebug_FilteredRecent.AsVariable()->SetOnChangedCallback(
		CVarShowDebugFilteredRecent);

	FConsoleVariableDelegate CVarShowDebugFilteredActivated;
	CVarShowDebugFilteredActivated.BindUObject(this, &UBSCheatManager::CVarOnChanged_ShowDebug_FilteredActivated);
	BeatShotConsoleVariables::CVarShowDebug_FilteredActivated.AsVariable()->SetOnChangedCallback(
		CVarShowDebugFilteredActivated);

	FConsoleVariableDelegate CVarShowDebugFilteredManaged;
	CVarShowDebugFilteredManaged.BindUObject(this, &UBSCheatManager::CVarOnChanged_ShowDebug_FilteredManaged);
	BeatShotConsoleVariables::CVarShowDebug_FilteredManaged.AsVariable()->SetOnChangedCallback(
		CVarShowDebugFilteredManaged);

	FConsoleVariableDelegate CVarShowDebugOverlappingVertices_Dynamic;
	CVarShowDebugOverlappingVertices_Dynamic.BindUObject(this,
		&UBSCheatManager::CVarOnChanged_ShowDebug_OverlappingVertices_Dynamic);
	BeatShotConsoleVariables::CVarShowDebug_OverlappingVertices_Dynamic.AsVariable()->SetOnChangedCallback(
		CVarShowDebugOverlappingVertices_Dynamic);

	FConsoleVariableDelegate CVarShowDebugOverlappingVertices_OnFlaggedManaged;
	CVarShowDebugOverlappingVertices_OnFlaggedManaged.BindUObject(this,
		&UBSCheatManager::CVarOnChanged_ShowDebug_OverlappingVertices_OnFlaggedManaged);
	BeatShotConsoleVariables::CVarShowDebug_OverlappingVertices_OnFlaggedManaged.AsVariable()->SetOnChangedCallback(
		CVarShowDebugOverlappingVertices_OnFlaggedManaged);

	FConsoleVariableDelegate CVarShowDebugTargetManagerDelegate;
	CVarShowDebugTargetManagerDelegate.BindUObject(this, &UBSCheatManager::CVarOnChanged_ShowDebug_TargetManager);
	BeatShotConsoleVariables::CVarShowDebug_TargetManager.AsVariable()->SetOnChangedCallback(
		CVarShowDebugTargetManagerDelegate);

	FConsoleVariableDelegate CVarShowDebugSpotLightFront;
	CVarShowDebugSpotLightFront.BindUObject(this, &UBSCheatManager::CVarOnChanged_ShowDebug_SpotLightFront);
	BeatShotConsoleVariables::CVarShowDebug_FrontSpotLight.AsVariable()->SetOnChangedCallback(
		CVarShowDebugSpotLightFront);

	FConsoleVariableDelegate CVarSetTimeOfDayDelegate;
	CVarSetTimeOfDayDelegate.BindUObject(this, &UBSCheatManager::CVarOnChanged_SetTimeOfDay);
	BeatShotConsoleVariables::CVarSetTimeOfDay.AsVariable()->SetOnChangedCallback(CVarSetTimeOfDayDelegate);
}

void UBSCheatManager::CVarOnChanged_Cheat_AimBot(IConsoleVariable* Variable)
{
	const ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	const ABSCharacter* Character = Cast<ABSCharacter>(
		Cast<ABSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->GetPawn());

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

	GameMode->GetTargetManager()->OnTargetActivated_AimBot.
	          AddUObject(AbilityInstance, &UBSGA_AimBot::OnTargetActivated);

	FVector IgnoreStartLocation = FVector::ZeroVector;
	if (StringVariable.Contains("X=") || StringVariable.Contains("Y=") || StringVariable.Contains("Z="))
	{
		IgnoreStartLocation.InitFromCompactString(StringVariable);
	}
	AbilityInstance->SetIgnoreStartLocation(IgnoreStartLocation);
	ASC->MarkAbilitySpecDirty(*Spec);

	UE_LOG(LogTemp, Display, TEXT("AimBot activated."));
}

void UBSCheatManager::CVarOnChanged_PrintDebug_NumRecentNumActive(IConsoleVariable* Variable)
{
	const ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode) return;

	ATargetManager* TargetManager = GameMode->GetTargetManager();
	if (!TargetManager) return;

	TargetManager->bPrintDebug_NumRecentNumActive = Variable->GetBool();
}

void UBSCheatManager::CVarOnChanged_PrintDebug_ChooseBestActionIndex(IConsoleVariable* Variable)
{
	const ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode) return;

	const ATargetManager* TargetManager = GameMode->GetTargetManager();
	if (!TargetManager || TargetManager->ReinforcementLearningComponent) return;

	TargetManager->ReinforcementLearningComponent->bPrintDebug_ChooseBestActionIndex = Variable->GetBool();
}

void UBSCheatManager::CVarOnChanged_PrintDebug_GetMaxIndex(IConsoleVariable* Variable)
{
	const ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode) return;

	const ATargetManager* TargetManager = GameMode->GetTargetManager();
	if (!TargetManager || TargetManager->ReinforcementLearningComponent) return;

	TargetManager->ReinforcementLearningComponent->bPrintDebug_GetMaxIndex = Variable->GetBool();
}

void UBSCheatManager::CVarOnChanged_PrintDebug_QTableUpdate(IConsoleVariable* Variable)
{
	const ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode) return;

	const ATargetManager* TargetManager = GameMode->GetTargetManager();
	if (!TargetManager || TargetManager->ReinforcementLearningComponent) return;

	TargetManager->ReinforcementLearningComponent->bPrintDebug_QTableUpdate = Variable->GetBool();
}

void UBSCheatManager::CVarOnChanged_ShowDebug_TargetManager(IConsoleVariable* Variable)
{
	CVarOnChanged_ShowDebug_SpawnBox(Variable);
	CVarOnChanged_ShowDebug_ValidSpawnAreas(Variable);
	CVarOnChanged_ShowDebug_RemovedSpawnAreas(Variable);
}

void UBSCheatManager::CVarOnChanged_ShowDebug_ReinforcementLearningWidget(IConsoleVariable* Variable)
{
	const ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode) return;

	const ATargetManager* TargetManager = GameMode->GetTargetManager();
	if (!TargetManager || TargetManager->ReinforcementLearningComponent) return;

	ABSPlayerController* Controller = Cast<ABSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!Controller) return;

	const FString VariableString = Variable->GetString();
	bool bShowWidget = false;
	bool bAvg = false;

	if (VariableString.Contains("avg", ESearchCase::IgnoreCase))
	{
		TargetManager->ReinforcementLearningComponent->bBroadcastAverageOnQTableUpdate = true;
		bShowWidget = true;
		bAvg = true;
	}
	else if (VariableString.Contains("max", ESearchCase::IgnoreCase))
	{
		TargetManager->ReinforcementLearningComponent->bBroadcastAverageOnQTableUpdate = false;
		bShowWidget = true;
		bAvg = false;
	}
	else if (Variable->GetBool())
	{
		bShowWidget = true;
		bAvg = false;
	}

	if (bShowWidget)
	{
		if (bAvg)
		{
			Controller->ShowRLAgentWidget(TargetManager->ReinforcementLearningComponent->OnQTableUpdate, 5, 5,
				TargetManager->ReinforcementLearningComponent->GetTArray_FromNdArray_QTableAvg());
		}
		else
		{
			Controller->ShowRLAgentWidget(TargetManager->ReinforcementLearningComponent->OnQTableUpdate, 5, 5,
				TargetManager->ReinforcementLearningComponent->GetTArray_FromNdArray_QTableMax());
		}
	}
	else
	{
		Controller->HideRLAgentWidget();
	}
}

void UBSCheatManager::CVarOnChanged_ShowDebug_SpawnBox(IConsoleVariable* Variable)
{
	const ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode) return;

	ATargetManager* TargetManager = GameMode->GetTargetManager();
	if (!TargetManager) return;

	if (Variable->GetBool())
	{
		TargetManager->SpawnBox->SetHiddenInGame(false);
		TargetManager->SpawnVolume->SetHiddenInGame(false);

		TargetManager->SpawnBox->SetVisibility(true);
		TargetManager->SpawnVolume->SetVisibility(true);

		TargetManager->SpawnBox->ShapeColor = FColor::Blue;
		TargetManager->SpawnVolume->ShapeColor = FColor::Orange;

		TargetManager->SpawnVolume->MarkRenderStateDirty();
		TargetManager->SpawnBox->MarkRenderStateDirty();
	}
	else
	{
		TargetManager->SpawnVolume->SetHiddenInGame(true);
		TargetManager->SpawnBox->SetHiddenInGame(true);
		TargetManager->SpawnVolume->SetVisibility(false);
		TargetManager->SpawnVolume->SetVisibility(false);
		TargetManager->SpawnVolume->MarkRenderStateDirty();
		TargetManager->SpawnBox->MarkRenderStateDirty();
		FlushPersistentDebugLines(GetWorld());
	}
}

void UBSCheatManager::CVarOnChanged_ShowDebug_AllSpawnAreas(IConsoleVariable* Variable)
{
	const ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode) return;

	const ATargetManager* TargetManager = GameMode->GetTargetManager();
	if (!TargetManager || !TargetManager->SpawnAreaManager) return;

	if (Variable->GetBool())
	{
		TargetManager->SpawnAreaManager->DrawDebug_AllSpawnAreas();
	}
	else
	{
		TargetManager->SpawnAreaManager->ClearDebug_AllSpawnAreas();
	}
}

void UBSCheatManager::CVarOnChanged_ShowDebug_ValidSpawnAreas(IConsoleVariable* Variable)
{
	const ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode) return;

	const ATargetManager* TargetManager = GameMode->GetTargetManager();
	if (!TargetManager || !TargetManager->SpawnAreaManager) return;

	TargetManager->SpawnAreaManager->bShowDebug_ValidSpawnLocations = Variable->GetBool();
}

void UBSCheatManager::CVarOnChanged_ShowDebug_RemovedSpawnAreas(IConsoleVariable* Variable)
{
	const ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode) return;

	const ATargetManager* TargetManager = GameMode->GetTargetManager();
	if (!TargetManager || !TargetManager->SpawnAreaManager) return;

	TargetManager->SpawnAreaManager->bShowDebug_RemovedSpawnLocations = Variable->GetBool();
}

void UBSCheatManager::CVarOnChanged_ShowDebug_FilteredRecent(IConsoleVariable* Variable)
{
	const ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode) return;

	const ATargetManager* TargetManager = GameMode->GetTargetManager();
	if (!TargetManager || !TargetManager->SpawnAreaManager) return;

	TargetManager->SpawnAreaManager->bShowDebug_FilteredRecentIndices = Variable->GetBool();
}

void UBSCheatManager::CVarOnChanged_ShowDebug_FilteredActivated(IConsoleVariable* Variable)
{
	const ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode) return;

	const ATargetManager* TargetManager = GameMode->GetTargetManager();
	if (!TargetManager || !TargetManager->SpawnAreaManager) return;

	TargetManager->SpawnAreaManager->bShowDebug_FilteredActivatedIndices = Variable->GetBool();
}

void UBSCheatManager::CVarOnChanged_ShowDebug_FilteredManaged(IConsoleVariable* Variable)
{
	const ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode) return;

	const ATargetManager* TargetManager = GameMode->GetTargetManager();
	if (!TargetManager || !TargetManager->SpawnAreaManager) return;

	TargetManager->SpawnAreaManager->bShowDebug_FilteredManagedIndices = Variable->GetBool();
}

void UBSCheatManager::CVarOnChanged_ShowDebug_OverlappingVertices_OnFlaggedManaged(IConsoleVariable* Variable)
{
	const ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode) return;

	const ATargetManager* TargetManager = GameMode->GetTargetManager();
	if (!TargetManager || !TargetManager->SpawnAreaManager) return;

	TargetManager->SpawnAreaManager->bShowDebug_OverlappingVertices_OnFlaggedManaged = Variable->GetBool();
}

void UBSCheatManager::CVarOnChanged_ShowDebug_OverlappingVertices_Dynamic(IConsoleVariable* Variable)
{
	const ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode) return;

	const ATargetManager* TargetManager = GameMode->GetTargetManager();
	if (!TargetManager || !TargetManager->SpawnAreaManager) return;

	TargetManager->SpawnAreaManager->bShowDebug_OverlappingVertices_Dynamic = Variable->GetBool();
}

void UBSCheatManager::CVarOnChanged_ShowDebug_SpotLightFront(IConsoleVariable* Variable)
{
	UBSGameInstance* GI = Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GI->TimeOfDayManager)
	{
		GI->TimeOfDayManager->SetSpotLightFrontEnabledState(Variable->GetBool());
	}
}

void UBSCheatManager::CVarOnChanged_SetTimeOfDay(IConsoleVariable* Variable)
{
	UBSGameInstance* GI = Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GI->TimeOfDayManager)
	{
		if (Variable->GetString().Equals("day", ESearchCase::IgnoreCase))
		{
			GI->TimeOfDayManager->SetTimeOfDay(ETimeOfDay::Day);
		}
		if (Variable->GetString().Equals("night", ESearchCase::IgnoreCase))
		{
			GI->TimeOfDayManager->SetTimeOfDay(ETimeOfDay::Night);
		}
	}
}
