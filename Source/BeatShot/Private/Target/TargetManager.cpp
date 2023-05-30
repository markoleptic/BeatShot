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
	ShouldSpawn = false;
	LastTargetSpawnedCenter = false;
	bSkipNextSpawn = false;
	ConsecutiveTargetsHit = 0;
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
	
	BSConfig.TargetConfig.OnSpawn_TargetColor = InPlayerSettings.StartTargetColor;
	BSConfig.TargetConfig.OnSpawn_TargetOutlineColor = InPlayerSettings.StartTargetColor;
	BSConfig.TargetConfig.StartToPeak_StartColor = InPlayerSettings.StartTargetColor;
	BSConfig.TargetConfig.StartToPeak_EndColor = InPlayerSettings.PeakTargetColor;
	BSConfig.TargetConfig.PeakToEnd_StartColor = InPlayerSettings.PeakTargetColor;
	BSConfig.TargetConfig.PeakToEnd_EndColor = InPlayerSettings.EndTargetColor;
	BSConfig.TargetConfig.InActiveTargetOutlineColor = InPlayerSettings.BeatGridInactiveTargetColor;
	BSConfig.TargetConfig.InActiveTargetColor = InPlayerSettings.BeatGridInactiveTargetColor;

	/* GameMode menu uses the full width, while box bounds are only half width / half height */
	StaticExtents = BSConfig.SpatialConfig.GenerateTargetManagerBoxBounds();

	/* Set new location & box extent */
	SpawnBox->SetRelativeLocation(BSConfig.SpatialConfig.GenerateSpawnBoxLocation());
	SpawnBox->SetBoxExtent(StaticExtents);
	StaticMinExtrema = SpawnBox->Bounds.GetBoxExtrema(0);
	StaticMaxExtrema = SpawnBox->Bounds.GetBoxExtrema(1);

	/* Set the overlap spawn box size */
	OverlapSpawnBox->SetRelativeLocation(FVector(GetBoxOrigin().X - BSConfig.SpatialConfig.MoveForwardDistance * 0.5f, GetBoxOrigin().Y, GetBoxOrigin().Z));
	OverlapSpawnBox->SetBoxExtent(FVector(BSConfig.SpatialConfig.MoveForwardDistance * 0.5f, GetBoxExtents_Static().Y, GetBoxExtents_Static().Z));

	if (BSConfig.SpatialConfig.bUseOverlapSpawnBox)
	{
		OverlapSpawnBox->OnComponentEndOverlap.AddDynamic(this, &ATargetManager::OnOverlapEnd_OverlapSpawnBox);
	}
	
	SpawnPointManager->InitSpawnPointManager(InBSConfig, GetBoxOrigin(), StaticExtents);

	if (BSConfig.SpatialConfig.TargetDistributionMethod == ETargetDistributionPolicy::Grid)
	{
		AllSpawnLocations = SpawnPointManager->InitializeSpawnPoints(BSConfig.GetBoxExtremaGrid(0, GetBoxOrigin()), BSConfig.GetBoxExtremaGrid(1, GetBoxOrigin()));
	}
	else
	{
		AllSpawnLocations = SpawnPointManager->InitializeSpawnPoints(GetBoxExtrema(0, false), GetBoxExtrema(1, false));
	}
		
	if (IsDynamicBoundsScalingPolicy(BSConfig.SpatialConfig.BoundsScalingMethod))
	{
		SetBoxExtents_Dynamic();
	}

	/* Enable / Disable Reinforcement Learning */
	if (BSConfig.AIConfig.bEnableReinforcementLearning)
	{
		FRLAgentParams Params;
		Params.InQTable = GetScoreInfoFromDefiningConfig(InBSConfig.DefiningConfig).QTable;
		Params.DefaultMode = BSConfig.DefiningConfig.BaseGameMode;
		Params.CustomGameModeName = BSConfig.DefiningConfig.CustomGameModeName;
		Params.Size = SpawnPointManager->GetSpawnCounter().Num();
		Params.SpawnCounterHeight = SpawnPointManager->GetSpawnPointsHeight();
		Params.SpawnCounterWidth = SpawnPointManager->GetSpawnPointsWidth();
		Params.InEpsilon = BSConfig.AIConfig.Epsilon;
		Params.InGamma = BSConfig.AIConfig.Gamma;
		Params.InAlpha = BSConfig.AIConfig.Alpha;
		ReinforcementLearningComponent->Init(Params);
	}
	else
	{
		ReinforcementLearningComponent->Deactivate();
	}

	/* Initial target size */
	TargetScale = GetNextTargetScale();

	/* Initial SpawnPoint and PreviousSpawnPoint */
	if (FSpawnPoint* Point = SpawnPointManager->FindSpawnPointFromLocation(GetBoxOrigin())) {
		SpawnPoint = Point;
	}
	else
	{
		SpawnPoint = &SpawnPointManager->GetSpawnCounter()[UKismetMathLibrary::RandomIntegerInRange(0, SpawnPointManager->GetSpawnCounter().Num() - 1)];
	}
	PreviousSpawnPoint = SpawnPoint;

	if (BSConfig.TargetConfig.NumUpfrontTargetsToSpawn > 0)
	{
		switch(BSConfig.SpatialConfig.TargetDistributionMethod) {
		case ETargetDistributionPolicy::None:
		case ETargetDistributionPolicy::HeadshotHeightOnly:
		case ETargetDistributionPolicy::EdgeOnly:
		case ETargetDistributionPolicy::FullRange:
			for (int i = 0; i < BSConfig.TargetConfig.NumUpfrontTargetsToSpawn; i++)
			{
				FindNextTargetProperties();
				SpawnTarget(*SpawnPoint, false);
			}
			break;
		case ETargetDistributionPolicy::Grid:
			for (int i = 0; i < SpawnPointManager->GetSpawnCounter().Num(); i++)
			{
				SpawnTarget(SpawnPointManager->GetSpawnCounter()[i], false);
			}
			break;
		default: ;
		}
	}
	
	UE_LOG(LogTargetManager, Display, TEXT("SpawnBoxWidth %f SpawnBoxHeight %f"), StaticExtents.Y * 2, StaticExtents.Z * 2);
}

void ATargetManager::SetShouldSpawn(const bool bShouldSpawn)
{
	ShouldSpawn = bShouldSpawn;
}

void ATargetManager::OnAudioAnalyzerBeat()
{
	if (!ShouldSpawn || !SpawnPointManager)
	{
		//UE_LOG(LogTemp, Display, TEXT("Shouldn't Spawn"));
		//goto SetNext;
		return;
	}

	switch (BSConfig.TargetConfig.TargetSpawningPolicy)
	{
	case ETargetSpawningPolicy::None:
	case ETargetSpawningPolicy::Upfront:
		break;
	case ETargetSpawningPolicy::Runtime:
		if (!bSkipNextSpawn)
		{
			SpawnTarget(*SpawnPoint, true);
			SpawnPointManager->FlagSpawnPointAsActivated(SpawnPoint->GetGuid());
		}
		if (!BSConfig.SpatialConfig.bContinuouslySpawn)
		{
			SetShouldSpawn(false);
		}
		break;
	}

	switch (BSConfig.TargetConfig.TargetActivationPolicy)
	{
	case ETargetActivationPolicy::None:
	case ETargetActivationPolicy::OnSpawn:
		break;
	case ETargetActivationPolicy::OnCooldown:
		ActivateTarget();
		break;
	}

	int NumRecent=0;
	int NumAct=0;
	for (FSpawnPoint& Hey : SpawnPointManager->GetSpawnCounter())
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

	PreviousSpawnPoint = SpawnPoint;
	FindNextTargetProperties();
	/*SetNext:
	{
		
	}*/
}

ASphereTarget* ATargetManager::SpawnTarget(FSpawnPoint& InSpawnPoint, const bool bBroadcastSpawnEvent)
{
	ASphereTarget* Target = GetWorld()->SpawnActorDeferred<ASphereTarget>(ActorToSpawn, FTransform(FRotator::ZeroRotator, InSpawnPoint.ActualChosenPoint, InSpawnPoint.Scale), this, nullptr,
	                                                                      ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	Target->InitTarget(BSConfig, PlayerSettings);
	Target->SetInitialSphereScale(InSpawnPoint.Scale);
	Target->OnTargetDamageEventOrTimeout.AddDynamic(this, &ATargetManager::OnOnTargetHealthChangedOrExpired);
	Target->FinishSpawning(FTransform(), true);
	InSpawnPoint.SetGuid(Target->GetGuid());
	AddToManagedTargets(Target);

	if (ReinforcementLearningComponent->IsActive())
	{
		ReinforcementLearningComponent->AddToActiveTargetPairs(PreviousSpawnPoint->ActualChosenPoint, InSpawnPoint.ActualChosenPoint);
	}

	if (InSpawnPoint.ActualChosenPoint.Equals(GetBoxOrigin()))
	{
		LastTargetSpawnedCenter = true;
	}
	else
	{
		LastTargetSpawnedCenter = false;
	}

	PreviousSpawnPoint = &InSpawnPoint;
	LocationBeforeDirectionChange = InSpawnPoint.ActualChosenPoint;

	if (bBroadcastSpawnEvent)
	{
		OnTargetActivatedOrSpawned.Broadcast();
		OnTargetActivated_AimBot.Broadcast(Target);
	}

	return Target;
}

void ATargetManager::ActivateTarget()
{
	if (SpawnPoint && GetManagedTargets().IsValidIndex(SpawnPoint->Index) && !GetManagedTargets()[SpawnPoint->Index]->IsTargetActiveAndDamageable())
	{
		GetManagedTargets()[SpawnPoint->Index]->ActivateTarget(BSConfig.TargetConfig.TargetMaxLifeSpan);
		OnTargetActivatedOrSpawned.Broadcast();
		SpawnPointManager->FlagSpawnPointAsActivated(SpawnPoint->GetGuid());
		PreviousSpawnPoint = SpawnPoint;
	}
}

void ATargetManager::SpawnChargedTarget()
{
	if (!GetManagedTargets().IsEmpty())
	{
		return;
	}

	SpawnTarget(*SpawnPoint, false);
	FindNextTargetProperties();
	MovingTargetSpeed = FMath::FRandRange(BSConfig.TargetConfig.MinTrackingSpeed, BSConfig.TargetConfig.MaxTrackingSpeed);
	EndLocation = GetRandomBeatTrackLocation(LocationBeforeDirectionChange);
	CurrentMovingTargetDirection = UKismetMathLibrary::GetDirectionUnitVector(LocationBeforeDirectionChange, EndLocation);
}

void ATargetManager::UpdateBeatTrackTarget()
{
	if (GetManagedTargets().IsEmpty())
	{
		return;
	}

	LocationBeforeDirectionChange = GetManagedTargets()[0]->GetActorLocation();
	GetManagedTargets()[0]->SetSphereScale(GetNextTargetScale());
	MovingTargetSpeed = FMath::FRandRange(BSConfig.TargetConfig.MinTrackingSpeed, BSConfig.TargetConfig.MaxTrackingSpeed);
	EndLocation = GetRandomBeatTrackLocation(LocationBeforeDirectionChange);
	CurrentMovingTargetDirection = UKismetMathLibrary::GetDirectionUnitVector(LocationBeforeDirectionChange, EndLocation);
}

void ATargetManager::OnTick_UpdateTargetLocation(const float DeltaTime)
{
	for (ASphereTarget* Target : GetManagedTargets())
	{
		CurrentMovingTargetLocation = Target->GetActorLocation();
		if (BSConfig.SpatialConfig.bMoveTargetsForward)
		{
			const FVector NewLoc = FVector(CurrentMovingTargetLocation.X - BSConfig.SpatialConfig.MoveForwardDistance, CurrentMovingTargetLocation.Y, CurrentMovingTargetLocation.Z);
			Target->SetActorLocation(UKismetMathLibrary::VInterpTo(CurrentMovingTargetLocation, NewLoc, DeltaTime, 1 / BSConfig.TargetConfig.TargetMaxLifeSpan));
		}
		Target->SetActorLocation(CurrentMovingTargetLocation += CurrentMovingTargetDirection * MovingTargetSpeed * DeltaTime);
	}
}

void ATargetManager::OnOnTargetHealthChangedOrExpired(const FTargetDamageEvent& TargetDamageEvent)
{
	/* Allow targets to spawn again if SingleBeat */
	if (BSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::SingleBeat)
	{
		SetShouldSpawn(true);
	}

	if (BSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::BeatTrack)
	{
		OnBeatTrackTargetDamaged.Broadcast(TargetDamageEvent.DamageDelta, TargetDamageEvent.TotalPossibleDamage);
	}

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

	/* Broadcast that a target has been destroyed or timed out, and give the location for combat text */
	const FVector Location = TargetDamageEvent.Location;
	const FVector CombatTextLocation = {Location.X, Location.Y, Location.Z + SphereTargetRadius * TargetDamageEvent.Scale.Z};
	OnTargetDestroyed.Broadcast(TargetDamageEvent.TimeAlive, ConsecutiveTargetsHit, CombatTextLocation);

	if (BSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::BeatTrack)
	{
		return;
	}
	
	/* Remove from active targets if out of charges, and don't do anything further if ChargedBeatTrack */
	if (BSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::ChargedBeatTrack)
	{
		if (TargetDamageEvent.CurrentHealth <= 0.f)
		{
			RemoveFromManagedTargets(TargetDamageEvent.Guid);
		}
		return;
	}

	/* Remove from active targets, and remove from recent targets after a delay */
	if (BSConfig.TargetConfig.TargetDestructionPolicy != ETargetDestructionPolicy::Persistant)
	{
		RemoveFromManagedTargets(TargetDamageEvent.Guid);
	}

	FTimerHandle TimerHandle;
	RemoveFromRecentDelegate.BindUObject(SpawnPointManager, &USpawnPointManager::RemoveRecentFlagFromSpawnPoint, TargetDamageEvent.Guid);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, RemoveFromRecentDelegate, BSConfig.TargetConfig.TargetSpawnCD, false);

	/* Find the index that corresponds to the destroyed target's location */
	FSpawnPoint* Point = SpawnPointManager->FindSpawnPointFromGuid(TargetDamageEvent.Guid);
	if (Point)
	{
		/* Update the total target spawns in SpawnCounter */
		if (Point->TotalSpawns == INDEX_NONE)
		{
			Point->TotalSpawns = 1;
		}
		else
		{
			Point->TotalSpawns++;
		}

		/* Update the total target hits in SpawnCounter */
		if (TargetDamageEvent.TimeAlive != INDEX_NONE)
		{
			Point->TotalHits++;
		}

		/* Update the Reinforcement Learning Component */
		if (ReinforcementLearningComponent->IsActive())
		{
			ReinforcementLearningComponent->UpdateReinforcementLearningReward(Location, TargetDamageEvent.TimeAlive != INDEX_NONE);
			ReinforcementLearningComponent->UpdateReinforcementLearningComponent(SpawnPointManager->GetSpawnCounter());
		}
		Point->SetIsActivated(false);
		Point->SetIsRecent(true);
	}
	else
	{
		UE_LOG(LogTargetManager, Warning, TEXT("Target not found in SpawnCounter %s"), *Location.ToString());
	}
}

void ATargetManager::OnOverlapEnd_OverlapSpawnBox(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	/* Reverse direction if not longer overlapping spawn box */
	if (Cast<ASphereTarget>(OtherActor))
	{
		CurrentMovingTargetDirection = -CurrentMovingTargetDirection;
	}
}

void ATargetManager::FindNextTargetProperties()
{
	TargetScale = GetNextTargetScale();
	SpawnPoint = GetNextTargetSpawnLocation(BSConfig.SpatialConfig.BoundsScalingMethod, TargetScale);
	SpawnPoint->SetScale(TargetScale);
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

FSpawnPoint* ATargetManager::GetNextTargetSpawnLocation(const EBoundsScalingPolicy BoundsScalingMethod, const FVector& NewTargetScale)
{
	/* Change the BoxExtent of the SpawnBox if dynamic */
	if (IsDynamicBoundsScalingPolicy(BoundsScalingMethod))
	{
		SetBoxExtents_Dynamic();
	}

	/* Get all points that haven't been occupied by a target recently */
	TArray<FVector> OpenLocations = GetValidSpawnLocations(NewTargetScale, BSConfig.SpatialConfig.TargetDistributionMethod, BoundsScalingMethod);
	if (OpenLocations.IsEmpty())
	{
		UE_LOG(LogTargetManager, Warning, TEXT("OpenLocations is empty."));
		bSkipNextSpawn = true;
		return SpawnPointManager->FindSpawnPointFromLocation(GetBoxOrigin());
	}
	bSkipNextSpawn = false;

	if (bShowDebug_SpawnBox)
	{
		for (const FVector Vector : OpenLocations)
		{
			FVector Loc = FVector(Vector.X, Vector.Y + SpawnPointManager->GetSpawnMemoryIncY() / 2.f, Vector.Z + SpawnPointManager->GetSpawnMemoryIncZ() / 2.f);
			DrawDebugBox(GetWorld(), Loc, FVector(0, SpawnPointManager->GetSpawnMemoryIncY() / 2.f, SpawnPointManager->GetSpawnMemoryIncZ() / 2.f), FColor::Emerald, false, BSConfig.TargetConfig.TargetSpawnCD, 0, 6);
		}
	}
	
	if (BSConfig.SpatialConfig.bSpawnAtOriginWheneverPossible && OpenLocations.Contains(GetBoxOrigin()))
	{
		return SpawnPointManager->FindSpawnPointFromLocation(GetBoxOrigin());
	}

	if (ReinforcementLearningComponent->IsActive())
	{
		const int32 ChosenPoint = TryGetSpawnLocationFromReinforcementLearningComponent(OpenLocations);
		if (ChosenPoint != INDEX_NONE)
		{
			SpawnPointManager->GetSpawnCounter()[ChosenPoint].ActualChosenPoint = SpawnPointManager->GetSpawnCounter()[ChosenPoint].GetRandomSubPoint(GetBorderingDirections(OpenLocations, SpawnPointManager->GetSpawnCounter()[ChosenPoint].Point));
			return &SpawnPointManager->GetSpawnCounter()[ChosenPoint];
		}
		UE_LOG(LogTargetManager, Warning, TEXT("Unable to Spawn Location suggested by RLAgent."));
	}
	
	if (!BSConfig.GridConfig.bRandomizeGridTargetActivation && BSConfig.SpatialConfig.TargetDistributionMethod == ETargetDistributionPolicy::Grid)
	{
		const TArray<int32> Indices = PreviousSpawnPoint->GetBorderingIndices();
		FSpawnPoint* OldestRecentSpawnPoint = PreviousSpawnPoint;
		OpenLocations = OpenLocations.FilterByPredicate([&] (const FVector& Vector)
		{
			if (FSpawnPoint* FoundPoint = SpawnPointManager->FindSpawnPointFromLocation(Vector))
			{
				if (!FoundPoint->IsActivated() && FoundPoint->IsRecent() && Indices.Contains(FoundPoint->Index))
				{
					if (FoundPoint->GetTimeSetRecent() < OldestRecentSpawnPoint->GetTimeSetRecent())
					{
						OldestRecentSpawnPoint = FoundPoint;
					}
				}
				if (!FoundPoint->IsActivated() && !FoundPoint->IsRecent() && Indices.Contains(FoundPoint->Index))
				{
					return true;
				}
			}
			return false;
		});
		if (OpenLocations.IsEmpty())
		{
			OldestRecentSpawnPoint->ActualChosenPoint = OldestRecentSpawnPoint->GetRandomSubPoint(GetBorderingDirections(OpenLocations, OldestRecentSpawnPoint->Point));
			//UE_LOG(LogTargetManager, Display, TEXT("Found Point %s Found Center %s Found ActualChosenPoint %s"), *Found->Point.ToString(), *Found->Center.ToString(), *Found->ActualChosenPoint.ToString());
			return OldestRecentSpawnPoint;
		}
	}
	
	const int32 RandomPoint = UKismetMathLibrary::RandomIntegerInRange(0, OpenLocations.Num() - 1);
	
	if (FSpawnPoint* Found = SpawnPointManager->FindSpawnPointFromLocation(OpenLocations[RandomPoint]))
	{
		Found->ActualChosenPoint = Found->GetRandomSubPoint(GetBorderingDirections(OpenLocations, Found->Point));
		//UE_LOG(LogTargetManager, Display, TEXT("Found Point %s Found Center %s Found ActualChosenPoint %s"), *Found->Point.ToString(), *Found->Center.ToString(), *Found->ActualChosenPoint.ToString());
		return Found;
	}
	//UE_LOG(LogTargetManager, Display, TEXT("Found Point %s Found Center %s Found ActualChosenPoint %s"), *Found->Point.ToString(), *Found->Center.ToString(), *Found->ActualChosenPoint.ToString());
	return nullptr;
}

FVector ATargetManager::GetRandomBeatTrackLocation(const FVector& LocationBeforeChange) const
{
	const FVector NewExtent = FVector(BSConfig.SpatialConfig.MoveForwardDistance * 0.5, GetBoxExtents_Static().Y * 0.5, GetBoxExtents_Static().Z * 0.5);
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
	return NewLocation + UKismetMathLibrary::GetDirectionUnitVector(LocationBeforeChange, NewLocation) * MovingTargetSpeed * BSConfig.TargetConfig.TargetSpawnCD;
}

TArray<FVector> ATargetManager::GetValidSpawnLocations(const FVector& Scale, const ETargetDistributionPolicy& TargetDistributionMethod, const EBoundsScalingPolicy& BoundsScalingMethod) const
{
	TArray<FVector> ValidSpawnLocations;

	/* Populate AllPoints according to TargetDistributionMethod */
	switch (TargetDistributionMethod)
	{
	case ETargetDistributionPolicy::EdgeOnly:
		{
			const FVector MinExtrema = GetBoxExtrema(0, BoundsScalingMethod == EBoundsScalingPolicy::Dynamic);
			const FVector MaxExtrema = GetBoxExtrema(1, BoundsScalingMethod == EBoundsScalingPolicy::Dynamic);
			const float MaxY = MaxExtrema.Y - SpawnPointManager->GetSpawnMemoryIncY();
			const float MaxZ = MaxExtrema.Z - SpawnPointManager->GetSpawnMemoryIncZ();
			const float OriginX = GetBoxOrigin().X;

			for (float Y = MinExtrema.Y; Y < MaxExtrema.Y; Y += SpawnPointManager->GetSpawnMemoryIncY())
			{
				ValidSpawnLocations.AddUnique(FVector(OriginX, Y, MinExtrema.Z));
				ValidSpawnLocations.AddUnique(FVector(OriginX, Y, MaxZ));
			}
			for (float Z = MinExtrema.Z; Z < MaxExtrema.Z; Z += SpawnPointManager->GetSpawnMemoryIncZ())
			{
				ValidSpawnLocations.AddUnique(FVector(OriginX, MinExtrema.Y, Z));
				ValidSpawnLocations.AddUnique(FVector(OriginX, MaxY, Z));
			}
			ValidSpawnLocations.Add(GetBoxOrigin());
		}
		break;
	case ETargetDistributionPolicy::FullRange:
		{
			const FVector NegativeExtrema = GetBoxExtrema(0, BoundsScalingMethod == EBoundsScalingPolicy::Dynamic);
			const FVector PositiveExtrema = GetBoxExtrema(1, BoundsScalingMethod == EBoundsScalingPolicy::Dynamic);
			ValidSpawnLocations = GetAllSpawnLocations().FilterByPredicate([&](const FVector& Vector)
			{
				if (Vector.Y < NegativeExtrema.Y || Vector.Y >= PositiveExtrema.Y || Vector.Z < NegativeExtrema.Z || Vector.Z >= PositiveExtrema.Z)
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

	SpawnPointManager->RemoveOverlappingPointsFromSpawnLocations(ValidSpawnLocations, Scale);
	/* Remove points occupied by recent targets (overlapping points) */

	if (BSConfig.SpatialConfig.TargetDistributionMethod != ETargetDistributionPolicy::Grid)
	{
		/* Remove points that don't have another point to the top and to the right */
		RemoveEdgePoints(ValidSpawnLocations);
	}
	return ValidSpawnLocations;
}

TArray<FVector> ATargetManager::GetAllSpawnLocations() const
{
	return AllSpawnLocations;
}

FVector ATargetManager::GetBoxOrigin() const
{
	return SpawnBox->Bounds.Origin;
}

FVector ATargetManager::GetBoxExtrema(const int32 PositiveExtrema, const bool bDynamic) const
{
	if (bDynamic)
	{
		return SpawnBox->Bounds.GetBoxExtrema(PositiveExtrema);
	}
	if (PositiveExtrema == 1)
	{
		return StaticMaxExtrema;
	}
	return StaticMinExtrema;
}

FVector ATargetManager::GetBoxExtents_Static() const
{
	return StaticExtents;
}

TArray<EBorderingDirection> ATargetManager::GetBorderingDirections(const TArray<FVector>& ValidLocations, const FVector& Location) const
{
	TArray<EBorderingDirection> Directions;
	const FVector MinExtrema = GetBoxExtrema(0, true);
	const FVector MaxExtrema = GetBoxExtrema(1, true);
	const FVector Left = Location + FVector(0, -SpawnPointManager->GetSpawnMemoryIncY(), 0);
	const FVector Right = Location + FVector(0, SpawnPointManager->GetSpawnMemoryIncY(), 0);
	const FVector Up = Location + FVector(0, 0, SpawnPointManager->GetSpawnMemoryIncZ());
	const FVector Down = Location + FVector(0, 0, -SpawnPointManager->GetSpawnMemoryIncZ());

	if (Left.Y != MinExtrema.Y && !ValidLocations.Contains(Left))
	{
		Directions.Add(EBorderingDirection::Left);
	}
	if (Right.Y != MaxExtrema.Y && !ValidLocations.Contains(Right))
	{
		Directions.Add(EBorderingDirection::Right);
	}
	if (Up.Z != MaxExtrema.Z && !ValidLocations.Contains(Up))
	{
		Directions.Add(EBorderingDirection::Up);
	}
	if (Down.Z != MinExtrema.Z && !ValidLocations.Contains(Down))
	{
		Directions.Add(EBorderingDirection::Down);
	}
	return Directions;
}

void ATargetManager::RemoveEdgePoints(TArray<FVector>& In) const
{
	const FVector MinExtrema = GetBoxExtrema(0, BSConfig.SpatialConfig.BoundsScalingMethod == EBoundsScalingPolicy::Dynamic);
	const FVector MaxExtrema = GetBoxExtrema(1, BSConfig.SpatialConfig.BoundsScalingMethod == EBoundsScalingPolicy::Dynamic);
	TArray<FVector> InCopy;
	if (bShowDebug_SpawnBox)
	{
		InCopy = In;
	}

	if (BSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::SingleBeat || BSConfig.SpatialConfig.TargetDistributionMethod == ETargetDistributionPolicy::EdgeOnly)
	{
		In = In.FilterByPredicate([&](const FVector& Vector)
		{
			const FVector Right = Vector + FVector(0, SpawnPointManager->GetSpawnMemoryIncY(), 0);
			const FVector Top = Vector + FVector(0, 0, SpawnPointManager->GetSpawnMemoryIncZ());
			if (Vector.Y != MinExtrema.Y && Right.Y < MaxExtrema.Y && !In.Contains(Right))
			{
				return false;
			}
			if (Vector.Z != MinExtrema.Z && Top.Z < MaxExtrema.Z && !In.Contains(Top))
			{
				return false;
			}
			return true;
		});
	}
	else
	{
		In = In.FilterByPredicate([&](const FVector& Vector)
		{
			const FVector Right = Vector + FVector(0, SpawnPointManager->GetSpawnMemoryIncY(), 0);
			const FVector Top = Vector + FVector(0, 0, SpawnPointManager->GetSpawnMemoryIncZ());
			if (Right.Y < MaxExtrema.Y && !In.Contains(Right))
			{
				return false;
			}
			if (Top.Z < MaxExtrema.Z && !In.Contains(Top))
			{
				return false;
			}
			return true;
		});
	}

	/* Debug stuff */
	if (bShowDebug_SpawnBox)
	{
		int Count = 0;
		InCopy = InCopy.FilterByPredicate([&](const FVector& Vector)
		{
			if (In.Contains(Vector))
			{
				return false;
			}
			return true;
		});
		for (const FVector& Vector : InCopy)
		{
			FVector Loc = FVector(Vector.X, Vector.Y + SpawnPointManager->GetSpawnMemoryIncY() / 2.f, Vector.Z + SpawnPointManager->GetSpawnMemoryIncZ() / 2.f);
			DrawDebugBox(GetWorld(), Loc, FVector(0, SpawnPointManager->GetSpawnMemoryIncY() / 2.f, SpawnPointManager->GetSpawnMemoryIncZ() / 2.f), FColor::Red, false, BSConfig.TargetConfig.TargetSpawnCD, 1, 5);
			Count++;
		}
	}
	//UE_LOG(LogTargetManager, Display, TEXT("Removed Edge Points: %d"), Count);
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
	const TArray<FSpawnPoint> SpawnCounterCopy = SpawnPointManager->GetSpawnCounter();
	for (const FVector Vector : OpenLocations)
	{
		if (const int32 FoundIndex = SpawnCounterCopy.Find(FSpawnPoint(Vector)); FoundIndex != INDEX_NONE)
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

int32 ATargetManager::AddToManagedTargets(ASphereTarget* SpawnTarget)
{
	TArray<ASphereTarget*> Targets = GetManagedTargets();
	const int32 NewIndex = Targets.Emplace(SpawnTarget);
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

	TArray<FSpawnPoint> Counter = SpawnPointManager->GetSpawnCounter();

	for (int i = 0; i < SpawnPointManager->GetSpawnCounter().Num(); i++)
	{
		if (const int32 Found = QTableIndices.Find(SpawnPointManager->GetOutArrayIndexFromSpawnCounterIndex(i)); Found != INDEX_NONE)
		{
			const int32 RowNum = Found / 5;
			const int32 ColNum = Found % 5;

			if (Counter[i].TotalSpawns != INDEX_NONE)
			{
				if (OutArray[RowNum].TotalSpawns[ColNum] == INDEX_NONE)
				{
					OutArray[RowNum].TotalSpawns[ColNum] = 0;
				}
				OutArray[RowNum].TotalSpawns[ColNum] += Counter[i].TotalSpawns;
				OutArray[RowNum].TotalHits[ColNum] += Counter[i].TotalHits;
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
	TArray<FSpawnPoint> Counter = SpawnPointManager->GetSpawnCounter();
	for (int i = 0; i < SpawnPointManager->GetSpawnCounter().Num(); i++)
	{
		if (const int32 Found = Indices.Find(FQTableIndex(SpawnPointManager->GetOutArrayIndexFromSpawnCounterIndex(i))); Found != INDEX_NONE)
		{
			if (Counter[i].TotalSpawns != INDEX_NONE)
			{
				if (CommonScoreInfo.TotalSpawns[Found] == INDEX_NONE)
				{
					CommonScoreInfo.TotalSpawns[Found] = 0;
				}
				CommonScoreInfo.TotalSpawns[Found] += Counter[i].TotalSpawns;
				CommonScoreInfo.TotalHits[Found] += Counter[i].TotalHits;
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
