// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "Target/TargetSpawner.h"
#include "Character/BSHealthComponent.h"
#include "Player/BSPlayerController.h"
#include "GlobalConstants.h"
#include "Target/SphereTarget.h"
#include "BeatShot/BSGameplayTags.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Target/ReinforcementLearningComponent.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogTargetSpawner);
using namespace Constants;

ATargetSpawner::ATargetSpawner()
{
	PrimaryActorTick.bCanEverTick = true;

	SpawnBox = CreateDefaultSubobject<UBoxComponent>("SpawnBox");
	RootComponent = SpawnBox;
	OverlapSpawnBox = CreateDefaultSubobject<UBoxComponent>("BeatTrack SpawnBox");
	ReinforcementLearningComponent = CreateDefaultSubobject<UReinforcementLearningComponent>("Reinforcement Learning Component");
	SetShouldSpawn(false);
	LastTargetSpawnedCenter = false;
	bSkipNextSpawn = false;
	ConsecutiveTargetsHit = 0;
}

void ATargetSpawner::BeginPlay()
{
	Super::BeginPlay();
}

void ATargetSpawner::Destroyed()
{
	if (!ActiveTargets.IsEmpty())
	{
		for (ASphereTarget* Target : ActiveTargets)
		{
			Target->Destroy();
		}
	}
	if (SpawnedBeatGridTargets.Num() > 0)
	{
		for (ASphereTarget* Target : SpawnedBeatGridTargets)
		{
			Target->Destroy();
		}
	}
	Super::Destroyed();
}

void ATargetSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// TEMP
	if (BSConfig.TargetConfig.NumCharges > 0)
	{
		OnTick_UpdateBeatTrackTargetLocation(DeltaTime);
		return;
	}
	// END TEMP

	if (BSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::BeatTrack)
	{
		OnTick_UpdateBeatTrackTargetLocation(DeltaTime);
		return;
	}

	if (BSConfig.SpatialConfig.bMoveTargetsForward)
	{
		for (ASphereTarget* Target : GetActiveTargets())
		{
			if (Target != nullptr)
			{
				MoveTargetForward(Target, DeltaTime);
			}
		}
	}
}

void ATargetSpawner::InitTargetSpawner(const FBSConfig& InBSConfig, const FPlayerSettings_Game& InPlayerSettings)
{
	/* Initialize local copy of FBSConfig */
	BSConfig = InBSConfig;
	PlayerSettings = InPlayerSettings;

	/* GameMode menu uses the full width, while box bounds are only half width / half height */
	StaticExtents = BSConfig.SpatialConfig.GenerateTargetSpawnerBoxBounds();

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
		OverlapSpawnBox->OnComponentEndOverlap.AddUniqueDynamic(this, &ATargetSpawner::OnBeatTrackOverlapEnd);
		SpawnBeatTrackTarget();
		return;
	}

	// TEMP
	if (BSConfig.TargetConfig.NumCharges > 0)
	{
		OverlapSpawnBox->OnComponentEndOverlap.AddUniqueDynamic(this, &ATargetSpawner::OnBeatTrackOverlapEnd);
	}
	// END TEMP

	if (BSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::BeatGrid)
	{
		InitBeatGrid();
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
		UE_LOG(LogTargetSpawner, Warning, TEXT("Couldn't Find Height/Width"));
		SpawnMemoryScaleY = 1.f / 50.f;
		SpawnMemoryScaleZ = 1.f / 50.f;
	}

	SpawnMemoryIncY = roundf(1.f / SpawnMemoryScaleY);
	SpawnMemoryIncZ = roundf(1.f / SpawnMemoryScaleZ);
	MinOverlapRadius = (SpawnMemoryIncY + SpawnMemoryIncZ) / 2.f;

	if (IsDynamicSpreadType(BSConfig.SpatialConfig.BoundsScalingMethod))
	{
		SetBoxExtents_Dynamic();
	}

	UE_LOG(LogTargetSpawner, Display, TEXT("SpawnBoxWidth %f SpawnBoxHeight %f"), StaticExtents.Y * 2, StaticExtents.Z * 2);
	UE_LOG(LogTargetSpawner, Display, TEXT("SpawnMemoryScaleY %f SpawnMemoryScaleZ %f"), SpawnMemoryScaleY, SpawnMemoryScaleZ);
	UE_LOG(LogTargetSpawner, Display, TEXT("SpawnMemoryIncY %d SpawnMemoryIncZ %d"), SpawnMemoryIncY, SpawnMemoryIncZ);

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

void ATargetSpawner::SetShouldSpawn(const bool bShouldSpawn)
{
	if (bShouldSpawn)
	{
		for (ASphereTarget* Target : GetActiveTargets())
		{
			if (Target->HasMatchingGameplayTag(FBSGameplayTags::Get().Target_State_PreGameModeStart))
			{
				Target->RemoveGameplayTag(FBSGameplayTags::Get().Target_State_PreGameModeStart);
			}
		}
	}
	ShouldSpawn = bShouldSpawn;
}

void ATargetSpawner::CallSpawnFunction()
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
		SetNewTrackingDirection();
		break;
	case EBaseGameMode::BeatGrid:
		ActivateBeatGridTarget();
		break;
	default:
		break;
	}
}

TArray<FAccuracyRow> ATargetSpawner::GetLocationAccuracy() const
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

FCommonScoreInfo ATargetSpawner::GetCommonScoreInfo() const
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

FIntPoint ATargetSpawner::InitializeSpawnCounter()
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
	UE_LOG(LogTargetSpawner, Display, TEXT("SpawnCounterSize: %d %llu"), SpawnCounter.Num(), SpawnCounter.GetAllocatedSize());
	return FIntPoint(Height, Width);
}

void ATargetSpawner::InitBeatGrid()
{
	if (!RecentBeatGridIndices.IsEmpty())
	{
		RecentBeatGridIndices.Empty();
	}
	if (!SpawnedBeatGridTargets.IsEmpty())
	{
		SpawnedBeatGridTargets.Empty();
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
			ASphereTarget* SpawnTarget = GetWorld()->SpawnActorDeferred<ASphereTarget>(ActorToSpawn, FTransform(FRotator::ZeroRotator, BeatGridSpawnLocation, FVector(1)), this, nullptr,
			                                                                           ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
			SpawnTarget->InitTarget(BSConfig, PlayerSettings);
			SpawnTarget->SetInitialSphereScale(FVector(GetNextTargetScale()));
			SpawnTarget->OnLifeSpanExpired.AddDynamic(this, &ATargetSpawner::OnTargetTimeout);
			SpawnTarget->FinishSpawning(FTransform(), true);
			BeatGridIndices.Emplace(FBeatGridIndex(Index, BSConfig.BeatGridConfig.NumHorizontalBeatGridTargets,
			                                       BSConfig.BeatGridConfig.NumVerticalBeatGridTargets * BSConfig.BeatGridConfig.NumHorizontalBeatGridTargets));
			SpawnedBeatGridTargets.Add(SpawnTarget);
			Index++;
		}
	}
}

void ATargetSpawner::SpawnMultiBeatTarget()
{
	if (bSkipNextSpawn)
	{
		FindNextTargetProperties();
		return;
	}
	
	// TEMP
	if (BSConfig.TargetConfig.NumCharges > 0)
	{
		if (!GetActiveTargets().IsEmpty())
		{
			ActivateChargedTarget();
			return;
		}
	}
	// END TEMP
	ASphereTarget* SpawnTarget = GetWorld()->SpawnActorDeferred<ASphereTarget>(ActorToSpawn, FTransform(FRotator::ZeroRotator, SpawnLocation, FVector(1)), this, nullptr,
	                                                                           ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	SpawnTarget->InitTarget(BSConfig, PlayerSettings);
	SpawnTarget->SetInitialSphereScale(FVector(TargetScale));
	SpawnTarget->OnLifeSpanExpired.AddDynamic(this, &ATargetSpawner::OnTargetTimeout);
	SpawnTarget->FinishSpawning(FTransform(), true);
	AddToActiveTargets(SpawnTarget);
	AddToRecentTargets(SpawnTarget, TargetScale);
	OnTargetSpawned.Broadcast();
	OnTargetSpawned_AimBot.Broadcast(SpawnTarget);

	if (!PreviousSpawnLocation.Equals(SpawnLocation) && BSConfig.AIConfig.bEnableReinforcementLearning)
	{
		ReinforcementLearningComponent->AddToActiveTargetPairs(PreviousSpawnLocation, SpawnLocation);
	}
	PreviousSpawnLocation = SpawnLocation;

	// TEMP
	if (BSConfig.TargetConfig.NumCharges > 0)
	{
		LocationBeforeDirectionChange = GetActiveTargets()[0]->GetActorLocation();
		MovingTargetSpeed = FMath::FRandRange(BSConfig.BeatTrackConfig.MinTrackingSpeed, BSConfig.BeatTrackConfig.MaxTrackingSpeed);
		EndLocation = GetRandomBeatTrackLocation(LocationBeforeDirectionChange);
		CurrentMovingTargetDirection = UKismetMathLibrary::GetDirectionUnitVector(LocationBeforeDirectionChange, EndLocation);
	}
	// END TEMP
	
	FindNextTargetProperties();
}

void ATargetSpawner::SpawnSingleBeatTarget()
{
	if (bSkipNextSpawn)
	{
		/* Don't continue spawning for SingleBeat */
		SetShouldSpawn(false);
		FindNextTargetProperties();
		return;
	}
	
	ASphereTarget* SpawnTarget = GetWorld()->SpawnActorDeferred<ASphereTarget>(ActorToSpawn, FTransform(FRotator::ZeroRotator, SpawnLocation, FVector(1)), this, nullptr,
	                                                                           ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	SpawnTarget->InitTarget(BSConfig, PlayerSettings);
	SpawnTarget->SetInitialSphereScale(FVector(TargetScale));
	SpawnTarget->OnLifeSpanExpired.AddDynamic(this, &ATargetSpawner::OnTargetTimeout);
	SpawnTarget->FinishSpawning(FTransform(), true);
	AddToActiveTargets(SpawnTarget);
	AddToRecentTargets(SpawnTarget, TargetScale);
	OnTargetSpawned.Broadcast();
	OnTargetSpawned_AimBot.Broadcast(SpawnTarget);

	if (!PreviousSpawnLocation.Equals(SpawnLocation) && BSConfig.AIConfig.bEnableReinforcementLearning)
	{
		ReinforcementLearningComponent->AddToActiveTargetPairs(PreviousSpawnLocation, SpawnLocation);
	}
	PreviousSpawnLocation = SpawnLocation;
	if (SpawnLocation.Equals(GetBoxOrigin()))
	{
		LastTargetSpawnedCenter = true;
	}
	else
	{
		LastTargetSpawnedCenter = false;
	}
	
	/* Don't continue spawning for SingleBeat */
	SetShouldSpawn(false);
	FindNextTargetProperties();
}

void ATargetSpawner::ActivateBeatGridTarget()
{
	int32 ChosenTargetIndex;
	if (LastBeatGridIndex == INDEX_NONE || BSConfig.BeatGridConfig.RandomizeBeatGrid == true)
	{
		ChosenTargetIndex = FMath::RandRange(0, SpawnedBeatGridTargets.Num() - 1);
	}
	else
	{
		int32 NumRemoved = 0;
		TArray<int32> SpawnCandidates = BeatGridIndices[LastBeatGridIndex].GetBorderingIndices();
		SpawnCandidates = SpawnCandidates.FilterByPredicate([&](const int32& Index)
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
		const int32 RandomIndex = FMath::RandRange(0, SpawnCandidates.Num() - 1);
		ChosenTargetIndex = SpawnCandidates[RandomIndex];
		RecentBeatGridIndices.Insert(ChosenTargetIndex, 0);
		RecentBeatGridIndices.SetNum(7);
	}

	ASphereTarget* ChosenTarget = SpawnedBeatGridTargets[ChosenTargetIndex];
	LastBeatGridIndex = ChosenTargetIndex;
	ChosenTarget->ActivateBeatGridTarget(BSConfig.TargetConfig.TargetMaxLifeSpan);
	AddToActiveTargets(ChosenTarget);
	OnTargetSpawned.Broadcast();
	OnTargetSpawned_AimBot.Broadcast(ChosenTarget);

	if (bShowDebug_SpawnMemory)
	{
		for (int i = 0; i < SpawnedBeatGridTargets.Num(); i++)
		{
			if (i == LastBeatGridIndex)
			{
				continue;
			}
			if (RecentBeatGridIndices.Contains(i))
			{
				SpawnedBeatGridTargets[i]->SetSphereColor(FLinearColor(FColor::Red));
				continue;
			}
			SpawnedBeatGridTargets[i]->SetColorToBeatGridColor();
		}
	}
}

void ATargetSpawner::ActivateChargedTarget()
{
	if (GetActiveTargets()[0])
	{
		GetActiveTargets()[0]->ActivateChargedTarget(BSConfig.TargetConfig.TargetMaxLifeSpan);
	}
}

void ATargetSpawner::SpawnBeatTrackTarget()
{
	if (!GetActiveTargets().IsEmpty())
	{
		return;
	}
	
	ASphereTarget* BeatTrackTarget = GetWorld()->SpawnActorDeferred<ASphereTarget>(ActorToSpawn, FTransform(FRotator::ZeroRotator, SpawnBox->Bounds.Origin, FVector(1)), this, nullptr,
	                                                                               ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	BeatTrackTarget->InitTarget(BSConfig, PlayerSettings);
	BeatTrackTarget->HealthComponent->OnHealthChanged.AddUObject(this, &ATargetSpawner::OnBeatTrackTargetHealthChanged);
	AddToActiveTargets(BeatTrackTarget);
	BeatTrackTarget->FinishSpawning(FTransform(), true);
	LocationBeforeDirectionChange = SpawnBox->Bounds.Origin;
	OnTargetSpawned.Broadcast();
}

void ATargetSpawner::SetNewTrackingDirection()
{
	if (!OnBeatTrackDirectionChanged.ExecuteIfBound(EndLocation))
	{
		UE_LOG(LogTargetSpawner, Display, TEXT("OnBeatTrackDirectionChanged not bound."));
	}

	LocationBeforeDirectionChange = GetActiveTargets()[0]->GetActorLocation();
	const float NewTargetScale = GetNextTargetScale();
	GetActiveTargets()[0]->SetInitialSphereScale(FVector(NewTargetScale));
	MovingTargetSpeed = FMath::FRandRange(BSConfig.BeatTrackConfig.MinTrackingSpeed, BSConfig.BeatTrackConfig.MaxTrackingSpeed);
	EndLocation = GetRandomBeatTrackLocation(LocationBeforeDirectionChange);
	CurrentMovingTargetDirection = UKismetMathLibrary::GetDirectionUnitVector(LocationBeforeDirectionChange, EndLocation);
}

void ATargetSpawner::OnTick_UpdateBeatTrackTargetLocation(const float DeltaTime)
{
	// TEMP
	if (GetActiveTargets().IsEmpty() || GetActiveTargets()[0]->HasMatchingGameplayTag(FBSGameplayTags::Get().Target_State_PreGameModeStart))
	// END TEMP
	{
		return;
	}
	CurrentMovingTargetLocation = GetActiveTargets()[0]->GetActorLocation();
	CurrentMovingTargetLocation += CurrentMovingTargetDirection * MovingTargetSpeed * DeltaTime;
	GetActiveTargets()[0]->SetActorLocation(CurrentMovingTargetLocation);
}

void ATargetSpawner::OnTargetTimeout(const bool DidExpire, const float TimeAlive, ASphereTarget* DestroyedTarget)
{
	if (BSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::SingleBeat) SetShouldSpawn(true);
	if (DidExpire)
	{
		ConsecutiveTargetsHit = 0;
		DynamicSpawnScale = FMath::Clamp(DynamicSpawnScale - 5, 0, 100);
	}
	else
	{
		ConsecutiveTargetsHit++;
		DynamicSpawnScale = FMath::Clamp(DynamicSpawnScale + 1, 0, 100);
	}

	const FVector Location = DestroyedTarget->GetActorLocation();
	const FVector CombatTextLocation = {Location.X, Location.Y, Location.Z + SphereTargetRadius * DestroyedTarget->GetActorScale3D().Z};
	OnTargetDestroyed.Broadcast(TimeAlive, ConsecutiveTargetsHit, CombatTextLocation);

	// TEMP
	if (BSConfig.TargetConfig.NumCharges > 0)
	{
		if (DestroyedTarget->BSConfig.TargetConfig.NumCharges > 0)
		{
			return;
		}
	}
	// END TEMP

	RemoveFromActiveTargets(DestroyedTarget);
	FTimerHandle TimerHandle;
	RemoveFromRecentDelegate.BindUObject(this, &ATargetSpawner::RemoveFromRecentTargets, DestroyedTarget->Guid);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, RemoveFromRecentDelegate, BSConfig.TargetConfig.TargetSpawnCD, false);

	if (BSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::BeatTrack)
	{
		return;
	}

	if (BSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::BeatGrid)
	{
		DestroyedTarget->SetActorLocation(FVector(GetBoxOrigin().X, Location.Y, Location.Z));
		return;
	}

	if (const int32 Index = SpawnCounter.Find(Location); Index != INDEX_NONE)
	{
		switch (SpawnCounter[Index].TotalSpawns)
		{
		case -1:
			SpawnCounter[Index].TotalSpawns = 1;
			break;
		default:
			SpawnCounter[Index].TotalSpawns++;
			break;
		}

		if (!DidExpire)
		{
			SpawnCounter[Index].TotalHits++;
		}
		const bool bTargetWasHit = !DidExpire;
		if (BSConfig.AIConfig.bEnableReinforcementLearning)
		{
			ReinforcementLearningComponent->UpdateReinforcementLearningReward(Location, bTargetWasHit);
			ReinforcementLearningComponent->UpdateReinforcementLearningComponent(GetSpawnCounter());
		}
	}
	else
	{
		UE_LOG(LogTargetSpawner, Warning, TEXT("NOTFOUND! SpawnCounter %s"), *Location.ToString());
	}
}

void ATargetSpawner::OnBeatTrackTargetHealthChanged(AActor* ActorInstigator, const float OldValue, const float NewValue, const float TotalPossibleDamage)
{
	OnBeatTrackTargetDamaged.Broadcast(OldValue, NewValue, TotalPossibleDamage);
}

void ATargetSpawner::OnBeatTrackOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	/* Reverse direction if not longer overlapping spawn box */
	if (Cast<ASphereTarget>(OtherActor))
	{
		CurrentMovingTargetDirection = -CurrentMovingTargetDirection;
	}
}

void ATargetSpawner::FindNextTargetProperties()
{
	TargetScale = GetNextTargetScale();
	SpawnLocation = GetNextTargetSpawnLocation(BSConfig.SpatialConfig.BoundsScalingMethod, TargetScale);
}

float ATargetSpawner::GetNextTargetScale() const
{
	if (BSConfig.TargetConfig.ConsecutiveTargetScaleMethod == EConsecutiveTargetScaleMethod::SkillBased)
	{
		const float NewFactor = DynamicSpawnCurve->GetFloatValue(DynamicSpawnScale);
		return UKismetMathLibrary::Lerp(BSConfig.TargetConfig.MinTargetScale, BSConfig.TargetConfig.MaxTargetScale, NewFactor);
	}
	return FMath::FRandRange(BSConfig.TargetConfig.MinTargetScale, BSConfig.TargetConfig.MaxTargetScale);
}

FVector ATargetSpawner::GetNextTargetSpawnLocation(const EBoundsScalingMethod BoundsScalingMethod, const float NewTargetScale)
{
	if (BSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::SingleBeat && !LastTargetSpawnedCenter)
	{
		bSkipNextSpawn = false;
		return GetBoxOrigin();
	}
	/* Change the BoxExtent of the SpawnBox if dynamic */
	if (IsDynamicSpreadType(BoundsScalingMethod))
	{
		SetBoxExtents_Dynamic();
	}

	/* Get all points that haven't been occupied by a target recently */
	TArray<FVector> OpenLocations = GetValidSpawnLocations(NewTargetScale, BSConfig.SpatialConfig.TargetDistributionMethod, BoundsScalingMethod);
	if (OpenLocations.IsEmpty())
	{
		UE_LOG(LogTargetSpawner, Warning, TEXT("OpenLocations is empty."));
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
	if (OpenLocations.Contains(GetBoxOrigin()))
	{
		return GetBoxOrigin();
	}

	if (BSConfig.AIConfig.bEnableReinforcementLearning)
	{
		const int32 ChosenPoint = TryGetSpawnLocationFromReinforcementLearningComponent(OpenLocations);
		if (ChosenPoint != INDEX_NONE)
		{
			FVectorCounter Found = GetSpawnCounter()[ChosenPoint];
			Found.ActualChosenPoint = Found.GetRandomSubPoint(GetBorderingDirections(OpenLocations, Found.Point));
			return Found.ActualChosenPoint;
		}
		UE_LOG(LogTargetSpawner, Warning, TEXT("Unable to Spawn Location suggested by RLAgent."));
	}

	const int32 RandomPoint = UKismetMathLibrary::RandomIntegerInRange(0, OpenLocations.Num() - 1);
	FVectorCounter Found = GetVectorCounterFromPoint(OpenLocations[RandomPoint]);
	Found.ActualChosenPoint = Found.GetRandomSubPoint(GetBorderingDirections(OpenLocations, Found.Point));
	//UE_LOG(LogTargetSpawner, Display, TEXT("Found Point %s Found Center %s Found ActualChosenPoint %s"), *Found.Point.ToString(), *Found.Center.ToString(), *Found.ActualChosenPoint.ToString());
	return Found.ActualChosenPoint;
}

FVector ATargetSpawner::GetRandomBeatTrackLocation(const FVector& LocationBeforeChange) const
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

TArray<FVector> ATargetSpawner::GetValidSpawnLocations(const float Scale, const ETargetDistributionMethod& TargetDistributionMethod, const EBoundsScalingMethod& BoundsScalingMethod) const
{
	TArray<FVector> ValidSpawnLocations;

	/* Populate AllPoints according to TargetDistributionMethod */
	switch (TargetDistributionMethod)
	{
	case ETargetDistributionMethod::EdgeOnly:
		{
			const FVector MinExtrema = GetBoxExtrema(0, BoundsScalingMethod == EBoundsScalingMethod::Dynamic);
			const FVector MaxExtrema = GetBoxExtrema(1, BoundsScalingMethod == EBoundsScalingMethod::Dynamic);
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
	case ETargetDistributionMethod::FullRange:
		{
			const FVector NegativeExtrema = GetBoxExtrema(0, BoundsScalingMethod == EBoundsScalingMethod::Dynamic);
			const FVector PositiveExtrema = GetBoxExtrema(1, BoundsScalingMethod == EBoundsScalingMethod::Dynamic);
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

TArray<FVector> ATargetSpawner::GetAllSpawnLocations() const
{
	return AllSpawnLocations;
}

TArray<FVector> ATargetSpawner::GetOverlappingPoints(const FVector& Center, const float Scale) const
{
	float Radius = Scale * SphereTargetRadius * 2.f + BSConfig.SpatialConfig.MinDistanceBetweenTargets / 2.f;
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
	//UE_LOG(LogTargetSpawner, Display, TEXT("GetOverlappingPoints Count %d"), Count);
	//DrawDebugSphere(GetWorld(), Center, Scale * SphereTargetRadius * 2 + (BSConfig.MinDistanceBetweenTargets / 2.f), 32, FColor::Magenta, false, 0.5f);
	//UE_LOG(LogTargetSpawner, Display, TEXT("BlockedPoints: %d"), BlockedPoints.Num());
	return BlockedPoints;
}

TArray<FVector> ATargetSpawner::GetAllOverlappingPoints(const float Scale) const
{
	TArray<FVector> OverlappingPoints;

	/* Resizing Overlapping Points if necessary */
	for (FRecentTarget Struct : GetRecentTargets())
	{
		TArray<FVector> CurrentBlockedLocations;
		if (Scale > Struct.TargetScale)
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

FVector ATargetSpawner::GetBoxOrigin() const
{
	return SpawnBox->Bounds.Origin;
}

FVector ATargetSpawner::GetBoxExtrema(const int32 PositiveExtrema, const bool bDynamic) const
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

FVector ATargetSpawner::GetBoxExtents_Static() const
{
	return StaticExtents;
}

TArray<EBorderingDirection> ATargetSpawner::GetBorderingDirections(const TArray<FVector>& ValidLocations, const FVector& Location) const
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

FVectorCounter ATargetSpawner::GetVectorCounterFromPoint(const FVector& Point) const
{
	const int32 Index = GetSpawnCounter().Find(Point);
	if (Index == INDEX_NONE)
	{
		UE_LOG(LogTargetSpawner, Warning, TEXT("Point Not Found in SpawnCounter"));
		return FVectorCounter();
	}
	return GetSpawnCounter()[Index];
}

void ATargetSpawner::MoveTargetForward(ASphereTarget* SpawnTarget, float DeltaTime) const
{
	const FVector Loc = SpawnTarget->GetActorLocation();
	const FVector NewLoc = FVector(Loc.X - BSConfig.SpatialConfig.MoveForwardDistance, Loc.Y, Loc.Z);
	SpawnTarget->SetActorLocation(UKismetMathLibrary::VInterpTo(Loc, NewLoc, DeltaTime, 1 / BSConfig.TargetConfig.TargetMaxLifeSpan));
}

int32 ATargetSpawner::AddToRecentTargets(const ASphereTarget* SpawnTarget, const float Scale)
{
	TArray<FRecentTarget> Targets = GetRecentTargets();
	const int32 NewIndex = Targets.Emplace(FRecentTarget(SpawnTarget->Guid, GetOverlappingPoints(SpawnTarget->GetActorLocation(), Scale), Scale, SpawnTarget->GetActorLocation()));
	RecentTargets = Targets;
	return NewIndex;
}

int32 ATargetSpawner::AddToActiveTargets(ASphereTarget* SpawnTarget)
{
	TArray<ASphereTarget*> Targets = GetActiveTargets();
	const int32 NewIndex = Targets.Emplace(SpawnTarget);
	ActiveTargets = Targets;
	return NewIndex;
}

void ATargetSpawner::RemoveFromRecentTargets(const FGuid GuidToRemove)
{
	TArray<FRecentTarget> Targets = GetRecentTargets();
	Targets.Remove(FRecentTarget(GuidToRemove));
	RecentTargets = Targets;
}

void ATargetSpawner::RemoveFromActiveTargets(ASphereTarget* SpawnTarget)
{
	TArray<ASphereTarget*> Targets = GetActiveTargets();
	Targets.Remove(SpawnTarget);
	ActiveTargets = Targets;
}

void ATargetSpawner::RemoveEdgePoints(TArray<FVector>& In) const
{
	const FVector MinExtrema = GetBoxExtrema(0, BSConfig.SpatialConfig.BoundsScalingMethod == EBoundsScalingMethod::Dynamic);
	const FVector MaxExtrema = GetBoxExtrema(1, BSConfig.SpatialConfig.BoundsScalingMethod == EBoundsScalingMethod::Dynamic);
	TArray<FVector> InCopy;
	if (bShowDebug_SpawnBox)
	{
		InCopy = In;
	}

	if (BSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::SingleBeat || BSConfig.SpatialConfig.TargetDistributionMethod == ETargetDistributionMethod::EdgeOnly)
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
	//UE_LOG(LogTargetSpawner, Display, TEXT("Removed Edge Points: %d"), Count);
}

void ATargetSpawner::SetBoxExtents_Dynamic() const
{
	const float NewFactor = DynamicSpawnCurve->GetFloatValue(DynamicSpawnScale);
	const float LerpY = UKismetMathLibrary::Lerp(GetBoxExtents_Static().Y, GetBoxExtents_Static().Y * 0.5f, NewFactor);
	const float LerpZ = UKismetMathLibrary::Lerp(GetBoxExtents_Static().Z, GetBoxExtents_Static().Z * 0.5f, NewFactor);
	const float Y = FMath::GridSnap(LerpY, SpawnMemoryIncY);
	const float Z = FMath::GridSnap(LerpZ, SpawnMemoryIncZ);
	SpawnBox->SetBoxExtent(FVector(0, Y, Z));
}

void ATargetSpawner::UpdatePlayerSettings(const FPlayerSettings_Game& InPlayerSettings)
{
	PlayerSettings = InPlayerSettings;
	if (!ActiveTargets.IsEmpty())
	{
		for (ASphereTarget* Target : GetActiveTargets())
		{
			Target->UpdatePlayerSettings(PlayerSettings);
		}
	}
}

int32 ATargetSpawner::GetOutArrayIndexFromSpawnCounterIndex(const int32 SpawnCounterIndex) const
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

int32 ATargetSpawner::TryGetSpawnLocationFromReinforcementLearningComponent(const TArray<FVector>& OpenLocations) const
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
		UE_LOG(LogTargetSpawner, Warning, TEXT("No targets in OpenLocations or No targets in TargetPairs"));
		return INDEX_NONE;
	}
	return ReinforcementLearningComponent->ChooseNextActionIndex(Indices);
}

void ATargetSpawner::ShowDebug_SpawnBox(const bool bShow)
{
	bShowDebug_SpawnBox = bShow;
	if (bShowDebug_SpawnBox)
	{
		DrawDebugBox(GetWorld(), GetBoxOrigin(), GetBoxExtents_Static(), FColor::Orange, true, -1, 3, 5);
	}
	else
	{
		FlushPersistentDebugLines(GetWorld());
	}
}

void ATargetSpawner::ShowDebug_SpawnMemory(const bool bShow)
{
	bShowDebug_SpawnMemory = bShow;
}

void ATargetSpawner::ShowDebug_ReinforcementLearningWidget(const bool bShow)
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
