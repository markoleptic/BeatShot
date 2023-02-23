// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "TargetSpawner.h"
#include "SphereTarget.h"
#include "DefaultGameMode.h"
#include "Components/BoxComponent.h"
#include "VisualGrid.h"
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
	TargetSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
}

void ATargetSpawner::Destroyed()
{
	if (SpawnAreaTotalsPoints.Num() > 0)
	{
		FString file = FPaths::ProjectDir();
		file.Append(TEXT("AccuracyMatrix.csv"));
		FString StringToWrite;
		for (const FGridPoint Point : SpawnAreaHitsPoints)
		{
			StringToWrite.Append(FString::FromInt(Point.Count));
			if (Point.Point.X == NumRowsGrid -1 )
			{
				StringToWrite.Append("\n");
			}
			else
			{
				StringToWrite.Append(",");
			}
		}
		StringToWrite.Append("\n");
		for (const FGridPoint Point : SpawnAreaTotalsPoints)
		{
			StringToWrite.Append(FString::FromInt(Point.Count));
			if (Point.Point.X == NumRowsGrid -1 )
			{
				StringToWrite.Append("\n");
			}
			else
			{
				StringToWrite.Append(",");
			}
		}
		FFileHelper::SaveStringToFile(StringToWrite, *file);
	}
	
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
	if (VisualGrid)
	{
		VisualGrid->Destroy();
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

	for (ASphereTarget* Target : ActiveTargetArray)
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

	/* RLProject */
	if (GameModeActorStruct.CustomGameModeName.Equals("RLProject"))
	{
		GameModeActorStruct.BoxBounds.Y = 2048 / 2;
		GameModeActorStruct.BoxBounds.Z = 1024 / 2;
		NumRowsGrid = 16;
		NumColsGrids = 8;
	}

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

	/*/* Set how precise the 2D representation of SpawnArea should be #1#
	if (GameModeActorStruct.BoxBounds.Y * GameModeActorStruct.BoxBounds.Z <= 250000)
	{
		SpawnAreaScale = 5;
	}
	else
	{
		SpawnAreaScale = 10;
	}*/
	SpawnAreaScale = 500;

	/* Set new location & box extent */
	SpawnBox->SetRelativeLocation(CenterOfSpawnBox);
	SpawnBox->SetBoxExtent(GameModeActorStruct.BoxBounds);

	/* Initial target spawn location */
	SpawnLocation = SpawnBox->Bounds.Origin;

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
		/* TODO: implement SpawnAreaPoints for BeatGrid */
		//NumRowsGrid = sqrt(GameModeActorStruct.NumTargetsAtOnceBeatGrid);
		//NumColsGrids = sqrt(GameModeActorStruct.NumTargetsAtOnceBeatGrid);
	}
	SpawnMemoryScale = 0.05f;
	SpawnMemoryTransform =  FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector(SpawnMemoryScale));
	FBox Box = FBox::BuildAABB(SpawnBox->GetComponentLocation(), SpawnBox->Bounds.BoxExtent).TransformBy(SpawnMemoryTransform);
	FVector BoxCenter;
	FVector Extents;
	Box.GetCenterAndExtents(BoxCenter, Extents);
	NumRowsGrid = Extents.Y * 2 + 1;
	NumColsGrids = Extents.Z * 2 + 1;
	/* RLProject */
	if (GameModeActorStruct.CustomGameModeName.Equals("RLProject"))
	{
		NumRowsGridScoring = 16;
		NumColsGridsScoring = 8;
	}
	else
	{
		NumRowsGridScoring = NumRowsGrid;
		NumColsGridsScoring = NumColsGrids;
	}

	if (GIsEditor)
	{
		VisualGrid = GetWorld()->SpawnActor<AVisualGrid>(VisualGridClass, FVector(3990, 0, 1500),
												 FRotator::ZeroRotator);
		VisualGrid->CreateGrid(NumRowsGrid, NumColsGrids);
	}
	SpawnAreaTotalsPoints.SetNum(NumRowsGrid * NumColsGrids);
	SpawnAreaHitsPoints.SetNum(NumRowsGrid * NumColsGrids);
	
	for (int i = 0; i < NumColsGrids; i++)
	{
		for (int j = 0; j < NumRowsGrid; j++)
		{
			SpawnAreaHitsPoints[i * NumRowsGrid + j].Point = FIntPoint(j,i);
			SpawnAreaTotalsPoints[i * NumRowsGrid + j].Point = FIntPoint(j,i);
		}
	}
	UE_LOG(LogTemp, Display, TEXT("NumRowsGrid: %d"), NumRowsGrid);
	UE_LOG(LogTemp, Display, TEXT("NumColsGrids: %d"), NumColsGrids);
}

void ATargetSpawner::InitBeatGrid()
{
	// clear any variables that could have been used prior to a restart
	if (RecentBeatGridIndices.IsEmpty() == false)
	{
		RecentBeatGridIndices.Empty();
		RecentBeatGridIndices.Shrink();
	}
	if (SpawnedBeatGridTargets.IsEmpty() == false)
	{
		SpawnedBeatGridTargets.Empty();
		SpawnedBeatGridTargets.Shrink();
	}

	if (ActiveBeatGridTarget)
	{
		ActiveBeatGridTarget = nullptr;
	}

	LastBeatGridIndex = -1.f;
	InitialBeatGridTargetActivated = false;
	
	const float HalfWidth = round(SpawnBox->Bounds.BoxExtent.Y);
	const float HalfHeight = round(SpawnBox->Bounds.BoxExtent.Z);
	const float NumTargets = (sqrt(GameModeActorStruct.BeatGridSize));
	FVector BeatGridSpawnLocation = SpawnBox->Bounds.Origin;
	const float OuterSpacing = 100.f;
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

			if (ASphereTarget* BeatGridTarget = GetWorld()->SpawnActor<ASphereTarget>(ActorToSpawn,
				BeatGridSpawnLocation, SpawnBox->GetComponentRotation()))
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

void ATargetSpawner::SpawnMultiBeatTarget()
{
	if (!bSkipNextSpawn)
	{
		if (ASphereTarget* SpawnTarget =
			GetWorld()->SpawnActor<ASphereTarget>(ActorToSpawn, SpawnLocation, FRotator::ZeroRotator,
			                                      TargetSpawnParams))
		{
			/* Setting the current target's scale that was previously calculated */
			SpawnTarget->SetSphereScale(FVector(TargetScale, TargetScale, TargetScale));
			SpawnTarget->OnLifeSpanExpired.AddDynamic(this, &ATargetSpawner::OnTargetTimeout);
			ActiveTargetArray.Emplace(SpawnTarget);
			Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->OnTargetSpawned.Broadcast(SpawnTarget);
			RecentTargetArray2.Emplace(FRecentTargetStruct(SpawnTarget->Guid,
				GetOverlappingPoints(SpawnTarget->GetActorLocation(),TargetScale), TargetScale,
				SpawnTarget->GetActorLocation()));
			
			if (GIsEditor)
			{
				VisualGrid->SetCustomDataValues(GetOverlappingPoints(SpawnTarget->GetActorLocation(), TargetScale),
					SpawnBox->Bounds.Origin, GameModeActorStruct.BoxBounds, SpawnMemoryScale, 1);
				DrawDebugBox(GetWorld(), SpawnBox->Bounds.Origin, SpawnBox->Bounds.BoxExtent, FColor::Orange, false, 1);
			}
		}
	}
	FindNextTargetProperties();
}

void ATargetSpawner::SpawnSingleBeatTarget()
{
	if (!bSkipNextSpawn)
	{
		ASphereTarget* SpawnTarget = GetWorld()->SpawnActor<ASphereTarget>(
			ActorToSpawn, SpawnLocation, FRotator::ZeroRotator,
			TargetSpawnParams);

		/* Setting the current target's scale that was previously calculated */
		SpawnTarget->SetSphereScale(FVector(TargetScale, TargetScale, TargetScale));
		SpawnTarget->OnLifeSpanExpired.AddDynamic(this, &ATargetSpawner::OnTargetTimeout);
		
		RecentTargetArray2.Emplace(FRecentTargetStruct(SpawnTarget->Guid,
		GetOverlappingPoints(SpawnTarget->GetActorLocation(),TargetScale), TargetScale,
		SpawnTarget->GetActorLocation()));

		Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->OnTargetSpawned.Broadcast(SpawnTarget);
		
		if (GIsEditor)
		{
			VisualGrid->SetCustomDataValues(GetOverlappingPoints(SpawnTarget->GetActorLocation(), TargetScale),
				SpawnBox->Bounds.Origin, GameModeActorStruct.BoxBounds, SpawnMemoryScale, 1);
			DrawDebugBox(GetWorld(), SpawnBox->Bounds.Origin, SpawnBox->Bounds.BoxExtent, FColor::Orange, false, 1);
		}
		
		if (SpawnLocation == SpawnBox->Bounds.Origin)
		{
			LastTargetSpawnedCenter = true;
		}
		else
		{
			LastTargetSpawnedCenter = false;
		}
	}
	FindNextTargetProperties();
	/* Don't continue spawning for SingleBeat */
	SetShouldSpawn(false);
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
		Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->OnTargetSpawned.Broadcast(ActiveBeatGridTarget);
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
		BeatTrackTarget = GetWorld()->SpawnActor<ASphereTarget>(ActorToSpawn, SpawnBox->Bounds.Origin,
		                                                        FRotator::ZeroRotator, TargetSpawnParams);
		BeatTrackTarget->OnActorEndOverlap.AddDynamic(this, &ATargetSpawner::OnBeatTrackOverlapEnd);
		LocationBeforeDirectionChange = SpawnBox->Bounds.Origin;

		/** Broadcast to GameModeActorBase and DefaultCharacter that a BeatTrack target has spawned */
		Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->OnBeatTrackTargetSpawned.Broadcast(
			BeatTrackTarget);
	}
	if (BeatTrackTarget)
	{
		LocationBeforeDirectionChange = BeatTrackTarget->GetActorLocation();
		const float NewTargetScale = GetNextTargetScale();
		BeatTrackTarget->SetSphereScale(FVector(NewTargetScale, NewTargetScale, NewTargetScale));
		BeatTrackTargetSpeed = FMath::FRandRange(GameModeActorStruct.MinTrackingSpeed,
		                                         GameModeActorStruct.MaxTrackingSpeed);
		EndLocation = GetRandomBeatTrackLocation(LocationBeforeDirectionChange);
		BeatTrackTargetDirection = UKismetMathLibrary::GetDirectionUnitVector(
			LocationBeforeDirectionChange, EndLocation);
	}
}

void ATargetSpawner::OnTargetTimeout(const bool DidExpire, const float TimeAlive, ASphereTarget* DestroyedTarget)
{
	ActiveTargetArray.Remove(DestroyedTarget);
	ActiveTargetArray.Shrink();
	FTimerHandle TimerHandle;
	RemoveFromRecentDelegate.BindUFunction(this, FName("RemoveFromRecentTargetArray"), DestroyedTarget->Guid);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, RemoveFromRecentDelegate,
	                                       GameModeActorStruct.TargetSpawnCD + 0.01, false);

	
	if (GameModeActorStruct.IsSingleBeatMode)
	{
		SetShouldSpawn(true);
	}

	if (DidExpire)
	{
		ConsecutiveTargetsHit = 0;
		if (DynamicScaleFactor > 80)
		{
			DynamicScaleFactor -= 20;
		}
		else if (DynamicScaleFactor > 60)
		{
			DynamicScaleFactor -= 15;
		}
		else if (DynamicScaleFactor > 40)
		{
			DynamicScaleFactor -= 10;
		}
		else
		{
			DynamicScaleFactor -= 5;
		}
		if (DynamicScaleFactor < 0)
		{
			DynamicScaleFactor = 0;
		}
	}
	else
	{
		ConsecutiveTargetsHit++;
		if (DynamicScaleFactor > 100)
		{
			DynamicScaleFactor = 100;
		}
		if (DynamicScaleFactor > 80)
		{
			DynamicScaleFactor += 2;
		}
		else if (DynamicScaleFactor > 60)
		{
			DynamicScaleFactor += 3;
		}
		else if (DynamicScaleFactor > 40)
		{
			DynamicScaleFactor += 4;
		}
		else
		{
			DynamicScaleFactor += 5;
		}
	}

	const FVector DestroyedTargetLocation = DestroyedTarget->GetActorLocation();
	const FVector Location = {
		DestroyedTargetLocation.X,
		DestroyedTargetLocation.Y,
		DestroyedTargetLocation.Z +
		SphereTargetRadius * DestroyedTarget->GetActorScale3D().Z
	};

	const ADefaultGameMode* GameMode = Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	GameMode->OnTargetDestroyed.Broadcast(TimeAlive, ConsecutiveTargetsHit, Location);

	if (GameModeActorStruct.IsBeatGridMode || GameModeActorStruct.IsBeatTrackMode)
	{
		return;
	}
	
	// FIntPoint DestroyedTargetCenterPoint;
	// for (FRecentTargetStruct Struct : GetRecentTargetArray())
	// {
	// 	if (Struct.TargetGuid == DestroyedTarget->Guid)
	// 	{
	// 		DestroyedTargetCenterPoint = Struct.Center;
	// 		break;
	// 	}
	// }
	//
	// SpawnAreaTotalsPoints[DestroyedTargetCenterPoint.Y * NumRowsGrid + DestroyedTargetCenterPoint.X]++;
	// if (!DidExpire)
	// {
	// 	SpawnAreaHitsPoints[DestroyedTargetCenterPoint.Y * NumRowsGrid + DestroyedTargetCenterPoint.X]++;
	// }
	/*FGridPoint* Pont = SpawnAreaTotalsPoints.FindByPredicate([&](FGridPoint GridPoint) {
		return GridPoint.Point == DestroyedTargetCenterPoint;
	});
	if (Pont)
	{
		UE_LOG(LogTemp, Display, TEXT("Pont %s"), *DestroyedTargetCenterPoint.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("NoPont %s"), *DestroyedTargetCenterPoint.ToString());
	}*/
}

void ATargetSpawner::OnBeatTrackOverlapEnd(AActor* OverlappedActor, AActor* OtherActor)
{
	/* Reverse direction if not longer overlapping spawn box */
	if (Cast<ATargetSpawner>(OverlappedActor) || Cast<ASphereTarget>(OverlappedActor) &&
		Cast<ASphereTarget>(OtherActor) || Cast<ATargetSpawner>(OtherActor))
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
		/* start with widest value and gradually lower */
		if (DynamicScaleFactor == 0)
		{
			return GameModeActorStruct.MaxTargetScale;
		}
		const float NewFactor = 1 - DynamicScaleFactor / 100.f;
		return UKismetMathLibrary::Lerp(GameModeActorStruct.MinTargetScale,
		                                GameModeActorStruct.MaxTargetScale, NewFactor);
	}
	return FMath::FRandRange(GameModeActorStruct.MinTargetScale, GameModeActorStruct.MaxTargetScale);
}

FVector ATargetSpawner::GetNextTargetSpawnLocation(const ESpreadType SpreadType, const float NewTargetScale)
{
	bSkipNextSpawn = false;
	bool bIsDynamicSpreadType = false;
	FVector BoxExtent = GameModeActorStruct.BoxBounds;

	if (GameModeActorStruct.IsSingleBeatMode && !LastTargetSpawnedCenter)
	{
		return SpawnBox->Bounds.Origin;
	}
	/* Change the BoxExtent of the SpawnBox if dynamic */
	if (SpreadType == ESpreadType::DynamicRandom || SpreadType == ESpreadType::DynamicEdgeOnly)
	{
		
		bIsDynamicSpreadType = true;
		const float BoxBoundsScaleFactor = (1 - DynamicScaleFactor / 100.f * 0.5f);
		BoxExtent = FVector(0, roundf(GameModeActorStruct.BoxBounds.Y * BoxBoundsScaleFactor),
		                    roundf(GameModeActorStruct.BoxBounds.Z * BoxBoundsScaleFactor));
		SpawnBox->SetBoxExtent(BoxExtent);
		DrawDebugBox(GetWorld(), SpawnBox->Bounds.Origin, SpawnBox->Bounds.BoxExtent, FColor::Orange, false, 1);
	}
	
	TArray<FVector> Open = GetValidSpawnPoints2(NewTargetScale, BoxExtent, bIsDynamicSpreadType);
	
	if (Open.Num() == 0)
	{
		bSkipNextSpawn = true;
		return SpawnBox->Bounds.Origin;
	}
	
	FBox Box = FBox::BuildAABB(SpawnBox->GetComponentLocation(), GameModeActorStruct.BoxBounds).TransformBy(SpawnMemoryTransform);
	FVector BoxCenter;
	FVector Extents;
	Box.GetCenterAndExtents(BoxCenter, Extents);
	FVector ScaledOrigin = FVector(roundf(SpawnBox->Bounds.Origin.X * SpawnMemoryScale), roundf(SpawnBox->Bounds.Origin.Y * SpawnMemoryScale),
							roundf(SpawnBox->Bounds.Origin.Z * SpawnMemoryScale));
	if (Open.ContainsByPredicate([ScaledOrigin] (const FVector Element)
	{
		if (Element.Equals(ScaledOrigin))
		{
			return true;
		}
		return false;
	}))
	{
		return SpawnBox->Bounds.Origin;
	}
	const int32 RandomPoint2 = UKismetMathLibrary::RandomFloatInRange(0, Open.Num() - 1);
	const FVector WorldVector = (1.f /SpawnMemoryScale) * Open[RandomPoint2];
	return WorldVector;
}

FVector ATargetSpawner::GetRandomBeatTrackLocation(const FVector& LocationBeforeChange) const
{
	/* try to spawn at origin if available */
	FVector LocationToReturn = SpawnBox->Bounds.Origin;

	/* So we don't get stuck in infinite loop */
	int OverloadProtect = 0;
	bool IsInsideBox = true;
	while (IsInsideBox)
	{
		if (OverloadProtect > 20)
		{
			break;
		}
		LocationToReturn = UKismetMathLibrary::RandomPointInBoundingBox(SpawnBox->Bounds.Origin, SpawnBox->Bounds.BoxExtent);
		if (UKismetMathLibrary::IsPointInBox(LocationBeforeChange +
		                                     UKismetMathLibrary::GetDirectionUnitVector(
			                                     LocationBeforeChange, LocationToReturn) *
		                                     BeatTrackTargetSpeed * GameModeActorStruct.TargetSpawnCD,
		                                     SpawnBox->Bounds.Origin, SpawnBox->Bounds.BoxExtent))
		{
			IsInsideBox = false;
		}
		OverloadProtect++;
	}
	return LocationToReturn;
}

void ATargetSpawner::MoveTargetForward(ASphereTarget* SpawnTarget, float DeltaTime) const
{
	const FVector Loc = SpawnTarget->GetActorLocation();
	const FVector NewLoc = FVector(Loc.X - GameModeActorStruct.MoveForwardDistance, Loc.Y, Loc.Z);
	SpawnTarget->SetActorLocation(
		UKismetMathLibrary::VInterpTo(Loc, NewLoc, DeltaTime, 1 / GameModeActorStruct.TargetMaxLifeSpan));
}

void ATargetSpawner::RemoveFromRecentTargetArray(const FGuid GuidToRemove)
{
	FRecentTargetStruct StructToRemove;
	for (FRecentTargetStruct Struct : GetRecentTargetArray2())
	{
		if (Struct.TargetGuid == GuidToRemove)
		{
			StructToRemove = Struct;
			break;
		}
	}
	RecentTargetArray2.Remove(StructToRemove);
	RecentTargetArray2.Shrink();
	if (!GIsEditor)
	{
		return;
	}
	VisualGrid->SetCustomDataValues(StructToRemove.OverlappingPoints,
		SpawnBox->Bounds.Origin, GameModeActorStruct.BoxBounds, SpawnMemoryScale, 0);
}

TArray<FVector> ATargetSpawner::GetValidSpawnPoints2(const float Scale, const FVector& BoxExtent,
	const bool bIsDynamicSpreadType)
{
	FBox Box = FBox::BuildAABB(SpawnBox->GetComponentLocation(), SpawnBox->Bounds.BoxExtent).TransformBy(SpawnMemoryTransform);
	
	TArray<FVector> AllPoints;
	TArray<FRecentTargetStruct> RecentTargets = GetRecentTargetArray2();
	FVector BoxCenter;
	FVector Extents;
	Box.GetCenterAndExtents(BoxCenter, Extents);
	const float LowerBound_Z = roundf(BoxCenter.Z - Extents.Z);
	const float UpperBound_Z = roundf(BoxCenter.Z + Extents.Z);
	const float LowerBound_Y = -roundf(Extents.Y);
	const float UpperBound_Y = roundf(Extents.Y);
	
	/* Resizing Blocked Points if necessary */
	for (FRecentTargetStruct Struct : RecentTargets)
	{
		if (Struct.TargetScale < Scale)
		{
			Struct.OverlappingPoints = GetOverlappingPoints(Struct.CenterVector, Scale);
		}
	}

	/* SingleBeat only cares about the current BoxExtent (Border) */
	if (GameModeActorStruct.IsSingleBeatMode)
	{
		for (int Y = LowerBound_Y; Y <= UpperBound_Y; Y++)
		{
			AllPoints.AddUnique(FVector(BoxCenter.X, Y, LowerBound_Z));
			AllPoints.AddUnique(FVector(BoxCenter.X, Y, UpperBound_Z));
		}
		for (int Z = LowerBound_Z; Z <= UpperBound_Z; Z++)
		{
			AllPoints.AddUnique(FVector(BoxCenter.X, LowerBound_Y, Z));
			AllPoints.AddUnique(FVector(BoxCenter.X, LowerBound_Y, Z));
		}
	}
	else
	{
		for (int Y = LowerBound_Y; Y <= UpperBound_Y; Y++)
		{
			for (int Z = LowerBound_Z; Z <= UpperBound_Z; Z++)
			{
				AllPoints.Emplace(FVector(BoxCenter.X, Y, Z));
			}
		}
		UE_LOG(LogTemp, Display, TEXT("AllPoints size: %d"), AllPoints.Num());
	}

	/* Remove any overlapping points between targets in RecentTargetArray */
	int Count = 0;
	for (FRecentTargetStruct Struct : RecentTargets)
	{
		for (FVector Vector : Struct.OverlappingPoints)
		{
			Count+=AllPoints.Remove(Vector);
		}
	}
	UE_LOG(LogTemp, Display, TEXT("NumRemoved: %d"), Count);
	//UE_LOG(LogTemp, Display, TEXT("AllPoints Size: %d"), AllPoints.Num());
	//UE_LOG(LogTemp, Display, TEXT("Extents: %s"), *Extents.ToString());
	//UE_LOG(LogTemp, Display, TEXT("BoxCenter: %s"), *BoxCenter.ToString());
	AllPoints.Shrink();
	return AllPoints;
}

TArray<FRecentTargetStruct> ATargetSpawner::GetRecentTargetArray2()
{
	return RecentTargetArray2;
}

TArray<FVector> ATargetSpawner::GetOverlappingPoints(const FVector Center, const float Scale) const
{
	const float Radius = roundf((Scale * SphereTargetRadius * 2 + GameModeActorStruct.MinDistanceBetweenTargets) * SpawnMemoryScale);
	FBox Box = FBox::BuildAABB(SpawnBox->GetComponentLocation(), GameModeActorStruct.BoxBounds).TransformBy(SpawnMemoryTransform);
	FSphere Sphere = FSphere(Center, roundf(Scale * SphereTargetRadius * 2 + GameModeActorStruct.MinDistanceBetweenTargets)).TransformBy(SpawnMemoryTransform);
	const float Vert = roundf(Center.Z * SpawnMemoryScale);
	const float Horiz = roundf(Center.Y * SpawnMemoryScale);
	FVector BoxCenter;
	FVector Extents;
	Box.GetCenterAndExtents(BoxCenter, Extents);
	const float UpperBoundVert = roundf(BoxCenter.Z + Extents.Z);
	const float UpperBoundHoriz = roundf(Extents.Y);
	const float LowerBoundVert = roundf(BoxCenter.Z - Extents.Z);
	const float LowerBoundHoriz = -roundf(Extents.Y);
	
	/* First create a square, and clamp the square inside of the box bounds */
	const float LeftSquare = FMath::Clamp(Horiz - Radius, LowerBoundHoriz, UpperBoundHoriz);
	const float RightSquare = FMath::Clamp(Horiz + Radius, LowerBoundHoriz, UpperBoundHoriz);
	const float BottomSquare = FMath::Clamp(Vert - Radius, LowerBoundVert, UpperBoundVert);
	const float TopSquare = FMath::Clamp(Vert + Radius, LowerBoundVert, UpperBoundVert);

	//UE_LOG(LogTemp, Display, TEXT("%f %f %f %f %f %f"), Vert, Horiz, LowerBoundVert, UpperBoundVert, LowerBoundHoriz, UpperBoundHoriz);
	//UE_LOG(LogTemp, Display, TEXT("%f %f %f %f"), LeftSquare, RightSquare, BottomSquare, TopSquare);
	
	/* An array that represents invalid spawn points */
	TArray<FVector> BlockedPoints;
	int Count = 0;
	
	/* Iterate over a square section with length & width equal to sphere diameter */
	for (int X = LeftSquare; X <= RightSquare; X++)
	{
		for (int Y = BottomSquare; Y <= TopSquare; Y++)
		{
			Count++;
			FVector Loc = FVector(BoxCenter.X, X, Y);
			/* Only add to BlockedPoints if inside circle radius */
			if (Sphere.IsInside(Loc))
			{
				BlockedPoints.Add(Loc);
			}
		}
	}
	//UE_LOG(LogTemp, Display, TEXT("BlockedPoints: %d"), BlockedPoints.Num());
	return BlockedPoints;
}

/*FVector ATargetSpawner::GenerateRandomTargetLocation(const ESpreadType SpreadType, const FVector& ScaledBoxExtent) const
{
	FVector NewSpawnLocation = BoxBounds.Origin;
	/* Non-dynamic #1#
	if (SpreadType != ESpreadType::DynamicRandom && SpreadType != ESpreadType::DynamicEdgeOnly)
	{
		NewSpawnLocation.Y += -BoxBounds.BoxExtent.Y + 2 * BoxBounds.BoxExtent.Y * FMath::FRand();
		NewSpawnLocation.Z += -BoxBounds.BoxExtent.Z + 2 * BoxBounds.BoxExtent.Z * FMath::FRand();
		return NewSpawnLocation;
	}

	/* DynamicRandom #1#
	if (SpreadType == ESpreadType::DynamicRandom)
	{
		return UKismetMathLibrary::RandomPointInBoundingBox(
			BoxBounds.Origin, BoxBounds.BoxExtent);
	}

	/* DynamicEdgeOnly #1#
	FVector Offset = FVector();
	switch (UKismetMathLibrary::RandomIntegerInRange(0, 3))
	{
	case 0: //top
		{
			/* Y is left-right Z is up-down #1#
			Offset.Y = UKismetMathLibrary::RandomFloatInRange(
				-ScaledBoxExtent.Y, ScaledBoxExtent.Y);
			Offset.Z = ScaledBoxExtent.Z;
			break;
		}
	case 1: // right
		{
			Offset.Y = ScaledBoxExtent.Y;
			Offset.Z = UKismetMathLibrary::RandomFloatInRange(
				-ScaledBoxExtent.Z, ScaledBoxExtent.Z);
			break;
		}
	case 2: // left
		{
			Offset.Y = -ScaledBoxExtent.Y;
			Offset.Z = UKismetMathLibrary::RandomFloatInRange(
				-ScaledBoxExtent.Z, ScaledBoxExtent.Z);
			break;
		}
	default: // bottom
		{
			Offset.Y = UKismetMathLibrary::RandomFloatInRange(
				-ScaledBoxExtent.Y, ScaledBoxExtent.Y);
			Offset.Z = -ScaledBoxExtent.Z;
		}
	}
	NewSpawnLocation = NewSpawnLocation + Offset;
	return NewSpawnLocation;
}*/

/*	int32 TotalSpawns = 0;
	int32 TotalHits = 0;
	int32 BottomLeft = 0;
	int32 TopLeft = 0;
	int32 BottomRight = 0;
	int32 TopRight = 0;
	int32 Count = 0;
 *for (std::vector<std::vector<int32>>::iterator Row = SpawnAreaTotals.begin(); Row != SpawnAreaTotals.end(); ++Row)
{
	for (std::vector<int32>::iterator Col = Row->begin(); Col != Row->end(); ++Col)
	{
		Count++;
		const int32 X = std::distance(SpawnAreaTotals.begin(), Row);
		const int32 Y = std::distance(Row->begin(), Col);

		/* Skip center target #1#
		if (DestroyedTargetCenterPoint.X == X && DestroyedTargetCenterPoint.Y == Y)
		{
			if (X != static_cast<int32>(roundf(GameModeActorStruct.BoxBounds.Y * 2 / SpawnAreaScale) / 2.f)
				&& Y != static_cast<int32>(roundf(GameModeActorStruct.BoxBounds.Z * 2 / SpawnAreaScale) / 2.f))
			{
				SpawnAreaTotals[X][Y] += 1;
			}
			if (!DidExpire)
			{
				SpawnAreaHits[X][Y] += 1;
			}
		}

		if (X < roundf(GameModeActorStruct.BoxBounds.Y * 2 / SpawnAreaScale) / 2.f && Y < roundf(
			GameModeActorStruct.BoxBounds.Z * 2 / SpawnAreaScale) / 2.f)
		{
			BottomLeft += SpawnAreaTotals[X][Y];
		}
		else if (X < roundf(GameModeActorStruct.BoxBounds.Y * 2 / SpawnAreaScale) / 2.f && Y >= roundf(
			GameModeActorStruct.BoxBounds.Z * 2 / SpawnAreaScale) / 2.f)
		{
			TopLeft += SpawnAreaTotals[X][Y];
		}
		else if (X >= roundf(GameModeActorStruct.BoxBounds.Y * 2 / SpawnAreaScale) / 2.f && Y < roundf(
			GameModeActorStruct.BoxBounds.Z * 2 / SpawnAreaScale) / 2.f)
		{
			TopRight += SpawnAreaTotals[X][Y];
		}
		else if (X >= roundf(GameModeActorStruct.BoxBounds.Y * 2 / SpawnAreaScale) / 2.f && Y >= roundf(
			GameModeActorStruct.BoxBounds.Z * 2 / SpawnAreaScale) / 2.f)
		{
			BottomRight += SpawnAreaTotals[X][Y];
		}
		TotalSpawns += SpawnAreaTotals[X][Y];
		TotalHits += SpawnAreaHits[X][Y];
	}
}
UE_LOG(LogTemp, Display, TEXT("Count: %d"), Count);
UE_LOG(LogTemp, Display, TEXT("TopLeft: %d TopRight: %d BottomLeft: %d BottomRight: %d"), TopLeft, TopRight,
	   BottomLeft, BottomRight);
UE_LOG(LogTemp, Display, TEXT("TotalSpawns: %d Total Hits: %d"), TotalSpawns, TotalHits);
*/

/*	SpawnAreaTotals = {
	static_cast<unsigned long>(NumColsGrids),
	std::vector(static_cast<unsigned long>(NumRowsGrid), 0)
};

SpawnAreaHits = {
	static_cast<unsigned long>(NumColsGrids),
	std::vector(static_cast<unsigned long>(NumRowsGrid), 0)
};
 *FString StringToWrite;
for (auto& Row : SpawnAreaTotals)
{
	for (auto Col : Row)
	{
		StringToWrite.Append(FString::FromInt(Col) + ",");
	}
	StringToWrite.Append("\n");
}
UE_LOG(LogTemp, Display, TEXT("%s"),*StringToWrite);*/

/*FString file = FPaths::ProjectDir();
file.Append(TEXT("MyConfig.csv"));

// We will use this FileManager to deal with the file.
IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

FString StringToWrite;
for (auto& Row : SpawnAreaTotals)
{
	for (auto Col : Row)
	{
		StringToWrite.Append(FString::FromInt(Col) + ",");
	}
	StringToWrite.Append("\n");
}

// Always first check if the file that you want to manipulate exist.
if (!FileManager.FileExists(*file))
{
	// We use the LoadFileToString to load the file into
	if (FFileHelper::SaveStringToFile(StringToWrite, *file))
	{
		UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Sucsesfuly Written: \"%s\" to the text file"),
			   *StringToWrite);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Failed to write FString to file."));
	}
}
else
{
	UE_LOG(LogTemp, Warning, TEXT("FileManipulation: ERROR: Can not read the file because it was not found."));
	UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Expected file location: %s"), *file);
}*/

/*for (auto& Point : SpawnAreaTotalsPoints)
{
	StringToWrite.Append(FString::FromInt(Point.Point.X) + "," + FString::FromInt(Point.Point.Y) + "," + FString::FromInt(Point.Count));
	StringToWrite.Append("\n");
}
FFileHelper::SaveStringToFile(StringToWrite, *file);*/

/*FVector Test1Before = {370.000 ,-80.000 ,61.000};
				FVector Test1After = {BoxCenter.X, Test1Before.Y + Extents.Y, abs(BoxCenter.Z + Extents.Z) - Test1Before.Z };

				const float Test1FloatValue = roundf((Test1Before.Y + Extents.Y) + ((abs(BoxCenter.Z + Extents.Z) - Test1Before.Z) * NumRowsGrid));

				FVector Test2Before = {370.000 ,80.000 ,11.000};
				FVector Test2After = {BoxCenter.X, Test2Before.Y + Extents.Y, abs(BoxCenter.Z + Extents.Z) - Test2Before.Z };

				const float Test1FloatValue2 = roundf((Test2Before.Y + Extents.Y) + ((abs(BoxCenter.Z + Extents.Z) - Test2Before.Z) * NumRowsGrid));

				FVector Test3Before = {370.000 ,-80.000 ,51.000};
				FVector Test3After = {BoxCenter.X, Test3Before.Y + Extents.Y, abs(BoxCenter.Z + Extents.Z) - Test3Before.Z };
				const float Test3FloatValue = roundf((Test3Before.Y + Extents.Y) + ((abs(BoxCenter.Z + Extents.Z) - Test3Before.Z) * NumRowsGrid));

				FVector Test4Before = {370.000 ,-80.000 ,21.000};
				FVector Test4After = {BoxCenter.X, Test4Before.Y + Extents.Y, abs(BoxCenter.Z + Extents.Z) - Test4Before.Z };
				const float Test4FloatValue = roundf((Test4Before.Y + Extents.Y) + ((abs(BoxCenter.Z + Extents.Z) - Test4Before.Z) * NumRowsGrid));

				UE_LOG(LogTemp, Display, TEXT("Test1Before: %s Test1After: %s %f"), *Test1Before.ToString(), *Test1After.ToString(),Test1FloatValue);
				UE_LOG(LogTemp, Display, TEXT("Test2Before: %s Test2After: %s %f"), *Test2Before.ToString(), *Test2After.ToString(),Test1FloatValue2);
				UE_LOG(LogTemp, Display, TEXT("Test2Before: %s Test2After: %s %f"), *Test3Before.ToString(), *Test3After.ToString(),Test3FloatValue);
				UE_LOG(LogTemp, Display, TEXT("Test2Before: %s Test2After: %s %f"), *Test4Before.ToString(), *Test4After.ToString(),Test4FloatValue);
				
				FVector TopLeft = {BoxCenter.X, BoxCenter.Y - Extents.Y, BoxCenter.Z + Extents.Z};
				FVector BotRight = {BoxCenter.X, BoxCenter.Y + Extents.Y, BoxCenter.Z - Extents.Z};

				UE_LOG(LogTemp, Display, TEXT("BoxCenter: %s BoxExtent: %s"), *BoxCenter.ToString(), *Extents.ToString());
				UE_LOG(LogTemp, Display, TEXT("TopLeft: %s BotRight: %s"), *TopLeft.ToString(), *BotRight.ToString());*/