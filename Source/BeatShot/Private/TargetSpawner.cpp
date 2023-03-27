// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "TargetSpawner.h"
#include "SphereTarget.h"
#include "BSGameMode.h"
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
	RLBase->PrintRewards();
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

	SpawnMemoryScaleY = 1.f / 50.f;
	SpawnMemoryScaleZ = 1.f / 50.f;

	SpawnMemoryIncY = roundf(1 / SpawnMemoryScaleY);
	SpawnMemoryIncZ = roundf(1 / SpawnMemoryScaleZ);

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
	Params.InEpsilon = 0.9f;
	Params.InGamma = 0.9f;
	Params.InAlpha = 0.9f;
	RLBase->Init(Params);
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
			Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->OnTargetSpawned.Broadcast(SpawnTarget);

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
			Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->OnTargetSpawned.Broadcast(SpawnTarget);

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
		Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->OnTargetSpawned.Broadcast(ActiveBeatGridTarget);
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
		LocationBeforeDirectionChange = SpawnBox->Bounds.Origin;

		Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->OnTargetSpawned.Broadcast(BeatTrackTarget);
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
	Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->OnTargetDestroyed.Broadcast(TimeAlive, ConsecutiveTargetsHit, CombatTextLocation);

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
	RemoveFromRecentDelegate.BindUFunction(this, FName("RemoveFromRecentTargets"), DestroyedTarget->Guid);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, RemoveFromRecentDelegate, GameModeActorStruct.TargetSpawnCD, false);

	if (bShowDebug_SpawnBox)
	{
		DrawDebugBox(GetWorld(), SpawnBox->Bounds.Origin, SpawnBox->Bounds.BoxExtent, FColor::Orange, false, 1);
	}
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

	TArray<FVector> OpenLocations = GetValidSpawnLocations(NewTargetScale);
	if (OpenLocations.IsEmpty())
	{
		UE_LOG(LogTemp, Display, TEXT("OpenLocations is empty."));
		bSkipNextSpawn = true;
		return GetBoxOrigin();
	}

	if (bShowDebug_SpawnBox)
	{
		for (const FVector Vector : OpenLocations)
		{
			FVector Loc = FVector(Vector.X, Vector.Y + SpawnMemoryIncY / 2.f, Vector.Z + SpawnMemoryIncZ / 2.f);
			DrawDebugBox(GetWorld(), Loc, FVector(0, SpawnMemoryIncY / 2.f, SpawnMemoryIncZ / 2.f), FColor::Emerald, false, GameModeActorStruct.TargetSpawnCD);
		}
	}

	bSkipNextSpawn = false;
	
	if (const FVector Location = TryGetSpawnLocationFromRLAgent(); Location != FVector::ZeroVector)
	{
		if (OpenLocations.ContainsByPredicate([&Location](const FVector Element)
		{
			if (Element.Equals(Location))
			{
				UE_LOG(LogTemp, Display, TEXT("Successful Spawn Location suggested by RLAgent."));
				return true;
			}
			return false;
		}))
		{
			FVectorCounter Found = GetVectorCounterFromPoint(Location);
			Found.ActualChosenPoint = Found.GetRandomSubPoint(GetBorderingDirections(OpenLocations, Found.Point));
			if (bShowDebug_SpawnBox)
			{
				DrawDebugBox(GetWorld(), Found.Center, FVector(0, SpawnMemoryIncY / 2.f, SpawnMemoryIncZ / 2.f), FColor::Red, false, 0.5);
			}
			//UE_LOG(LogTemp, Display, TEXT("Found Point %s Found Center %s Found ActualChosenPoint %s"), *Found.Point.ToString(), *Found.Center.ToString(), *Found.ActualChosenPoint.ToString());
			return Found.ActualChosenPoint;
		}
		UE_LOG(LogTemp, Display, TEXT("Unable to Spawn Location suggested by RLAgent. %s"), *Location.ToString());
	}

	if (const FVector Origin = GetBoxOrigin(); OpenLocations.ContainsByPredicate([&Origin](const FVector Element)
	{
		if (Element.Equals(Origin))
		{
			return true;
		}
		return false;
	}))
	{
		if (bShowDebug_SpawnBox)
		{
			FVector Loc = FVector(Origin.X, Origin.Y + SpawnMemoryIncY / 2.f, Origin.Z + SpawnMemoryIncZ / 2.f);
			DrawDebugBox(GetWorld(), Loc, FVector(0, SpawnMemoryIncY / 2.f, SpawnMemoryIncZ / 2.f), FColor::Red, false, 0.5);
		}
		return Origin;
	}
	const int32 RandomPoint = UKismetMathLibrary::RandomIntegerInRange(0, OpenLocations.Num() - 1);
	FVectorCounter Found = GetVectorCounterFromPoint(OpenLocations[RandomPoint]);
	Found.ActualChosenPoint = Found.GetRandomSubPoint(GetBorderingDirections(OpenLocations, Found.Point));
	if (bShowDebug_SpawnBox)
	{
		DrawDebugBox(GetWorld(), Found.Center, FVector(0, SpawnMemoryIncY / 2.f, SpawnMemoryIncZ / 2.f), FColor::Red, false, 0.5);
	}
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

void ATargetSpawner::RemoveFromRecentTargets(const FGuid GuidToRemove)
{
	TArray<FRecentTarget> Targets = GetRecentTargets();
	Targets.Remove(FRecentTarget(GuidToRemove));
	RecentTargets = Targets;
}

int32 ATargetSpawner::AddToActiveTargets(ASphereTarget* SpawnTarget)
{
	TArray<ASphereTarget*> Targets = GetActiveTargets();
	const int32 NewIndex = Targets.Emplace(SpawnTarget);
	ActiveTargets = Targets;
	return NewIndex;
}

void ATargetSpawner::RemoveFromActiveTargets(ASphereTarget* SpawnTarget)
{
	TArray<ASphereTarget*> Targets = GetActiveTargets();
	Targets.Remove(SpawnTarget);
	ActiveTargets = Targets;
}

TArray<FVector> ATargetSpawner::GetValidSpawnLocations(const float Scale)
{
	TArray<FRecentTarget> RecentTargetsCopy = GetRecentTargets();
	TArray<FVector> AllPoints = GetAllSpawnLocations();
	
	/* Resizing Overlapping Points if necessary */
	for (FRecentTarget Struct : RecentTargetsCopy)
	{
		if (Struct.TargetScale < Scale)
		{
			Struct.OverlappingPoints = GetOverlappingPoints(Struct.CenterVector, Scale);
		}
	}

	switch (GameModeActorStruct.SpreadType)
	{
	case ESpreadType::DynamicEdgeOnly:

		/* TODO: Fix this since the max Y and max Z were removed */
		/*
		FVector NegativeExtents = GetBoxExtrema(0, true);
		FVector PositiveExtents = GetBoxExtrema(1, true);
		for (float Y = roundf(-Extents.Y); Y <= roundf(Extents.Y) + 0.01; Y++)
		{
			AllPoints.AddUnique(FVector(GetBoxOrigin().X, Y * SpawnMemoryIncY, GetBoxOrigin().Z + -Extents.Z * SpawnMemoryIncZ));
			AllPoints.AddUnique(FVector(GetBoxOrigin().X, Y * SpawnMemoryIncY, GetBoxOrigin().Z + Extents.Z * SpawnMemoryIncZ));
		}
		for (float Z = roundf(-Extents.Z); Z <= roundf(Extents.Z) + 0.01; Z++)
		{
			AllPoints.AddUnique(FVector(GetBoxOrigin().X, -Extents.Y * SpawnMemoryIncY, GetBoxOrigin().Z + Z * SpawnMemoryIncZ));
			AllPoints.AddUnique(FVector(GetBoxOrigin().X, Extents.Y * SpawnMemoryIncY, GetBoxOrigin().Z + Z * SpawnMemoryIncZ));
		}*/
		break;
	case ESpreadType::DynamicRandom: RemoveDynamicScaledExtents(AllPoints);
		break;
	case ESpreadType::None:
		break;
	case ESpreadType::StaticNarrow: break;
	case ESpreadType::StaticWide: break;
	default: break;
	}

	/* Remove any overlapping points from AllPoints */
	for (FRecentTarget Struct : RecentTargetsCopy)
	{
		for (FVector Vector : Struct.OverlappingPoints)
		{
			if (AllPoints.Remove(Vector) >= 1 && bShowDebug_SpawnMemory)
			{
				DrawDebugPoint(GetWorld(), Vector, 10, FColor::Red, false, 0.35f);
			}
		}
	}
	RemoveEdgePoints(AllPoints);
	return AllPoints;
}

void ATargetSpawner::RemoveDynamicScaledExtents(TArray<FVector>& In) const
{
	//UE_LOG(LogTemp, Display, TEXT("Size before removing ScaledExtents: %d"), In.Num());
	/* Dynamic will always be smaller than the static */
	const FVector NegativeExtrema = GetBoxExtrema(0, true);
	const FVector PositiveExtrema = GetBoxExtrema(1, true);
	TArray<FVector> Remove;
	for (FVector Vector : In)
	{
		if (Vector.Y < NegativeExtrema.Y || Vector.Y > PositiveExtrema.Y || Vector.Z < NegativeExtrema.Z || Vector.Z > PositiveExtrema.Z)
		{
			Remove.Add(Vector);
		}
	}
	for (FVector Vector : Remove)
	{
		In.Remove(Vector);
	}
	//UE_LOG(LogTemp, Display, TEXT("Size after removing ScaledExtents: %d"), In.Num());
}

void ATargetSpawner::RemoveEdgePoints(TArray<FVector>& In) const
{
	const FVector MaxExtrema = GetBoxExtrema(1, true);
	TArray<FVector> Remove;
	for (FVector Vector : In)
	{
		int Count = 2;
		const FVector Right = Vector + FVector(0, SpawnMemoryIncY, 0);
		const FVector Top = Vector + FVector(0, 0, SpawnMemoryIncZ);
		
		if (Right.Y != MaxExtrema.Y)
		{
			if (!In.Contains(Right))
			{
				Count--;
			}
		}
		if (Top.Z != MaxExtrema.Z)
		{
			if (!In.Contains(Top))
			{
				Count--;
			}
		}
		if (Count != 2)
		{
			Remove.Add(Vector);
		}
	}
	int Count = 0;
	for (FVector Vector : Remove)
	{
		In.Remove(Vector);
		Count++;
	}
	UE_LOG(LogTemp, Display, TEXT("Removed Edge Points: %d"), Count);
}

TArray<EDirection> ATargetSpawner::GetBorderingDirections(const TArray<FVector> ValidLocations, const FVector Location) const
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

FVector ATargetSpawner::GetBoxExtrema(const int32 PositiveExtreme, const bool bDynamic) const
{
	if (bDynamic)
	{
		//UE_LOG(LogTemp, Display, TEXT("Extrema %s"), *SpawnBox->Bounds.GetBoxExtrema(PositiveExtreme).ToString());
		return SpawnBox->Bounds.GetBoxExtrema(PositiveExtreme);
	}
	if (PositiveExtreme == 1)
	{
		return StaticMaxExtrema;
	}
	return StaticMinExtrema;
}

FVector ATargetSpawner::GetBoxExtents_Static() const
{
	return FVector(0, GameModeActorStruct.BoxBounds.Y, GameModeActorStruct.BoxBounds.Z);
}

FVector ATargetSpawner::GetBoxExtents_Dynamic() const
{
	return SpawnBox->Bounds.BoxExtent;
}

FVector ATargetSpawner::GetBoxOrigin() const
{
	return SpawnBox->Bounds.Origin;
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

TArray<FVector> ATargetSpawner::GetOverlappingPoints(const FVector Center, const float Scale) const
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

FIntPoint ATargetSpawner::InitializeSpawnCounter()
{
	const FVector NegativeExtents = GetBoxExtrema(0, false);
	const FVector PositiveExtents = GetBoxExtrema(1, false);
	
	//int BigCount = 0;
	int ColumnCount = 0;
	int RowCount = 0;

	for (float Z = NegativeExtents.Z; Z < PositiveExtents.Z; Z += SpawnMemoryIncZ)
	{
		RowCount = 0;
		for (float Y = NegativeExtents.Y; Y < PositiveExtents.Y; Y += SpawnMemoryIncY)
		{
			SpawnCounter.Emplace(FVectorCounter(FVector(GetBoxOrigin().X, Y, Z), SpawnMemoryIncY, SpawnMemoryIncZ));
			AllSpawnLocations.Emplace(FVector(GetBoxOrigin().X, Y, Z));
			/*TArray<FVector> SubPoints;
			 if (Z == Extents.Z || Y == Extents.Y)
			 {○
			 	continue;
			 }
			 int SmallCount = 0;
			 const float ZSubInc = Z + ZInc;
			 const float YSubInc = Y + YInc;
			 for (float SubZ = Z; SubZ < ZSubInc; SubZ+=1.f)
			 {
			 	for (float SubY = Y; SubY< YSubInc; SubY+=1.f)
			 	{
			 		SubPoints.Emplace(FVector(GetBoxOrigin().X, SubY, GetBoxOrigin().Z + SubZ));
			 		BigCount++;
			 		SmallCount++;
			 	}
			 }
			 float SubZ = Z;
			 float SubY = Y;
			 UE_LOG(LogTemp, Display, TEXT("SubZ %f"), SubZ);
			 UE_LOG(LogTemp, Display, TEXT("SubZ %f"), SubZ + ZInc);
			 UE_LOG(LogTemp, Display, TEXT("SubY %f"), SubY);
			 UE_LOG(LogTemp, Display, TEXT("SubY %f"), SubY + YInc);
			 UE_LOG(LogTemp, Display, TEXT("Small Count %d"), SmallCount);
			 UE_LOG(LogTemp, Display, TEXT("%s"), *FVector(GetBoxOrigin().X, Y, GetBoxOrigin().Z + Z).ToString());*/
			RowCount++;
		}
		ColumnCount++;
	}
	UE_LOG(LogTemp, Display, TEXT("SpawnCounterSize: %d %llu"), SpawnCounter.Num(), SpawnCounter.GetAllocatedSize());
	return FIntPoint(RowCount, ColumnCount);
}

FVectorCounter ATargetSpawner::GetVectorCounterFromPoint(const FVector Point) const
{
	TArray<FVectorCounter> SpawnCounterCopy = GetSpawnCounter();
	for (const FVectorCounter VectorCounter : SpawnCounterCopy)
	{
		if (VectorCounter.Point.Y == Point.Y && VectorCounter.Point.Z == Point.Z)
		{
			return VectorCounter;
		}
	}
	UE_LOG(LogTemp, Display, TEXT("Point Not Found in SpawnCounter"));
	return FVectorCounter();
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
		FoundTargetPair.Reward = 0.f;
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

FVector ATargetSpawner::TryGetSpawnLocationFromRLAgent()
{
	FTargetPair TargetPair;
	if (!TargetPairs.Peek(TargetPair))
	{
		return FVector::ZeroVector;
	}
	TargetPairs.Pop();

	const int32 StateIndex = SpawnCounter.Find(TargetPair.Previous);
	const int32 State2Index = SpawnCounter.Find(TargetPair.Current);
	const int32 ActionIndex = State2Index;
	const int32 Action2Index = RLBase->GetNextActionIndex(State2Index);

	if (Action2Index >= SpawnCounter.Num() || Action2Index == INDEX_NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Spawn Point from RLAgent: TargetPair.Previous %s TargetPair.Current %s"),*TargetPair.Previous.ToString(), *TargetPair.Current.ToString());
		return FVector::ZeroVector;
	}

	UpdateRLAgent(FAlgoInput(StateIndex, State2Index, ActionIndex, Action2Index, TargetPair.Reward));
	return SpawnCounter[Action2Index].Point;
}

void ATargetSpawner::UpdateRLAgent(const FAlgoInput In)
{
	RLBase->UpdateEpisodeRewards(In.Reward);
	RLBase->UpdateQTable(In);
}

void ATargetSpawner::AppendStringLocAccRow(const F2DArray Row, FString& StringToWriteTo)
{
	for (const float AccValue : Row.Accuracy)
	{
		StringToWriteTo.Append(FString::SanitizeFloat(AccValue) + ",");
	}
	StringToWriteTo.Append("\n");
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
