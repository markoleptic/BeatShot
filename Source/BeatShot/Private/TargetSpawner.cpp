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
	if (!SpawnCounter.IsEmpty())
	{
		SpawnCounter.Sort();
		FString file = FPaths::ProjectDir();
		file.Append(TEXT("AccuracyMatrix.csv"));
		FString StringToWrite;

		for (FVectorCounter Counter : SpawnCounter)
		{
			if (Counter.TotalSpawns != 0)
			{
				StringToWrite.Append(FString::SanitizeFloat(Counter.Point.Y) + "," + FString::SanitizeFloat(Counter.Point.Z) + ",");
				StringToWrite.Append(FString::FromInt(Counter.TotalSpawns) + ",");
				StringToWrite.Append(FString::FromInt(Counter.TotalHits));
				StringToWrite.Append("\n");
			}
		}
		FFileHelper::SaveStringToFile(StringToWrite, *file);
		SpawnCounter.Empty();
		SpawnCounter.Shrink();
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
		GameModeActorStruct.BoxBounds.Y = 2048;
		GameModeActorStruct.BoxBounds.Z = 1024;
		SpawnMemoryScale = 1.f / 128.f;
	}
	else
	{
		SpawnMemoryScale = 0.05f;
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
	NumRowsGrid = GetBoxExtents_Scaled_Static().Y * 2 + 1;
	NumColsGrids = GetBoxExtents_Scaled_Static().Z * 2 + 1;
	InitializeSpawnCounter();

	if (GIsEditor)
	{
		VisualGrid = GetWorld()->SpawnActor<AVisualGrid>(VisualGridClass, FVector(3990, 0, 1500),
		                                                 FRotator::ZeroRotator);
		VisualGrid->CreateGrid(NumRowsGrid, NumColsGrids);
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
			RecentTargets.Emplace(FRecentTarget(SpawnTarget->Guid,
			                                    GetOverlappingPoints(SpawnTarget->GetActorLocation(), TargetScale),
			                                    TargetScale,
			                                    SpawnTarget->GetActorLocation()));
			if (GIsEditor)
			{
				UpdateVisualGrid(SpawnTarget->GetActorLocation(), 1);
			}
		}
	}
	FindNextTargetProperties();
}

void ATargetSpawner::SpawnSingleBeatTarget()
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

			RecentTargets.Emplace(FRecentTarget(SpawnTarget->Guid,
			                                    GetOverlappingPoints(SpawnTarget->GetActorLocation(), TargetScale),
			                                    TargetScale,
			                                    SpawnTarget->GetActorLocation()));

			Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->OnTargetSpawned.Broadcast(SpawnTarget);
			if (GIsEditor)
			{
				UpdateVisualGrid(SpawnTarget->GetActorLocation(), 1);
			}
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
		Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->OnTargetSpawned.Broadcast(
			ActiveBeatGridTarget);
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
	if (const int32 Index = SpawnCounter.Find(FVectorCounter(DestroyedTarget->GetActorLocation())); Index != INDEX_NONE)
	{
		SpawnCounter[Index].TotalSpawns++;
		if (!DidExpire)
		{
			SpawnCounter[Index].TotalHits++;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NOTFOUND!!! %s"), *DestroyedTarget->GetActorLocation().ToString());
	}
	ActiveTargetArray.Remove(DestroyedTarget);
	ActiveTargetArray.Shrink();
	FTimerHandle TimerHandle;
	RemoveFromRecentDelegate.BindUFunction(this, FName("RemoveFromRecentTargets"), DestroyedTarget->Guid);
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
	if (GameModeActorStruct.IsSingleBeatMode && !LastTargetSpawnedCenter)
	{
		return GetBoxOrigin_Unscaled();
	}
	/* Change the BoxExtent of the SpawnBox if dynamic */
	if (IsDynamicSpreadType(SpreadType))
	{
		SetBoxExtents_Dynamic();
	}

	TArray<FVector> Open = GetValidSpawnLocations(NewTargetScale);
	if (Open.IsEmpty())
	{
		bSkipNextSpawn = true;
		return GetBoxOrigin_Unscaled();
	}
	
	if (GameModeActorStruct.CustomGameModeName.Equals("RLProject"))
	{
		const int32 RandomPoint = UKismetMathLibrary::RandomIntegerInRange(0, Open.Num() - 1);
		return Open[RandomPoint];
	}
	const FVector Origin = GetBoxOrigin_Unscaled();
	if (Open.ContainsByPredicate([&Origin](const FVector Element)
	{
		if (Element.Equals(Origin))
		{
			return true;
		}
		return false;
	}))
	{
		return GetBoxOrigin_Unscaled();
	}
	const int32 RandomPoint = UKismetMathLibrary::RandomIntegerInRange(0, Open.Num() - 1);
	return Open[RandomPoint];
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
		LocationToReturn = UKismetMathLibrary::RandomPointInBoundingBox(
			SpawnBox->Bounds.Origin, SpawnBox->Bounds.BoxExtent);
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

void ATargetSpawner::RemoveFromRecentTargets(const FGuid GuidToRemove)
{
	if (GIsEditor)
	{
		for (FRecentTarget Struct : GetRecentTargets())
		{
			if (Struct.TargetGuid == GuidToRemove)
			{
				VisualGrid->SetCustomDataValues(Struct.OverlappingPoints,
				                                GetBoxOrigin_Unscaled(), GetBoxExtents_Unscaled_Static(),
				                                SpawnMemoryScale, 0);
				break;
			}
		}
	}
	RecentTargets.Remove(FRecentTarget(GuidToRemove));
	RecentTargets.Shrink();
}

TArray<FVector> ATargetSpawner::GetValidSpawnLocations(const float Scale) const
{
	const FVector BoxCenter = GetBoxOrigin_Scaled();
	const FVector Extents = GetBoxExtents_Scaled_Current();
	const float LowerBound_Z = BoxCenter.Z - Extents.Z;
	const float UpperBound_Z = BoxCenter.Z + Extents.Z;
	const float LowerBound_Y = -Extents.Y;
	const float UpperBound_Y = Extents.Y;
	TArray<FRecentTarget> RecentTargetsCopy = GetRecentTargets();
	/* Resizing Overlapping Points if necessary */
	for (FRecentTarget Struct : RecentTargetsCopy)
	{
		if (Struct.TargetScale < Scale)
		{
			Struct.OverlappingPoints = GetOverlappingPoints(Struct.CenterVector, Scale);
		}
	}

	TArray<FVector> AllPoints;
	/* Populate AllPoints with all possible spawn locations at current time */
	if (GameModeActorStruct.SpreadType == ESpreadType::DynamicEdgeOnly)
	{
		for (float Y = LowerBound_Y; Y <= UpperBound_Y; Y++)
		{
			AllPoints.AddUnique(FVector(BoxCenter.X, Y, LowerBound_Z));
			AllPoints.AddUnique(FVector(BoxCenter.X, Y, UpperBound_Z));
		}
		for (float Z = LowerBound_Z; Z <= UpperBound_Z; Z++)
		{
			AllPoints.AddUnique(FVector(BoxCenter.X, LowerBound_Y, Z));
			AllPoints.AddUnique(FVector(BoxCenter.X, LowerBound_Y, Z));
		}
	}
	else
	{
		for (float Y = LowerBound_Y; Y <= UpperBound_Y; Y++)
		{
			for (float Z = LowerBound_Z; Z <= UpperBound_Z; Z++)
			{
				
				//AllPoints.Emplace(FVector(BoxCenter.X, Y, Z));
				AllPoints.Emplace(FVector(roundf(BoxCenter.X * (1/SpawnMemoryScale)), roundf(Y * (1/SpawnMemoryScale)), roundf(Z* (1/SpawnMemoryScale))));
			}
		}
	}

	/* Remove any overlapping points from AllPoints */
	int Count = 0;
	for (FRecentTarget Struct : RecentTargetsCopy)
	{
		for (FVector Vector : Struct.OverlappingPoints)
		{
			Count += AllPoints.Remove(Vector);
		}
	}
	AllPoints.Shrink();
	UE_LOG(LogTemp, Display, TEXT("NumRemoved: %d"), Count);
	UE_LOG(LogTemp, Display, TEXT("AllPoints size: %d"), AllPoints.Num());
	return AllPoints;
}

TArray<FRecentTarget> ATargetSpawner::GetRecentTargets() const
{
	return RecentTargets;
}

FVector ATargetSpawner::GetBoxExtents_Unscaled_Static() const
{
	return FVector(0, GameModeActorStruct.BoxBounds.Y, GameModeActorStruct.BoxBounds.Z);
}

FVector ATargetSpawner::GetBoxExtents_Scaled_Current() const
{
	return SpawnBox->Bounds.BoxExtent * SpawnMemoryScale;
}

FVector ATargetSpawner::GetBoxExtents_Scaled_Static() const
{
	return SpawnMemoryScale * GameModeActorStruct.BoxBounds;
}

FVector ATargetSpawner::GetBoxOrigin_Unscaled() const
{
	return SpawnBox->Bounds.Origin;
}

FVector ATargetSpawner::GetBoxOrigin_Scaled() const
{
	return SpawnBox->Bounds.Origin * SpawnMemoryScale;
}

void ATargetSpawner::SetBoxExtents_Dynamic() const
{
	const float ScaleFactor = 1 - DynamicScaleFactor / 100.f * 0.5f;
	const float Y = roundf(GetBoxExtents_Unscaled_Static().Y * ScaleFactor);
	const float Z = roundf(GetBoxExtents_Unscaled_Static().Z * ScaleFactor);
	SpawnBox->SetBoxExtent(FVector(0, Y, Z));
	DrawDebugBox(GetWorld(), SpawnBox->Bounds.Origin, SpawnBox->Bounds.BoxExtent, FColor::Orange, false, 1);
}

void ATargetSpawner::UpdateVisualGrid(const FVector Center, const int32 CustomDataValue) const
{
	VisualGrid->SetCustomDataValues(GetOverlappingPoints(Center, TargetScale),
	                                SpawnBox->Bounds.Origin, GameModeActorStruct.BoxBounds, SpawnMemoryScale,
	                                CustomDataValue);
}

TArray<FVector> ATargetSpawner::GetOverlappingPoints(const FVector Center, const float Scale) const
{
	const float Radius = (Scale * SphereTargetRadius * 2 + GameModeActorStruct.MinDistanceBetweenTargets);
	FSphere Sphere = FSphere(Center, Radius);
	const FTransform SpawnMemoryTransform = FTransform(FRotator::ZeroRotator, FVector::ZeroVector,
	                                                   FVector(SpawnMemoryScale));
	Sphere = Sphere.TransformBy(SpawnMemoryTransform);
	const FVector BoxCenter = GetBoxOrigin_Scaled();
	const FVector Extents = GetBoxExtents_Scaled_Static();

	/* Create a square, and clamp the square inside of the box bounds */
	
	TArray<FVector> AllPoints;
	const float LowerBound_Z = BoxCenter.Z - Extents.Z;
	const float UpperBound_Z = BoxCenter.Z + Extents.Z;
	const float LowerBound_Y = -Extents.Y;
	const float UpperBound_Y = Extents.Y;
	TArray<FVector> BlockedPoints;
	for (float Y = LowerBound_Y; Y <= UpperBound_Y; Y++)
	{
		for (float Z = LowerBound_Z; Z <= UpperBound_Z; Z++)
		{
			if (FVector Loc = FVector(BoxCenter.X, Y, Z); Sphere.IsInside(Loc))
			{
				
				BlockedPoints.AddUnique(FVector(roundf(BoxCenter.X * (1/SpawnMemoryScale)), roundf(Loc.Y * (1/SpawnMemoryScale)), roundf(Loc.Z * (1/SpawnMemoryScale))));
			}
		}
	}
	return BlockedPoints;
	// const float ScaledZ = Center.Z * SpawnMemoryScale;
	// const float ScaledY = Center.Y * SpawnMemoryScale;
	/*const float Left = roundf(FMath::Clamp(ScaledY - Radius, -Extents.Y, Extents.Y));
	const float Right = roundf(FMath::Clamp(ScaledY + Radius, -Extents.Y, Extents.Y));
	const float Bottom = roundf(FMath::Clamp(ScaledZ - Radius, BoxCenter.Z - Extents.Z, BoxCenter.Z + Extents.Z));
	const float Top = roundf(FMath::Clamp(ScaledZ + Radius, BoxCenter.Z - Extents.Z, BoxCenter.Z + Extents.Z));*/
	/* Iterate over a square section with length & width equal to sphere diameter */
	/*for (float X = Left; X <= Right; X++)
	{
		for (float Y = Bottom; Y <= Top; Y++)
		{
			if (FVector Loc = FVector(BoxCenter.X, X, Y); Sphere.IsInside(Loc))
			{
				//BlockedPoints.AddUnique(Loc);
				BlockedPoints.AddUnique(FVector(roundf(BoxCenter.X * (1/SpawnMemoryScale)), roundf(Loc.Y * (1/SpawnMemoryScale)), roundf(Loc.Z * (1/SpawnMemoryScale))));
				UE_LOG(LogTemp, Display, TEXT("BlockedPoint: %s"), *FVector(roundf(BoxCenter.X * (1/SpawnMemoryScale)), roundf(Loc.Y * (1/SpawnMemoryScale)), roundf(Loc.Z * (1/SpawnMemoryScale))).ToString());
			}
		}
	}*/
	//UE_LOG(LogTemp, Display, TEXT("%f %f %f %f %f %f"), Vert, Horiz, LowerBoundVert, UpperBoundVert, LowerBoundHoriz, UpperBoundHoriz);
	//UE_LOG(LogTemp, Display, TEXT("%f %f %f %f"), LeftSquare, RightSquare, BottomSquare, TopSquare);
	//UE_LOG(LogTemp, Display, TEXT("BlockedPoints: %d"), BlockedPoints.Num());
}

void ATargetSpawner::InitializeSpawnCounter()
{
	const FVector BoxCenter = GetBoxOrigin_Unscaled();
	const FVector Extents = GetBoxExtents_Unscaled_Static();
	const float Left = BoxCenter.Y - Extents.Y;
	const float Right = BoxCenter.Y + Extents.Y;
	const float Bottom = BoxCenter.Z - Extents.Z;
	const float Top = BoxCenter.Z + Extents.Z;
	for (int X = Left; X <= Right; X++)
	{
		for (int Y = Bottom; Y <= Top; Y++)
		{
			SpawnCounter.Emplace(FVectorCounter(FVector(BoxCenter.X, X, Y)));
		}
	}
	UE_LOG(LogTemp, Display, TEXT("SpawnCounterSize: %d %llu"), SpawnCounter.Num(), SpawnCounter.GetAllocatedSize());
}

FVector ATargetSpawner::ClampInsideBox(FVector VectorToClamp) const
{
	//const float Y = roundf(FMath::Clamp(VectorToClamp.Y, -GetBoxExtents_Unscaled_Static().Y, GetBoxExtents_Unscaled_Static().Y));
	//const float Z = roundf(FMath::Clamp(VectorToClamp.Z, GetBoxOrigin_Unscaled().Z - GetBoxExtents_Unscaled_Static().Z, GetBoxOrigin_Unscaled().Z + GetBoxExtents_Unscaled_Static().Z));
	const float Y = roundf(VectorToClamp.Y);
	const float Z = roundf(VectorToClamp.Z);
	return FVector(GetBoxOrigin_Unscaled().X, Y, Z);
}
