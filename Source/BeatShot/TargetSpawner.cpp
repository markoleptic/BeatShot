// Fill out your copyright notice in the Description page of Project Settings.

#include "TargetSpawner.h"
#include <algorithm>
#include "PlayerHUD.h"
#include "SphereTarget.h"
#include "DefaultGameMode.h"
#include "Components/BoxComponent.h"
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
	BoxBounds = SpawnBox->CalcBounds(GetActorTransform());
}

void ATargetSpawner::Destroyed()
{
	Super::Destroyed();
	if (TrackingTarget)
	{
		TrackingTarget->Destroy();
	}
	if (SpawnedBeatGridTargets.Num() > 0)
	{
		for (ASphereTarget* Target : SpawnedBeatGridTargets)
		{
			Target->Destroy();
		}
	}
}

void ATargetSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsValid(TrackingTarget))
	{
		CurrentTrackerLocation = TrackingTarget->GetActorLocation();
		CurrentTrackerLocation += TrackingDirection * TrackingSpeed * DeltaTime;
		TrackingTarget->SetActorLocation(CurrentTrackerLocation);
	}

	if (!GameModeActorStruct.bMoveTargetsForward)
	{
		return;
	}

	for (const FActiveTargetStruct ActiveTarget : ActiveTargetArray)
	{
		if (ActiveTarget.ActiveTarget != nullptr)
		{
			MoveTargetForward(ActiveTarget.ActiveTarget, DeltaTime);
		}
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
		FindNextTrackingDirection();
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

void ATargetSpawner::SpawnSingleBeatTarget()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
	if (!bSkipNextSpawn)
	{
		if (ASphereTarget* SpawnTarget = Cast<ASphereTarget>(
			GetWorld()->SpawnActor(ActorToSpawn, &SpawnLocation, &FRotator::ZeroRotator, SpawnParams)))
		{
			/* Setting the current target's scale that was previously calculated */
			SpawnTarget->SetActorScale3D(FVector(TargetScale, TargetScale, TargetScale));
			SpawnTarget->OnLifeSpanExpired.AddDynamic(this, &ATargetSpawner::OnTargetTimeout);

			/* Broadcast to GameModeActorBase that a target has spawned */
			if (!Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->OnTargetSpawned.ExecuteIfBound())
			{
				UE_LOG(LogTemp, Display, TEXT("OnTargetSpawned not bound."));
			}

			if (SpawnLocation == BoxBounds.Origin)
			{
				LastTargetSpawnedCenter = true;
			}
			else
			{
				LastTargetSpawnedCenter = false;
			}
		}
	}

	/* Find the next target's location and scale using the most recently spawned target's data */
	FindNextTargetProperties(SpawnLocation, TargetScale);

	/* Don't continue spawning for SingleBeat */
	SetShouldSpawn(false);
}

void ATargetSpawner::SpawnMultiBeatTarget()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
	if (!bSkipNextSpawn)
	{
		if (ASphereTarget* SpawnTarget = Cast<ASphereTarget>(
			GetWorld()->SpawnActor(ActorToSpawn, &SpawnLocation, &FRotator::ZeroRotator, SpawnParams)))
		{
			ActiveTargetArray.Add(FActiveTargetStruct(SpawnTarget));

			/* Setting the current target's scale that was previously calculated */
			SpawnTarget->SetActorScale3D(FVector(TargetScale, TargetScale, TargetScale));
			SpawnTarget->OnLifeSpanExpired.AddDynamic(this, &ATargetSpawner::OnTargetTimeout);

			/* Broadcast to GameModeActorBase that a target has spawned */
			if (!Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->OnTargetSpawned.ExecuteIfBound())
			{
				UE_LOG(LogTemp, Display, TEXT("OnTargetSpawned not bound."));
			}
		}
	}
	/* Find the next target's location and scale */
	FindNextTargetProperties(SpawnLocation, TargetScale);
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

	const float HalfWidth = round(BoxBounds.BoxExtent.Y);
	const float HalfHeight = round(BoxBounds.BoxExtent.Z);
	const float NumTargets = (sqrt(GameModeActorStruct.BeatGridSize));
	FVector BeatGridSpawnLocation = BoxBounds.Origin;
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
				const float NewTargetScale = GenerateTargetScale();
				BeatGridTarget->SetActorScale3D(FVector(NewTargetScale, NewTargetScale, NewTargetScale));
				SpawnedBeatGridTargets.Add(BeatGridTarget);
			}
		}
		BeatGridSpawnLocation.Y = HStart;
		BeatGridSpawnLocation.Z -= BasicVSpacing;
	}
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
	/* only "spawn" target if it hasn't been destroyed by GameModeActorBase */
	if (ActiveBeatGridTarget)
	{
		/* notify GameModeActorBase that target has "spawned" */
		ActiveBeatGridTarget->StartBeatGridTimer(GameModeActorStruct.TargetMaxLifeSpan);
		if (!Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->OnTargetSpawned.ExecuteIfBound())
		{
			UE_LOG(LogTemp, Display, TEXT("OnTargetSpawned not bound."));
		}
		//OnTargetSpawn.Broadcast();
	}
	if (GameModeActorStruct.IsSingleBeatMode == true)
	{
		SetShouldSpawn(false);
	}
}

void ATargetSpawner::FindNextTargetProperties(const FVector LastSpawnLocation, const float LastTargetScale)
{
	/* Only add a recently spawned target to the SpawnArea and ActiveTargetArray if the last target spawn was not skipped */
	if (!bSkipNextSpawn)
	{
		const TArray<FIntPoint> BlockedPoints = SetSpawnAreaOccupied(ConvertLocationToPoint(LastSpawnLocation), LastTargetScale);
		/* Set the most recently spawned target's values in ActiveTargetArray */
		ActiveTargetArray.Last().BlockedSpawnPoints = BlockedPoints;
		ActiveTargetArray.Last().Center = ConvertLocationToPoint(LastSpawnLocation);
		ActiveTargetArray.Last().TargetScale = LastTargetScale;
	}

	TargetScale = GenerateTargetScale();
	
	/* Find location */
	SpawnLocation = FindNextTargetSpawnLocation(GameModeActorStruct.SpreadType, TargetScale);
}

FVector ATargetSpawner::FindNextTargetSpawnLocation(const ESpreadType SpreadType, const float NewTargetScale)
{
	/* Reset SkipNextSpawn */
	bSkipNextSpawn = false;

	/* The location to return, initially centered inside SpawnBox */
	FVector NewSpawnLocation = BoxBounds.Origin;

	/* If single beat needs to spawn at center, exit early */
	if (GameModeActorStruct.IsSingleBeatMode && !LastTargetSpawnedCenter)
	{
		return NewSpawnLocation;
	}

	/* The new value of the BoxExtent for use with dynamic spread types */
	const FVector ScaledBoxExtent = GameModeActorStruct.BoxBounds * (1 - DynamicScaleFactor / 100.f * 0.5f);

	/* Change the size of the SpawnBox if dynamic */
	if (SpreadType == ESpreadType::DynamicRandom ||
		SpreadType == ESpreadType::DynamicEdgeOnly)
	{
		SpawnBox->SetBoxExtent(ScaledBoxExtent);
		BoxBounds = SpawnBox->CalcBounds(GetActorTransform());
		DrawDebugBox(GetWorld(), BoxBounds.Origin, ScaledBoxExtent, FColor::Orange, false, 1);
	}

	const int32 XValueClip = abs(ScaledBoxExtent.Y - GameModeActorStruct.BoxBounds.Y);
	const int32 YValueClip = abs(ScaledBoxExtent.Z - GameModeActorStruct.BoxBounds.Z);
	const int32 XValueUpperClip = GameModeActorStruct.BoxBounds.Y * 2 - XValueClip;
	const int32 YValueUpperClip = GameModeActorStruct.BoxBounds.Z * 2 - YValueClip;
	int32 NumClips = 0;
	int32 NumUpperClips = 0;
	int32 NonClips = 0;

	std::vector<std::vector<int32>> TempSpawnArea = SpawnArea;
	
	for (FActiveTargetStruct TargetStruct : ActiveTargetArray)
	{
		if (TargetStruct.TargetScale < NewTargetScale)
		{
			
			TempSpawnArea = SetTempSpawnAreaOccupied(TargetStruct.Center, NewTargetScale, TempSpawnArea);
		}
	}
	
	/* Create an array of valid spawn locations */
	TArray<FIntPoint> OpenPoints;

	for (std::vector<std::vector<int32>>::iterator Row = TempSpawnArea.begin(); Row != TempSpawnArea.end(); ++Row)
	{
		for (std::vector<int32>::iterator Col = Row->begin(); Col != Row->end(); ++Col)
		{
			const int32 X = std::distance(TempSpawnArea.begin(), Row);
			const int32 Y = std::distance(Row->begin(), Col);

			/** The min size of BoxExtend is half the BoxExtent */
			if (TempSpawnArea[X][Y] != 1)
			{
				if (X < XValueClip || Y < YValueClip)
				{
					TempSpawnArea[X][Y] = 2;
					NumClips++;
				}
				else if ((X > XValueUpperClip && X <= GameModeActorStruct.BoxBounds.Y * 2) || (Y > YValueUpperClip && Y
					<= GameModeActorStruct.BoxBounds.Z * 2))
				{
					TempSpawnArea[X][Y] = 2;
					NumUpperClips++;
				}
				else
				{
					NonClips++;
					TempSpawnArea[X][Y] = 0;
					OpenPoints.Add(FIntPoint(X, Y));
				}
			}
			else
			{
				NonClips++;
			}
		}
	}

	/*for (std::vector<std::vector<int32>>::iterator Row = SpawnArea.begin(); Row != SpawnArea.end(); ++Row)
	{
		const int32 X = std::distance(SpawnArea.begin(), Row);
		for (std::vector<int32>::iterator Col = Row->begin(); Col != Row->end(); ++Col)
		{
			const int32 Y = std::distance(Row->begin(), Col);

			/** The min size of BoxExtend is half the BoxExtent #1#
			if (SpawnArea[X][Y] != 1)
			{
				if (X < XValueClip || Y < YValueClip)
				{
					SpawnArea[X][Y] = 2;
					NumClips++;
				}
				else if ((X > XValueUpperClip && X <= GameModeActorStruct.BoxBounds.Y * 2) || (Y > YValueUpperClip && Y
					<= GameModeActorStruct.BoxBounds.Z * 2))
				{
					SpawnArea[X][Y] = 2;
					NumUpperClips++;
				}
				else
				{
					NonClips++;
					SpawnArea[X][Y] = 0;
					OpenPoints.Add(FIntPoint(X, Y));
				}
			}
			else
			{
				NonClips++;
			}
		}
	}*/
	
	UE_LOG(LogTemp, Display, TEXT("NumClips %d NumUpperClips %d NonClips %d"), NumClips, NumUpperClips, NonClips);
	UE_LOG(LogTemp, Display, TEXT("XValueClip: %d XValueUpperClip: %d YValueClip: %d YValueUpperClip: %d"), XValueClip,XValueUpperClip, YValueClip,YValueUpperClip);
	UE_LOG(LogTemp, Display, TEXT("Number of Valid Spawn Points: %d "), OpenPoints.Num());
	
	if (OpenPoints.Num() == 0)
	{
		bSkipNextSpawn = true;
		return NewSpawnLocation;
	}

	/* Spawn at BoxBounds origin whenever possible */
	//if (OpenPoints.Contains(FIntPoint(GameModeActorStruct.BoxBounds.Y, GameModeActorStruct.BoxBounds.Z)))
	//{
	//	NewSpawnLocation = BoxBounds.Origin;
	//}
	//else
	//{
		const int32 RandomPoint = UKismetMathLibrary::RandomFloatInRange(0, OpenPoints.Num() - 1);
		NewSpawnLocation = ConvertPointToLocation(OpenPoints[RandomPoint]);
	//}
	return NewSpawnLocation;
}

/*FVector ATargetSpawner::FindNextTargetSpawnLocation(const ESpreadType SpreadType, const float CollisionSphereRadius)
{
	/* SpawnLocation initially centered inside SpawnBox #1#
	FVector NewSpawnLocation = BoxBounds.Origin;

	/* If single beat needs to spawn at center, exit early #1#
	if (GameModeActorStruct.IsSingleBeatMode && !LastTargetSpawnedCenter)
	{
		return NewSpawnLocation;
	}

	/* counting how many times the loop failed to try to spawn a target #1#
	int OverloadProtect = 0;

	/* simulate intersection with spheres inside of RecentSpawnBounds #1#
	FSphere NewTarget = FSphere(NewSpawnLocation, CollisionSphereRadius);

	/* dynamic sizing variables #1#
	const FVector ScaledBoxExtent = GameModeActorStruct.BoxBounds * (1 - DynamicScaleFactor / 100.f * 0.5f);

	/* Change the size of the SpawnBox if dynamic #1#
	if (SpreadType == ESpreadType::DynamicRandom ||
		SpreadType == ESpreadType::DynamicEdgeOnly)
	{
		SpawnBox->SetBoxExtent(ScaledBoxExtent);
		BoxBounds = SpawnBox->CalcBounds(GetActorTransform());
		DrawDebugBox(GetWorld(), BoxBounds.Origin, ScaledBoxExtent, FColor::Orange, false, 1);
	}

	int i = 0;
	while (i < RecentSpawnBounds.Num() && OverloadProtect <= 50)
	{
		/* Loop through Recent sphere locations #1#
		for (i = 0; i < RecentSpawnBounds.Num(); i++)
		{
			if (!RecentSpawnBounds[i].Intersects(NewTarget))
			{
				continue;
			}
			OverloadProtect++;
			break;
		}
		/* If reached the last index of RecentSpawnBounds, it did not intersect any spheres, so it is safe to spawn #1#
		if (i == RecentSpawnBounds.Num())
		{
			return NewSpawnLocation;
		}
		/* Continue with new target locations, reset i so that it always loops through entire RecentSpawnBounds #1#
		NewSpawnLocation = GenerateRandomTargetLocation(SpreadType, ScaledBoxExtent);
		NewTarget = FSphere(NewSpawnLocation, CollisionSphereRadius);
		i = -1;
	}
	if (OverloadProtect > 50)
	{
		bSkipNextSpawn = true;
	}
	return NewSpawnLocation;
}*/

void ATargetSpawner::FindNextTrackingDirection()
{
	if (!TrackingTarget)
	{
		/** Initial tracking target spawn */
		TrackingTarget = GetWorld()->SpawnActor<ASphereTarget>(ActorToSpawn, BoxBounds.Origin,
		                                                       SpawnBox->GetComponentRotation());
		TrackingTarget->OnActorEndOverlap.AddDynamic(this, &ATargetSpawner::OnBeatTrackOverlapEnd);
		LocationBeforeDirectionChange = BoxBounds.Origin;

		/** Broadcast to GameModeActorBase and DefaultCharacter that a BeatTrack target has spawned */
		Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->OnBeatTrackTargetSpawned.Broadcast(
			TrackingTarget);
	}
	if (TrackingTarget)
	{
		LocationBeforeDirectionChange = TrackingTarget->GetActorLocation();
		const float NewTargetScale = GenerateTargetScale();
		TrackingTarget->SetActorScale3D(FVector(NewTargetScale, NewTargetScale, NewTargetScale));
		TrackingSpeed = FMath::FRandRange(GameModeActorStruct.MinTrackingSpeed, GameModeActorStruct.MaxTrackingSpeed);
		EndLocation = GenerateRandomTrackerLocation(LocationBeforeDirectionChange);
		TrackingDirection = UKismetMathLibrary::GetDirectionUnitVector(LocationBeforeDirectionChange, EndLocation);
	}
}

float ATargetSpawner::GenerateTargetScale() const
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

FVector ATargetSpawner::GenerateRandomTargetLocation(const ESpreadType SpreadType, const FVector& ScaledBoxExtent) const
{
	FVector NewSpawnLocation = BoxBounds.Origin;
	/* Non-dynamic */
	if (SpreadType != ESpreadType::DynamicRandom && SpreadType != ESpreadType::DynamicEdgeOnly)
	{
		NewSpawnLocation.Y += -BoxBounds.BoxExtent.Y + 2 * BoxBounds.BoxExtent.Y * FMath::FRand();
		NewSpawnLocation.Z += -BoxBounds.BoxExtent.Z + 2 * BoxBounds.BoxExtent.Z * FMath::FRand();
		return NewSpawnLocation;
	}

	/* DynamicRandom */
	if (SpreadType == ESpreadType::DynamicRandom)
	{
		return UKismetMathLibrary::RandomPointInBoundingBox(
			BoxBounds.Origin, BoxBounds.BoxExtent);
	}

	/* DynamicEdgeOnly */
	FVector Offset = FVector();
	switch (UKismetMathLibrary::RandomIntegerInRange(0, 3))
	{
	case 0: //top
		{
			/* Y is left-right Z is up-down */
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
}

FVector ATargetSpawner::GenerateRandomTrackerLocation(const FVector LocationBeforeChange) const
{
	/* try to spawn at origin if available */
	FVector LocationToReturn = BoxBounds.Origin;

	/* So we don't get stuck in infinite loop */
	int OverloadProtect = 0;
	bool IsInsideBox = true;
	while (IsInsideBox)
	{
		if (OverloadProtect > 20)
		{
			break;
		}
		LocationToReturn = UKismetMathLibrary::RandomPointInBoundingBox(BoxBounds.Origin, BoxBounds.BoxExtent);
		if (UKismetMathLibrary::IsPointInBox(LocationBeforeChange +
		                                     UKismetMathLibrary::GetDirectionUnitVector(
			                                     LocationBeforeChange, LocationToReturn) *
		                                     TrackingSpeed * GameModeActorStruct.TargetSpawnCD,
		                                     BoxBounds.Origin, BoxBounds.BoxExtent))
		{
			IsInsideBox = false;
		}
		OverloadProtect++;
	}
	return LocationToReturn;
}

void ATargetSpawner::OnTargetTimeout(const bool DidExpire, const float TimeAlive, ASphereTarget* DestroyedTarget)
{
	FVector Location;
	if (DestroyedTarget)
	{
		FVector DestroyedTargetLocation = DestroyedTarget->GetActorLocation();
		const FVector DestroyedTargetScale = DestroyedTarget->GetActorScale3D();
		Location = {
			DestroyedTargetLocation.X,
			DestroyedTargetLocation.Y,
			DestroyedTargetLocation.Z +
			SphereTargetRadius * DestroyedTargetScale.Z
		};
	}
	
	if (const int Index = ActiveTargetArray.Find(FActiveTargetStruct(DestroyedTarget)); Index != INDEX_NONE)
	{
		for (const FIntPoint Point : ActiveTargetArray[Index].BlockedSpawnPoints)
		{
			/* Reset all occupied points back to unoccupied */
			SpawnArea[Point.X][Point.Y] = 0;
		}
		/* Remove the ActiveTargetStruct from ActiveTargetArray */
		ActiveTargetArray.RemoveAtSwap(Index);
	}
	ActiveTargetArray.Shrink();
	
	if (GameModeActorStruct.IsSingleBeatMode)
	{
		SetShouldSpawn(true);
	}
	if (DidExpire)
	{
		ConsecutiveTargetsHit = 0;
		if (GameModeActorStruct.SpreadType == ESpreadType::None)
		{
			return;
		}
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
		const ADefaultGameMode* GameMode = Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		/* Only update player reaction time if the target did not expire */
		if (!GameMode->OnTargetDestroyed.ExecuteIfBound(TimeAlive))
		{
			UE_LOG(LogTemp, Display, TEXT("OnTargetDestroyed not bound."));
		}
		GameMode->OnStreakUpdate.Broadcast(ConsecutiveTargetsHit, Location);

		if (GameModeActorStruct.SpreadType == ESpreadType::None)
		{
			return;
		}
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
}

void ATargetSpawner::OnBeatTrackOverlapEnd(AActor* OverlappedActor, AActor* OtherActor)
{
	/* Reverse direction if not longer overlapping spawn box */
	if (Cast<ATargetSpawner>(OverlappedActor) || Cast<ASphereTarget>(OverlappedActor) &&
		Cast<ASphereTarget>(OtherActor) || Cast<ATargetSpawner>(OtherActor))
	{
		TrackingDirection = TrackingDirection * -1;
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
	FVector CenterOfSpawnBox = {3590.f, 0.f, 160.f};
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
	BoxBounds = SpawnBox->CalcBounds(GetActorTransform());

	/* Initial target spawn location */
	SpawnLocation = BoxBounds.Origin;

	/* Initial target size */
	TargetScale = GenerateTargetScale();

	/* Setting max targets at one time for the size of RecentSpawnLocations & RecentSpawnBounds. Only used for MultiBeat */
	MaxNumberOfTargetsAtOnce = ceil(GameModeActorStruct.TargetMaxLifeSpan / GameModeActorStruct.TargetSpawnCD);
	RecentSpawnBounds.Init(FSphere(FVector(0, 0, 0), 1), MaxNumberOfTargetsAtOnce);

	if (GameModeActorStruct.IsBeatGridMode)
	{
		InitBeatGrid();
	}

	SpawnArea = {
		static_cast<unsigned long>(GameModeActorStruct.BoxBounds.Y * 2),
		std::vector(static_cast<unsigned long>(GameModeActorStruct.BoxBounds.Z) * 2, 0)
	};
}

void ATargetSpawner::MoveTargetForward(ASphereTarget* SpawnTarget, float DeltaTime) const
{
	const FVector Loc = SpawnTarget->GetActorLocation();
	const FVector NewLoc = FVector(Loc.X - GameModeActorStruct.MoveForwardDistance, Loc.Y, Loc.Z);
	SpawnTarget->SetActorLocation(
		UKismetMathLibrary::VInterpTo(Loc, NewLoc, DeltaTime, 1 / GameModeActorStruct.TargetMaxLifeSpan));
}

TArray<FIntPoint> ATargetSpawner::SetSpawnAreaOccupied(const FIntPoint Center, const float Scale)
{
	/* Multiply by 2 so that any point outside of circle is a valid spawn location */
	const int32 Radius = (Scale * SphereTargetRadius * 2) + GameModeActorStruct.MinDistanceBetweenTargets;
	const int32 LeftSquare = Center.X - Radius;
	const int32 RightSquare = Center.X + Radius;
	const int32 BottomSquare = Center.Y - Radius;
	const int32 TopSquare = Center.Y + Radius;

	/* An array that represents invalid spawn points */
	TArray<FIntPoint> BlockedPoints;
	/* Iterate over a square section with length & width equal to sphere diameter */
	for (int x = LeftSquare; x < RightSquare; x++)
	{
		for (int y = BottomSquare; y < TopSquare; y++)
		{
			/* Only add to BlockedPoints if inside circle radius */
			if ((x - Center.X) * (x - Center.X) + (y - Center.Y) * (y - Center.Y) <=
				Radius * Radius)
			{
				/* Don't add points that are outside of the box extent (like when near an edge or corner) */
				if (x > 0 && y > 0 && (x < BoxBounds.BoxExtent.Y * 2) && (y < BoxBounds.BoxExtent.Z * 2))
				{
					SpawnArea[x][y] = 1;
					BlockedPoints.Add(FIntPoint(x, y));
				}
			}
		}
	}
	return BlockedPoints;
}

std::vector<std::vector<int32>> ATargetSpawner::SetTempSpawnAreaOccupied(const FIntPoint Center, const float Scale, std::vector<std::vector<int32>>& TempSpawnArea)
{
	/* Multiply by 2 so that any point outside of circle is a valid spawn location */
	const int32 Radius = (Scale * SphereTargetRadius * 2) + GameModeActorStruct.MinDistanceBetweenTargets;
	const int32 LeftSquare = Center.X - Radius;
	const int32 RightSquare = Center.X + Radius;
	const int32 BottomSquare = Center.Y - Radius;
	const int32 TopSquare = Center.Y + Radius;
	
	/* Iterate over a square section with length & width equal to sphere diameter */
	for (int x = LeftSquare; x < RightSquare; x++)
	{
		for (int y = BottomSquare; y < TopSquare; y++)
		{
			/* Only add to BlockedPoints if inside circle radius */
			if ((x - Center.X) * (x - Center.X) + (y - Center.Y) * (y - Center.Y) <=
				Radius * Radius)
			{
				/* Don't add points that are outside of the box extent (like when near an edge or corner) */
				if (x > 0 && y > 0 && (x < BoxBounds.BoxExtent.Y * 2) && (y < BoxBounds.BoxExtent.Z * 2))
				{
					TempSpawnArea[x][y] = 1;
				}
			}
		}
	}
	return TempSpawnArea;
}

FIntPoint ATargetSpawner::ConvertLocationToPoint(const FVector Location) const
{
	const int32 CenterXValue = Location.Y + GameModeActorStruct.BoxBounds.Y;
	const int32 CenterYValue = Location.Z + GameModeActorStruct.BoxBounds.Z - BoxBounds.Origin.Z;
	return FIntPoint(CenterXValue,CenterYValue);
}

FVector ATargetSpawner::ConvertPointToLocation(const FIntPoint Point) const
{
	const int32 Y = Point.X - GameModeActorStruct.BoxBounds.Y;
	const int32 Z = Point.Y - GameModeActorStruct.BoxBounds.Z + BoxBounds.Origin.Z;
	return FVector(BoxBounds.Origin.X, Y, Z);
}
