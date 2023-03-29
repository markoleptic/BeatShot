// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "TargetSpawner.h"

#include "BSHealthComponent.h"
#include "SphereTarget.h"
#include "RLBase.h"
#include "Components/BoxComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ATargetSpawner::ATargetSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	SpawnBox = CreateDefaultSubobject<UBoxComponent>("SpawnBox");
	RootComponent = SpawnBox;
	SetShouldSpawn(false);
	LastTargetSpawnedCenter = false;
	bSkipNextSpawn = false;
	ConsecutiveTargetsHit = 0;
}

void ATargetSpawner::BeginPlay()
{
	Super::BeginPlay();
	TargetSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
}

void ATargetSpawner::Destroyed()
{
	if (BeatTrackTarget)
	{
		BeatTrackTarget->Destroy();
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
	if (IsValid(BeatTrackTarget))
	{
		BeatTrackTargetLocation = BeatTrackTarget->GetActorLocation();
		BeatTrackTargetLocation += BeatTrackTargetDirection * BeatTrackTargetSpeed * DeltaTime;
		BeatTrackTarget->SetActorLocation(BeatTrackTargetLocation);
	}

	if (!GameModeActorStruct.bMoveTargetsForward)
	{
		return;
	}

	for (ASphereTarget* Target : GetActiveTargets())
	{
		if (Target != nullptr)
		{
			MoveTargetForward(Target, DeltaTime);
		}
	}
}

void ATargetSpawner::InitializeGameModeActor(const FGameModeActorStruct NewGameModeActor)
{
	/* Initialize local copy of struct passed by GameModeActorBase */
	GameModeActorStruct = NewGameModeActor;

	/* GameMode menu uses the full width, while box bounds are only half width / half height */
	GameModeActorStruct.BoxBounds.X = 0.f;
	GameModeActorStruct.BoxBounds.Y = GameModeActorStruct.BoxBounds.Y / 2.f;
	GameModeActorStruct.BoxBounds.Z = GameModeActorStruct.BoxBounds.Z / 2.f;

	/* Set the center of spawn box based on user selection */
	FVector CenterOfSpawnBox = StartingSpawnBoxLocation;
	if (GameModeActorStruct.HeadshotHeight == true)
	{
		CenterOfSpawnBox.Z = HeadshotHeight;
		GameModeActorStruct.BoxBounds.Z = 0.f;
	}
	else if (GameModeActorStruct.WallCentered == true)
	{
		CenterOfSpawnBox.Z = CenterBackWallHeight;
	}
	else
	{
		CenterOfSpawnBox.Z = GameModeActorStruct.BoxBounds.Z + DistanceFromFloor;
	}

	const int32 Width = GameModeActorStruct.BoxBounds.Y * 2;
	const int32 Height = GameModeActorStruct.BoxBounds.Z * 2;
	TArray PreferredScales = { 100, 95, 90, 85, 80, 75, 70, 65, 60, 55, 50, 45, 40 };
	bool bWidthScaleSelected = false;
	bool bHeightScaleSelected = false;
	for (const int32 Scale : PreferredScales)
	{
		if (!bWidthScaleSelected)
		{
			if (Width / Scale % 5 == 0 || Width / Scale % 3 == 0)
			{
				SpawnMemoryScaleY = 1.f / static_cast<float>(Scale);
				bWidthScaleSelected = true;
			}
		}
		if (!bHeightScaleSelected)
		{
			if (Height / Scale % 5 == 0 || Height / Scale % 3 == 0)
			{
				SpawnMemoryScaleZ = 1.f / static_cast<float>(Scale);
				bHeightScaleSelected = true;
			}
		}
		if (bHeightScaleSelected && bWidthScaleSelected)
		{
			break;
		}
	}
	if (!bWidthScaleSelected || !bHeightScaleSelected)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't Find Height/Width"));
		SpawnMemoryScaleY = 1.f / 50.f;
		SpawnMemoryScaleZ = 1.f / 50.f;
	}

	SpawnMemoryIncY = roundf(1 / SpawnMemoryScaleY);
	SpawnMemoryIncZ = roundf(1 / SpawnMemoryScaleZ);
	
	/* Set new location & box extent */
	SpawnBox->SetRelativeLocation(CenterOfSpawnBox);
	SpawnBox->SetBoxExtent(GameModeActorStruct.BoxBounds);
	StaticMinExtrema = SpawnBox->Bounds.GetBoxExtrema(0);
	StaticMaxExtrema = SpawnBox->Bounds.GetBoxExtrema(1);
	if (IsDynamicSpreadType(GameModeActorStruct.SpreadType))
	{
		SpawnBox->SetBoxExtent(GameModeActorStruct.MinBoxBounds);
	}

	/* Initial target spawn location */
	SpawnLocation = SpawnBox->Bounds.Origin;
	PreviousSpawnLocation = SpawnLocation;

	/* Initial target size */
	TargetScale = GetNextTargetScale();

	if (GameModeActorStruct.IsBeatTrackMode)
	{
		return;
	}

	if (GameModeActorStruct.IsBeatGridMode)
	{
		InitBeatGrid();
		return;
	}
	FIntPoint RowsColumns = InitializeSpawnCounter();

	RLBase = NewObject<URLBase>();
	FRLAgentParams Params;
	Params.GameModeActorName = GameModeActorStruct.GameModeActorName;
	Params.CustomGameModeName = GameModeActorStruct.CustomGameModeName;
	Params.Size = GetSpawnCounter().Num();
	Params.Rows = RowsColumns.X;
	Params.Columns = RowsColumns.Y;
	Params.InEpsilon = 0.5f;
	Params.InGamma = 0.9f;
	Params.InAlpha = 0.9f;
	RLBase->Init(Params);
}

void ATargetSpawner::CallSpawnFunction()
{
	if (ShouldSpawn == false)
	{
		return;
	}
	if (GameModeActorStruct.IsSingleBeatMode)
	{
		SpawnSingleBeatTarget();
	}
	else if (GameModeActorStruct.IsBeatTrackMode)
	{
		SetNewTrackingDirection();
	}
	else if (GameModeActorStruct.IsBeatGridMode)
	{
		ActivateBeatGridTarget();
	}
	else
	{
		SpawnMultiBeatTarget();
	}
}

TArray<F2DArray> ATargetSpawner::GetLocationAccuracy() const
{
	/*FString File = FPaths::ProjectDir();
	File.Append(TEXT("AccuracyMatrix.csv"));
	FString StringToWrite;*/
	TArray<F2DArray> OutArray;
	F2DArray LowerRowHalf = F2DArray();
	F2DArray CurrentRow = F2DArray();
	bool OddRow = true;
	float Total = 0.f;
	for (const FVectorCounter Counter : GetSpawnCounter())
	{
		Total += Counter.TotalHits + Counter.TotalSpawns;
		if (Counter.TotalSpawns == -1)
		{
			CurrentRow.Accuracy.Add(-1);
		}
		else
		{
			CurrentRow.Accuracy.Add(static_cast<float>(Counter.TotalHits) / static_cast<float>(Counter.TotalSpawns));
		}
		// End of "row" or left-to-right
		if (Counter.Point.Y == GetBoxExtents_Static().Y)
		{
			OutArray.Emplace(CurrentRow);
			// AppendStringLocAccRow(CurrentRow, StringToWrite);
			// Don't average the middle row
			if (Counter.Point.Z == GetBoxOrigin().Z)
			{
				OutArray.Emplace(CurrentRow);
				//AppendStringLocAccRow(CurrentRow, StringToWrite);
			}
			// Save the current row as the 1/2 of a row
			else if (OddRow)
			{
				LowerRowHalf = CurrentRow;
				OddRow = false;
			}
			else
			{
				F2DArray AveragedRow = F2DArray();
				for (int i = 0; i < LowerRowHalf.Accuracy.Num(); i++)
				{
					if (LowerRowHalf.Accuracy[i] == -1.f && CurrentRow.Accuracy[i] != -1.f)
					{
						AveragedRow.Accuracy.Add(CurrentRow.Accuracy[i]);
					}
					else if (CurrentRow.Accuracy[i] == -1.f && LowerRowHalf.Accuracy[i] != -1.f)
					{
						AveragedRow.Accuracy.Add(LowerRowHalf.Accuracy[i]);
					}
					else
					{
						AveragedRow.Accuracy.Add((LowerRowHalf.Accuracy[i] + CurrentRow.Accuracy[i]) / 2);
					}
				}
				OutArray.Emplace(AveragedRow);
				//AppendStringLocAccRow(CurrentRow, StringToWrite);
				OddRow = true;
			}
			CurrentRow = F2DArray();
		}
	}
	//FFileHelper::SaveStringToFile(StringToWrite, *File);
	return OutArray;
}

FIntPoint ATargetSpawner::InitializeSpawnCounter()
{
	const FVector NegativeExtents = GetBoxExtrema(0, false);
	const FVector PositiveExtents = GetBoxExtrema(1, false);

	int ColumnCount = 0;
	int RowCount = 0;
	int Index = 0;
	for (float Z = NegativeExtents.Z; Z < PositiveExtents.Z; Z += SpawnMemoryIncZ)
	{
		RowCount = 0;
		for (float Y = NegativeExtents.Y; Y < PositiveExtents.Y; Y += SpawnMemoryIncY)
		{
			SpawnCounter.Emplace(FVectorCounter(Index, FVector(GetBoxOrigin().X, Y, Z), SpawnMemoryIncY, SpawnMemoryIncZ));
			AllSpawnLocations.Emplace(FVector(GetBoxOrigin().X, Y, Z));
			Index++;
			RowCount++;
		}
		ColumnCount++;
	}
	UE_LOG(LogTemp, Display, TEXT("SpawnCounterSize: %d %llu"), SpawnCounter.Num(), SpawnCounter.GetAllocatedSize());
	return FIntPoint(RowCount, ColumnCount);
}

void ATargetSpawner::InitBeatGrid()
{
	// clear any variables that could have been used prior to a restart
	if (!RecentBeatGridIndices.IsEmpty())
	{
		RecentBeatGridIndices.Empty();
		RecentBeatGridIndices.Shrink();
	}
	if (!SpawnedBeatGridTargets.IsEmpty())
	{
		SpawnedBeatGridTargets.Empty();
		SpawnedBeatGridTargets.Shrink();
	}
	if (ActiveBeatGridTarget)
	{
		ActiveBeatGridTarget = nullptr;
	}

	LastBeatGridIndex = -1;
	InitialBeatGridTargetActivated = false;

	const float HalfWidth = round(SpawnBox->Bounds.BoxExtent.Y);
	const float HalfHeight = round(SpawnBox->Bounds.BoxExtent.Z);
	const float NumTargets = sqrt(GameModeActorStruct.BeatGridSize);
	FVector BeatGridSpawnLocation = SpawnBox->Bounds.Origin;
	constexpr float OuterSpacing = 100.f;
	const float BasicHSpacing = (HalfWidth - OuterSpacing) * 2 / (NumTargets - 1);
	const float HStart = -HalfWidth + OuterSpacing;
	const float BasicVSpacing = (HalfHeight - OuterSpacing) * 2 / (NumTargets - 1);
	const float VStart = HalfHeight - OuterSpacing;

	// Distributing only based on Spawn Height/Width and number of targets for now
	for (int32 i = 0; i < NumTargets; i++)
	{
		for (int32 j = 0; j < NumTargets; j++)
		{
			// initial top left spot
			if (i == 0 && j == 0)
			{
				BeatGridSpawnLocation.Z += VStart;
				BeatGridSpawnLocation.Y += HStart;
			}
			else if (i != 0 && j == 0)
			{
				BeatGridSpawnLocation.Y = HStart;
			}
			else if (i != 0 || j != 0)
			{
				BeatGridSpawnLocation.Y += BasicHSpacing;
			}

			if (ASphereTarget* BeatGridTarget = GetWorld()->SpawnActor<ASphereTarget>(ActorToSpawn, BeatGridSpawnLocation, FRotator::ZeroRotator, TargetSpawnParams))
			{
				BeatGridTarget->OnLifeSpanExpired.AddDynamic(this, &ATargetSpawner::OnTargetTimeout);
				const float NewTargetScale = GetNextTargetScale();
				BeatGridTarget->SetSphereScale(FVector(NewTargetScale, NewTargetScale, NewTargetScale));
				SpawnedBeatGridTargets.Add(BeatGridTarget);
			}
		}
		BeatGridSpawnLocation.Y = HStart;
		BeatGridSpawnLocation.Z -= BasicVSpacing;
	}
}

void ATargetSpawner::SpawnMultiBeatTarget()
{
	if (!bSkipNextSpawn)
	{
		if (ASphereTarget* SpawnTarget = GetWorld()->SpawnActor<ASphereTarget>(ActorToSpawn, SpawnLocation, FRotator::ZeroRotator, TargetSpawnParams))
		{
			/* Setting the current target's scale that was previously calculated */
			SpawnTarget->SetSphereScale(FVector(TargetScale));
			SpawnTarget->OnLifeSpanExpired.AddDynamic(this, &ATargetSpawner::OnTargetTimeout);
			AddToActiveTargets(SpawnTarget);
			AddToRecentTargets(SpawnTarget, TargetScale);
			OnTargetSpawned.Broadcast();
			OnTargetSpawned_AimBot.Broadcast(SpawnTarget);

			if (!PreviousSpawnLocation.Equals(SpawnLocation))
			{
				AddToActiveTargetPairs(PreviousSpawnLocation, SpawnLocation);
			}
			PreviousSpawnLocation = SpawnLocation;
		}
		else
		{
			DrawDebugSphere(GetWorld(), SpawnLocation, TargetScale * SphereTargetRadius, 20, FColor::Red, false, 0.5);
		}
	}
	FindNextTargetProperties();
}

void ATargetSpawner::SpawnSingleBeatTarget()
{
	if (!bSkipNextSpawn)
	{
		if (ASphereTarget* SpawnTarget = GetWorld()->SpawnActor<ASphereTarget>(ActorToSpawn, SpawnLocation, FRotator::ZeroRotator, TargetSpawnParams))
		{
			/* Setting the current target's scale that was previously calculated */
			SpawnTarget->SetSphereScale(FVector(TargetScale));
			SpawnTarget->OnLifeSpanExpired.AddDynamic(this, &ATargetSpawner::OnTargetTimeout);
			AddToActiveTargets(SpawnTarget);
			AddToRecentTargets(SpawnTarget, TargetScale);
			OnTargetSpawned.Broadcast();
			OnTargetSpawned_AimBot.Broadcast(SpawnTarget);

			if (!PreviousSpawnLocation.Equals(SpawnLocation))
			{
				AddToActiveTargetPairs(PreviousSpawnLocation, SpawnLocation);
			}
			PreviousSpawnLocation = SpawnLocation;
		}
		if (SpawnLocation.Equals(SpawnBox->Bounds.Origin))
		{
			LastTargetSpawnedCenter = true;
		}
		else
		{
			LastTargetSpawnedCenter = false;
		}
	}
	/* Don't continue spawning for SingleBeat */
	SetShouldSpawn(false);
	FindNextTargetProperties();
}

void ATargetSpawner::ActivateBeatGridTarget()
{
	if (InitialBeatGridTargetActivated == false)
	{
		if (const int32 InitialArraySize = SpawnedBeatGridTargets.Num(); InitialArraySize > 0)
		{
			const int32 RandomIndex = FMath::RandRange(0, InitialArraySize - 1);
			ActiveBeatGridTarget = SpawnedBeatGridTargets[RandomIndex];
			LastBeatGridIndex = RandomIndex;
			InitialBeatGridTargetActivated = true;
		}
	}
	else if (GameModeActorStruct.RandomizeBeatGrid == true)
	{
		const int32 ArraySize = SpawnedBeatGridTargets.Num();
		const int32 RandomIndex = FMath::RandRange(0, ArraySize - 1);
		ActiveBeatGridTarget = SpawnedBeatGridTargets[RandomIndex];
	}
	else if (GameModeActorStruct.RandomizeBeatGrid == false)
	{
		TArray<int32> SpawnCandidates;
		const int32 MaxIndex = SpawnedBeatGridTargets.Num() - 1;
		const int32 Width = sqrt(SpawnedBeatGridTargets.Num());
		// const int32 Height = Width;
		const int32 AdjFor = Width + 1;
		const int32 AdjBack = Width - 1;
		// corners
		if (const int32 i = LastBeatGridIndex; i == 0)
		{
			SpawnCandidates = {1, Width, AdjFor};
		}
		else if (i == AdjBack)
		{
			SpawnCandidates = {i - 1, i + AdjBack, i + Width};
		}
		else if (i == (Width * AdjBack))
		{
			SpawnCandidates = {i - Width, i - AdjBack, i + 1};
		}
		else if (i == MaxIndex)
		{
			SpawnCandidates = {i - AdjFor, i - Width, i - 1};
		}
		// top
		else if (i > 0 && i < AdjBack)
		{
			SpawnCandidates = {i - 1, i + 1, i + AdjBack, i + Width, i + AdjFor};
		}
		//left
		else if (i % Width == 0 && i < Width * AdjBack)
		{
			SpawnCandidates = {i - Width, i - AdjBack, i + 1, i + AdjFor, i + Width};
		}
		//bottom
		else if (i > Width * AdjBack && i < MaxIndex)
		{
			SpawnCandidates = {i - AdjFor, i - Width, i - AdjBack, i - 1, i + 1};
		}
		//right
		else if ((i + 1) % Width == 0 && i < MaxIndex)
		{
			SpawnCandidates = {i - AdjFor, i - Width, i - 1, i + AdjBack, i + Width};
		}
		//middle
		else
		{
			SpawnCandidates = {i - AdjFor, i - Width, i - AdjBack, i - 1, i + 1, i + AdjBack, i + Width, i + AdjFor};
		}

		// remove recently spawned targets
		if (RecentBeatGridIndices.IsEmpty() == false)
		{
			for (int32 j = 0; j < RecentBeatGridIndices.Num(); j++)
			{
				if (SpawnCandidates.Contains(RecentBeatGridIndices[j]))
				{
					SpawnCandidates.Remove(RecentBeatGridIndices[j]);
				}
			}
		}

		// shrink after removing recently spawned target indices
		SpawnCandidates.Shrink();

		// choose a random adjacent target index
		const int32 CandidateArraySize = SpawnCandidates.Num();
		const int32 RandomBorderedIndex = FMath::RandRange(0, CandidateArraySize - 1);
		const int32 RandomIndex = SpawnCandidates[RandomBorderedIndex];
		ActiveBeatGridTarget = SpawnedBeatGridTargets[RandomIndex];

		// update recently spawned target indices
		LastBeatGridIndex = RandomIndex;
		RecentBeatGridIndices.Insert(LastBeatGridIndex, 0);
		RecentBeatGridIndices.SetNum(2);
	}
	/* only "spawn" target if it hasn't been destroyed */
	if (ActiveBeatGridTarget)
	{
		ActiveBeatGridTarget->StartBeatGridTimer(GameModeActorStruct.TargetMaxLifeSpan);
		OnTargetSpawned.Broadcast();
		OnTargetSpawned_AimBot.Broadcast(ActiveBeatGridTarget);
	}
	if (GameModeActorStruct.IsSingleBeatMode == true)
	{
		SetShouldSpawn(false);
	}
}

void ATargetSpawner::SetNewTrackingDirection()
{
	if (!BeatTrackTarget)
	{
		/** Initial tracking target spawn */
		BeatTrackTarget = GetWorld()->SpawnActor<ASphereTarget>(ActorToSpawn, SpawnBox->Bounds.Origin, FRotator::ZeroRotator, TargetSpawnParams);
		BeatTrackTarget->OnActorEndOverlap.AddDynamic(this, &ATargetSpawner::OnBeatTrackOverlapEnd);
		BeatTrackTarget->HealthComponent->OnHealthChanged.AddUObject(this, &ATargetSpawner::OnBeatTrackTargetHealthChanged);
		LocationBeforeDirectionChange = SpawnBox->Bounds.Origin;
		OnTargetSpawned.Broadcast();
	}
	if (BeatTrackTarget)
	{
		if (!OnBeatTrackDirectionChanged.ExecuteIfBound(EndLocation))
		{
			UE_LOG(LogTemp, Display, TEXT("OnBeatTrackDirectionChanged not bound."));
		}
		LocationBeforeDirectionChange = BeatTrackTarget->GetActorLocation();
		const float NewTargetScale = GetNextTargetScale();
		BeatTrackTarget->SetSphereScale(FVector(NewTargetScale, NewTargetScale, NewTargetScale));
		BeatTrackTargetSpeed = FMath::FRandRange(GameModeActorStruct.MinTrackingSpeed, GameModeActorStruct.MaxTrackingSpeed);
		EndLocation = GetRandomBeatTrackLocation(LocationBeforeDirectionChange);
		BeatTrackTargetDirection = UKismetMathLibrary::GetDirectionUnitVector(LocationBeforeDirectionChange, EndLocation);
	}
}

void ATargetSpawner::OnTargetTimeout(const bool DidExpire, const float TimeAlive, ASphereTarget* DestroyedTarget)
{
	if (GameModeActorStruct.IsSingleBeatMode) SetShouldSpawn(true);
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

	if (const int32 Index = SpawnCounter.Find(Location); Index != INDEX_NONE)
	{
		switch (SpawnCounter[Index].TotalSpawns)
		{
		case -1: SpawnCounter[Index].TotalSpawns = 1;
			break;
		default: SpawnCounter[Index].TotalSpawns++;
			break;
		}

		if (!DidExpire)
		{
			SpawnCounter[Index].TotalHits++;
		}
		const bool bTargetWasHit = !DidExpire;
		UpdateRLAgentReward(Location, bTargetWasHit);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NOTFOUND! SpawnCounter %s"), *Location.ToString());
	}

	RemoveFromActiveTargets(DestroyedTarget);
	FTimerHandle TimerHandle;
	RemoveFromRecentDelegate.BindUObject(this, &ATargetSpawner::RemoveFromRecentTargets, DestroyedTarget->Guid);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, RemoveFromRecentDelegate, GameModeActorStruct.TargetSpawnCD, false);

	if (bShowDebug_SpawnBox)
	{
		DrawDebugBox(GetWorld(), SpawnBox->Bounds.Origin, SpawnBox->Bounds.BoxExtent, FColor::Orange, false, 1);
	}
}

void ATargetSpawner::OnBeatTrackTargetHealthChanged(AActor* ActorInstigator, const float OldValue, const float NewValue, const float MaxHealth)
{
	OnBeatTrackTargetDamaged.Broadcast(OldValue - NewValue, MaxHealth);
}

void ATargetSpawner::OnBeatTrackOverlapEnd(AActor* OverlappedActor, AActor* OtherActor)
{
	/* Reverse direction if not longer overlapping spawn box */
	if (Cast<ATargetSpawner>(OverlappedActor) || Cast<ASphereTarget>(OverlappedActor) && Cast<ASphereTarget>(OtherActor) || Cast<ATargetSpawner>(OtherActor))
	{
		BeatTrackTargetDirection = BeatTrackTargetDirection * -1;
	}
}

void ATargetSpawner::FindNextTargetProperties()
{
	TargetScale = GetNextTargetScale();
	SpawnLocation = GetNextTargetSpawnLocation(GameModeActorStruct.SpreadType, TargetScale);
}

float ATargetSpawner::GetNextTargetScale() const
{
	if (GameModeActorStruct.UseDynamicSizing)
	{
		const float NewFactor = DynamicSpawnCurve->GetFloatValue(DynamicSpawnScale);
		return UKismetMathLibrary::Lerp(GameModeActorStruct.MinTargetScale, GameModeActorStruct.MaxTargetScale, NewFactor);
	}
	return FMath::FRandRange(GameModeActorStruct.MinTargetScale, GameModeActorStruct.MaxTargetScale);
}

FVector ATargetSpawner::GetNextTargetSpawnLocation(const ESpreadType SpreadType, const float NewTargetScale)
{
	if (GameModeActorStruct.IsSingleBeatMode && !LastTargetSpawnedCenter)
	{
		bSkipNextSpawn = false;
		return GetBoxOrigin();
	}
	/* Change the BoxExtent of the SpawnBox if dynamic */
	if (IsDynamicSpreadType(SpreadType))
	{
		SetBoxExtents_Dynamic();
	}

	/* Get all points that haven't been occupied by a target recently */
	TArray<FVector> OpenLocations = GetValidSpawnLocations(NewTargetScale);
	if (OpenLocations.IsEmpty())
	{
		UE_LOG(LogTemp, Display, TEXT("OpenLocations is empty."));
		bSkipNextSpawn = true;
		return GetBoxOrigin();
	}
	bSkipNextSpawn = false;

	if (bShowDebug_SpawnBox)
	{
		for (const FVector Vector : OpenLocations)
		{
			FVector Loc = FVector(Vector.X, Vector.Y + SpawnMemoryIncY / 2.f, Vector.Z + SpawnMemoryIncZ / 2.f);
			DrawDebugBox(GetWorld(), Loc, FVector(0, SpawnMemoryIncY / 2.f, SpawnMemoryIncZ / 2.f), FColor::Emerald, false, GameModeActorStruct.TargetSpawnCD);
		}
	}

	/* Spawn at origin whenever possible */
	if (OpenLocations.Contains(GetBoxOrigin()))
	{
		return GetBoxOrigin();
	}
	
	const int32 ChosenPoint = TryGetSpawnLocationFromRLAgent(OpenLocations);
	if (ChosenPoint != INDEX_NONE)
	{
		FVectorCounter Found = GetSpawnCounter()[ChosenPoint];
		Found.ActualChosenPoint = Found.GetRandomSubPoint(GetBorderingDirections(OpenLocations, Found.Point));
		return Found.ActualChosenPoint;
	}
	UE_LOG(LogTemp, Display, TEXT("Unable to Spawn Location suggested by RLAgent."));
	
	const int32 RandomPoint = UKismetMathLibrary::RandomIntegerInRange(0, OpenLocations.Num() - 1);
	FVectorCounter Found = GetVectorCounterFromPoint(OpenLocations[RandomPoint]);
	Found.ActualChosenPoint = Found.GetRandomSubPoint(GetBorderingDirections(OpenLocations, Found.Point));
	//UE_LOG(LogTemp, Display, TEXT("Found Point %s Found Center %s Found ActualChosenPoint %s"), *Found.Point.ToString(), *Found.Center.ToString(), *Found.ActualChosenPoint.ToString());
	return Found.ActualChosenPoint;
	//return OpenLocations[RandomPoint];
}

FVector ATargetSpawner::GetRandomBeatTrackLocation(const FVector& LocationBeforeChange) const
{
	const FVector NewExtent = FVector(0, GetBoxExtents_Static().Y * 0.5, GetBoxExtents_Static().Z * 0.5);

	const FVector BotLeft = UKismetMathLibrary::RandomPointInBoundingBox(SpawnBox->Bounds.Origin + FVector(0, -GetBoxExtents_Static().Y * 0.5, -GetBoxExtents_Static().Z * 0.5), NewExtent);

	const FVector BotRight = UKismetMathLibrary::RandomPointInBoundingBox(SpawnBox->Bounds.Origin + FVector(0, GetBoxExtents_Static().Y * 0.5, -GetBoxExtents_Static().Z * 0.5), NewExtent);

	const FVector TopLeft = UKismetMathLibrary::RandomPointInBoundingBox(SpawnBox->Bounds.Origin + FVector(0, -GetBoxExtents_Static().Y * 0.5, GetBoxExtents_Static().Z * 0.5), NewExtent);

	const FVector TopRight = UKismetMathLibrary::RandomPointInBoundingBox(SpawnBox->Bounds.Origin + FVector(0, GetBoxExtents_Static().Y * 0.5, GetBoxExtents_Static().Z * 0.5), NewExtent);

	TArray<FVector> PossibleLocations;
	PossibleLocations.Add(BotLeft);
	PossibleLocations.Add(BotRight);
	PossibleLocations.Add(TopLeft);
	PossibleLocations.Add(TopRight);

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

	PossibleLocations.Shrink();
	const FVector NewLocation = PossibleLocations[UKismetMathLibrary::RandomIntegerInRange(0, 2)];
	return NewLocation + UKismetMathLibrary::GetDirectionUnitVector(LocationBeforeChange, NewLocation) * BeatTrackTargetSpeed * GameModeActorStruct.TargetSpawnCD;
}

TArray<FVector> ATargetSpawner::GetValidSpawnLocations(const float Scale) const
{
	TArray<FVector> AllPoints;

	/* Populate AllPoints according to SpreadType */
	switch (GameModeActorStruct.SpreadType)
	{
	case ESpreadType::DynamicEdgeOnly: AllPoints = GetAllEdgeOnlySpawnLocations();
		break;
	case ESpreadType::DynamicRandom: AllPoints = GetAllDynamicRandomSpawnLocations();
		break;
	default: AllPoints = GetAllSpawnLocations();
		break;
	}

	/* Remove points occupied by recent targets (overlapping points) */
	for (FVector Location : GetAllOverlappingPoints(Scale))
	{
		AllPoints.Remove(Location);
	}

	/* Remove points that don't have another point to the top and to the right */
	RemoveEdgePoints(AllPoints);

	return AllPoints;
}

TArray<FVector> ATargetSpawner::GetAllSpawnLocations() const
{
	return AllSpawnLocations;
}

TArray<FVector> ATargetSpawner::GetAllDynamicRandomSpawnLocations() const
{
	const FVector NegativeExtrema = GetBoxExtrema(0, true);
	const FVector PositiveExtrema = GetBoxExtrema(1, true);

	TArray<FVector> AllPoints = GetAllSpawnLocations();
	TArray<FVector> Remove;

	for (FVector Vector : AllPoints)
	{
		if (Vector.Y < NegativeExtrema.Y || Vector.Y > PositiveExtrema.Y || Vector.Z < NegativeExtrema.Z || Vector.Z > PositiveExtrema.Z)
		{
			Remove.Add(Vector);
		}
	}

	for (FVector Vector : Remove)
	{
		AllPoints.Remove(Vector);
	}

	return AllPoints;
}

TArray<FVector> ATargetSpawner::GetAllEdgeOnlySpawnLocations() const
{
	const FVector MinExtrema = GetBoxExtrema(0, true);
	const FVector MaxExtrema = GetBoxExtrema(1, true);

	TArray<FVector> Edges;
	for (float Y = MinExtrema.Y; Y <= MaxExtrema.Y; Y += SpawnMemoryIncY)
	{
		Edges.AddUnique(FVector(GetBoxOrigin().X, Y, MinExtrema.Z));
		Edges.AddUnique(FVector(GetBoxOrigin().X, Y, MaxExtrema.Z));
	}
	for (float Z = MinExtrema.Z; Z <= MaxExtrema.Z; Z += SpawnMemoryIncZ)
	{
		Edges.AddUnique(FVector(GetBoxOrigin().X, MinExtrema.Y, Z));
		Edges.AddUnique(FVector(GetBoxOrigin().X, MaxExtrema.Y, Z));
	}
	Edges.Add(GetBoxOrigin());
	return Edges;
}

TArray<FVector> ATargetSpawner::GetOverlappingPoints(const FVector& Center, const float Scale) const
{
	const float Radius = Scale * SphereTargetRadius * 2.f + GameModeActorStruct.MinDistanceBetweenTargets / 2.f;
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
	//UE_LOG(LogTemp, Display, TEXT("GetOverlappingPoints Count %d"), Count);
	//DrawDebugSphere(GetWorld(), Center, Scale * SphereTargetRadius * 2 + (GameModeActorStruct.MinDistanceBetweenTargets / 2.f), 32, FColor::Magenta, false, 0.5f);
	//UE_LOG(LogTemp, Display, TEXT("BlockedPoints: %d"), BlockedPoints.Num());
	return BlockedPoints;
}

TArray<FVector> ATargetSpawner::GetAllOverlappingPoints(const float Scale) const
{
	TArray<FVector> BlockedLocations;
	/* Resizing Overlapping Points if necessary */
	for (FRecentTarget Struct : GetRecentTargets())
	{
		TArray<FVector> CurrentBlockedLocations;
		if (Struct.TargetScale < Scale)
		{
			CurrentBlockedLocations = GetOverlappingPoints(Struct.CenterVector, Scale);
		}
		else
		{
			CurrentBlockedLocations = Struct.OverlappingPoints;
		}
		BlockedLocations.Append(CurrentBlockedLocations);
		if (bShowDebug_SpawnMemory)
		{
			for (FVector Vector : Struct.OverlappingPoints)
			{
				DrawDebugPoint(GetWorld(), Vector, 10, FColor::Red, false, 0.35f);
			}
		}
	}
	return BlockedLocations;
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
	return FVector(0, GameModeActorStruct.BoxBounds.Y, GameModeActorStruct.BoxBounds.Z);
}

TArray<EDirection> ATargetSpawner::GetBorderingDirections(const TArray<FVector>& ValidLocations, const FVector& Location) const
{
	TArray<EDirection> Directions;
	const FVector MinExtrema = GetBoxExtrema(0, true);
	const FVector MaxExtrema = GetBoxExtrema(1, true);
	const FVector Left = Location + FVector(0, -SpawnMemoryIncY, 0);
	const FVector Right = Location + FVector(0, SpawnMemoryIncY, 0);
	const FVector Up = Location + FVector(0, 0, SpawnMemoryIncZ);
	const FVector Down = Location + FVector(0, 0, -SpawnMemoryIncZ);

	if (Left.Y != MinExtrema.Y && !ValidLocations.Contains(Left))
	{
		Directions.Add(EDirection::Left);
	}
	if (Right.Y != MaxExtrema.Y && !ValidLocations.Contains(Right))
	{
		Directions.Add(EDirection::Right);
	}
	if (Up.Z != MaxExtrema.Z && !ValidLocations.Contains(Up))
	{
		Directions.Add(EDirection::Up);
	}
	if (Down.Z != MinExtrema.Z && !ValidLocations.Contains(Down))
	{
		Directions.Add(EDirection::Down);
	}
	return Directions;
}

FVectorCounter ATargetSpawner::GetVectorCounterFromPoint(const FVector& Point) const
{
	const int32 Index = GetSpawnCounter().Find(Point);
	if (Index == INDEX_NONE)
	{
		UE_LOG(LogTemp, Display, TEXT("Point Not Found in SpawnCounter"));
		return FVectorCounter();
	}
	return GetSpawnCounter()[Index];
}

void ATargetSpawner::MoveTargetForward(ASphereTarget* SpawnTarget, float DeltaTime) const
{
	const FVector Loc = SpawnTarget->GetActorLocation();
	const FVector NewLoc = FVector(Loc.X - GameModeActorStruct.MoveForwardDistance, Loc.Y, Loc.Z);
	SpawnTarget->SetActorLocation(UKismetMathLibrary::VInterpTo(Loc, NewLoc, DeltaTime, 1 / GameModeActorStruct.TargetMaxLifeSpan));
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
	const FVector MinExtrema = GetBoxExtrema(0, true);
	const FVector MaxExtrema = GetBoxExtrema(1, true);
	TArray<FVector> Remove;
	// ReSharper disable once CppTooWideScope
	const uint8 SingleBeatOrEdgeOnly = GameModeActorStruct.IsSingleBeatMode || GameModeActorStruct.SpreadType == ESpreadType::DynamicEdgeOnly;
	switch (SingleBeatOrEdgeOnly)
	{
	case 1: for (FVector Vector : In)
		{
			const FVector Right = Vector + FVector(0, SpawnMemoryIncY, 0);
			const FVector Top = Vector + FVector(0, 0, SpawnMemoryIncZ);
			// Only consider valid spawn locations
			if (Vector.Y != MinExtrema.Y && Right.Y < MaxExtrema.Y && !In.Contains(Right))
			{
				Remove.AddUnique(Vector);
			}
			if (Vector.Z != MinExtrema.Z && Top.Z < MaxExtrema.Z && !In.Contains(Top))
			{
				Remove.AddUnique(Vector);
			}
		}
		break;
	default: for (FVector Vector : In)
		{
			const FVector Right = Vector + FVector(0, SpawnMemoryIncY, 0);
			const FVector Top = Vector + FVector(0, 0, SpawnMemoryIncZ);
			// Only consider valid spawn locations
			if (Right.Y < MaxExtrema.Y && !In.Contains(Right))
			{
				Remove.AddUnique(Vector);
			}
			if (Top.Z < MaxExtrema.Z && !In.Contains(Top))
			{
				Remove.AddUnique(Vector);
			}
		}
	}
	int Count = 0;
	for (FVector Vector : Remove)
	{
		In.Remove(Vector);
		Count++;
		if (bShowDebug_SpawnBox)
		{
			FVector Loc = FVector(Vector.X, Vector.Y + SpawnMemoryIncY / 2.f, Vector.Z + SpawnMemoryIncZ / 2.f);
			DrawDebugBox(GetWorld(), Loc, FVector(0, SpawnMemoryIncY / 2.f, SpawnMemoryIncZ / 2.f), FColor::Red, false, GameModeActorStruct.TargetSpawnCD);
		}
	}
	//UE_LOG(LogTemp, Display, TEXT("Removed Edge Points: %d"), Count);
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

void ATargetSpawner::AppendStringLocAccRow(const F2DArray Row, FString& StringToWriteTo)
{
	for (const float AccValue : Row.Accuracy)
	{
		StringToWriteTo.Append(FString::SanitizeFloat(AccValue) + ",");
	}
	StringToWriteTo.Append("\n");
}

void ATargetSpawner::UpdateRLAgentReward(const FVector& WorldLocation, const bool bHit)
{
	const int32 ActiveTargetPairIndex = ActiveTargetPairs.Find(WorldLocation);
	if (ActiveTargetPairIndex == INDEX_NONE)
	{
		UE_LOG(LogTemp, Display, TEXT("Location not found in ActiveTargetPairs %s"), *WorldLocation.ToString());
		return;
	}
	FTargetPair FoundTargetPair = ActiveTargetPairs[ActiveTargetPairIndex];

	/* Update reward */
	if (bHit)
	{
		FoundTargetPair.Reward = -1.f;
	}
	else
	{
		FoundTargetPair.Reward = 1.f;
	}
	ActiveTargetPairs.Remove(FoundTargetPair);
	TargetPairs.Enqueue(FoundTargetPair);
}

void ATargetSpawner::AddToActiveTargetPairs(const FVector& PreviousWorldLocation, const FVector& NextWorldLocation)
{
	ActiveTargetPairs.Emplace(PreviousWorldLocation, NextWorldLocation);
}

int32 ATargetSpawner::TryGetSpawnLocationFromRLAgent(const TArray<FVector>& OpenLocations)
{
	/* Converting all OpenLocations to indices */
	TArray<int32> Indices;
	const TArray<FVectorCounter> SpawnCounterCopy = GetSpawnCounter();
	for (const FVector Vector : OpenLocations)
	{
		const int32 FoundIndex = SpawnCounterCopy.Find(Vector);
		if (FoundIndex != INDEX_NONE)
		{
			Indices.Add(FoundIndex);
		}
	}
	//UE_LOG(LogTemp, Display, TEXT("Indices Size inside TryGetSpawnLocationFromRLAgent %d"), Indices.Num());
	
	/* Updating RL Agent */
	FTargetPair TargetPair;
	if (!TargetPairs.Peek(TargetPair) || Indices.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("No targets in OpenLocations or No targets in TargetPairs"));
		return INDEX_NONE;
	}
	TargetPairs.Pop();
	const int32 StateIndex = SpawnCounter.Find(TargetPair.Previous);
	const int32 State2Index = SpawnCounter.Find(TargetPair.Current);
	const int32 ActionIndex = State2Index;
	const int32 Action2Index = RLBase->GetMaxActionIndex(State2Index);

	/* Don't update RLAgent if something went wrong */
	if (Action2Index < SpawnCounter.Num() && Action2Index != INDEX_NONE)
	{
		RLBase->UpdateEpisodeRewards(TargetPair.Reward);
		RLBase->UpdateQTable(FAlgoInput(StateIndex, State2Index, ActionIndex, Action2Index, TargetPair.Reward));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Spawn Point from RLAgent: TargetPair.Previous %s TargetPair.Current %s"), *TargetPair.Previous.ToString(), *TargetPair.Current.ToString());
	}
	
	return RLBase->ChooseNextActionIndex(Indices);
	//return SpawnCounterCopy[RLBase->ChooseNextActionIndex(Indices)];
	//return SpawnCounter[Action2Index].Point;
}

void ATargetSpawner::ShowDebug_SpawnBox()
{
	bShowDebug_SpawnBox = true;
}

void ATargetSpawner::HideDebug_SpawnBox()
{
	bShowDebug_SpawnBox = false;
}

void ATargetSpawner::ShowDebug_SpawnMemory()
{
	bShowDebug_SpawnMemory = true;
}

void ATargetSpawner::HideDebug_SpawnMemory()
{
	bShowDebug_SpawnMemory = false;
}
