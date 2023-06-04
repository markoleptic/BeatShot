// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "Target/TargetManager.h"
#include "BSGameMode.h"
#include "Player/BSPlayerController.h"
#include "GlobalConstants.h"
#include "Target/SphereTarget.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Target/ReinforcementLearningComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Target/SpawnPointManager.h"

DEFINE_LOG_CATEGORY(LogTargetManager);
using namespace Constants;

ATargetManager::ATargetManager()
{
	PrimaryActorTick.bCanEverTick = true;

	SpawnBox = CreateDefaultSubobject<UBoxComponent>("SpawnBox");
	RootComponent = SpawnBox;
	OverlapSpawnBox = CreateDefaultSubobject<UBoxComponent>("BeatTrack SpawnBox");
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
	RemoveFromRecentDelegate = FTimerDelegate();
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
	OnTick_UpdateTargetLocation(DeltaTime);
}

void ATargetManager::InitTargetManager(const FBSConfig& InBSConfig, const FPlayerSettings_Game& InPlayerSettings)
{
	SpawnPointManager = NewObject<USpawnPointManager>();
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
	
	/* Set the overlap spawn box size */
	if (BSConfig.TargetConfig.bUseOverlapSpawnBox)
	{
		OverlapSpawnBox->SetRelativeLocation(FVector(GetBoxOrigin().X - BSConfig.TargetConfig.MoveForwardDistance * 0.5f, GetBoxOrigin().Y, GetBoxOrigin().Z));
		OverlapSpawnBox->SetBoxExtent(FVector(BSConfig.TargetConfig.MoveForwardDistance * 0.5f, GetBoxExtents_Static().Y, GetBoxExtents_Static().Z));
		OverlapSpawnBox->OnComponentEndOverlap.AddDynamic(this, &ATargetManager::OnOverlapEnd_OverlapSpawnBox);
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
	// We're relying on FindNextTargetProperties to have a fresh SpawnPoint lined up,
	// which is called at the end of this function
	
	if (!ShouldSpawn || !SpawnPoint)
	{
		UE_LOG(LogTemp, Display, TEXT("Invalid SpawnPoint"));
		return;
	}

	// TODO: Needs to be more distinct differences when activating a SpawnPoint.
	// TODO: Currently, still finding a new SpawnPoint every beat for BeatTrack, but
	// TODO: don't even use the SpawnPoint for the mode.

	// TODO: Maybe a distinction between activating a deactivated SpawnPoint/Target
	// TODO: and activating an always active SpawnPoint/Target
	
	/*if (!BSConfig.TargetConfig.bContinuouslySpawn && SpawnPointManager->GetActivatedSpawnPoints().Num() > 0)
	{
		return;
	}*/
	
	bool bSuccessfulActivation = false;

	// Check to see if the game mode spawns targets at runtime
	if (BSConfig.TargetConfig.TargetSpawningPolicy == ETargetSpawningPolicy::RuntimeOnly)
	{
		SpawnTarget(*SpawnPoint);
	}
	if (ActivateTarget())
	{
		bSuccessfulActivation = true;
	}

	// SpawnPoint will be valid if we make it past this
	if (bSuccessfulActivation)
	{
		// Broadcast that a target has been spawned or activated, mainly to keep track of total target spawns on PlayerHUD
		OnTargetActivatedOrSpawned.Broadcast();
		// Add to ActiveTargetParis if using RL Comp
		if (ReinforcementLearningComponent->IsActive() && PreviousSpawnPoint && PreviousSpawnPoint != SpawnPoint)
		{
			ReinforcementLearningComponent->AddToActiveTargetPairs(PreviousSpawnPoint->ChosenPoint, SpawnPoint->ChosenPoint);
		}
		// If we make it this far, flag the point as activated
		SpawnPointManager->FlagSpawnPointAsActivated(SpawnPoint->GetGuid());
	}
	
	if (bShowDebug_SpawnMemory)
	{
		ShowDebug_NumRecentNumActivated();
	}

	// Immediately find the next target scale and SpawnPoint
	FindNextTargetProperties();
}

bool ATargetManager::SpawnTarget(FSpawnPoint& InSpawnPoint)
{
	ASphereTarget* Target = GetWorld()->SpawnActorDeferred<ASphereTarget>(TargetToSpawn, FTransform(FRotator::ZeroRotator, InSpawnPoint.ChosenPoint, InSpawnPoint.GetScale()), this, nullptr,
	                                                                      ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	Target->InitTarget(BSConfig.TargetConfig);
	Target->SetInitialSphereScale(InSpawnPoint.GetScale());
	Target->OnTargetDamageEventOrTimeout.AddDynamic(this, &ATargetManager::OnOnTargetHealthChangedOrExpired);
	Target->FinishSpawning(FTransform(), true);
	InSpawnPoint.SetGuid(Target->GetGuid());
	InSpawnPoint.SetManagedTargetIndex(AddToManagedTargets(Target));
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
			// TODO: this seems sketch
			FindNextTargetProperties();
			if (SpawnPoint)
			{
				SpawnTarget(*SpawnPoint);
			}
		}
		break;
	case ETargetDistributionPolicy::Grid:
		for (int i = 0; i < SpawnPointManager->GetSpawnPoints().Num(); i++)
		{
			SpawnPointManager->GetSpawnPointsRef()[i].SetScale(GetNextTargetScale());
			SpawnTarget(SpawnPointManager->GetSpawnPointsRef()[i]);
		}
		FindNextTargetProperties();
		break;
	default: ;
	}
}

bool ATargetManager::ActivateTarget() const
{
	if (/*bSpawnedTarget && */GetManagedTargets().IsValidIndex(SpawnPoint->GetManagedTargetIndex()))
	{
		ASphereTarget* Target = GetManagedTargets()[SpawnPoint->GetManagedTargetIndex()];
		if (BSConfig.TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::RemoveImmunity))
		{
			Target->RemoveImmunityEffect();
		}
		if (!Target->IsDamageWindowActive())
		{
			Target->ActivateTarget(BSConfig.TargetConfig.TargetMaxLifeSpan);
		}
		if (BSConfig.TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::ChangeVelocity))
		{
			Target->SetMovingTargetSpeed(FMath::FRandRange(BSConfig.TargetConfig.MinTargetSpeed, BSConfig.TargetConfig.MaxTargetSpeed));
		}
		if (BSConfig.TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::ChangeDirection))
		{
			Target->SetMovingTargetDirection(UKismetMathLibrary::GetDirectionUnitVector(Target->GetActorLocation(), GetRandomMovingTargetEndLocation(Target->GetActorLocation(), Target->GetMovingTargetSpeed())));
		}
		if (BSConfig.TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::ChangeScale))
		{
			Target->SetSphereScale(GetNextTargetScale());
		}
		return true;
	}
	// TODO: TEMP
	if (BSConfig.TargetConfig.TargetDestructionConditions.Contains(ETargetDestructionCondition::Persistant) &&
		BSConfig.TargetConfig.TargetDeactivationConditions.Contains(ETargetDeactivationCondition::Persistant) &&
		!GetManagedTargets().IsEmpty())
	{
		ASphereTarget* Target = GetManagedTargets()[0];
		if (BSConfig.TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::RemoveImmunity))
		{
			Target->RemoveImmunityEffect();
		}
		if (!Target->IsDamageWindowActive())
		{
			Target->ActivateTarget(BSConfig.TargetConfig.TargetMaxLifeSpan);
		}
		if (BSConfig.TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::ChangeVelocity))
		{
			Target->SetMovingTargetSpeed(FMath::FRandRange(BSConfig.TargetConfig.MinTargetSpeed, BSConfig.TargetConfig.MaxTargetSpeed));
		}
		if (BSConfig.TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::ChangeDirection))
		{
			Target->SetMovingTargetDirection(UKismetMathLibrary::GetDirectionUnitVector(Target->GetActorLocation(), GetRandomMovingTargetEndLocation(Target->GetActorLocation(), Target->GetMovingTargetSpeed())));
		}
		if (BSConfig.TargetConfig.TargetActivationResponses.Contains(ETargetActivationResponse::ChangeScale))
		{
			Target->SetSphereScale(GetNextTargetScale());
		}
		return true;
	}
	return false;
}

void ATargetManager::OnTick_UpdateTargetLocation(const float DeltaTime) const
{
	for (ASphereTarget* Target : GetManagedTargets())
	{
		FVector StartLoc = Target->GetActorLocation();
		if (BSConfig.TargetConfig.bMoveTargetsForward)
		{
			const FVector NewLoc = FVector(StartLoc.X - BSConfig.TargetConfig.MoveForwardDistance, StartLoc.Y, StartLoc.Z);
			Target->SetActorLocation(UKismetMathLibrary::VInterpTo(StartLoc, NewLoc, DeltaTime, 1 / BSConfig.TargetConfig.TargetMaxLifeSpan));
		}
		Target->SetActorLocation(StartLoc += Target->GetMovingTargetDirection() * Target->GetMovingTargetSpeed() * DeltaTime);
	}
}

void ATargetManager::OnOnTargetHealthChangedOrExpired(const FTargetDamageEvent& TargetDamageEvent)
{
	/* Update ConsecutiveTargetsHit, Adjust DynamicSpawnScale */
	if (TargetDamageEvent.TimeAlive == -1)
	{
		ConsecutiveTargetsHit = 0;
		DynamicSpawnScale = FMath::Clamp(DynamicSpawnScale - 5, 0, 100);
	}
	else
	{
		ConsecutiveTargetsHit++;
		DynamicSpawnScale = FMath::Clamp(DynamicSpawnScale + 1, 0, 100);
	}

	/* Broadcast damage event to GameMode */
	if (BSConfig.TargetConfig.TargetDamageType == ETargetDamageType::Tracking)
	{
		OnBeatTrackTargetDamaged.Broadcast(TargetDamageEvent.DamageDelta, TargetDamageEvent.TotalPossibleDamage);
	}
	else
	{
		OnTargetDeactivated.Broadcast(TargetDamageEvent.TimeAlive, ConsecutiveTargetsHit, FVector(TargetDamageEvent.Location.X,
			TargetDamageEvent.Location.Y, TargetDamageEvent.Location.Z + SphereTargetRadius * TargetDamageEvent.Scale.Z));
	}
	
	/* Remove from managed targets */
	if (!BSConfig.TargetConfig.TargetDestructionConditions.Contains(ETargetDestructionCondition::Persistant))
	{
		if (BSConfig.TargetConfig.TargetDestructionConditions.Contains(ETargetDestructionCondition::OnDeactivation))
		{
			UE_LOG(LogTemp, Display, TEXT("Removed based on OnDeactivation"));
			RemoveFromManagedTargets(TargetDamageEvent.Guid);
		}
		if (BSConfig.TargetConfig.TargetDestructionConditions.Contains(ETargetDestructionCondition::OnExpiration))
		{
			UE_LOG(LogTemp, Display, TEXT("Removed based on OnExpiration"));
			RemoveFromManagedTargets(TargetDamageEvent.Guid);
		}
		if (BSConfig.TargetConfig.TargetDestructionConditions.Contains(ETargetDestructionCondition::OnHealthReachedZero) &&
			TargetDamageEvent.CurrentHealth <= 0.f)
		{
			UE_LOG(LogTemp, Display, TEXT("Removed based on OnHealthReachedZero"));
			RemoveFromManagedTargets(TargetDamageEvent.Guid);
		}
		if (BSConfig.TargetConfig.TargetDestructionConditions.Contains(ETargetDestructionCondition::OnAnyExternalDamageTaken) &&
			TargetDamageEvent.TimeAlive != -1)
		{
			UE_LOG(LogTemp, Display, TEXT("Removed based on OnAnyExternalDamageTaken"));
			RemoveFromManagedTargets(TargetDamageEvent.Guid);
		}
	}
	
	/* Update Reinforcement Learning Component */
	if (ReinforcementLearningComponent->IsActive())
	{
		ReinforcementLearningComponent->UpdateReinforcementLearningReward(TargetDamageEvent.Location, TargetDamageEvent.TimeAlive != INDEX_NONE);
		ReinforcementLearningComponent->UpdateReinforcementLearningComponent(SpawnPointManager);
	}

	/* Update flags */
	SpawnPointManager->RemoveActivatedFlagFromSpawnPoint(TargetDamageEvent);
	SpawnPointManager->FlagSpawnPointAsRecent(TargetDamageEvent.Guid);

	/* Handle removing recent flag from SpawnPoint */
	FTimerHandle TimerHandle;
	RemoveFromRecentDelegate.BindUObject(SpawnPointManager, &USpawnPointManager::RemoveRecentFlagFromSpawnPoint, TargetDamageEvent.Guid);
	switch (BSConfig.TargetConfig.RecentTargetMemoryPolicy)
	{
	case ERecentTargetMemoryPolicy::None:
		break;
	case ERecentTargetMemoryPolicy::CustomTimeBased:
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, RemoveFromRecentDelegate, BSConfig.TargetConfig.RecentTargetTimeLength, false);
		break;
	case ERecentTargetMemoryPolicy::NumTargetsBased:
		if (SpawnPointManager->GetRecentSpawnPoints().Num() > BSConfig.TargetConfig.MaxNumRecentTargets)
		{
			if (const FSpawnPoint* Found = SpawnPointManager->FindOldestRecentSpawnPoint())
			{
				SpawnPointManager->RemoveRecentFlagFromSpawnPoint(Found->GetGuid());
			}
		}
		break;
	case ERecentTargetMemoryPolicy::UseTargetSpawnCD:
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, RemoveFromRecentDelegate, BSConfig.TargetConfig.TargetSpawnCD, false);
		break;
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ATargetManager::OnOverlapEnd_OverlapSpawnBox(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	/* Reverse direction if not longer overlapping spawn box */
	if (ASphereTarget* Target = Cast<ASphereTarget>(OtherActor))
	{
		Target->SetMovingTargetDirection(-Target->GetMovingTargetDirection());
	}
}

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

FSpawnPoint* ATargetManager::GetNextSpawnPoint(const EBoundsScalingPolicy BoundsScalingMethod, const FVector& NewTargetScale) const
{
	/* Change the BoxExtent of the SpawnBox if dynamic */
	if (IsDynamicBoundsScalingPolicy(BoundsScalingMethod))
	{
		SetBoxExtents_Dynamic();
	}

	/* First SpawnPoint */
	if (!SpawnPoint && !ShouldSpawn)
	{
		if (FSpawnPoint* Point = SpawnPointManager->FindSpawnPointFromLocation(GetBoxOrigin()))
		{
			return Point;
		}
		const int32 RandomPoint = UKismetMathLibrary::RandomIntegerInRange(0,  SpawnPointManager->GetSpawnPoints().Num() - 1);
		if (SpawnPointManager->GetSpawnPoints().IsValidIndex(RandomPoint))
		{
			return &SpawnPointManager->GetSpawnPointsRef()[RandomPoint];
		}
		return nullptr;
	}
	
	/* Spawn every other target in center if specified */
	if (BSConfig.TargetConfig.bSpawnEveryOtherTargetInCenter && SpawnPoint != SpawnPointManager->FindSpawnPointFromLocation(GetBoxOrigin()))
	{
		return SpawnPointManager->FindSpawnPointFromLocation(GetBoxOrigin());
	}

	/* Get all points that haven't been occupied by a target recently */
	TArray<FVector> OpenLocations = GetValidSpawnLocations(NewTargetScale, BSConfig.TargetConfig.TargetDistributionPolicy, BoundsScalingMethod);
	if (OpenLocations.IsEmpty())
	{
		UE_LOG(LogTargetManager, Warning, TEXT("OpenLocations is empty."));
		return nullptr;
	}

	/* Spawn at origin whenever possible if specified */
	if (BSConfig.TargetConfig.bSpawnAtOriginWheneverPossible && OpenLocations.Contains(GetBoxOrigin()))
	{
		return SpawnPointManager->FindSpawnPointFromLocation(GetBoxOrigin());
	}

	/* Get location from RL Comp if specified */
	if (ReinforcementLearningComponent->IsActive())
	{
		if (const int32 ChosenPoint = TryGetSpawnLocationFromReinforcementLearningComponent(OpenLocations); ChosenPoint != INDEX_NONE)
		{
			TArray<FSpawnPoint>& SpawnPointsRef = SpawnPointManager->GetSpawnPointsRef();
			const TArray<EBorderingDirection> BorderingDirections = SpawnPointsRef[ChosenPoint].GetBorderingDirections(OpenLocations, GetBoxExtrema(true));
			SpawnPointsRef[ChosenPoint].SetChosenPointAsRandomSubPoint(BorderingDirections);
			return &SpawnPointsRef[ChosenPoint];
		}
		UE_LOG(LogTargetManager, Warning, TEXT("Unable to Spawn Location suggested by RLAgent."));
	}
	
	if (!BSConfig.GridConfig.bRandomizeGridTargetActivation && BSConfig.TargetConfig.TargetDistributionPolicy == ETargetDistributionPolicy::Grid)
	{
		const TArray<int32> Indices = SpawnPoint->GetBorderingIndices();
		OpenLocations = GetAllSpawnLocations().FilterByPredicate([&] (const FVector& Vector)
		{
			if (const FSpawnPoint* FoundPoint = SpawnPointManager->FindSpawnPointFromLocation(Vector))
			{
				if (!FoundPoint->IsActivated() && !FoundPoint->IsRecent() && Indices.Contains(FoundPoint->Index))
				{
					return true;
				}
			}
			return false;
		});
		if (OpenLocations.IsEmpty())
		{
			FSpawnPoint* OldestRecentSpawnPoint = SpawnPointManager->FindOldestRecentSpawnPoint();
			//UE_LOG(LogTargetManager, Display, TEXT("Found CornerPoint %s Found CenterPoint %s Found ChosenPoint %s"), *Found->CornerPoint.ToString(), *Found->CenterPoint.ToString(), *Found->ChosenPoint.ToString());
			return OldestRecentSpawnPoint;
		}
	}
	
	const int32 RandomPoint = UKismetMathLibrary::RandomIntegerInRange(0, OpenLocations.Num() - 1);
	
	if (FSpawnPoint* Found = SpawnPointManager->FindSpawnPointFromLocation(OpenLocations[RandomPoint]))
	{
		const TArray<EBorderingDirection> BorderingDirections = Found->GetBorderingDirections(OpenLocations, GetBoxExtrema(true));
		Found->SetChosenPointAsRandomSubPoint(BorderingDirections);
		//UE_LOG(LogTargetManager, Display, TEXT("Found CornerPoint %s Found CenterPoint %s Found ChosenPoint %s"), *Found->CornerPoint.ToString(), *Found->CenterPoint.ToString(), *Found->ChosenPoint.ToString());
		return Found;
	}
	//UE_LOG(LogTargetManager, Display, TEXT("Found CornerPoint %s Found CenterPoint %s Found ChosenPoint %s"), *Found->CornerPoint.ToString(), *Found->CenterPoint.ToString(), *Found->ChosenPoint.ToString());
	return nullptr;
}

TArray<FVector> ATargetManager::GetValidSpawnLocations(const FVector& Scale, const ETargetDistributionPolicy& DistributionPolicy, const EBoundsScalingPolicy& BoundsScalingPolicy) const
{
	TArray<FVector> ValidSpawnLocations;
	const FExtrema Extrema = GetBoxExtrema(BoundsScalingPolicy == EBoundsScalingPolicy::Dynamic);
	
	/* Populate AllPoints according to DistributionPolicy */
	switch (DistributionPolicy)
	{
	case ETargetDistributionPolicy::EdgeOnly:
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
		}
		break;
	case ETargetDistributionPolicy::FullRange:
		{
			ValidSpawnLocations = GetAllSpawnLocations().FilterByPredicate([&](const FVector& Vector)
			{
				if (Vector.Y < Extrema.Min.Y || Vector.Y >= Extrema.Max.Y || Vector.Z < Extrema.Min.Z || Vector.Z >= Extrema.Max.Z)
				{
					return false;
				}
				return true;
			});
		}
		break;
	default:
		{
			ValidSpawnLocations = GetAllSpawnLocations();
		}
		break;
	}
	
	/* Remove points occupied by recent targets (overlapping points) */
	const TArray<FVector> OverlappingPoints = SpawnPointManager->RemoveOverlappingPointsFromSpawnLocations(ValidSpawnLocations, Scale, Extrema);
	
	if (BSConfig.TargetConfig.TargetDistributionPolicy != ETargetDistributionPolicy::Grid)
	{
		/* Remove points that don't have another point to the top and to the right */
		SpawnPointManager->RemoveEdgePoints(ValidSpawnLocations, Extrema);
	}

	if (bShowDebug_SpawnBox)
	{
		DrawDebugBox_OpenLocations(ValidSpawnLocations, FColor::Emerald, 6);
		const TArray<FVector> Blocked = GetAllSpawnLocations().FilterByPredicate([&ValidSpawnLocations] (const FVector& Vector)
		{
			return !ValidSpawnLocations.Contains(Vector);
		});
		DrawDebugBox_OpenLocations(OverlappingPoints, FColor::Red, 4);
	}
	
	return ValidSpawnLocations;
}

FVector ATargetManager::GetRandomMovingTargetEndLocation(const FVector& LocationBeforeChange, const float TargetSpeed) const
{
	const FVector NewExtent = FVector(BSConfig.TargetConfig.MoveForwardDistance * 0.5, GetBoxExtents_Static().Y * 0.5, GetBoxExtents_Static().Z * 0.5);
	const FVector BotLeft = UKismetMathLibrary::RandomPointInBoundingBox(OverlapSpawnBox->Bounds.Origin + FVector(0, -GetBoxExtents_Static().Y * 0.5, -GetBoxExtents_Static().Z * 0.5), NewExtent);
	const FVector BotRight = UKismetMathLibrary::RandomPointInBoundingBox(OverlapSpawnBox->Bounds.Origin + FVector(0, GetBoxExtents_Static().Y * 0.5, -GetBoxExtents_Static().Z * 0.5), NewExtent);
	const FVector TopLeft = UKismetMathLibrary::RandomPointInBoundingBox(OverlapSpawnBox->Bounds.Origin + FVector(0, -GetBoxExtents_Static().Y * 0.5, GetBoxExtents_Static().Z * 0.5), NewExtent);
	const FVector TopRight = UKismetMathLibrary::RandomPointInBoundingBox(OverlapSpawnBox->Bounds.Origin + FVector(0, GetBoxExtents_Static().Y * 0.5, GetBoxExtents_Static().Z * 0.5), NewExtent);

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

int32 ATargetManager::AddToManagedTargets(ASphereTarget* SpawnTarget)
{
	TArray<ASphereTarget*> Targets = GetManagedTargets();
	const int32 NewIndex = Targets.Add(SpawnTarget);
	ManagedTargets = Targets;
	return NewIndex;
}

void ATargetManager::RemoveFromManagedTargets(const FGuid GuidToRemove)
{
	const TArray<ASphereTarget*> Targets = GetManagedTargets().FilterByPredicate([&] (const ASphereTarget* OtherTarget)
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

void ATargetManager::SetBoxExtents_Dynamic() const
{
	const float NewFactor = DynamicSpawnCurve->GetFloatValue(DynamicSpawnScale);
	const float LerpY = UKismetMathLibrary::Lerp(GetBoxExtents_Static().Y, GetBoxExtents_Static().Y * 0.5f, NewFactor);
	const float LerpZ = UKismetMathLibrary::Lerp(GetBoxExtents_Static().Z, GetBoxExtents_Static().Z * 0.5f, NewFactor);
	const float Y = FMath::GridSnap(LerpY, SpawnPointManager->GetSpawnMemoryIncY());
	const float Z = FMath::GridSnap(LerpZ, SpawnPointManager->GetSpawnMemoryIncZ());
	SpawnBox->SetBoxExtent(FVector(0, Y, Z));
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

int32 ATargetManager::TryGetSpawnLocationFromReinforcementLearningComponent(const TArray<FVector>& OpenLocations) const
{
	/* Converting all OpenLocations to indices */
	TArray<int32> Indices;
	for (const FVector Vector : OpenLocations)
	{
		if (const int32 FoundIndex = SpawnPointManager->FindIndexFromLocation(Vector); FoundIndex != INDEX_NONE)
		{
			Indices.Add(FoundIndex);
		}
	}
	if (Indices.IsEmpty())
	{
		UE_LOG(LogTargetManager, Warning, TEXT("No targets in OpenLocations or No targets in TargetPairs"));
		return INDEX_NONE;
	}
	return ReinforcementLearningComponent->ChooseNextActionIndex(Indices);
}

void ATargetManager::DrawDebugBox_OpenLocations(const TArray<FVector>& InOpenLocations, const FColor& InColor, const int32 InThickness) const
{
	for (const FVector& Vector : InOpenLocations)
	{
		FVector Loc = FVector(Vector.X, Vector.Y + SpawnPointManager->GetSpawnMemoryIncY() / 2.f, Vector.Z + SpawnPointManager->GetSpawnMemoryIncZ() / 2.f);
		DrawDebugBox(GetWorld(), Loc, FVector(0, SpawnPointManager->GetSpawnMemoryIncY() / 2.f, SpawnPointManager->GetSpawnMemoryIncZ() / 2.f),
			InColor, false, BSConfig.TargetConfig.TargetSpawnCD, 0, InThickness);
	}
}

void ATargetManager::ShowDebug_SpawnBox(const bool bShow)
{
	bShowDebug_SpawnBox = bShow;
	if (bShowDebug_SpawnBox)
	{
		DrawDebugBox(GetWorld(), GetBoxOrigin(), GetBoxExtents_Static(), FColor::Orange, true, -1, 3, 5);
		DrawDebugBox(GetWorld(), OverlapSpawnBox->Bounds.Origin, OverlapSpawnBox->GetScaledBoxExtent(), FColor::Blue, true, -1, 3, 5);
	}
	else
	{
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
	for (FSpawnPoint& Hey : SpawnPointManager->GetSpawnPoints())
	{
		if (Hey.IsRecent())
		{
			NumRecent++;
		}
		if (Hey.IsActivated())
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

	TArray<FSpawnPoint> Counter = SpawnPointManager->GetSpawnPoints();

	for (int i = 0; i < SpawnPointManager->GetSpawnPoints().Num(); i++)
	{
		if (const int32 Found = QTableIndices.Find(SpawnPointManager->GetOutArrayIndexFromSpawnCounterIndex(i)); Found != INDEX_NONE)
		{
			const int32 RowNum = Found / 5;
			const int32 ColNum = Found % 5;

			if (Counter[i].GetTotalSpawns() != INDEX_NONE)
			{
				if (OutArray[RowNum].TotalSpawns[ColNum] == INDEX_NONE)
				{
					OutArray[RowNum].TotalSpawns[ColNum] = 0;
				}
				OutArray[RowNum].TotalSpawns[ColNum] += Counter[i].GetTotalSpawns();
				OutArray[RowNum].TotalHits[ColNum] += Counter[i].GetTotalHits();
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
	TArray<FSpawnPoint> Counter = SpawnPointManager->GetSpawnPoints();
	for (int i = 0; i < SpawnPointManager->GetSpawnPoints().Num(); i++)
	{
		if (const int32 Found = Indices.Find(FQTableIndex(SpawnPointManager->GetOutArrayIndexFromSpawnCounterIndex(i))); Found != INDEX_NONE)
		{
			if (Counter[i].GetTotalSpawns() != INDEX_NONE)
			{
				if (CommonScoreInfo.TotalSpawns[Found] == INDEX_NONE)
				{
					CommonScoreInfo.TotalSpawns[Found] = 0;
				}
				CommonScoreInfo.TotalSpawns[Found] += Counter[i].GetTotalSpawns();
				CommonScoreInfo.TotalHits[Found] += Counter[i].GetTotalHits();
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
