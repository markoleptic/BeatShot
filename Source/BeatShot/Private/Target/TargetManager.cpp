// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "Target/TargetManager.h"
#include "BSGameMode.h"
#include "Player/BSPlayerController.h"
#include "GlobalConstants.h"
#include "Target/Target.h"
#include "Components/BoxComponent.h"
#include "Engine/CompositeCurveTable.h"
#include "Kismet/DataTableFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Target/ReinforcementLearningComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Target/SpawnAreaManagerComponent.h"

DEFINE_LOG_CATEGORY(LogTargetManager);
using namespace Constants;

ATargetManager::ATargetManager()
{
	PrimaryActorTick.bCanEverTick = true;

	SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Spawn Box"));
	RootComponent = SpawnBox;
	
	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Spawn Volume"));
	TopBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Top Box"));
	BottomBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Bottom Box"));
	LeftBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Left Box"));
	RightBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Right Box"));
	ForwardBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Forward Box"));
	BackwardBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Backward Box"));

	SpawnBox->SetLineThickness(5.f);
	TopBox->SetLineThickness(5.f);
	BottomBox->SetLineThickness(5.f);
	LeftBox->SetLineThickness(5.f);
	RightBox->SetLineThickness(5.f);
	ForwardBox->SetLineThickness(5.f);
	BackwardBox->SetLineThickness(5.f);

	SpawnAreaManager = CreateDefaultSubobject<USpawnAreaManagerComponent>(TEXT("Spawn Area Manager Component"));
	ReinforcementLearningComponent = CreateDefaultSubobject<UReinforcementLearningComponent>(TEXT("Reinforcement Learning Component"));
	
	ConsecutiveTargetsHit = 0;
	BSConfigLocal = FBSConfig();
	BSConfig = nullptr;
	PlayerSettings = FPlayerSettings_Game();
	LastTargetSpawnedCenter = false;
	ShouldSpawn = false;
	bShowDebug_SpawnBox = false;
	bShowDebug_SpawnMemory = false;
	bShowDebug_ReinforcementLearningWidget = false;
	CurrentSpawnArea = nullptr;
	PreviousSpawnArea = nullptr;
	CurrentTargetScale = FVector(1.f);
	StaticExtrema = FExtrema();
	StaticExtents = FVector();
	ConsecutiveTargetsHit = 0;
	DynamicLookUpValue_TargetScale = 0;
	DynamicLookUpValue_SpawnAreaScale = 0;
	ManagedTargets = TArray<ATarget*>();
	TotalPossibleDamage = 0.f;
	bLastSpawnedTargetDirectionChangeHorizontal = false;
	bLastActivatedTargetDirectionChangeHorizontal = false;
}

void ATargetManager::BeginPlay()
{
	Super::BeginPlay();
}

void ATargetManager::Destroyed()
{
	if (ShouldSpawn)
	{
		SetShouldSpawn(false);
	}
	
	if (!GetManagedTargets().IsEmpty())
	{
		for (TObjectPtr<ATarget> Target : GetManagedTargets())
		{
			if (Target)
			{
				Target->Destroy();
			}
		}
		ManagedTargets.Empty();
	}

	BSConfig = nullptr;
	
	Super::Destroyed();
}

void ATargetManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TrackingTargetIsDamageable())
	{
		UpdateTotalPossibleDamage();
	}
}

void ATargetManager::Init(const FBSConfig& InBSConfig, const FPlayerSettings_Game& InPlayerSettings)
{
	if (ShouldSpawn)
	{
		SetShouldSpawn(false);
	}
	
	// Initialize local copy of BSConfig
	BSConfigLocal = InBSConfig;
	BSConfig = &BSConfigLocal;
	PlayerSettings = InPlayerSettings;

	Init_Internal();
}

void ATargetManager::Init(FBSConfig* InBSConfig, const FPlayerSettings_Game& InPlayerSettings)
{
	if (ShouldSpawn)
	{
		SetShouldSpawn(false);
	}
	// Initialize pointer to another BSConfig
	BSConfig = InBSConfig;
	PlayerSettings = InPlayerSettings;

	Init_Internal();
}

void ATargetManager::Init_Internal()
{
	// Clean Up
	if (!GetManagedTargets().IsEmpty())
	{
		for (TObjectPtr<ATarget> Target : GetManagedTargets())
		{
			if (Target)
			{
				Target->Destroy();
			}
		}
		ManagedTargets.Empty();
	}
	ConsecutiveTargetsHit = 0;
	LastTargetSpawnedCenter = false;
	CurrentSpawnArea = nullptr;
	PreviousSpawnArea = nullptr;
	CurrentTargetScale = FVector(1.f);
	StaticExtrema = FExtrema();
	StaticExtents = FVector();
	ConsecutiveTargetsHit = 0;
	DynamicLookUpValue_TargetScale = 0;
	DynamicLookUpValue_SpawnAreaScale = 0;
	TotalPossibleDamage = 0.f;

	// Initialize target colors
	GetBSConfig()->InitColors(PlayerSettings.bUseSeparateOutlineColor, PlayerSettings.InactiveTargetColor, PlayerSettings.TargetOutlineColor,
		PlayerSettings.StartTargetColor,PlayerSettings.PeakTargetColor, PlayerSettings.EndTargetColor);
	
	// Set SpawnBox location & BoxExtent, StaticExtents, and StaticExtrema
	SpawnBox->SetRelativeLocation(GenerateSpawnBoxLocation());
	StaticExtents = GenerateSpawnBoxExtents();
	SpawnBox->SetBoxExtent(StaticExtents);
	StaticExtrema = GenerateBoxExtrema();

	// Initialize the CompositeCurveTables in case they need to be modified
	Init_Tables();

	// Set the size of all the boxes that make up the SpawnVolume. Also calls UpdateSpawnVolume
	UpdateSpawnBox();

	// Initialize the SpawnAreaManager
	SpawnAreaManager->Init(GetBSConfig(), GetBoxOrigin(), GetBoxExtents(false, true), GetBoxExtrema(false, true));
	
	// Init RLC
	if (GetBSConfig()->IsCompatibleWithReinforcementLearning())
	{
		const FCommonScoreInfo CommonScoreInfo = FindCommonScoreInfo(GetBSConfig()->DefiningConfig);
		const FRLAgentParams Params(GetBSConfig()->AIConfig, SpawnAreaManager->GetSpawnAreas().Num(), SpawnAreaManager->GetSpawnAreasHeight(), SpawnAreaManager->GetSpawnAreasWidth(),
			CommonScoreInfo.QTable, CommonScoreInfo.TrainingSamples, CommonScoreInfo.TotalTrainingSamples);
		ReinforcementLearningComponent->Init(Params);
		
#if !UE_BUILD_SHIPPING
		// Print loaded QTable
		FNumberFormattingOptions Options;
		Options.MinimumFractionalDigits = 2;
		Options.MaximumFractionalDigits = 2;
		Options.MaximumIntegralDigits = 1;
		Options.MinimumIntegralDigits = 1;
		UE_LOG(LogTemp, Display, TEXT("Loaded QTable:"));
		USaveGamePlayerScore::PrintQTable(GetBSConfig()->DefiningConfig, CommonScoreInfo, Options);
#endif
	}
	
	// Spawn any targets if needed
	if(GetBSConfig()->TargetConfig.TargetSpawningPolicy == ETargetSpawningPolicy::UpfrontOnly)
	{
		HandleUpfrontSpawning();
	}
}

void ATargetManager::Init_Tables()
{
	FRealCurve* PreThresholdCurve = CompositeCurveTable_SpawnArea->GetCurves()[0].CurveToEdit;
	int32 CurveIndex = GetBSConfig()->DynamicSpawnAreaScaling.bIsCubicInterpolation ? 2 : 1;
	FRealCurve* ThresholdMetCurve = CompositeCurveTable_SpawnArea->GetCurves()[CurveIndex].CurveToEdit;
	
	PreThresholdCurve->SetKeyTime(PreThresholdCurve->GetFirstKeyHandle(), 0);
	PreThresholdCurve->SetKeyTime(PreThresholdCurve->GetLastKeyHandle(), GetBSConfig()->DynamicSpawnAreaScaling.StartThreshold);
	ThresholdMetCurve->SetKeyTime(ThresholdMetCurve->GetFirstKeyHandle(), GetBSConfig()->DynamicSpawnAreaScaling.StartThreshold);
	ThresholdMetCurve->SetKeyTime(ThresholdMetCurve->GetLastKeyHandle(), GetBSConfig()->DynamicSpawnAreaScaling.EndThreshold);
	
	PreThresholdCurve = CompositeCurveTable_TargetScale->GetCurves()[0].CurveToEdit;
	CurveIndex = GetBSConfig()->DynamicTargetScaling.bIsCubicInterpolation ? 2 : 1;
	ThresholdMetCurve = CompositeCurveTable_TargetScale->GetCurves()[CurveIndex].CurveToEdit;
	
	PreThresholdCurve->SetKeyTime(PreThresholdCurve->GetFirstKeyHandle(), 0);
	PreThresholdCurve->SetKeyTime(PreThresholdCurve->GetLastKeyHandle(), GetBSConfig()->DynamicTargetScaling.StartThreshold);
	ThresholdMetCurve->SetKeyTime(ThresholdMetCurve->GetFirstKeyHandle(), GetBSConfig()->DynamicTargetScaling.StartThreshold);
	ThresholdMetCurve->SetKeyTime(ThresholdMetCurve->GetLastKeyHandle(), GetBSConfig()->DynamicTargetScaling.EndThreshold);
}

void ATargetManager::SetShouldSpawn(const bool bShouldSpawn)
{
	if (bShouldSpawn)
	{
		if (GetBSConfig()->TargetConfig.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::HideTarget))
		{
			for (const TObjectPtr<ATarget> Target : GetManagedTargets())
			{
				Target->SetActorHiddenInGame(true);
			}
		}
	}
	ShouldSpawn = bShouldSpawn;
}

void ATargetManager::OnPlayerStopTrackingTarget()
{
	if (GetBSConfig()->TargetConfig.TargetDamageType != ETargetDamageType::Tracking)
	{
		return;
	}
	for (const TObjectPtr<ATarget> Target : GetManagedTargets())
	{
		if (Target && !Target->IsImmuneToTrackingDamage())
		{
			Target->SetTargetColor(GetBSConfig()->TargetConfig.EndColor);
		}
	}
}

/* ------------------------------------ */
/* -- Target spawning and activation -- */
/* ------------------------------------ */

void ATargetManager::OnAudioAnalyzerBeat()
{
	if (!ShouldSpawn) return;
	
	HandleRuntimeSpawning();
	HandleTargetActivation();
	SpawnAreaManager->RefreshRecentFlags();

#if !UE_BUILD_SHIPPING
	if (bShowDebug_SpawnMemory)
	{
		ShowDebug_NumRecentNumActivated();
	}
#endif
}

ATarget* ATargetManager::SpawnTarget(USpawnArea* InSpawnArea)
{
	if (!InSpawnArea)
	{
		return nullptr;
	}
	ATarget* Target = GetWorld()->SpawnActorDeferred<ATarget>(TargetToSpawn,
		FTransform(FRotator::ZeroRotator, InSpawnArea->GetChosenPoint(), InSpawnArea->GetTargetScale()),
		this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	Target->Init(GetBSConfig()->TargetConfig);
	Target->OnTargetDamageEventOrTimeout.AddDynamic(this, &ATargetManager::OnTargetHealthChangedOrExpired);
	if (BSConfig->TargetConfig.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::ChangeDirection))
	{
		Target->OnDeactivationResponse_ChangeDirection.AddUObject(this, &ATargetManager::ChangeTargetDirection);
	}
	InSpawnArea->SetTargetGuid(Target->GetGuid());
	Target->FinishSpawning(FTransform(), true);
	AddToManagedTargets(Target);
	if (BSConfig->TargetConfig.bApplyVelocityWhenSpawned)
	{
		Target->SetTargetSpeed(FMath::FRandRange(GetBSConfig()->TargetConfig.MinSpawnedTargetSpeed, GetBSConfig()->TargetConfig.MaxSpawnedTargetSpeed));
		ChangeTargetDirection(Target, 0);
	}
	return Target;
}

int32 ATargetManager::AddToManagedTargets(ATarget* SpawnTarget)
{
	SpawnAreaManager->FlagSpawnAreaAsManaged(SpawnTarget->GetGuid());
	TArray<TObjectPtr<ATarget>> Targets = GetManagedTargets();
	const int32 NewIndex = Targets.Add(TObjectPtr<ATarget>(SpawnTarget));
	ManagedTargets = Targets;
	return NewIndex;
}

bool ATargetManager::ActivateTarget(ATarget* InTarget) const
{
	// TargetManager handles all TargetActivationResponses
	// Each target handles their own TargetDeactivationResponses & TargetDestructionConditions
	
	if (!InTarget || !SpawnAreaManager->IsSpawnAreaValid(SpawnAreaManager->FindSpawnAreaFromGuid(InTarget->GetGuid())))
	{
		return false;
	}

	// Make sure it isn't hidden
	if (InTarget->IsHidden())
	{
		InTarget->SetActorHiddenInGame(false);
	}

	if (GetBSConfig()->TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::AddImmunity))
	{
		InTarget->ApplyImmunityEffect();
	}
	if (GetBSConfig()->TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::RemoveImmunity))
	{
		InTarget->RemoveImmunityEffect();
	}
	if (GetBSConfig()->TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::ToggleImmunity))
	{
		InTarget->IsImmuneToDamage() ? InTarget->RemoveImmunityEffect() : InTarget->ApplyImmunityEffect();
	}
	if (GetBSConfig()->TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::ChangeVelocity))
	{
		InTarget->SetTargetSpeed(FMath::FRandRange(GetBSConfig()->TargetConfig.MinActivatedTargetSpeed, GetBSConfig()->TargetConfig.MaxActivatedTargetSpeed));
		if (!GetBSConfig()->TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::ChangeDirection) &&
			GetBSConfig()->TargetConfig.MovingTargetDirectionMode != EMovingTargetDirectionMode::None)
		{
			ChangeTargetDirection(InTarget, 1);
		}
	}
	if (GetBSConfig()->TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::ChangeDirection))
	{
		ChangeTargetDirection(InTarget, 1);
	}
	
	if (InTarget->HasTargetBeenActivatedBefore() && GetBSConfig()->TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::ApplyConsecutiveTargetScale))
	{
		InTarget->SetTargetScale(FindNextTargetScale());
	}
	
	if (InTarget->ActivateTarget(GetBSConfig()->TargetConfig.TargetMaxLifeSpan))
	{
		SpawnAreaManager->FlagSpawnAreaAsActivated(InTarget->GetGuid());
		OnTargetActivated.Broadcast();
		OnTargetActivated_AimBot.Broadcast(InTarget);
		if (ReinforcementLearningComponent->GetReinforcementLearningMode() != EReinforcementLearningMode::None)
		{
			if (SpawnAreaManager->IsSpawnAreaValid(PreviousSpawnArea))
			{
				ReinforcementLearningComponent->AddToActiveTargetPairs(PreviousSpawnArea->GetIndex(), CurrentSpawnArea->GetIndex());
			}
			PreviousSpawnArea = CurrentSpawnArea;
		}
		return true;
	}
	return false;
}

void ATargetManager::HandleUpfrontSpawning()
{
	if (GetBSConfig()->TargetConfig.TargetDistributionPolicy == ETargetDistributionPolicy::Grid)
	{
		for (int i = 0; i < SpawnAreaManager->GetSpawnAreas().Num(); i++)
		{
			SpawnAreaManager->GetSpawnAreasRef()[i]->SetTargetScale(FindNextTargetScale());
			SpawnTarget(SpawnAreaManager->GetSpawnAreasRef()[i]);
		}
		//FindNextTargetProperties();
	}
	else
	{
		for (int i = 0; i < GetBSConfig()->TargetConfig.NumUpfrontTargetsToSpawn; i++)
		{
			FindNextTargetProperties();
			if (CurrentSpawnArea)
			{
				SpawnTarget(CurrentSpawnArea);
			}
		}
	}
}

void ATargetManager::HandleRuntimeSpawning()
{
	if (GetBSConfig()->TargetConfig.TargetSpawningPolicy != ETargetSpawningPolicy::RuntimeOnly) return;
	
	int32 NumberToSpawn = GetNumberOfRuntimeTargetsToSpawn();

	// Only spawn targets that can be activated unless allowed
	if (!GetBSConfig()->TargetConfig.bAllowSpawnWithoutActivation)
	{
		const int32 NumberToActivate = GetNumberOfTargetsToActivateAtOnce(NumberToSpawn);
		if (NumberToSpawn > NumberToActivate)
		{
			NumberToSpawn = NumberToActivate;
		}
	}
	
	for (int i = 0; i < NumberToSpawn; i++)
	{
		FindNextTargetProperties();
		SpawnTarget(CurrentSpawnArea);
	}
}

void ATargetManager::HandleTargetActivation() 
{
	if (GetManagedTargets().IsEmpty()) { return; }
	
	// Persistant Targets are the only type that can always receive continuous activation
	if (GetBSConfig()->TargetConfig.TargetDeactivationConditions.Contains(ETargetDeactivationCondition::Persistant))
	{
		HandlePermanentlyActiveTargetActivation();
		return;
	}

	// Check to see if theres any targets available to activate
	const int32 NumAvailableToActivate = SpawnAreaManager->GetDeactivatedManagedSpawnAreas().Num();
	const int32 NumToActivate = GetNumberOfTargetsToActivateAtOnce(NumAvailableToActivate);
	
	for (int i = 0; i < NumToActivate; i++)
	{
		switch (GetBSConfig()->TargetConfig.TargetSpawningPolicy)
		{
		case ETargetSpawningPolicy::None:
			break;
		case ETargetSpawningPolicy::UpfrontOnly:
			{
				FindNextTargetProperties();
				if (CurrentSpawnArea)
				{
					if (ATarget* Target = FindManagedTargetByGuid(CurrentSpawnArea->GetTargetGuid()))
					{
						ActivateTarget(Target);
						//if (ActivateTarget(Target))
						//{
						//	FindNextTargetProperties();
						//}
					}
				}
			}
			break;
		// This is the only case where the CurrentSpawnArea cannot be used to find the next target to activate. Instead, use GetManagedTargets
		case ETargetSpawningPolicy::RuntimeOnly:
			{
				for (TObjectPtr<ATarget> Target : GetManagedTargets())
				{
					if (const USpawnArea* SpawnArea = SpawnAreaManager->FindSpawnAreaFromGuid(Target->GetGuid()))
					{
						if (!SpawnArea->IsActivated())
						{
							ActivateTarget(Target);
							break;
						}
					}
				}
			}
			break;
		}
	}
}

void ATargetManager::HandlePermanentlyActiveTargetActivation() const
{
	// Handle initial activation
	TArray<USpawnArea*> SpawnAreas = SpawnAreaManager->GetActivatedSpawnAreas();
	if (SpawnAreas.IsEmpty())
	{
		SpawnAreas = SpawnAreaManager->GetDeactivatedManagedSpawnAreas();
	}
	
	for (const USpawnArea* SpawnArea : SpawnAreas)
	{
		if (ATarget* Target = FindManagedTargetByGuid(SpawnArea->GetTargetGuid()))
		{
			ActivateTarget(Target);
		}
	}
}

int32 ATargetManager::GetNumberOfRuntimeTargetsToSpawn() const
{
	// Depends on: MaxNumTargetsAtOnce, NumRuntimeTargetsToSpawn, ManagedTargets, bUseBatchSpawning

	// Batch spawning waits until there are no more Activated and Deactivated target(s)
	if (GetBSConfig()->TargetConfig.bUseBatchSpawning)
	{
		if (GetManagedTargets().Num() > 0 ||
			SpawnAreaManager->GetActivatedSpawnAreas().Num() > 0 ||
			SpawnAreaManager->GetDeactivatedManagedSpawnAreas().Num() > 0)
		{
			return 0;
		}
	}

	// Set default value to number of runtime targets to spawn to NumRuntimeTargetsToSpawn
	int32 NumAllowedToSpawn = (GetBSConfig()->TargetConfig.NumRuntimeTargetsToSpawn == -1) ? 1 : GetBSConfig()->TargetConfig.NumRuntimeTargetsToSpawn;

	// Return NumRuntimeTargetsToSpawn if no Max
	if (GetBSConfig()->TargetConfig.MaxNumTargetsAtOnce == -1)
	{
		return NumAllowedToSpawn;
	}
	
	NumAllowedToSpawn = GetBSConfig()->TargetConfig.MaxNumTargetsAtOnce - GetManagedTargets().Num();

	// Don't let NumAllowedToSpawn exceed NumRuntimeTargetsToSpawn
	if (NumAllowedToSpawn > GetBSConfig()->TargetConfig.NumRuntimeTargetsToSpawn)
	{
		return GetBSConfig()->TargetConfig.NumRuntimeTargetsToSpawn;
	}
	
	return NumAllowedToSpawn;
}

int32 ATargetManager::GetNumberOfTargetsToActivateAtOnce(const int32 MaxPossibleToActivate) const
{
	// Depends on: MaxNumActivatedTargetsAtOnce, MinNumTargetsToActivateAtOnce, MaxNumTargetsToActivateAtOnce, ActivatedSpawnAreas

	// Default to very high number if less than 1
	const int32 MaxAllowed = GetBSConfig()->TargetConfig.MaxNumActivatedTargetsAtOnce < 1 ? 100 : GetBSConfig()->TargetConfig.MaxNumActivatedTargetsAtOnce;

	// Constraints: Max Possible & Max Allowed (both must be satisfied, so pick min)
	const int32 ConstrainedLimit = FMath::Min(MaxAllowed - SpawnAreaManager->GetActivatedSpawnAreas().Num(), MaxPossibleToActivate);
	if (ConstrainedLimit <= 0)
	{
		return 0;
	}

	// Can activate at least 1 at this point
	
	int32 MinToActivate = FMath::Min(GetBSConfig()->TargetConfig.MinNumTargetsToActivateAtOnce, GetBSConfig()->TargetConfig.MaxNumTargetsToActivateAtOnce);
	int32 MaxToActivate = FMath::Max(GetBSConfig()->TargetConfig.MinNumTargetsToActivateAtOnce, GetBSConfig()->TargetConfig.MaxNumTargetsToActivateAtOnce);

	// Allow 0, but don't default to it
	if (MinToActivate != 0)
	{
		MinToActivate = FMath::Clamp(MinToActivate, 1, ConstrainedLimit);
	}
	MaxToActivate = FMath::Clamp(MaxToActivate, 1, ConstrainedLimit);
	UE_LOG(LogTemp, Display, TEXT("MinToActivate: %d MaxToActivate: %d"), MinToActivate, MaxToActivate);
	return FMath::RandRange(MinToActivate, MaxToActivate);
}

void ATargetManager::FindNextTargetProperties()
{
	const FVector NewScale = FindNextTargetScale();
	LastTargetSpawnedCenter = CurrentSpawnArea ? CurrentSpawnArea->GetChosenPoint().Equals(GetBoxOrigin()) : false;
	
	CurrentSpawnArea = FindNextSpawnArea(GetBSConfig()->TargetConfig.BoundsScalingPolicy, NewScale);
	if (CurrentSpawnArea && SpawnAreaManager->GetSpawnAreas().IsValidIndex(CurrentSpawnArea->GetIndex()))
	{
		/*for (const USpawnArea* SpawnArea : SpawnAreaManager->GetActivatedOrRecentSpawnAreas())
		{
			if (FVector::Distance(SpawnArea->ChosenPoint, CurrentSpawnArea->GetChosenPoint()) < 200.f)
			{
				UE_LOG(LogTemp, Display, TEXT("Distance less than 200: %f"), FVector::Distance(SpawnArea->GetChosenPoint(), CurrentSpawnArea->GetChosenPoint()));
			}
		}*/
		CurrentSpawnArea->SetTargetScale(NewScale);
	}
}

FVector ATargetManager::FindNextTargetScale() const
{
	if (GetBSConfig()->TargetConfig.ConsecutiveTargetScalePolicy == EConsecutiveTargetScalePolicy::SkillBased)
	{
		const float NewFactor = GetDynamicValueFromCurveTable(false, DynamicLookUpValue_TargetScale);
		return FVector(UKismetMathLibrary::Lerp(GetBSConfig()->TargetConfig.MaxSpawnedTargetScale, GetBSConfig()->TargetConfig.MinSpawnedTargetScale, NewFactor));
	}
	return FVector(FMath::FRandRange(GetBSConfig()->TargetConfig.MinSpawnedTargetScale, GetBSConfig()->TargetConfig.MaxSpawnedTargetScale));
}

USpawnArea* ATargetManager::FindNextSpawnArea(const EBoundsScalingPolicy BoundsScalingPolicy, const FVector& NewTargetScale) const
{
	// Change the BoxExtent of the SpawnBox if dynamic
	UpdateSpawnBox();

	// Can skip GetValidSpawnLocations if forcing every other target in center
	if (CurrentSpawnArea && GetBSConfig()->TargetConfig.bSpawnEveryOtherTargetInCenter && 
		CurrentSpawnArea != SpawnAreaManager->FindSpawnAreaFromLocation(GetBoxOrigin()))
	{
		return SpawnAreaManager->FindSpawnAreaFromLocation(GetBoxOrigin());
	}
	
	TArray<FVector> OpenLocations = SpawnAreaManager->GetValidSpawnLocations(NewTargetScale,
		GetBoxExtrema(GetBSConfig()->TargetConfig.BoundsScalingPolicy == EBoundsScalingPolicy::Dynamic),
		CurrentSpawnArea);
	if (OpenLocations.IsEmpty())
	{
		UE_LOG(LogTargetManager, Warning, TEXT("OpenLocations is empty."));
		return nullptr;
	}
	
	if (OpenLocations.Contains(GetBoxOrigin()) && GetBSConfig()->TargetConfig.bSpawnAtOriginWheneverPossible)
	{
		return SpawnAreaManager->FindSpawnAreaFromLocation(GetBoxOrigin());
	}
	
	if (ReinforcementLearningComponent->GetReinforcementLearningMode() == EReinforcementLearningMode::Exploration ||
		ReinforcementLearningComponent->GetReinforcementLearningMode() == EReinforcementLearningMode::ActiveAgent)
	{
		if (USpawnArea* NextSpawnArea = TryGetSpawnAreaFromReinforcementLearningComponent(OpenLocations))
		{
			if (GetBSConfig()->TargetConfig.TargetDistributionPolicy != ETargetDistributionPolicy::Grid)
			{
				NextSpawnArea->SetRandomChosenPoint();
			}
			return NextSpawnArea;
		}
		UE_LOG(LogTargetManager, Warning, TEXT("Unable to Spawn Location suggested by RLAgent."));
	}

	const int32 OpenLocationIndex = FMath::RandRange(0, OpenLocations.Num() - 1);
	if (USpawnArea* NextSpawnArea = SpawnAreaManager->FindSpawnAreaFromLocation(OpenLocations[OpenLocationIndex]))
	{
		if (GetBSConfig()->TargetConfig.TargetDistributionPolicy != ETargetDistributionPolicy::Grid)
		{
			NextSpawnArea->SetRandomChosenPoint();
		}
		return NextSpawnArea;
	}
	//UE_LOG(LogTargetManager, Display, TEXT("Found Vertex_BottomLeft %s Found CenterPoint %s Found ChosenPoint %s"),
	//*Found->Vertex_BottomLeft.ToString(), *Found->CenterPoint.ToString(), *Found->ChosenPoint.ToString());
	return nullptr;
}

USpawnArea* ATargetManager::TryGetSpawnAreaFromReinforcementLearningComponent(const TArray<FVector>& OpenLocations) const
{
	/* Converting all OpenLocations to indices */
	TArray<int32> Indices;
	for (const FVector Vector : OpenLocations)
	{
		if (const int32 FoundIndex = SpawnAreaManager->FindSpawnAreaIndexFromLocation(Vector); FoundIndex != INDEX_NONE)
		{
			Indices.Add(FoundIndex);
		}
	}
	if (Indices.IsEmpty())
	{
		UE_LOG(LogTargetManager, Warning, TEXT("No targets in OpenLocations or No targets in TargetPairs"));
		return nullptr;
	}

	
	int32 PreviousIndex = INDEX_NONE;
	if (PreviousSpawnArea)
	{
		PreviousIndex = PreviousSpawnArea->GetIndex();
	}
	const int32 ChosenIndex = ReinforcementLearningComponent->ChooseNextActionIndex(Indices, PreviousIndex);
	if (!SpawnAreaManager->GetSpawnAreas().IsValidIndex(ChosenIndex))
	{
		return nullptr;
	}

	USpawnArea* ChosenPoint = SpawnAreaManager->GetSpawnAreasRef()[ChosenIndex];
	ChosenPoint->SetRandomChosenPoint();
	return ChosenPoint;
}

/* ---------------------------------- */
/* -- Deactivation and Destruction -- */
/* ---------------------------------- */

void ATargetManager::OnTargetHealthChangedOrExpired(const FTargetDamageEvent& TargetDamageEvent)
{
	UpdateConsecutiveTargetsHit(TargetDamageEvent.TimeAlive);
	UpdateDynamicLookUpValues(TargetDamageEvent.TimeAlive);
	HandleTargetExpirationDelegate(GetBSConfig()->TargetConfig.TargetDamageType, TargetDamageEvent);
	HandleManagedTargetRemoval(GetBSConfig()->TargetConfig.TargetDestructionConditions, TargetDamageEvent);

	if (ReinforcementLearningComponent->GetReinforcementLearningMode() != EReinforcementLearningMode::None)
	{
		if (const USpawnArea* Found = SpawnAreaManager->FindSpawnAreaFromGuid(TargetDamageEvent.Guid))
		{
			ReinforcementLearningComponent->SetActiveTargetPairReward(Found->GetIndex(), TargetDamageEvent.TimeAlive != -1);
		}
	}
	
	SpawnAreaManager->HandleRecentTargetRemoval(GetBSConfig()->TargetConfig.RecentTargetMemoryPolicy, TargetDamageEvent);
}

void ATargetManager::UpdateConsecutiveTargetsHit(const float TimeAlive)
{
	if (TimeAlive == -1)
	{
		ConsecutiveTargetsHit = 0;
	}
	else
	{
		ConsecutiveTargetsHit++;
	}
}

void ATargetManager::UpdateDynamicLookUpValues(const float TimeAlive)
{
	if (TimeAlive < 0.f)
	{
		DynamicLookUpValue_TargetScale = FMath::Clamp(DynamicLookUpValue_TargetScale - GetBSConfig()->DynamicTargetScaling.DecrementAmount,
			0, GetBSConfig()->DynamicTargetScaling.EndThreshold);
		DynamicLookUpValue_SpawnAreaScale = FMath::Clamp(DynamicLookUpValue_SpawnAreaScale - GetBSConfig()->DynamicSpawnAreaScaling.DecrementAmount,
			0, GetBSConfig()->DynamicSpawnAreaScaling.EndThreshold);
	}
	else
	{
		DynamicLookUpValue_TargetScale = FMath::Clamp(DynamicLookUpValue_TargetScale + 1, 0, GetBSConfig()->DynamicTargetScaling.EndThreshold);
		DynamicLookUpValue_SpawnAreaScale = FMath::Clamp(DynamicLookUpValue_SpawnAreaScale + 1, 0, GetBSConfig()->DynamicSpawnAreaScaling.EndThreshold);
	}
}

void ATargetManager::HandleTargetExpirationDelegate(const ETargetDamageType& DamageType, const FTargetDamageEvent& TargetDamageEvent) const
{
	if (DamageType == ETargetDamageType::Tracking)
	{
		OnBeatTrackTargetDamaged.Broadcast(TargetDamageEvent.DamageDelta, TotalPossibleDamage);
	}
	else
	{
		OnTargetDeactivated.Broadcast(TargetDamageEvent.TimeAlive, ConsecutiveTargetsHit, TargetDamageEvent.Transform);
	}
}

void ATargetManager::HandleManagedTargetRemoval(const TArray<ETargetDestructionCondition>& TargetDestructionConditions, const FTargetDamageEvent& TargetDamageEvent)
{
	if (TargetDestructionConditions.Contains(ETargetDestructionCondition::Persistant))
	{
		return;
	}
	
	if (TargetDestructionConditions.Contains(ETargetDestructionCondition::OnDeactivation))
	{
		RemoveFromManagedTargets(TargetDamageEvent.Guid);
	}
	if (TargetDestructionConditions.Contains(ETargetDestructionCondition::OnExpiration))
	{
		RemoveFromManagedTargets(TargetDamageEvent.Guid);
	}
	if (TargetDestructionConditions.Contains(ETargetDestructionCondition::OnHealthReachedZero) &&
		TargetDamageEvent.CurrentHealth <= 0.f)
	{
		RemoveFromManagedTargets(TargetDamageEvent.Guid);
	}
	if (TargetDestructionConditions.Contains(ETargetDestructionCondition::OnAnyExternalDamageTaken) &&
		TargetDamageEvent.TimeAlive != -1)
	{
		RemoveFromManagedTargets(TargetDamageEvent.Guid);
	}
	
}

void ATargetManager::RemoveFromManagedTargets(const FGuid GuidToRemove)
{
	SpawnAreaManager->RemoveManagedFlagFromSpawnArea(GuidToRemove);
	const TArray<TObjectPtr<ATarget>> Targets = GetManagedTargets().FilterByPredicate([&] (const TObjectPtr<ATarget>& OtherTarget)
	{
		if (!OtherTarget)
		{
			return false;
		}
		if (OtherTarget->GetGuid() == GuidToRemove)
		{
			return false;
		}
		return true;
	});
	ManagedTargets = Targets;
}

/* ------------------------------ */
/* -- SpawnBox and SpawnVolume -- */
/* ------------------------------ */

FVector ATargetManager::GenerateSpawnBoxLocation() const
{
	FVector SpawnBoxCenter = DefaultTargetManagerLocation;
	
	switch (GetBSConfig()->TargetConfig.TargetDistributionPolicy) {
	case ETargetDistributionPolicy::HeadshotHeightOnly:
		{
			SpawnBoxCenter.Z = HeadshotHeight;
		}
		break;
	case ETargetDistributionPolicy::None:
	case ETargetDistributionPolicy::EdgeOnly:
	case ETargetDistributionPolicy::FullRange:
		{
			const float MaxHalfHeight = FMath::Max(GetBSConfig()->TargetConfig.BoxBounds.Z, GetBSConfig()->DynamicSpawnAreaScaling.MinSize.Z) / 2.f;
			SpawnBoxCenter.Z = MaxHalfHeight  + GetBSConfig()->TargetConfig.FloorDistance;
		}
		break;
	case ETargetDistributionPolicy::Grid:
		{
			const float MaxTargetDiameter = GetBSConfig()->TargetConfig.MaxSpawnedTargetScale * SphereTargetDiameter;
			const float VSpacing = GetBSConfig()->GridConfig.GridSpacing.Y + MaxTargetDiameter;
			const float HalfHeight = VSpacing * (GetBSConfig()->GridConfig.NumVerticalGridTargets - 1) * 0.5f;
			SpawnBoxCenter.Z = HalfHeight + GetBSConfig()->TargetConfig.FloorDistance;
		}
		break;
	}
	return SpawnBoxCenter;
}

FVector ATargetManager::GenerateSpawnBoxExtents() const
{
	switch (GetBSConfig()->TargetConfig.TargetDistributionPolicy) {
	case ETargetDistributionPolicy::HeadshotHeightOnly:
		return FVector(
		GetBSConfig()->TargetConfig.BoxBounds.X / 2.f,
		GetBSConfig()->TargetConfig.BoxBounds.Y / 2.f,
		1.f);
	case ETargetDistributionPolicy::None:
	case ETargetDistributionPolicy::EdgeOnly:
	case ETargetDistributionPolicy::FullRange:
		return FVector(
		GetBSConfig()->TargetConfig.BoxBounds.X / 2.f,
		GetBSConfig()->TargetConfig.BoxBounds.Y / 2.f,
		GetBSConfig()->TargetConfig.BoxBounds.Z / 2.f);
	case ETargetDistributionPolicy::Grid:
		{
			const float MaxTargetDiameter = GetBSConfig()->TargetConfig.MaxSpawnedTargetScale * SphereTargetDiameter;
			// This will be SpawnMemoryIncY
			const float HSpacing = GetBSConfig()->GridConfig.GridSpacing.X + MaxTargetDiameter;
			// This will be SpawnMemoryIncZ
			const float VSpacing = GetBSConfig()->GridConfig.GridSpacing.Y + MaxTargetDiameter;
			const float HalfWidth = HSpacing * (GetBSConfig()->GridConfig.NumHorizontalGridTargets - 1) * 0.5f;
			const float HalfHeight = VSpacing * (GetBSConfig()->GridConfig.NumVerticalGridTargets - 1) * 0.5f;
			return FVector(
			GetBSConfig()->TargetConfig.BoxBounds.X / 2.f,
			HalfWidth,
			HalfHeight);
		}
	}
	return FVector::ZeroVector;
}

FExtrema ATargetManager::GenerateBoxExtrema() const
{
	switch (GetBSConfig()->TargetConfig.TargetDistributionPolicy) {

	case ETargetDistributionPolicy::HeadshotHeightOnly:
	case ETargetDistributionPolicy::None:
	case ETargetDistributionPolicy::EdgeOnly:
	case ETargetDistributionPolicy::FullRange:
		{
			return FExtrema(SpawnBox->Bounds.GetBoxExtrema(0), SpawnBox->Bounds.GetBoxExtrema(1));
		}
	case ETargetDistributionPolicy::Grid:
		{
			const float MaxTargetDiameter = GetBSConfig()->TargetConfig.MaxSpawnedTargetScale * SphereTargetDiameter;
			// This will be SpawnMemoryIncY
			const float HSpacing = GetBSConfig()->GridConfig.GridSpacing.X + MaxTargetDiameter;
			// This will be SpawnMemoryIncZ
			const float VSpacing = GetBSConfig()->GridConfig.GridSpacing.Y + MaxTargetDiameter;
	
			const float HalfWidth = HSpacing * (GetBSConfig()->GridConfig.NumHorizontalGridTargets - 1) * 0.5f;
			const float HalfHeight = VSpacing * (GetBSConfig()->GridConfig.NumVerticalGridTargets - 1) * 0.5f;

			// Add 1 to max so that for loop includes MaxY and MaxZ
			const float MinY = GetBoxOrigin().Y - HalfWidth;
			const float MaxY = GetBoxOrigin().Y + HalfWidth;
			const float MinZ = GetBoxOrigin().Z - HalfHeight;
			const float MaxZ = GetBoxOrigin().Z + HalfHeight;
	
			return FExtrema(FVector(GetBoxOrigin().X, MinY, MinZ), FVector(GetBoxOrigin().X, MaxY, MaxZ));
		}
	}
	return FExtrema(FVector::ZeroVector, FVector::ZeroVector);
}

FVector ATargetManager::GetBoxOrigin() const
{
	return SpawnBox->Bounds.Origin;
}

FVector ATargetManager::GetBoxExtents(const bool bDynamic, const bool bAbsoluteMax) const
{
	if (bAbsoluteMax)
	{
		// If user made MinExtent > MaxExtent
		const FVector MinExtents = GetBSConfig()->DynamicSpawnAreaScaling.GetMinExtent();
		return FVector(
			FMath::Max(MinExtents.X, StaticExtents.X),
			FMath::Max(MinExtents.Y, StaticExtents.Y),
			FMath::Max(MinExtents.Z, StaticExtents.Z)
			);
	}
	if (bDynamic)
	{
		return SpawnBox->Bounds.BoxExtent;
	}
	return StaticExtents;
}

FExtrema ATargetManager::GetBoxExtrema(const bool bDynamic, const bool bAbsoluteMax) const
{
	if (bAbsoluteMax)
	{
		const FVector MaxExtents = GetBoxExtents(false, true);
		return FExtrema(FVector(0.f, GetBoxOrigin().Y - MaxExtents.Y, GetBoxOrigin().Z - MaxExtents.Z),
			FVector(0.f, GetBoxOrigin().Y + MaxExtents.Y, GetBoxOrigin().Z + MaxExtents.Z));
	}
	if (bDynamic)
	{
		return FExtrema(SpawnBox->Bounds.GetBoxExtrema(0), SpawnBox->Bounds.GetBoxExtrema(1));
	}
	return StaticExtrema;
}

void ATargetManager::UpdateSpawnBox() const
{
	// Update dynamic Y & Z extent. Dynamic X extent updated in UpdateSpawnVolume
	if (GetBSConfig()->TargetConfig.BoundsScalingPolicy == EBoundsScalingPolicy::Dynamic)
	{
		const float NewFactor = GetDynamicValueFromCurveTable(true, DynamicLookUpValue_SpawnAreaScale);
	
		const float LerpY = UKismetMathLibrary::Lerp(GetBSConfig()->DynamicSpawnAreaScaling.GetMinExtent().Y, GetBoxExtents().Y, NewFactor);
		const float LerpZ = UKismetMathLibrary::Lerp(GetBSConfig()->DynamicSpawnAreaScaling.GetMinExtent().Z, GetBoxExtents().Z, NewFactor);
	
		const float Y = FMath::GridSnap<float>(LerpY, SpawnAreaManager->GetSpawnMemoryIncY());
		const float Z = FMath::GridSnap<float>(LerpZ, SpawnAreaManager->GetSpawnMemoryIncZ());
	
		SpawnBox->SetBoxExtent(FVector(0, Y, Z));
	}
	
	UpdateSpawnVolume();
}

void ATargetManager::UpdateSpawnVolume() const
{
	float LocationX;
	float ExtentX;

	// Update dynamic X extent
	if (GetBSConfig()->TargetConfig.BoundsScalingPolicy == EBoundsScalingPolicy::Dynamic)
	{
		const float NewFactor = GetDynamicValueFromCurveTable(true, DynamicLookUpValue_SpawnAreaScale);
		// Min X Extent needs to be multiplied by 0.5
		const float DynamicExtentX = FMath::GridSnap<float>(UKismetMathLibrary::Lerp(
			GetBSConfig()->DynamicSpawnAreaScaling.GetMinExtent().X,
			GetBoxExtents().X,
			NewFactor), 100.f);
		
		LocationX = GetBoxOrigin().X - DynamicExtentX;
		// X Extent (BoxBounds.X/2) + max sphere radius
		ExtentX = DynamicExtentX + GetBSConfig()->TargetConfig.MaxSpawnedTargetScale * SphereTargetRadius + 10.f;
	}
	else
	{
		LocationX = GetBoxOrigin().X - GetBoxExtents().X;
		// X Extent (BoxBounds.X/2) + max sphere radius
		ExtentX = GetBoxExtents().X + GetBSConfig()->TargetConfig.MaxSpawnedTargetScale * SphereTargetRadius + 10.f;
	}
	
	const FVector DynamicExtent = SpawnBox->Bounds.BoxExtent;
		
	SpawnVolume->SetRelativeLocation(FVector(LocationX, GetBoxOrigin().Y, GetBoxOrigin().Z));
	SpawnVolume->SetBoxExtent(FVector(ExtentX, DynamicExtent.Y, DynamicExtent.Z));
		
	TopBox->SetRelativeLocation(FVector(LocationX, GetBoxOrigin().Y, GetBoxOrigin().Z + DynamicExtent.Z));
	BottomBox->SetRelativeLocation(FVector(LocationX, GetBoxOrigin().Y, GetBoxOrigin().Z - DynamicExtent.Z));
	LeftBox->SetRelativeLocation(FVector(LocationX, GetBoxOrigin().Y - DynamicExtent.Y, GetBoxOrigin().Z));
	RightBox->SetRelativeLocation(FVector(LocationX, GetBoxOrigin().Y + DynamicExtent.Y, GetBoxOrigin().Z));
	ForwardBox->SetRelativeLocation(FVector(LocationX + ExtentX, GetBoxOrigin().Y, GetBoxOrigin().Z));
	BackwardBox->SetRelativeLocation(FVector(LocationX - ExtentX, GetBoxOrigin().Y, GetBoxOrigin().Z));
		
	TopBox->SetBoxExtent(FVector(ExtentX, DynamicExtent.Y, 0));
	BottomBox->SetBoxExtent(FVector(ExtentX, DynamicExtent.Y, 0));
	LeftBox->SetBoxExtent(FVector(ExtentX, 0, DynamicExtent.Z));
	RightBox->SetBoxExtent(FVector(ExtentX, 0, DynamicExtent.Z));
	ForwardBox->SetBoxExtent(FVector(0, DynamicExtent.Y, DynamicExtent.Z));
	BackwardBox->SetBoxExtent(FVector(0, DynamicExtent.Y, DynamicExtent.Z));
}

/* ------------------------------ */
/* -- Misc or Helper functions -- */
/* ------------------------------ */

void ATargetManager::ChangeTargetDirection(ATarget* InTarget, const uint8 InSpawnActivationDeactivation) const
{
	// Alternate directions of spawned targets, while also alternating directions of individual target direction changes
	bool bLastDirectionChangeHorizontal;
	
	if (InSpawnActivationDeactivation == 0)
	{
		bLastDirectionChangeHorizontal = bLastSpawnedTargetDirectionChangeHorizontal;
		bLastSpawnedTargetDirectionChangeHorizontal = !bLastDirectionChangeHorizontal;
	}
	else if (InSpawnActivationDeactivation == 1)
	{
		bLastDirectionChangeHorizontal = bLastActivatedTargetDirectionChangeHorizontal;
		bLastActivatedTargetDirectionChangeHorizontal = !bLastDirectionChangeHorizontal;
	}
	else
	{
		bLastDirectionChangeHorizontal = InTarget->GetLastDirectionChangeHorizontal();
		InTarget->SetLastDirectionChangeHorizontal(!bLastDirectionChangeHorizontal);
	}

	InTarget->SetTargetDirection(GetNewTargetDirection(InTarget->GetActorLocation(), bLastDirectionChangeHorizontal));
}

FVector ATargetManager::GetNewTargetDirection(const FVector& LocationBeforeChange, const bool bLastDirectionChangeHorizontal) const
{
	switch (GetBSConfig()->TargetConfig.MovingTargetDirectionMode)
	{
	case EMovingTargetDirectionMode::HorizontalOnly:
		{
			if (FMath::RandBool())
			{
				return FVector(0, 1, 0);
			}
			return FVector(0, -1, 0);
		}
	case EMovingTargetDirectionMode::VerticalOnly:
		{
			if (FMath::RandBool())
			{
				return FVector(0, 0, 1);
			}
			return FVector(0, 0, -1);
		}
	case EMovingTargetDirectionMode::AlternateHorizontalVertical:
		{
			if (bLastDirectionChangeHorizontal)
			{
				if (FMath::RandBool())
				{
					return FVector(0, 0, 1);
				}
				return FVector(0, 0, -1);
			}
			if (FMath::RandBool())
			{
				return FVector(0, 1, 0);
			}
			return FVector(0, -1, 0);
		}
	case EMovingTargetDirectionMode::Any:
		{
			// TODO something is probably wrong with this
			const FVector NewExtent = FVector(GetBoxExtents().Z * 0.5f, GetBoxExtents().Y * 0.5f, GetBoxExtents().Z * 0.5f);
			const FVector OriginOffset = FVector(0, GetBoxExtents().Y * 0.5f, GetBoxExtents().Z * 0.5f);
			const FVector BotLeft = UKismetMathLibrary::RandomPointInBoundingBox(SpawnVolume->Bounds.Origin + FVector(0, -OriginOffset.Y, -OriginOffset.Z), NewExtent);
			const FVector BotRight = UKismetMathLibrary::RandomPointInBoundingBox(SpawnVolume->Bounds.Origin + FVector(0, OriginOffset.Y, -OriginOffset.Z), NewExtent);
			const FVector TopLeft = UKismetMathLibrary::RandomPointInBoundingBox(SpawnVolume->Bounds.Origin + FVector(0, -OriginOffset.Y, OriginOffset.Z), NewExtent);
			const FVector TopRight = UKismetMathLibrary::RandomPointInBoundingBox(SpawnVolume->Bounds.Origin + FVector(0, OriginOffset.Y, OriginOffset.Z), NewExtent);

			TArray PossibleLocations = {BotLeft, BotRight, TopLeft, TopRight};

			if (LocationBeforeChange.Y < 0)
			{
				if (LocationBeforeChange.Z < GetBoxOrigin().Z)
				{
					PossibleLocations.Remove(BotLeft);
				}
				else
				{
					PossibleLocations.Remove(TopLeft);
				}
			}
			else
			{
				if (LocationBeforeChange.Z < GetBoxOrigin().Z)
				{
					PossibleLocations.Remove(BotRight);
				}
				else
				{
					PossibleLocations.Remove(TopRight);
				}
			}
			const FVector NewLocation = PossibleLocations[UKismetMathLibrary::RandomIntegerInRange(0, 2)];
			return UKismetMathLibrary::GetDirectionUnitVector(LocationBeforeChange, NewLocation);
		}
	case EMovingTargetDirectionMode::ForwardOnly:
		{
			return FVector(-1.f, 0.f, 0.f);
		}
	case EMovingTargetDirectionMode::None:
		break;
	}
	return FVector::ZeroVector;
}

void ATargetManager::UpdateTotalPossibleDamage()
{
	TotalPossibleDamage++;
}

bool ATargetManager::TrackingTargetIsDamageable() const
{
	if (GetBSConfig()->TargetConfig.TargetDamageType == ETargetDamageType::Hit || GetManagedTargets().IsEmpty())
	{
		return false;
	}
	if (GetManagedTargets().FindByPredicate([] (const TObjectPtr<ATarget>& Target)
	{
		return !Target->IsImmuneToTrackingDamage();
	}))
	{
		return true;
	}
	return false;
}

ATarget* ATargetManager::FindManagedTargetByGuid(const FGuid Guid) const
{
	const TObjectPtr<ATarget>* Found = ManagedTargets.FindByPredicate([&] (const ATarget* target)
	{
		return target->GetGuid() == Guid;
	});
	if (Found && Found->Get())
	{
		return Found->Get();
	}
	return nullptr;
}

float ATargetManager::GetDynamicValueFromCurveTable(const bool bIsSpawnArea, const int32 InTime) const
{
	UCompositeCurveTable* Table = bIsSpawnArea ? CompositeCurveTable_SpawnArea : CompositeCurveTable_TargetScale;
	const bool bThresholdMet = bIsSpawnArea ? (InTime > GetBSConfig()->DynamicSpawnAreaScaling.StartThreshold) : InTime > GetBSConfig()->DynamicTargetScaling.StartThreshold;
	const bool bIsCubicInterpolation = bIsSpawnArea ? GetBSConfig()->DynamicSpawnAreaScaling.bIsCubicInterpolation : GetBSConfig()->DynamicTargetScaling.bIsCubicInterpolation;

	float OutXY = -1.f;
	TEnumAsByte<EEvaluateCurveTableResult::Type> OutResult;

	if (bThresholdMet)
	{
		if (bIsCubicInterpolation)
		{
			UDataTableFunctionLibrary::EvaluateCurveTableRow(Table,  FName("Cubic_ThresholdMet"), InTime, OutResult, OutXY, "");
		}
		else
		{
			UDataTableFunctionLibrary::EvaluateCurveTableRow(Table,  FName("Linear_ThresholdMet"), InTime, OutResult, OutXY, "");
		}
	}
	else
	{
		UDataTableFunctionLibrary::EvaluateCurveTableRow(Table,  FName("Linear_PreThreshold"), InTime, OutResult, OutXY, "");
	}
	
	return OutXY;
}

void ATargetManager::UpdatePlayerSettings(const FPlayerSettings_Game& InPlayerSettings)
{
	PlayerSettings = InPlayerSettings;
	if (!ManagedTargets.IsEmpty())
	{
		for (ATarget* Target : GetManagedTargets())
		{
			Target->UpdatePlayerSettings(PlayerSettings);
		}
	}
}

/* --------------------- */
/* -- CommonScoreInfo -- */
/* --------------------- */

FAccuracyData ATargetManager::GetLocationAccuracy() const
{
	FAccuracyData OutData(5, 5);

	TArray<USpawnArea*> SpawnAreas = SpawnAreaManager->GetSpawnAreas();

	// Update TotalSpawns and TotalHits from all SpawnAreas
	for (int i = 0; i < SpawnAreas.Num(); i++)
	{
		const int32 Found = SpawnAreaManager->GetOutArrayIndexFromSpawnAreaIndex(i);
		
		if (Found == INDEX_NONE)
		{
			continue;
		}

		const int32 RowNum = Found / 5;
		const int32 ColNum = Found % 5;

		if (SpawnAreas[i]->GetTotalSpawns() != INDEX_NONE)
		{
			if (OutData.AccuracyRows[RowNum].TotalSpawns[ColNum] == INDEX_NONE)
			{
				OutData.AccuracyRows[RowNum].TotalSpawns[ColNum] = 0;
			}
			OutData.AccuracyRows[RowNum].TotalSpawns[ColNum] += SpawnAreas[i]->GetTotalSpawns();
			OutData.AccuracyRows[RowNum].TotalHits[ColNum] += SpawnAreas[i]->GetTotalHits();
		}
	}
	OutData.CalculateAccuracy();
	return OutData;
}

void ATargetManager::SaveQTable(FCommonScoreInfo& InCommonScoreInfo) const
{
	if (ReinforcementLearningComponent->GetReinforcementLearningMode() != EReinforcementLearningMode::None)
	{
		ReinforcementLearningComponent->ClearCachedTargetPairs();
		InCommonScoreInfo.UpdateQTable(ReinforcementLearningComponent->GetTArray_FromNdArray_QTable(),
		ReinforcementLearningComponent->GetTArray_FromNdArray_TrainingSamples(),
			ReinforcementLearningComponent->GetQTableRowLength(),
			ReinforcementLearningComponent->GetTotalTrainingSamples());
	}
}

/* ----------- */
/* -- Debug -- */
/* ----------- */

void ATargetManager::ShowDebug_SpawnBox(const bool bShow)
{
	bShowDebug_SpawnBox = bShow;
	if (bShowDebug_SpawnBox)
	{
		SpawnBox->SetHiddenInGame(false);
		SpawnVolume->SetHiddenInGame(false);
		SpawnBox->SetVisibility(true);
		SpawnVolume->SetVisibility(true);
		SpawnBox->ShapeColor = FColor::Blue;
		SpawnVolume->MarkRenderStateDirty();
		SpawnBox->MarkRenderStateDirty();
		/*TopBox->SetHiddenInGame(false);
		BottomBox->SetHiddenInGame(false);
		LeftBox->SetHiddenInGame(false);
		RightBox->SetHiddenInGame(false);
		ForwardBox->SetHiddenInGame(false);
		BackwardBox->SetHiddenInGame(false);*/
	}
	else
	{
		SpawnVolume->SetHiddenInGame(true);
		SpawnBox->SetHiddenInGame(true);
		SpawnVolume->SetVisibility(false);
		SpawnVolume->SetVisibility(false);
		SpawnVolume->MarkRenderStateDirty();
		SpawnBox->MarkRenderStateDirty();
		FlushPersistentDebugLines(GetWorld());
	}
}

void ATargetManager::ShowDebug_SpawnMemory(const bool bShow)
{
	bShowDebug_SpawnMemory = bShow;
	SpawnAreaManager->bShowDebug_SpawnMemory = bShow;
}

void ATargetManager::ShowDebug_ReinforcementLearningWidget(const bool bShow)
{
	if (!ReinforcementLearningComponent)
	{
		return;
	}

	bShowDebug_ReinforcementLearningWidget = bShow;

	ABSPlayerController* Controller = Cast<ABSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (bShowDebug_ReinforcementLearningWidget)
	{
		Controller->ShowRLAgentWidget(ReinforcementLearningComponent->OnQTableUpdate, ReinforcementLearningComponent->GetHeight(), ReinforcementLearningComponent->GetWidth(),
		                              ReinforcementLearningComponent->GetTArray_FromNdArray_QTableAvg());
		return;
	}
	Controller->HideRLAgentWidget();
}

void ATargetManager::ShowDebug_NumRecentNumActivated() const
{
	int NumRecent=0;
	int NumAct=0;
	for (const USpawnArea* Hey : SpawnAreaManager->GetSpawnAreas())
	{
		if (Hey->IsRecent())
		{
			NumRecent++;
		}
		if (Hey->IsActivated())
		{
			NumAct++;
		}
	}
	UE_LOG(LogTemp, Display, TEXT("NumRecent: %d NumActivated: %d"), NumRecent, NumAct);
}

void ATargetManager::ShowDebug_OverlappingVertices(const bool bShow)
{
	SpawnAreaManager->bShowDebug_OverlappingVertices = bShow;
}