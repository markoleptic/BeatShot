// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "Target/TargetManager.h"
#include "BSGameMode.h"
#include "Player/BSPlayerController.h"
#include "GlobalConstants.h"
#include "Target/SphereTarget.h"
#include "BeatShot/BSGameplayTags.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Target/ReinforcementLearningComponent.h"
#include "Kismet/KismetMathLibrary.h"

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
	
	ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	GameMode->OnGameModeStarted.AddUObject(this, &ATargetManager::OnGameModeStarted);
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

	/* Initial target spawn location */
	SpawnLocation = SpawnBox->Bounds.Origin;
	PreviousSpawnLocation = SpawnLocation;

	/* Initial target size */
	TargetScale = GetNextTargetScale();

	/* Enable / Disable Reinforcement Learning */
	if (!BSConfig.AIConfig.bEnableReinforcementLearning)
	{
		ReinforcementLearningComponent->Deactivate();
	}

	if (BSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::BeatTrack)
	{
		OverlapSpawnBox->OnComponentEndOverlap.AddDynamic(this, &ATargetManager::OnOverlapEnd_OverlapSpawnBox);
		SpawnBeatTrackTarget();
	}
	else if (BSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::ChargedBeatTrack)
	{
		OverlapSpawnBox->OnComponentEndOverlap.AddDynamic(this, &ATargetManager::OnOverlapEnd_OverlapSpawnBox);
	}
	else if (BSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::BeatGrid)
	{
		InitBeatGrid();
	}
	
	if (BSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::BeatGrid ||
		BSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::BeatTrack)
	{
		return;
	}
	
	const int32 HalfWidth = StaticExtents.Y;
	const int32 HalfHeight = StaticExtents.Z;
	TArray PreferredScales = {100, 95, 90, 85, 80, 75, 70, 65, 60, 55, 50, 45, 40, 30, 25, 20, 15, 10, 5};
	bool bWidthScaleSelected = false;
	bool bHeightScaleSelected = false;
	for (const int32 Scale : PreferredScales)
	{
		if (!bWidthScaleSelected)
		{
			if (HalfWidth % Scale == 0)
			{
				if (HalfWidth / Scale % 5 == 0)
				{
					SpawnMemoryScaleY = 1.f / static_cast<float>(Scale);
					bWidthScaleSelected = true;
				}
			}
		}
		if (!bHeightScaleSelected)
		{
			if (HalfHeight % Scale == 0)
			{
				if (HalfHeight / Scale % 5 == 0)
				{
					SpawnMemoryScaleZ = 1.f / static_cast<float>(Scale);
					bHeightScaleSelected = true;
				}
			}
		}
		if (bHeightScaleSelected && bWidthScaleSelected)
		{
			break;
		}
	}
	if (!bWidthScaleSelected || !bHeightScaleSelected)
	{
		UE_LOG(LogTargetManager, Warning, TEXT("Couldn't Find Height/Width"));
		SpawnMemoryScaleY = 1.f / 50.f;
		SpawnMemoryScaleZ = 1.f / 50.f;
	}

	SpawnMemoryIncY = roundf(1.f / SpawnMemoryScaleY);
	SpawnMemoryIncZ = roundf(1.f / SpawnMemoryScaleZ);
	MinOverlapRadius = (SpawnMemoryIncY + SpawnMemoryIncZ) / 2.f;

	if (IsDynamicBoundsScalingPolicy(BSConfig.SpatialConfig.BoundsScalingMethod))
	{
		SetBoxExtents_Dynamic();
	}

	UE_LOG(LogTargetManager, Display, TEXT("SpawnBoxWidth %f SpawnBoxHeight %f"), StaticExtents.Y * 2, StaticExtents.Z * 2);
	UE_LOG(LogTargetManager, Display, TEXT("SpawnMemoryScaleY %f SpawnMemoryScaleZ %f"), SpawnMemoryScaleY, SpawnMemoryScaleZ);
	UE_LOG(LogTargetManager, Display, TEXT("SpawnMemoryIncY %d SpawnMemoryIncZ %d"), SpawnMemoryIncY, SpawnMemoryIncZ);

	const FIntPoint HeightWidth = InitializeSpawnCounter();

	SpawnCounterHeight = HeightWidth.X;
	SpawnCounterWidth = HeightWidth.Y;

	if (!BSConfig.AIConfig.bEnableReinforcementLearning)
	{
		return;
	}

	FRLAgentParams Params;
	Params.InQTable = GetScoreInfoFromDefiningConfig(InBSConfig.DefiningConfig).QTable;
	Params.DefaultMode = BSConfig.DefiningConfig.BaseGameMode;
	Params.CustomGameModeName = BSConfig.DefiningConfig.CustomGameModeName;
	Params.Size = GetSpawnCounter().Num();
	Params.SpawnCounterHeight = HeightWidth.X;
	Params.SpawnCounterWidth = HeightWidth.Y;
	Params.InEpsilon = BSConfig.AIConfig.Epsilon;
	Params.InGamma = BSConfig.AIConfig.Gamma;
	Params.InAlpha = BSConfig.AIConfig.Alpha;
	ReinforcementLearningComponent->Init(Params);
}

void ATargetManager::SetShouldSpawn(const bool bShouldSpawn)
{
	ShouldSpawn = bShouldSpawn;
}

void ATargetManager::OnAudioAnalyzerBeat()
{
	if (!ShouldSpawn)
	{
		return;
	}
	switch (BSConfig.DefiningConfig.BaseGameMode)
	{
	case EBaseGameMode::MultiBeat:
		SpawnMultiBeatTarget();
		break;
	case EBaseGameMode::SingleBeat:
		SpawnSingleBeatTarget();
		break;
	case EBaseGameMode::BeatTrack:
		UpdateBeatTrackTarget();
		break;
	case EBaseGameMode::BeatGrid:
		ActivateBeatGridTarget();
		break;
	case EBaseGameMode::ChargedBeatTrack:
		ActivateChargedTarget();
		break;
	case EBaseGameMode::None:
		break;
	}
}

void ATargetManager::OnGameModeStarted()
{
	if (BSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::BeatTrack)
	{
		for (const ASphereTarget* Target : GetManagedTargets())
		{
			if (Target->HasMatchingGameplayTag(FBSGameplayTags::Get().Target_State_Immune))
			{
				Target->RemoveImmunityEffect();
			}
		}
		UpdateBeatTrackTarget();
	}
	else if (BSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::ChargedBeatTrack)
	{
		UpdateBeatTrackTarget();
	}
}

FIntPoint ATargetManager::InitializeSpawnCounter()
{
	const FVector NegativeExtents = GetBoxExtrema(0, false);
	const FVector PositiveExtents = GetBoxExtrema(1, false);

	int Width = 0;
	int Height = 0;
	int Index = 0;
	for (float Z = NegativeExtents.Z; Z < PositiveExtents.Z; Z += SpawnMemoryIncZ)
	{
		Width = 0;
		for (float Y = NegativeExtents.Y; Y < PositiveExtents.Y; Y += SpawnMemoryIncY)
		{
			SpawnCounter.Emplace(FVectorCounter(Index, FVector(GetBoxOrigin().X, Y, Z), SpawnMemoryIncY, SpawnMemoryIncZ));
			AllSpawnLocations.Emplace(FVector(GetBoxOrigin().X, Y, Z));
			Index++;
			Width++;
		}
		Height++;
	}
	UE_LOG(LogTargetManager, Display, TEXT("SpawnCounterSize: %d %llu"), SpawnCounter.Num(), SpawnCounter.GetAllocatedSize());
	return FIntPoint(Height, Width);
}

void ATargetManager::InitBeatGrid()
{
	if (!RecentBeatGridIndices.IsEmpty())
	{
		RecentBeatGridIndices.Empty();
	}
	if (!ManagedTargets.IsEmpty())
	{
		ManagedTargets.Empty();
	}

	LastBeatGridIndex = INDEX_NONE;

	const float MaxTargetSize = BSConfig.TargetConfig.MaxTargetScale * SphereTargetRadius * 2;
	const float HSpacing = BSConfig.BeatGridConfig.BeatGridSpacing.X + MaxTargetSize;
	const float VSpacing = BSConfig.BeatGridConfig.BeatGridSpacing.Y + MaxTargetSize;
	const float TotalWidth = HSpacing * (BSConfig.BeatGridConfig.NumHorizontalBeatGridTargets - 1);
	const float TotalHeight = VSpacing * (BSConfig.BeatGridConfig.NumVerticalBeatGridTargets - 1);

	const float HStart = -TotalWidth / 2.f;
	const float HEnd = TotalWidth / 2.f;
	const float VStart = TotalHeight / 2.f;
	const float VEnd = -TotalHeight / 2.f;

	int32 Index = 0;
	for (float j = VStart; j >= VEnd; j -= VSpacing)
	{
		for (float i = HStart; i <= HEnd; i += HSpacing)
		{
			FVector BeatGridSpawnLocation = FVector(GetBoxOrigin().X, i, GetBoxOrigin().Z + j);
			SpawnTarget(BeatGridSpawnLocation, GetNextTargetScale(), false);
			BeatGridIndices.Emplace(FBeatGridIndex(Index, BSConfig.BeatGridConfig.NumHorizontalBeatGridTargets,
			                                       BSConfig.BeatGridConfig.NumVerticalBeatGridTargets * BSConfig.BeatGridConfig.NumHorizontalBeatGridTargets));
			Index++;
		}
	}
}

ASphereTarget* ATargetManager::SpawnTarget(const FVector& Location, const FVector& Scale, const bool bBroadcastSpawnEvent)
{
	ASphereTarget* Target = GetWorld()->SpawnActorDeferred<ASphereTarget>(ActorToSpawn, FTransform(FRotator::ZeroRotator, Location, Scale), this, nullptr,
	                                                                      ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	Target->InitTarget(BSConfig, PlayerSettings);
	Target->SetInitialSphereScale(Scale);
	Target->OnTargetDamageEventOrTimeout.AddDynamic(this, &ATargetManager::OnOnTargetHealthChangedOrExpired);
	Target->FinishSpawning(FTransform(), true);

	AddToManagedTargets(Target);
	
	if (BSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::SingleBeat ||
		BSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::MultiBeat)
	{
		AddToRecentTargets(Target, Scale);
	}

	if (ReinforcementLearningComponent->IsActive())
	{
		ReinforcementLearningComponent->AddToActiveTargetPairs(PreviousSpawnLocation, Location);
	}

	if (Location.Equals(GetBoxOrigin()))
	{
		LastTargetSpawnedCenter = true;
	}
	else
	{
		LastTargetSpawnedCenter = false;
	}

	PreviousSpawnLocation = Location;
	LocationBeforeDirectionChange = Location;

	if (bBroadcastSpawnEvent)
	{
		OnTargetActivatedOrSpawned.Broadcast();
		OnTargetActivated_AimBot.Broadcast(Target);
	}

	return Target;
}

void ATargetManager::SpawnMultiBeatTarget()
{
	if (!bSkipNextSpawn)
	{
		SpawnTarget(SpawnLocation, TargetScale, true);
	}
	FindNextTargetProperties();
}

void ATargetManager::SpawnSingleBeatTarget()
{
	if (!bSkipNextSpawn)
	{
		SpawnTarget(SpawnLocation, TargetScale, true);
	}
	
	/* Don't continue spawning for SingleBeat */
	SetShouldSpawn(false);
	FindNextTargetProperties();
}

/* TODO: Use SpawnCounter for BeatGrid */

void ATargetManager::ActivateBeatGridTarget()
{
	int32 ChosenTargetIndex;
	if (LastBeatGridIndex == INDEX_NONE || BSConfig.BeatGridConfig.RandomizeBeatGrid == true)
	{
		ChosenTargetIndex = FMath::RandRange(0, ManagedTargets.Num() - 1);
	}
	else
	{
		int32 NumRemoved = 0;
		TArray<int32> SpawnCandidates = BeatGridIndices[LastBeatGridIndex].GetBorderingIndices().FilterByPredicate([&] (const int32& Index)
		{
			/* Only remove up to 2 indices for a corner */
			if (BeatGridIndices[LastBeatGridIndex].IsCornerIndex() && NumRemoved >= 2)
			{
				return true;
			}
			/* Only remove up to 4 indices for a border */
			if (BeatGridIndices[LastBeatGridIndex].IsBorderIndex() && NumRemoved >= 4)
			{
				return true;
			}
			/* Only remove up to 7 indices for a middle */
			if (!BeatGridIndices[LastBeatGridIndex].IsBorderIndex() && !BeatGridIndices[LastBeatGridIndex].IsCornerIndex() && NumRemoved >= 7)
			{
				return true;
			}
			if (RecentBeatGridIndices.Contains(Index))
			{
				NumRemoved++;
				return false;
			}
			return true;
		});
		ChosenTargetIndex = SpawnCandidates[FMath::RandRange(0, SpawnCandidates.Num() - 1)];
		RecentBeatGridIndices.Insert(ChosenTargetIndex, 0);
		RecentBeatGridIndices.SetNum(7);
	}

	if (!GetManagedTargets()[ChosenTargetIndex])
	{
		return;
	}
	
	GetManagedTargets()[ChosenTargetIndex]->ActivateTarget(BSConfig.TargetConfig.TargetMaxLifeSpan);
	AddToActiveBeatGridGuids(GetManagedTargets()[ChosenTargetIndex]->GetGuid());
	LastBeatGridIndex = ChosenTargetIndex;
	OnTargetActivatedOrSpawned.Broadcast();
	OnTargetActivated_AimBot.Broadcast(GetManagedTargets()[ChosenTargetIndex]);

	if (bShowDebug_SpawnMemory)
	{
		for (int i = 0; i < GetManagedTargets().Num(); i++)
		{
			if (i == LastBeatGridIndex)
			{
				continue;
			}
			if (RecentBeatGridIndices.Contains(i))
			{
				ManagedTargets[i]->SetSphereColor(FLinearColor(FColor::Red));
				continue;
			}
			ManagedTargets[i]->SetColorToInactiveColor();
		}
	}
}

void ATargetManager::ActivateChargedTarget()
{
	if (!GetManagedTargets().IsEmpty() && !GetManagedTargets()[0]->IsTargetActiveAndDamageable())
	{
		OnTargetActivatedOrSpawned.Broadcast();
		GetManagedTargets()[0]->ActivateTarget(BSConfig.TargetConfig.TargetMaxLifeSpan);
	}
	else
	{
		SpawnChargedTarget();
	}
}

void ATargetManager::SpawnBeatTrackTarget()
{
	if (!GetManagedTargets().IsEmpty())
	{
		return;
	}
	SpawnTarget(SpawnLocation, TargetScale);
}

void ATargetManager::SpawnChargedTarget()
{
	if (!GetManagedTargets().IsEmpty())
	{
		return;
	}

	SpawnTarget(SpawnLocation, TargetScale, false);
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
	if (GetManagedTargets().IsEmpty())
	{
		return;
	}
	switch (BSConfig.DefiningConfig.BaseGameMode)
	{
	case EBaseGameMode::SingleBeat:
	case EBaseGameMode::MultiBeat:
		if (!BSConfig.SpatialConfig.bMoveTargetsForward)
		{
			return;
		}
		for (ASphereTarget* Target : GetManagedTargets())
		{
			if (Target != nullptr)
			{
				MoveTargetForward(Target, DeltaTime);
			}
		}
		return;
	case EBaseGameMode::BeatGrid:
		{
			TArray<FGuid> Guids = GetActiveBeatGridGuids();
			TArray<ASphereTarget*> Filtered = GetManagedTargets().FilterByPredicate([&Guids] (const ASphereTarget* Target)
			{
				if (Guids.Contains(Target->GetGuid()))
				{
					return true;
				}
				return false;
			});
			for (ASphereTarget* Target : Filtered)
			{
				MoveTargetForward(Target, DeltaTime);
			}
		}
		return;
	case EBaseGameMode::BeatTrack:
		{
			if (GetManagedTargets()[0]->HasMatchingGameplayTag(FBSGameplayTags::Get().Target_State_PreGameModeStart))
			{
				return;
			}
			CurrentMovingTargetLocation = GetManagedTargets()[0]->GetActorLocation();
			CurrentMovingTargetLocation += CurrentMovingTargetDirection * MovingTargetSpeed * DeltaTime;
			GetManagedTargets()[0]->SetActorLocation(CurrentMovingTargetLocation);
		}
		return;
	case EBaseGameMode::ChargedBeatTrack:
		{
			if (GetManagedTargets()[0]->HasMatchingGameplayTag(FBSGameplayTags::Get().Target_State_PreGameModeStart))
			{
				return;
			}
			CurrentMovingTargetLocation = GetManagedTargets()[0]->GetActorLocation();
			CurrentMovingTargetLocation += CurrentMovingTargetDirection * MovingTargetSpeed * DeltaTime;
			GetManagedTargets()[0]->SetActorLocation(CurrentMovingTargetLocation);
		}
	case EBaseGameMode::None:
		break;
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
	
	/* BeatTrack only has one target, so it doesn't need to do anything further */
	if (BSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::BeatGrid)
	{
		RemoveFromActiveBeatGridGuids(TargetDamageEvent.Guid);
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
	RemoveFromManagedTargets(TargetDamageEvent.Guid);
	FTimerHandle TimerHandle;
	RemoveFromRecentDelegate.BindUObject(this, &ATargetManager::RemoveFromRecentTargets, TargetDamageEvent.Guid);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, RemoveFromRecentDelegate, BSConfig.TargetConfig.TargetSpawnCD, false);

	/* Find the index that corresponds to the destroyed target's location */
	if (const int32 Index = SpawnCounter.Find(Location); Index != INDEX_NONE)
	{
		/* Update the total target spawns in SpawnCounter */
		if (SpawnCounter[Index].TotalSpawns == INDEX_NONE)
		{
			SpawnCounter[Index].TotalSpawns = 1;
		}
		else
		{
			SpawnCounter[Index].TotalSpawns++;
		}

		/* Update the total target hits in SpawnCounter */
		if (TargetDamageEvent.TimeAlive != INDEX_NONE)
		{
			SpawnCounter[Index].TotalHits++;
		}

		/* Update the Reinforcement Learning Component */
		if (ReinforcementLearningComponent->IsActive())
		{
			ReinforcementLearningComponent->UpdateReinforcementLearningReward(Location, TargetDamageEvent.TimeAlive != INDEX_NONE);
			ReinforcementLearningComponent->UpdateReinforcementLearningComponent(GetSpawnCounter());
		}
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
	SpawnLocation = GetNextTargetSpawnLocation(BSConfig.SpatialConfig.BoundsScalingMethod, TargetScale);
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

FVector ATargetManager::GetNextTargetSpawnLocation(const EBoundsScalingPolicy BoundsScalingMethod, const FVector& NewTargetScale)
{
	if (BSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::SingleBeat && !LastTargetSpawnedCenter)
	{
		bSkipNextSpawn = false;
		return GetBoxOrigin();
	}
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
		return GetBoxOrigin();
	}
	bSkipNextSpawn = false;

	if (bShowDebug_SpawnBox)
	{
		for (const FVector Vector : OpenLocations)
		{
			FVector Loc = FVector(Vector.X, Vector.Y + SpawnMemoryIncY / 2.f, Vector.Z + SpawnMemoryIncZ / 2.f);
			DrawDebugBox(GetWorld(), Loc, FVector(0, SpawnMemoryIncY / 2.f, SpawnMemoryIncZ / 2.f), FColor::Emerald, false, BSConfig.TargetConfig.TargetSpawnCD, 0, 6);
		}
	}

	/* Spawn at origin whenever possible */
	if (OpenLocations.Contains(GetBoxOrigin()) && BSConfig.DefiningConfig.BaseGameMode != EBaseGameMode::ChargedBeatTrack)
	{
		return GetBoxOrigin();
	}

	if (ReinforcementLearningComponent->IsActive())
	{
		const int32 ChosenPoint = TryGetSpawnLocationFromReinforcementLearningComponent(OpenLocations);
		if (ChosenPoint != INDEX_NONE)
		{
			FVectorCounter Found = GetSpawnCounter()[ChosenPoint];
			Found.ActualChosenPoint = Found.GetRandomSubPoint(GetBorderingDirections(OpenLocations, Found.Point));
			return Found.ActualChosenPoint;
		}
		UE_LOG(LogTargetManager, Warning, TEXT("Unable to Spawn Location suggested by RLAgent."));
	}

	const int32 RandomPoint = UKismetMathLibrary::RandomIntegerInRange(0, OpenLocations.Num() - 1);
	FVectorCounter Found = GetVectorCounterFromPoint(OpenLocations[RandomPoint]);
	Found.ActualChosenPoint = Found.GetRandomSubPoint(GetBorderingDirections(OpenLocations, Found.Point));
	//UE_LOG(LogTargetManager, Display, TEXT("Found Point %s Found Center %s Found ActualChosenPoint %s"), *Found.Point.ToString(), *Found.Center.ToString(), *Found.ActualChosenPoint.ToString());
	return Found.ActualChosenPoint;
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
			const float MaxY = MaxExtrema.Y - SpawnMemoryIncY;
			const float MaxZ = MaxExtrema.Z - SpawnMemoryIncZ;
			const float OriginX = GetBoxOrigin().X;

			for (float Y = MinExtrema.Y; Y < MaxExtrema.Y; Y += SpawnMemoryIncY)
			{
				ValidSpawnLocations.AddUnique(FVector(OriginX, Y, MinExtrema.Z));
				ValidSpawnLocations.AddUnique(FVector(OriginX, Y, MaxZ));
			}
			for (float Z = MinExtrema.Z; Z < MaxExtrema.Z; Z += SpawnMemoryIncZ)
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

	/* Remove points occupied by recent targets (overlapping points) */
	for (FVector Location : GetAllOverlappingPoints(Scale))
	{
		ValidSpawnLocations.Remove(Location);
	}

	/* Remove points that don't have another point to the top and to the right */
	RemoveEdgePoints(ValidSpawnLocations);

	return ValidSpawnLocations;
}

TArray<FVector> ATargetManager::GetAllSpawnLocations() const
{
	return AllSpawnLocations;
}

TArray<FVector> ATargetManager::GetOverlappingPoints(const FVector& Center, const FVector& Scale) const
{
	float Radius = Scale.X * SphereTargetRadius * 2.f + BSConfig.SpatialConfig.MinDistanceBetweenTargets / 2.f;
	Radius = FMath::Max(Radius, MinOverlapRadius);
	const FSphere Sphere = FSphere(Center, Radius);
	const FVector NegativeExtents = GetBoxExtrema(0, false);
	const FVector PositiveExtents = GetBoxExtrema(1, false);
	TArray<FVector> BlockedPoints;
	int Count = 0;
	for (float Z = NegativeExtents.Z; Z < PositiveExtents.Z; Z += SpawnMemoryIncZ)
	{
		for (float Y = NegativeExtents.Y; Y < PositiveExtents.Y; Y += SpawnMemoryIncY)
		{
			Count++;
			if (FVector Loc = FVector(GetBoxOrigin().X, Y, Z); Sphere.IsInside(Loc))
			{
				BlockedPoints.AddUnique(Loc);
			}
		}
	}
	//UE_LOG(LogTargetManager, Display, TEXT("GetOverlappingPoints Count %d"), Count);
	//DrawDebugSphere(GetWorld(), Center, Scale * SphereTargetRadius * 2 + (BSConfig.MinDistanceBetweenTargets / 2.f), 32, FColor::Magenta, false, 0.5f);
	//UE_LOG(LogTargetManager, Display, TEXT("BlockedPoints: %d"), BlockedPoints.Num());
	return BlockedPoints;
}

TArray<FVector> ATargetManager::GetAllOverlappingPoints(const FVector& Scale) const
{
	TArray<FVector> OverlappingPoints;
	/* Resizing Overlapping Points if necessary */
	for (FRecentTarget Struct : GetRecentTargets())
	{
		TArray<FVector> CurrentBlockedLocations;
		if (Scale.Length() > Struct.TargetScale.Length())
		{
			CurrentBlockedLocations = GetOverlappingPoints(Struct.CenterVector, Scale);
		}
		else
		{
			CurrentBlockedLocations = Struct.OverlappingPoints;
		}
		OverlappingPoints.Append(CurrentBlockedLocations);
		if (bShowDebug_SpawnMemory)
		{
			for (FVector Vector : Struct.OverlappingPoints)
			{
				DrawDebugPoint(GetWorld(), Vector, 10, FColor::Red, false, 0.35f);
			}
		}
	}
	return OverlappingPoints;
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
	const FVector Left = Location + FVector(0, -SpawnMemoryIncY, 0);
	const FVector Right = Location + FVector(0, SpawnMemoryIncY, 0);
	const FVector Up = Location + FVector(0, 0, SpawnMemoryIncZ);
	const FVector Down = Location + FVector(0, 0, -SpawnMemoryIncZ);

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

FVectorCounter ATargetManager::GetVectorCounterFromPoint(const FVector& Point) const
{
	const int32 Index = GetSpawnCounter().Find(Point);
	if (Index == INDEX_NONE)
	{
		UE_LOG(LogTargetManager, Warning, TEXT("Point Not Found in SpawnCounter"));
		return FVectorCounter();
	}
	return GetSpawnCounter()[Index];
}

void ATargetManager::MoveTargetForward(ASphereTarget* SpawnTarget, float DeltaTime) const
{
	const FVector Loc = SpawnTarget->GetActorLocation();
	const FVector NewLoc = FVector(Loc.X - BSConfig.SpatialConfig.MoveForwardDistance, Loc.Y, Loc.Z);
	SpawnTarget->SetActorLocation(UKismetMathLibrary::VInterpTo(Loc, NewLoc, DeltaTime, 1 / BSConfig.TargetConfig.TargetMaxLifeSpan));
}

int32 ATargetManager::AddToRecentTargets(const ASphereTarget* SpawnTarget, const FVector& Scale)
{
	TArray<FRecentTarget> Targets = GetRecentTargets();
	const int32 NewIndex = Targets.Emplace(FRecentTarget(SpawnTarget->GetGuid(), GetOverlappingPoints(SpawnTarget->GetActorLocation(), Scale), Scale, SpawnTarget->GetActorLocation()));
	RecentTargets = Targets;
	return NewIndex;
}

int32 ATargetManager::AddToManagedTargets(ASphereTarget* SpawnTarget)
{
	TArray<ASphereTarget*> Targets = GetManagedTargets();
	const int32 NewIndex = Targets.Emplace(SpawnTarget);
	ManagedTargets = Targets;
	return NewIndex;
}

void ATargetManager::AddToActiveBeatGridGuids(const FGuid GuidToAdd)
{
	TArray<FGuid> ActiveBeatGridGuidsCopy = GetActiveBeatGridGuids();
	ActiveBeatGridGuidsCopy.Add(GuidToAdd);
	ActiveBeatGridGuids = ActiveBeatGridGuidsCopy;
}

void ATargetManager::RemoveFromRecentTargets(const FGuid GuidToRemove)
{
	TArray<FRecentTarget> Targets = GetRecentTargets();
	Targets.Remove(FRecentTarget(GuidToRemove));
	RecentTargets = Targets;
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

void ATargetManager::RemoveFromActiveBeatGridGuids(const FGuid GuidToRemove)
{
	TArray<FGuid> ActiveBeatGridGuidsCopy = GetActiveBeatGridGuids();
	ActiveBeatGridGuidsCopy.Remove(GuidToRemove);
	ActiveBeatGridGuids = ActiveBeatGridGuidsCopy;
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
			const FVector Right = Vector + FVector(0, SpawnMemoryIncY, 0);
			const FVector Top = Vector + FVector(0, 0, SpawnMemoryIncZ);
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
			const FVector Right = Vector + FVector(0, SpawnMemoryIncY, 0);
			const FVector Top = Vector + FVector(0, 0, SpawnMemoryIncZ);
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
			FVector Loc = FVector(Vector.X, Vector.Y + SpawnMemoryIncY / 2.f, Vector.Z + SpawnMemoryIncZ / 2.f);
			DrawDebugBox(GetWorld(), Loc, FVector(0, SpawnMemoryIncY / 2.f, SpawnMemoryIncZ / 2.f), FColor::Red, false, BSConfig.TargetConfig.TargetSpawnCD, 1, 5);
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
	const float Y = FMath::GridSnap(LerpY, SpawnMemoryIncY);
	const float Z = FMath::GridSnap(LerpZ, SpawnMemoryIncZ);
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

int32 ATargetManager::GetOutArrayIndexFromSpawnCounterIndex(const int32 SpawnCounterIndex) const
{
	/* First find the Row and Column number that corresponds to the SpawnCounter index */
	const int32 SpawnCounterRowNum = SpawnCounterIndex / SpawnCounterWidth;
	const int32 SpawnCounterColNum = SpawnCounterIndex % SpawnCounterWidth;

	const int32 WidthScaleFactor = SpawnCounterWidth / 5;
	const int32 HeightScaleFactor = SpawnCounterHeight / 5;

	/* Scale down the SpawnCounter row and column numbers */
	const int32 Row = SpawnCounterRowNum / HeightScaleFactor;
	const int32 Col = SpawnCounterColNum / WidthScaleFactor;
	const int32 Index = Row * 5 + Col;

	return Index;
}

int32 ATargetManager::TryGetSpawnLocationFromReinforcementLearningComponent(const TArray<FVector>& OpenLocations) const
{
	/* Converting all OpenLocations to indices */
	TArray<int32> Indices;
	const TArray<FVectorCounter> SpawnCounterCopy = GetSpawnCounter();
	for (const FVector Vector : OpenLocations)
	{
		if (const int32 FoundIndex = SpawnCounterCopy.Find(Vector); FoundIndex != INDEX_NONE)
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

	TArray<FVectorCounter> Counter = GetSpawnCounter();

	for (int i = 0; i < GetSpawnCounter().Num(); i++)
	{
		if (const int32 Found = QTableIndices.Find(GetOutArrayIndexFromSpawnCounterIndex(i)); Found != INDEX_NONE)
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
	TArray<FVectorCounter> Counter = GetSpawnCounter();
	for (int i = 0; i < GetSpawnCounter().Num(); i++)
	{
		if (const int32 Found = Indices.Find(GetOutArrayIndexFromSpawnCounterIndex(i)); Found != INDEX_NONE)
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
