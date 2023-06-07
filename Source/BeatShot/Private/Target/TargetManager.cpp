// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "Target/TargetManager.h"
#include "BSGameMode.h"
#include "Player/BSPlayerController.h"
#include "GlobalConstants.h"
#include "BeatShot/BSGameplayTags.h"
#include "Target/SphereTarget.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Target/ReinforcementLearningComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Target/SpawnPointManagerComponent.h"

DEFINE_LOG_CATEGORY(LogTargetManager);
using namespace Constants;

ATargetManager::ATargetManager()
{
	PrimaryActorTick.bCanEverTick = true;

	SpawnBox = CreateDefaultSubobject<UBoxComponent>("Spawn Box");
	RootComponent = SpawnBox;
	
	SpawnArea = CreateDefaultSubobject<UBoxComponent>("Spawn Area");
	TopBox = CreateDefaultSubobject<UBoxComponent>("Top Box");
	BottomBox = CreateDefaultSubobject<UBoxComponent>("Bottom Box");
	LeftBox = CreateDefaultSubobject<UBoxComponent>("Left Box");
	RightBox = CreateDefaultSubobject<UBoxComponent>("Right Box");
	ForwardBox = CreateDefaultSubobject<UBoxComponent>("Forward Box");
	BackwardBox = CreateDefaultSubobject<UBoxComponent>("Backward Box");

	SpawnBox->SetLineThickness(5.f);
	TopBox->SetLineThickness(5.f);
	BottomBox->SetLineThickness(5.f);
	LeftBox->SetLineThickness(5.f);
	RightBox->SetLineThickness(5.f);
	ForwardBox->SetLineThickness(5.f);
	BackwardBox->SetLineThickness(5.f);

	SpawnPointManager = CreateDefaultSubobject<USpawnPointManagerComponent>("Spawn Point Manager Component");
	ReinforcementLearningComponent = CreateDefaultSubobject<UReinforcementLearningComponent>("Reinforcement Learning Component");
	
	ConsecutiveTargetsHit = 0;
	BSConfig = FBSConfig();
	PlayerSettings = FPlayerSettings_Game();
	LastTargetSpawnedCenter = false;
	ShouldSpawn = false;
	bShowDebug_SpawnBox = false;
	bShowDebug_SpawnMemory = false;
	bShowDebug_ReinforcementLearningWidget = false;
	SpawnPoint = nullptr;
	PreviousSpawnPoint = nullptr;
	TargetScale = FVector(1.f);
	StaticExtrema = FExtrema();
	StaticExtents = FVector();
	ConsecutiveTargetsHit = 0;
	DynamicSpawnScale = 0;
	ManagedTargets = TArray<ASphereTarget*>();
	AllSpawnLocations = TArray<FVector>();
}

void ATargetManager::BeginPlay()
{
	Super::BeginPlay();
}

void ATargetManager::Destroyed()
{
	if (!GetManagedTargets().IsEmpty())
	{
		for (ASphereTarget* Target : GetManagedTargets())
		{
			Target->Destroy();
		}
	}
	Super::Destroyed();
}

void ATargetManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATargetManager::InitTargetManager(const FBSConfig& InBSConfig, const FPlayerSettings_Game& InPlayerSettings)
{
	/* Initialize local copy of FBSConfig */
	BSConfig = InBSConfig;
	PlayerSettings = InPlayerSettings;

	BSConfig.TargetConfig.bUseSeparateOutlineColor = InPlayerSettings.bUseSeparateOutlineColor;
	if (BSConfig.TargetConfig.TargetSpawningPolicy == ETargetSpawningPolicy::UpfrontOnly)
	{
		BSConfig.TargetConfig.OnSpawnColor = InPlayerSettings.InactiveTargetColor;
	}
	else
	{
		BSConfig.TargetConfig.OnSpawnColor = InPlayerSettings.StartTargetColor;
	}
	BSConfig.TargetConfig.InactiveTargetColor = InPlayerSettings.InactiveTargetColor;
	BSConfig.TargetConfig.StartColor = InPlayerSettings.StartTargetColor;
	BSConfig.TargetConfig.PeakColor = InPlayerSettings.PeakTargetColor;
	BSConfig.TargetConfig.EndColor = InPlayerSettings.EndTargetColor;
	
	/* GameMode menu uses the full width, while box bounds are only half width / half height */
	StaticExtents = BSConfig.TargetConfig.GenerateTargetManagerBoxBounds();

	/* Set new location & box extent */
	SpawnBox->SetRelativeLocation(BSConfig.TargetConfig.GenerateSpawnBoxLocation());
	SpawnBox->SetBoxExtent(StaticExtents);
	StaticExtrema.Min = SpawnBox->Bounds.GetBoxExtrema(0);
	StaticExtrema.Max = SpawnBox->Bounds.GetBoxExtrema(1);

	UpdateSpawnArea();

	if (BSConfig.TargetConfig.TargetDamageType == ETargetDamageType::Tracking)
	{
		BSConfig.TargetConfig.OnSpawn_ApplyTags.AddTag(FBSGameplayTags::Get().Target_State_Tracking);
	}

	// Initialize SpawnPointManager and SpawnPoints
	SpawnPointManager->InitSpawnPointManager(InBSConfig, GetBoxOrigin(), StaticExtents);
	if (BSConfig.TargetConfig.TargetDistributionPolicy == ETargetDistributionPolicy::Grid)
	{
		AllSpawnLocations = SpawnPointManager->InitializeSpawnPoints(GenerateBoxExtremaGrid());
	}
	else
	{
		AllSpawnLocations = SpawnPointManager->InitializeSpawnPoints(GetBoxExtrema(false));
	}

	// Initialize Dynamic SpawnBox
	if (IsDynamicBoundsScalingPolicy(BSConfig.TargetConfig.BoundsScalingPolicy))
	{
		SetBoxExtents_Dynamic();
	}

	// Enable / Disable Reinforcement Learning
	if (BSConfig.AIConfig.bEnableReinforcementLearning)
	{
		FRLAgentParams Params;
		Params.InQTable = GetScoreInfoFromDefiningConfig(InBSConfig.DefiningConfig).QTable;
		Params.DefaultMode = BSConfig.DefiningConfig.BaseGameMode;
		Params.CustomGameModeName = BSConfig.DefiningConfig.CustomGameModeName;
		Params.Size = SpawnPointManager->GetSpawnPoints().Num();
		Params.SpawnPointsHeight = SpawnPointManager->GetSpawnPointsHeight();
		Params.SpawnPointsWidth = SpawnPointManager->GetSpawnPointsWidth();
		Params.InEpsilon = BSConfig.AIConfig.Epsilon;
		Params.InGamma = BSConfig.AIConfig.Gamma;
		Params.InAlpha = BSConfig.AIConfig.Alpha;
		ReinforcementLearningComponent->SetActive(true);
		ReinforcementLearningComponent->Init(Params);
	}
	else
	{
		ReinforcementLearningComponent->SetActive(false);
		ReinforcementLearningComponent->Deactivate();
	}

	// Initialize first SpawnPoint, and spawn any targets if needed
	switch(BSConfig.TargetConfig.TargetSpawningPolicy)
	{
	case ETargetSpawningPolicy::None:
	case ETargetSpawningPolicy::RuntimeOnly:
		FindNextTargetProperties();
		break;
	case ETargetSpawningPolicy::UpfrontOnly:
		SpawnUpfrontOnlyTargets();
	}
	
	UE_LOG(LogTargetManager, Display, TEXT("SpawnBoxWidth %f SpawnBoxHeight %f"), StaticExtents.Y * 2, StaticExtents.Z * 2);
}

void ATargetManager::SetShouldSpawn(const bool bShouldSpawn)
{
	ShouldSpawn = bShouldSpawn;
}

void ATargetManager::OnAudioAnalyzerBeat()
{
	// TODO: Needs to be more distinct differences when activating a SpawnPoint.
	// TODO: Currently, still finding a new SpawnPoint every beat for BeatTrack, but
	// TODO: don't even use the SpawnPoint for the mode.
	// TODO: Maybe a distinction between activating a deactivated SpawnPoint/Target
	// TODO: and activating an always active SpawnPoint/Target
	
	if (!ShouldSpawn) return;
	
	// We're relying on FindNextTargetProperties to have a fresh SpawnPoint lined up
	if (!SpawnPoint)
	{
		UE_LOG(LogTemp, Display, TEXT("Invalid SpawnPoint"));
		FindNextTargetProperties();
		return;
	}

	bool bSuccessfulActivation = false;
	// Handles spawning and activating the spawned point
	if (BSConfig.TargetConfig.TargetSpawningPolicy == ETargetSpawningPolicy::RuntimeOnly)
	{
		bSuccessfulActivation = HandleRuntimeSpawnAndActivation(SpawnPoint);
	}
	
	// Handle activating an existing target if we didn't spawn one
	if (!bSuccessfulActivation)
	{
		bSuccessfulActivation = BSConfig.TargetConfig.bContinuouslyActivate ? ActivateTarget(SpawnPoint) : TryActivateExistingTarget();
	}

	// Handle a permanently activated target
	// TODO: Clean up
	if (!bSuccessfulActivation && BSConfig.TargetConfig.TargetDeactivationConditions.Contains(ETargetDeactivationCondition::Persistant))
	{
		if (GetManagedTargets().Num() > 0)
		{
			for (const TObjectPtr<ASphereTarget> Target : GetManagedTargets())
			{
				if (!bSuccessfulActivation)
				{
					bSuccessfulActivation = ActivateTarget(Target);
				}
				else
				{
					ActivateTarget(Target);
				}
			}
		}
	}
	
	// TODO: Initialize a moving target

	// RL Component stuff
	if (bSuccessfulActivation)
	{
		// Add to ActiveTargetParis if using RL Comp
		if (ReinforcementLearningComponent->IsActive() && SpawnPointManager->IsSpawnPointValid(PreviousSpawnPoint) && PreviousSpawnPoint != SpawnPoint)
		{
			ReinforcementLearningComponent->AddToActiveTargetPairs(PreviousSpawnPoint->ChosenPoint, SpawnPoint->ChosenPoint);
		}
	}

	// Debug stuff
	if (bShowDebug_SpawnMemory)
	{
		ShowDebug_NumRecentNumActivated();
	}

	// Remove recent targets if based on number of targets
	if (BSConfig.TargetConfig.RecentTargetMemoryPolicy == ERecentTargetMemoryPolicy::NumTargetsBased)
	{
		SpawnPointManager->RefreshRecentTargetFlags();
	}
	
	// Immediately find the next target scale and SpawnPoint
	FindNextTargetProperties();
}

bool ATargetManager::HandleRuntimeSpawnAndActivation(const TObjectPtr<USpawnPoint> InSpawnPoint)
{
	if ((BSConfig.TargetConfig.bContinuouslySpawn || GetManagedTargets().IsEmpty()) && InSpawnPoint)
	{
		if (SpawnTarget(InSpawnPoint))
		{
			return ActivateTarget(InSpawnPoint);
		}
	}
	return false;
}

bool ATargetManager::TryActivateExistingTarget()
{
	// TODO: might need method to manage reactivation

	// Points that are referencing managed targets, but are not activated
	TArray<USpawnPoint*> Points = SpawnPointManager->GetDeactivatedManagedPoints();
	if (Points.IsEmpty())
	{
		return false;
	}
	const int32 RandomPointsIndex = FMath::RandRange(0, Points.Num() - 1);
	if (Points.IsValidIndex(RandomPointsIndex))
	{
		return false;
	}
	const USpawnPoint* Point = Points[RandomPointsIndex];
	if (!GetManagedTargets().IsValidIndex(Point->GetManagedTargetIndex()))
	{
		return false;
	}
	const int32 ManagedTargetIndex = Point->GetManagedTargetIndex();
	if (ActivateTarget(ManagedTargets[ManagedTargetIndex]))
	{
		return true;
	}
	return false;
}

bool ATargetManager::SpawnTarget(const TObjectPtr<USpawnPoint> InSpawnPoint)
{
	ASphereTarget* Target = GetWorld()->SpawnActorDeferred<ASphereTarget>(TargetToSpawn, FTransform(FRotator::ZeroRotator, InSpawnPoint->ChosenPoint, InSpawnPoint->GetScale()), this, nullptr,
	                                                                      ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	Target->InitTarget(BSConfig.TargetConfig);
	Target->OnTargetDamageEventOrTimeout.AddDynamic(this, &ATargetManager::OnTargetHealthChangedOrExpired);
	Target->FinishSpawning(FTransform(), true);
	InSpawnPoint->SetGuid(Target->GetGuid());
	InSpawnPoint->SetManagedTargetIndex(AddToManagedTargets(Target, InSpawnPoint));
	return Target != nullptr;
}

void ATargetManager::SpawnUpfrontOnlyTargets()
{
	switch(BSConfig.TargetConfig.TargetDistributionPolicy) {
	case ETargetDistributionPolicy::None:
	case ETargetDistributionPolicy::HeadshotHeightOnly:
	case ETargetDistributionPolicy::EdgeOnly:
	case ETargetDistributionPolicy::FullRange:
		for (int i = 0; i < BSConfig.TargetConfig.NumUpfrontTargetsToSpawn; i++)
		{
			FindNextTargetProperties();
			if (SpawnPoint)
			{
				SpawnTarget(SpawnPoint);
			}
		}
		break;
	case ETargetDistributionPolicy::Grid:
		for (int i = 0; i < SpawnPointManager->GetSpawnPoints().Num(); i++)
		{
			SpawnPointManager->GetSpawnPointsRef()[i]->SetScale(GetNextTargetScale());
			SpawnTarget(SpawnPointManager->GetSpawnPointsRef()[i]);
		}
		FindNextTargetProperties();
		break;
	default:
		break;
	}
}

bool ATargetManager::ActivateTarget(const TObjectPtr<USpawnPoint> InSpawnPoint) const
{
	// TargetManager handles all TargetActivationResponses
	// Each target handles their own TargetDeactivationResponses & TargetDestructionConditions
	
	if (!GetManagedTargets().IsValidIndex(InSpawnPoint->GetManagedTargetIndex()))
	{
		return false;
	}

	const TObjectPtr<ASphereTarget> Target = GetManagedTargets()[InSpawnPoint->GetManagedTargetIndex()];

	if (BSConfig.TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::AddImmunity))
	{
		Target->ApplyImmunityEffect();
	}
	else if (BSConfig.TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::RemoveImmunity))
	{
		Target->RemoveImmunityEffect();
	}
	else if (BSConfig.TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::ToggleImmunity))
	{
		Target->IsTargetImmune() ? Target->RemoveImmunityEffect() : Target->ApplyImmunityEffect();
	}
	
	if (BSConfig.TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::ChangeVelocity))
	{
		Target->SetTargetSpeed(FMath::FRandRange(BSConfig.TargetConfig.MinTargetSpeed, BSConfig.TargetConfig.MaxTargetSpeed));
	}
	if (BSConfig.TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::ChangeDirection))
	{
		const FVector NewDirection = UKismetMathLibrary::GetDirectionUnitVector(Target->GetActorLocation(),
			GetRandomMovingTargetEndLocation(Target->GetActorLocation(), Target->GetTargetSpeed()));
		Target->SetTargetDirection(NewDirection);
	}
	if (BSConfig.TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::ChangeScale))
	{
		Target->SetSphereScale(GetNextTargetScale());
	}
	
	Target->ActivateTarget(BSConfig.TargetConfig.TargetMaxLifeSpan);
	SpawnPointManager->FlagSpawnPointAsActivated(InSpawnPoint->GetGuid());
	// Broadcast that a target has been activated, mainly to keep track of total target spawns on PlayerHUD
	OnTargetActivatedOrSpawned.Broadcast();
	
	return true;
}

bool ATargetManager::ActivateTarget(const TObjectPtr<ASphereTarget> InTarget) const
{
	// TargetManager handles all TargetActivationResponses
	// Each target handles their own TargetDeactivationResponses & TargetDestructionConditions
	
	if (!InTarget || !SpawnPointManager->IsSpawnPointValid(SpawnPointManager->FindSpawnPointFromGuid(InTarget->GetGuid())))
	{
		return false;
	}

	if (BSConfig.TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::AddImmunity))
	{
		InTarget->ApplyImmunityEffect();
	}
	else if (BSConfig.TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::RemoveImmunity))
	{
		InTarget->RemoveImmunityEffect();
	}
	else if (BSConfig.TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::ToggleImmunity))
	{
		InTarget->IsTargetImmune() ? InTarget->RemoveImmunityEffect() : InTarget->ApplyImmunityEffect();
	}
	
	if (BSConfig.TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::ChangeVelocity))
	{
		InTarget->SetTargetSpeed(FMath::FRandRange(BSConfig.TargetConfig.MinTargetSpeed, BSConfig.TargetConfig.MaxTargetSpeed));
	}
	if (BSConfig.TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::ChangeDirection))
	{
		const FVector NewDirection = UKismetMathLibrary::GetDirectionUnitVector(InTarget->GetActorLocation(),
			GetRandomMovingTargetEndLocation(InTarget->GetActorLocation(), InTarget->GetTargetSpeed()));
		InTarget->SetTargetDirection(NewDirection);
	}
	if (BSConfig.TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::ChangeScale))
	{
		InTarget->SetSphereScale(GetNextTargetScale());
	}
	
	InTarget->ActivateTarget(BSConfig.TargetConfig.TargetMaxLifeSpan);
	SpawnPointManager->FlagSpawnPointAsActivated(InTarget->GetGuid());
	// Broadcast that a target has been activated, mainly to keep track of total target spawns on PlayerHUD
	OnTargetActivatedOrSpawned.Broadcast();
	
	return true;
}

// Deactivation and Destruction

void ATargetManager::OnTargetHealthChangedOrExpired(const FTargetDamageEvent& TargetDamageEvent)
{
	UpdateConsecutiveTargetsHit(TargetDamageEvent.TimeAlive);
	UpdateDynamicSpawnScale(TargetDamageEvent.TimeAlive);
	HandleTargetExpirationDelegate(BSConfig.TargetConfig.TargetDamageType, TargetDamageEvent);
	HandleManagedTargetRemoval(BSConfig.TargetConfig.TargetDestructionConditions, TargetDamageEvent);

	if (ReinforcementLearningComponent->IsActive())
	{
		ReinforcementLearningComponent->UpdateReinforcementLearningReward(TargetDamageEvent.Location, TargetDamageEvent.TimeAlive != INDEX_NONE);
		ReinforcementLearningComponent->UpdateReinforcementLearningComponent(SpawnPointManager);
	}
	
	SpawnPointManager->HandleRecentTargetRemoval(BSConfig.TargetConfig.RecentTargetMemoryPolicy, TargetDamageEvent);
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

void ATargetManager::UpdateDynamicSpawnScale(const float TimeAlive)
{
	if (TimeAlive == -1)
	{
		DynamicSpawnScale = FMath::Clamp(DynamicSpawnScale - 5, 0, 100);
	}
	else
	{
		DynamicSpawnScale = FMath::Clamp(DynamicSpawnScale + 1, 0, 100);
	}
}

void ATargetManager::HandleTargetExpirationDelegate(const ETargetDamageType& DamageType, const FTargetDamageEvent& TargetDamageEvent) const
{
	if (DamageType == ETargetDamageType::Tracking)
	{
		OnBeatTrackTargetDamaged.Broadcast(TargetDamageEvent.DamageDelta, TargetDamageEvent.TotalPossibleDamage);
	}
	else
	{
		OnTargetDeactivated.Broadcast(TargetDamageEvent.TimeAlive, ConsecutiveTargetsHit, FVector(TargetDamageEvent.Location.X,
			TargetDamageEvent.Location.Y, TargetDamageEvent.Location.Z + SphereTargetRadius * TargetDamageEvent.Scale.Z));
	}
}

void ATargetManager::HandleManagedTargetRemoval(const TArray<ETargetDestructionCondition>& TargetDestructionConditions, const FTargetDamageEvent& TargetDamageEvent)
{
	if (!TargetDestructionConditions.Contains(ETargetDestructionCondition::Persistant))
	{
		if (TargetDestructionConditions.Contains(ETargetDestructionCondition::OnDeactivation))
		{
			UE_LOG(LogTemp, Display, TEXT("Removed based on OnDeactivation"));
			RemoveFromManagedTargets(TargetDamageEvent.Guid);
		}
		if (TargetDestructionConditions.Contains(ETargetDestructionCondition::OnExpiration))
		{
			UE_LOG(LogTemp, Display, TEXT("Removed based on OnExpiration"));
			RemoveFromManagedTargets(TargetDamageEvent.Guid);
		}
		if (TargetDestructionConditions.Contains(ETargetDestructionCondition::OnHealthReachedZero) &&
			TargetDamageEvent.CurrentHealth <= 0.f)
		{
			UE_LOG(LogTemp, Display, TEXT("Removed based on OnHealthReachedZero"));
			RemoveFromManagedTargets(TargetDamageEvent.Guid);
		}
		if (TargetDestructionConditions.Contains(ETargetDestructionCondition::OnAnyExternalDamageTaken) &&
			TargetDamageEvent.TimeAlive != -1)
		{
			UE_LOG(LogTemp, Display, TEXT("Removed based on OnAnyExternalDamageTaken"));
			RemoveFromManagedTargets(TargetDamageEvent.Guid);
		}
	}
}

// Finding next Point

void ATargetManager::FindNextTargetProperties()
{
	const FVector NewScale = GetNextTargetScale();

	if (SpawnPoint)
	{
		LastTargetSpawnedCenter = SpawnPoint->ChosenPoint.Equals(GetBoxOrigin());
		// Assign SpawnPoint address to PreviousSpawnPoint just before finding a new SpawnPoint
		PreviousSpawnPoint = SpawnPoint;
	}
	else
	{
		LastTargetSpawnedCenter = false;
		PreviousSpawnPoint = nullptr;
	}
	
	SpawnPoint = GetNextSpawnPoint(BSConfig.TargetConfig.BoundsScalingPolicy, NewScale);
	if (SpawnPoint && SpawnPointManager->GetSpawnPoints().IsValidIndex(SpawnPoint->Index))
	{
		for (USpawnPoint* Point : SpawnPointManager->GetActivatedOrRecentSpawnPoints())
		{
			if (FVector::Distance(SpawnPoint->ChosenPoint, Point->ChosenPoint) < 200.f)
			{
				UE_LOG(LogTemp, Display, TEXT("Distance less than 200: %f"), FVector::Distance(SpawnPoint->ChosenPoint, Point->ChosenPoint));
			}
		}
		SpawnPoint->SetScale(NewScale);
	}
}

FVector ATargetManager::GetNextTargetScale() const
{
	if (BSConfig.TargetConfig.ConsecutiveTargetScalePolicy == EConsecutiveTargetScalePolicy::SkillBased)
	{
		const float NewFactor = DynamicSpawnCurve->GetFloatValue(DynamicSpawnScale);
		return FVector(UKismetMathLibrary::Lerp(BSConfig.TargetConfig.MinTargetScale, BSConfig.TargetConfig.MaxTargetScale, NewFactor));
	}
	return FVector(FMath::FRandRange(BSConfig.TargetConfig.MinTargetScale, BSConfig.TargetConfig.MaxTargetScale));
}

USpawnPoint* ATargetManager::GetNextSpawnPoint(const EBoundsScalingPolicy BoundsScalingPolicy, const FVector& NewTargetScale) const
{
	// Change the BoxExtent of the SpawnBox if dynamic
	if (IsDynamicBoundsScalingPolicy(BoundsScalingPolicy))
	{
		SetBoxExtents_Dynamic();
	}

	// Get valid spawn locations based on TargetDistributionPolicy, BoundsScalingPolicy
	TArray<FVector> OpenLocations = GetValidSpawnLocations(NewTargetScale, BSConfig.TargetConfig.TargetDistributionPolicy, BoundsScalingPolicy);
	
	if (OpenLocations.IsEmpty())
	{
		UE_LOG(LogTargetManager, Warning, TEXT("OpenLocations is empty."));
		return nullptr;
	}
	
	if (OpenLocations.Contains(GetBoxOrigin()))
	{
		if (BSConfig.TargetConfig.bSpawnAtOriginWheneverPossible)
		{
			return SpawnPointManager->FindSpawnPointFromLocation(GetBoxOrigin());
		}
		if (BSConfig.TargetConfig.bSpawnEveryOtherTargetInCenter && SpawnPoint && SpawnPoint != SpawnPointManager->FindSpawnPointFromLocation(GetBoxOrigin()))
		{
			return SpawnPointManager->FindSpawnPointFromLocation(GetBoxOrigin());
		}
	}

	/* Get location from RL Comp if specified */
	if (ReinforcementLearningComponent->IsActive())
	{
		if (USpawnPoint* ChosenPoint = TryGetSpawnPointFromReinforcementLearningComponent(OpenLocations))
		{
			return ChosenPoint;
		}
		UE_LOG(LogTargetManager, Warning, TEXT("Unable to Spawn Location suggested by RLAgent."));
	}
	
	if (USpawnPoint* Found = SpawnPointManager->FindSpawnPointFromLocation(OpenLocations[FMath::RandRange(0, OpenLocations.Num() - 1)]))
	{
		Found->SetChosenPointAsRandomSubPoint(Found->GetBorderingDirections(OpenLocations, GetBoxExtrema(true)));
		return Found;
	}
	//UE_LOG(LogTargetManager, Display, TEXT("Found CornerPoint %s Found CenterPoint %s Found ChosenPoint %s"), *Found->CornerPoint.ToString(), *Found->CenterPoint.ToString(), *Found->ChosenPoint.ToString());
	return nullptr;
}

TArray<FVector> ATargetManager::GetValidSpawnLocations(const FVector& Scale, const ETargetDistributionPolicy& DistributionPolicy, const EBoundsScalingPolicy& BoundsScalingPolicy) const
{
	TArray<FVector> ValidSpawnLocations;
	const FExtrema Extrema = GetBoxExtrema(BoundsScalingPolicy == EBoundsScalingPolicy::Dynamic);
	
	switch (DistributionPolicy)
	{
	case ETargetDistributionPolicy::EdgeOnly:
		HandleEdgeOnlySpawnLocations(ValidSpawnLocations, Extrema);
		SpawnPointManager->RemoveOverlappingPointsFromSpawnLocations(ValidSpawnLocations, Scale, bShowDebug_SpawnMemory);
		SpawnPointManager->RemoveEdgePoints(ValidSpawnLocations, Extrema);
		break;
	case ETargetDistributionPolicy::FullRange:
		HandleFullRangeSpawnLocations(ValidSpawnLocations, Extrema);
		SpawnPointManager->RemoveOverlappingPointsFromSpawnLocations(ValidSpawnLocations, Scale, bShowDebug_SpawnMemory);
		SpawnPointManager->RemoveEdgePoints(ValidSpawnLocations, Extrema);
		break;
	case ETargetDistributionPolicy::Grid:
		HandleGridSpawnLocations(ValidSpawnLocations, bShowDebug_SpawnMemory);
		break;
	case ETargetDistributionPolicy::None:
	case ETargetDistributionPolicy::HeadshotHeightOnly:
	default:
		ValidSpawnLocations = GetAllSpawnLocations();
		SpawnPointManager->RemoveOverlappingPointsFromSpawnLocations(ValidSpawnLocations, Scale, bShowDebug_SpawnMemory);
		SpawnPointManager->RemoveEdgePoints(ValidSpawnLocations, Extrema);
		break;
	}
	
	if (bShowDebug_SpawnMemory)
	{
		SpawnPointManager->DrawDebug_Boxes(ValidSpawnLocations, FColor::Emerald, 6);
	}
	
	return ValidSpawnLocations;
}

void ATargetManager::HandleEdgeOnlySpawnLocations(TArray<FVector>& ValidSpawnLocations, const FExtrema &Extrema, const bool bShowDebug) const
{
	const float MaxY = Extrema.Max.Y - SpawnPointManager->GetSpawnMemoryIncY();
	const float MaxZ = Extrema.Max.Z - SpawnPointManager->GetSpawnMemoryIncZ();
	const float OriginX = GetBoxOrigin().X;
	for (float Y = Extrema.Min.Y; Y < Extrema.Max.Y; Y += SpawnPointManager->GetSpawnMemoryIncY())
	{
		ValidSpawnLocations.AddUnique(FVector(OriginX, Y, Extrema.Min.Z));
		ValidSpawnLocations.AddUnique(FVector(OriginX, Y, MaxZ));
	}
	for (float Z = Extrema.Min.Z; Z < Extrema.Max.Z; Z += SpawnPointManager->GetSpawnMemoryIncZ())
	{
		ValidSpawnLocations.AddUnique(FVector(OriginX, Extrema.Min.Y, Z));
		ValidSpawnLocations.AddUnique(FVector(OriginX, MaxY, Z));
	}
	ValidSpawnLocations.Add(GetBoxOrigin());
	if (bShowDebug)
	{
		const TArray<FVector> RemovedLocations = GetAllSpawnLocations().FilterByPredicate([&] (const FVector& Vector)
		{
			return !ValidSpawnLocations.Contains(Vector);
		});
		SpawnPointManager->DrawDebug_Boxes(RemovedLocations, FColor::Red, 4);
	}
}

void ATargetManager::HandleFullRangeSpawnLocations(TArray<FVector>& ValidSpawnLocations, const FExtrema &Extrema, const bool bShowDebug) const
{
	TArray<FVector> RemovedLocations;
	ValidSpawnLocations = GetAllSpawnLocations().FilterByPredicate([&](const FVector& Vector)
	{
		if (Vector.Y < Extrema.Min.Y || Vector.Y >= Extrema.Max.Y || Vector.Z < Extrema.Min.Z || Vector.Z >= Extrema.Max.Z)
		{
			if (bShowDebug)
			{
				RemovedLocations.Add(Vector);
			}
			return false;
		}
		return true;
	});
	if (bShowDebug)
	{
		SpawnPointManager->DrawDebug_Boxes(RemovedLocations, FColor::Red, 4);
	}
}

void ATargetManager::HandleGridSpawnLocations(TArray<FVector>& ValidSpawnLocations, const bool bShowDebug) const
{
	ValidSpawnLocations = GetAllSpawnLocations();
	switch (BSConfig.TargetConfig.TargetActivationSelectionPolicy)
	{
	case ETargetActivationSelectionPolicy::None:
	case ETargetActivationSelectionPolicy::Random:
		HandleFilterActivated(ValidSpawnLocations, bShowDebug);
		HandleFilterRecent(ValidSpawnLocations, bShowDebug);
		break;
	case ETargetActivationSelectionPolicy::Bordering:
		HandleBorderingSelectionPolicy(ValidSpawnLocations, bShowDebug);
		break;
	default:
		break;
	}
}

void ATargetManager::HandleBorderingSelectionPolicy(TArray<FVector>& ValidSpawnLocations, const bool bShowDebug) const
{
	// Filter out non-bordering points
	ValidSpawnLocations = ValidSpawnLocations.FilterByPredicate([&] (const FVector& Vector)
	{
		if (const USpawnPoint* FoundPoint = SpawnPointManager->FindSpawnPointFromLocation(Vector))
		{
			if (SpawnPoint)
			{
				return SpawnPoint->GetBorderingIndices().Contains(FoundPoint->Index);
			}
			return true;
		}
		return false;
	});
	
	// First try filtering out activated points
	TArray<FVector> NoActive = ValidSpawnLocations;
	HandleFilterActivated(NoActive, bShowDebug);
	
	if (!NoActive.IsEmpty())
	{
		ValidSpawnLocations = NoActive;
		
		// Then try filtering out recent points too
		TArray<FVector> NoRecent = NoActive;
		HandleFilterRecent(NoRecent, bShowDebug);

		if (!NoRecent.IsEmpty())
		{
			ValidSpawnLocations = NoRecent;
		}
	}
}

void ATargetManager::HandleFilterActivated(TArray<FVector>& ValidSpawnLocations, const bool bShowDebug) const
{
	TArray<FVector> RemovedLocations;
	ValidSpawnLocations = ValidSpawnLocations.FilterByPredicate([&] (const FVector& Vector)
	{
		if (const USpawnPoint* FoundPoint = SpawnPointManager->FindSpawnPointFromLocation(Vector))
		{
			if (FoundPoint->IsActivated())
			{
				RemovedLocations.Add(Vector);
				return false;
			}
			return true;
		}
		return false;
	});
	if (bShowDebug)
	{
		SpawnPointManager->DrawDebug_Boxes(RemovedLocations, FColor::Red, 4);
	}
}

void ATargetManager::HandleFilterRecent(TArray<FVector>& ValidSpawnLocations, const bool bShowDebug) const
{
	TArray<FVector> RemovedLocations;
	ValidSpawnLocations = ValidSpawnLocations.FilterByPredicate([&] (const FVector& Vector)
	{
		if (const USpawnPoint* FoundPoint = SpawnPointManager->FindSpawnPointFromLocation(Vector))
		{
			if (FoundPoint->IsRecent())
			{
				RemovedLocations.Add(Vector);
				return false;
			}
			return true;
		}
		return false;
	});
	if (bShowDebug)
	{
		SpawnPointManager->DrawDebug_Boxes(RemovedLocations, FColor::Red, 4);
	}
}

void ATargetManager::UpdateSpawnArea() const
{
	const float LocationX = GetBoxOrigin().X - BSConfig.TargetConfig.MoveForwardDistance * 0.5f;
	// X Extent should be half move forward distance + max sphere radius
	const float ExtentX = BSConfig.TargetConfig.MoveForwardDistance * 0.5f + BSConfig.TargetConfig.MaxTargetScale * SphereTargetRadius + 10.f;

	const FVector DynamicExtent =  SpawnBox->Bounds.BoxExtent;
		
	SpawnArea->SetRelativeLocation(FVector(LocationX, GetBoxOrigin().Y, GetBoxOrigin().Z));
	SpawnArea->SetBoxExtent(FVector(ExtentX, DynamicExtent.Y, DynamicExtent.Z));
		
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

ASphereTarget* ATargetManager::GetManagedTargetByGuid(const FGuid Guid)
{
	const TObjectPtr<ASphereTarget>* Found = ManagedTargets.FindByPredicate([&] (const ASphereTarget* SphereTarget)
	{
		return SphereTarget->GetGuid() == Guid;
	});
	if (Found)
	{
		return *Found;
	}
	return nullptr;
}

FVector ATargetManager::GetRandomMovingTargetEndLocation(const FVector& LocationBeforeChange, const float TargetSpeed) const
{
	const FVector NewExtent = FVector(BSConfig.TargetConfig.MoveForwardDistance * 0.5, GetBoxExtents_Static().Y * 0.5, GetBoxExtents_Static().Z * 0.5);
	const FVector BotLeft = UKismetMathLibrary::RandomPointInBoundingBox(SpawnArea->Bounds.Origin + FVector(0, -GetBoxExtents_Static().Y * 0.5, -GetBoxExtents_Static().Z * 0.5), NewExtent);
	const FVector BotRight = UKismetMathLibrary::RandomPointInBoundingBox(SpawnArea->Bounds.Origin + FVector(0, GetBoxExtents_Static().Y * 0.5, -GetBoxExtents_Static().Z * 0.5), NewExtent);
	const FVector TopLeft = UKismetMathLibrary::RandomPointInBoundingBox(SpawnArea->Bounds.Origin + FVector(0, -GetBoxExtents_Static().Y * 0.5, GetBoxExtents_Static().Z * 0.5), NewExtent);
	const FVector TopRight = UKismetMathLibrary::RandomPointInBoundingBox(SpawnArea->Bounds.Origin + FVector(0, GetBoxExtents_Static().Y * 0.5, GetBoxExtents_Static().Z * 0.5), NewExtent);

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
	return NewLocation + UKismetMathLibrary::GetDirectionUnitVector(LocationBeforeChange, NewLocation) * TargetSpeed * BSConfig.TargetConfig.TargetSpawnCD;
}

FExtrema ATargetManager::GetBoxExtrema(const bool bDynamic) const
{
	if (bDynamic)
	{
		return FExtrema(SpawnBox->Bounds.GetBoxExtrema(0), SpawnBox->Bounds.GetBoxExtrema(1));
	}
	return StaticExtrema;
}

FExtrema ATargetManager::GenerateBoxExtremaGrid() const
{
	const float MaxTargetSize = BSConfig.TargetConfig.MaxTargetScale * SphereTargetDiameter;
	const float HSpacing = BSConfig.GridConfig.GridSpacing.X + MaxTargetSize;
	const float VSpacing = BSConfig.GridConfig.GridSpacing.Y + MaxTargetSize;
	const float HalfWidth = (HSpacing * (BSConfig.GridConfig.NumHorizontalGridTargets - 1)) / 2.f;
	const float HalfHeight = (VSpacing * (BSConfig.GridConfig.NumVerticalGridTargets - 1)) / 2.f;
	
	return FExtrema(FVector(GetBoxOrigin().X, -HalfWidth + GetBoxOrigin().Y, -HalfHeight + GetBoxOrigin().Z),
		FVector(GetBoxOrigin().X, HalfWidth + 1.f + GetBoxOrigin().Y, HalfHeight + 1.f + GetBoxOrigin().Z));
}

int32 ATargetManager::AddToManagedTargets(TObjectPtr<ASphereTarget> SpawnTarget, const TObjectPtr<USpawnPoint> AssociatedSpawnPoint)
{
	TArray<TObjectPtr<ASphereTarget>> Targets = GetManagedTargets();
	const int32 NewIndex = Targets.Add(SpawnTarget);
	ManagedTargets = Targets;
	AssociatedSpawnPoint->SetIsCurrentlyManaged(true);
	return NewIndex;
}

void ATargetManager::RemoveFromManagedTargets(const FGuid GuidToRemove)
{
	const TArray<TObjectPtr<ASphereTarget>> Targets = GetManagedTargets().FilterByPredicate([&] (const ASphereTarget* OtherTarget)
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
	if (USpawnPoint* AssociatedSpawnPoint = SpawnPointManager->FindSpawnPointFromGuid(GuidToRemove))
	{
		AssociatedSpawnPoint->SetIsCurrentlyManaged(false);
	}
	ManagedTargets = Targets;
}

void ATargetManager::SetBoxExtents_Dynamic() const
{
	const float NewFactor = DynamicSpawnCurve->GetFloatValue(DynamicSpawnScale);
	const float LerpY = UKismetMathLibrary::Lerp(GetBoxExtents_Static().Y, GetBoxExtents_Static().Y * 0.5f, NewFactor);
	const float LerpZ = UKismetMathLibrary::Lerp(GetBoxExtents_Static().Z, GetBoxExtents_Static().Z * 0.5f, NewFactor);
	const float Y = FMath::GridSnap(LerpY, SpawnPointManager->GetSpawnMemoryIncY());
	const float Z = FMath::GridSnap(LerpZ, SpawnPointManager->GetSpawnMemoryIncZ());
	SpawnBox->SetBoxExtent(FVector(0, Y, Z));
	UpdateSpawnArea();
}

void ATargetManager::UpdatePlayerSettings(const FPlayerSettings_Game& InPlayerSettings)
{
	PlayerSettings = InPlayerSettings;
	if (!ManagedTargets.IsEmpty())
	{
		for (ASphereTarget* Target : GetManagedTargets())
		{
			Target->UpdatePlayerSettings(PlayerSettings);
		}
	}
}

USpawnPoint* ATargetManager::TryGetSpawnPointFromReinforcementLearningComponent(const TArray<FVector>& OpenLocations) const
{
	/* Converting all OpenLocations to indices */
	TArray<int32> Indices;
	for (const FVector Vector : OpenLocations)
	{
		if (const int32 FoundIndex = SpawnPointManager->FindSpawnPointIndexFromLocation(Vector); FoundIndex != INDEX_NONE)
		{
			Indices.Add(FoundIndex);
		}
	}
	if (Indices.IsEmpty())
	{
		UE_LOG(LogTargetManager, Warning, TEXT("No targets in OpenLocations or No targets in TargetPairs"));
		return nullptr;
	}
	const int32 ChosenIndex = ReinforcementLearningComponent->ChooseNextActionIndex(Indices);
	if (!SpawnPointManager->GetSpawnPoints().IsValidIndex(ChosenIndex))
	{
		return nullptr;
	}
	TArray<USpawnPoint*> SpawnPointsRef = SpawnPointManager->GetSpawnPointsRef();
	const TArray<EBorderingDirection> BorderingDirections = SpawnPointsRef[ChosenIndex]->GetBorderingDirections(OpenLocations, GetBoxExtrema(true));
	SpawnPointsRef[ChosenIndex]->SetChosenPointAsRandomSubPoint(BorderingDirections);
	return SpawnPointsRef[ChosenIndex];
}

void ATargetManager::ShowDebug_SpawnBox(const bool bShow)
{
	bShowDebug_SpawnBox = bShow;
	if (bShowDebug_SpawnBox)
	{
		SpawnBox->SetHiddenInGame(false);
		TopBox->SetHiddenInGame(false);
		BottomBox->SetHiddenInGame(false);
		LeftBox->SetHiddenInGame(false);
		RightBox->SetHiddenInGame(false);
		ForwardBox->SetHiddenInGame(false);
		BackwardBox->SetHiddenInGame(false);
	}
	else
	{
		SpawnBox->SetHiddenInGame(true);
		TopBox->SetHiddenInGame(true);
		BottomBox->SetHiddenInGame(true);
		LeftBox->SetHiddenInGame(true);
		RightBox->SetHiddenInGame(true);
		ForwardBox->SetHiddenInGame(true);
		BackwardBox->SetHiddenInGame(true);
		FlushPersistentDebugLines(GetWorld());
	}
}

void ATargetManager::ShowDebug_SpawnMemory(const bool bShow)
{
	bShowDebug_SpawnMemory = bShow;
}

void ATargetManager::ShowDebug_ReinforcementLearningWidget(const bool bShow)
{
	if (!ReinforcementLearningComponent->IsActive())
	{
		return;
	}

	bShowDebug_ReinforcementLearningWidget = bShow;

	ABSPlayerController* Controller = Cast<ABSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (bShowDebug_ReinforcementLearningWidget)
	{
		Controller->ShowRLAgentWidget(ReinforcementLearningComponent->OnQTableUpdate, ReinforcementLearningComponent->GetHeight(), ReinforcementLearningComponent->GetWidth(),
		                              ReinforcementLearningComponent->GetAveragedTArrayFromQTable());
		return;
	}
	Controller->HideRLAgentWidget();
}

void ATargetManager::ShowDebug_NumRecentNumActivated() const
{
	int NumRecent=0;
	int NumAct=0;
	for (const USpawnPoint* Hey : SpawnPointManager->GetSpawnPoints())
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

TArray<FAccuracyRow> ATargetManager::GetLocationAccuracy() const
{
	/* Each row in QTable has size equal to ScaledSize, and so does each column */
	TArray<FAccuracyRow> OutArray;
	OutArray.Init(FAccuracyRow(5), 5);
	TArray<FQTableIndex> QTableIndices;

	for (int i = 0; i < 25; i++)
	{
		QTableIndices.Add(FQTableIndex(i));
	}

	TArray<USpawnPoint*> Counter = SpawnPointManager->GetSpawnPoints();

	for (int i = 0; i < SpawnPointManager->GetSpawnPoints().Num(); i++)
	{
		if (const int32 Found = QTableIndices.Find(SpawnPointManager->GetOutArrayIndexFromSpawnCounterIndex(i)); Found != INDEX_NONE)
		{
			const int32 RowNum = Found / 5;
			const int32 ColNum = Found % 5;

			if (Counter[i]->GetTotalSpawns() != INDEX_NONE)
			{
				if (OutArray[RowNum].TotalSpawns[ColNum] == INDEX_NONE)
				{
					OutArray[RowNum].TotalSpawns[ColNum] = 0;
				}
				OutArray[RowNum].TotalSpawns[ColNum] += Counter[i]->GetTotalSpawns();
				OutArray[RowNum].TotalHits[ColNum] += Counter[i]->GetTotalHits();
			}
		}
	}

	for (FAccuracyRow& AccuracyRow : OutArray)
	{
		AccuracyRow.UpdateAccuracy();
	}
	return OutArray;
}

FCommonScoreInfo ATargetManager::GetCommonScoreInfo() const
{
	TArray<FQTableIndex> Indices;
	for (int i = 0; i < 25; i++)
	{
		Indices.Add(FQTableIndex(i));
	}

	FCommonScoreInfo CommonScoreInfo = FCommonScoreInfo(25);
	TArray<USpawnPoint*> Counter = SpawnPointManager->GetSpawnPoints();
	for (int i = 0; i < SpawnPointManager->GetSpawnPoints().Num(); i++)
	{
		if (const int32 Found = Indices.Find(FQTableIndex(SpawnPointManager->GetOutArrayIndexFromSpawnCounterIndex(i))); Found != INDEX_NONE)
		{
			if (Counter[i]->GetTotalSpawns() != INDEX_NONE)
			{
				if (CommonScoreInfo.TotalSpawns[Found] == INDEX_NONE)
				{
					CommonScoreInfo.TotalSpawns[Found] = 0;
				}
				CommonScoreInfo.TotalSpawns[Found] += Counter[i]->GetTotalSpawns();
				CommonScoreInfo.TotalHits[Found] += Counter[i]->GetTotalHits();
			}
		}
	}
	if (ReinforcementLearningComponent->IsActive())
	{
		CommonScoreInfo.QTable = ReinforcementLearningComponent->GetSaveReadyQTable();
		ReinforcementLearningComponent->PrintRewards();
	}
	return CommonScoreInfo;
}
