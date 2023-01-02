// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetSpawner.h"
#include "PlayerHUD.h"
#include "SphereTarget.h"
#include "DefaultGameInstance.h"
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
	LastSpawnLocation = FVector::ZeroVector;
	LastTargetScale = 1.f;
	ConsecutiveTargetsHit = 0;
}

void ATargetSpawner::BeginPlay()
{
	Super::BeginPlay();
	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	GI->RegisterTargetSpawner(this);
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
			OnTargetSpawn.Broadcast();

			/* LAST target's location and scale */
			LastSpawnLocation = SpawnLocation;
			LastTargetScale = TargetScale;

			if (LastSpawnLocation == BoxBounds.Origin)
			{
				LastTargetSpawnedCenter = true;
			}
			else
			{
				LastTargetSpawnedCenter = false;
			}
		}
	}

	/* Find the next target's location and scale */
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
			/* Setting the current target's scale that was previously calculated */
			SpawnTarget->SetActorScale3D(FVector(TargetScale, TargetScale, TargetScale));
			SpawnTarget->OnLifeSpanExpired.AddDynamic(this, &ATargetSpawner::OnTargetTimeout);

			/* Broadcast to GameModeActorBase that a target has spawned */
			OnTargetSpawn.Broadcast();

			/* LAST target's location and scale */
			LastSpawnLocation = SpawnLocation;
			LastTargetScale = TargetScale;
		}
	}
	/* Find the next target's location and scale */
	FindNextTargetProperties(LastSpawnLocation, LastTargetScale);
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
				const float NewTargetScale = GenerateRandomTargetScale();
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
		OnTargetSpawn.Broadcast();
	}
	if (GameModeActorStruct.IsSingleBeatMode == true)
	{
		SetShouldSpawn(false);
	}
}

void ATargetSpawner::FindNextTargetProperties(const FVector FLastSpawnLocation, const float LastTargetScaleValue)
{
	/* Insert sphere of CheckSpawnRadius radius into sphere array. If skipping, insert a blank */
	if (bSkipNextSpawn)
	{
		RecentSpawnBounds.Insert(FSphere(FVector(0, 0, 0), 1), 0);
		bSkipNextSpawn = false;
	}
	else
	{
		RecentSpawnBounds.Insert(FSphere(FLastSpawnLocation, LastTargetScaleValue * SphereTargetRadius), 0);
	}
	RecentSpawnBounds.SetNum(MaxNumberOfTargetsAtOnce);
	DrawDebugSphere(GetWorld(), FLastSpawnLocation, LastTargetScaleValue * SphereTargetRadius, 20, FColor::Blue, false,
	                MaxNumberOfTargetsAtOnce);

	/* set the scale before we find the location */
	TargetScale = GenerateRandomTargetScale();

	/* the radius of the sphere target to test for collisions against */
	const float CollisionSphereRadius = TargetScale * SphereTargetRadius + GameModeActorStruct.
		MinDistanceBetweenTargets;

	/* find location */
	SpawnLocation = FindNextTargetSpawnLocation(GameModeActorStruct.SpreadType, CollisionSphereRadius);
}

FVector ATargetSpawner::FindNextTargetSpawnLocation(const ESpreadType SpreadType, const float CollisionSphereRadius)
{
	/* SpawnLocation initially centered inside SpawnBox */
	FVector NewSpawnLocation = BoxBounds.Origin;
	
	/* If single beat needs to spawn at center, exit early */
	if (GameModeActorStruct.IsSingleBeatMode && !LastTargetSpawnedCenter)
	{
		return NewSpawnLocation;
	}

	/* counting how many times the loop failed to try to spawn a target */
	int OverloadProtect = 0;
	
	/* simulate intersection with spheres inside of RecentSpawnBounds */
	FSphere NewTarget = FSphere(NewSpawnLocation, CollisionSphereRadius);

	/* dynamic sizing variables */
	const FVector ScaledBoxExtent = GameModeActorStruct.BoxBounds * (1 - DynamicScaleFactor / 100.f * 0.5f);

	/* Change the size of the SpawnBox if dynamic */
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
		/* Loop through Recent sphere locations */
		for (i = 0; i < RecentSpawnBounds.Num(); i++)
		{
			if (!RecentSpawnBounds[i].Intersects(NewTarget))
			{
				continue;
			}
			OverloadProtect++;
			break;
		}
		/* If reached the last index of RecentSpawnBounds, it did not intersect any spheres, so it is safe to spawn */
		if (i == RecentSpawnBounds.Num())
		{
			return NewSpawnLocation;
		}
		/* Continue with new target locations, reset i so that it always loops through entire RecentSpawnBounds */
		NewSpawnLocation = GenerateRandomTargetLocation(SpreadType, ScaledBoxExtent);
		NewTarget = FSphere(NewSpawnLocation, CollisionSphereRadius);
		i = -1;
	}
	if (OverloadProtect > 50)
	{
		bSkipNextSpawn = true;
	}
	return NewSpawnLocation;
}

void ATargetSpawner::FindNextTrackingDirection()
{
	if (TrackingTarget)
	{
		LocationBeforeDirectionChange = TrackingTarget->GetActorLocation();
	}

	if (!IsValid(TrackingTarget))
	{
		// Initial tracking target spawn
		TrackingTarget = GetWorld()->SpawnActor<ASphereTarget>(ActorToSpawn, BoxBounds.Origin,
		                                                       SpawnBox->GetComponentRotation());
		Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->OnBeatTrackTargetSpawned.ExecuteIfBound(TrackingTarget);
		TrackingTarget->OnActorEndOverlap.AddDynamic(this, &ATargetSpawner::OnBeatTrackOverlapEnd);
		LocationBeforeDirectionChange = BoxBounds.Origin;

		// Broadcast to GameModeActorBase that a target has spawned
		OnTargetSpawn.Broadcast();
	}

	if (IsValid(TrackingTarget))
	{
		const float NewTargetScale = GenerateRandomTargetScale();
		TrackingTarget->SetActorScale3D(FVector(NewTargetScale, NewTargetScale, NewTargetScale));
		TrackingSpeed = FMath::FRandRange(GameModeActorStruct.MinTrackingSpeed, GameModeActorStruct.MaxTrackingSpeed);
		EndLocation = GenerateRandomTrackerLocation(LocationBeforeDirectionChange);
		TrackingDirection = UKismetMathLibrary::GetDirectionUnitVector(LocationBeforeDirectionChange, EndLocation);
	}
}

float ATargetSpawner::GenerateRandomTargetScale() const
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

void ATargetSpawner::OnTargetTimeout(const bool DidExpire, const float TimeAlive, const FVector Location)
{
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
		/* Only update player reaction time if the target did not expire */
		Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this))->GameModeActorBaseRef->
		                                                                     UpdatePlayerScores(TimeAlive);
		ConsecutiveTargetsHit++;
		OnStreakUpdate.Broadcast(ConsecutiveTargetsHit, Location);
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
	TargetScale = GenerateRandomTargetScale();

	/* Setting max targets at one time for the size of RecentSpawnLocations & RecentSpawnBounds. Only used for MultiBeat */
	MaxNumberOfTargetsAtOnce = ceil(GameModeActorStruct.TargetMaxLifeSpan / GameModeActorStruct.TargetSpawnCD);
	RecentSpawnBounds.Init(FSphere(FVector(0, 0, 0), 1), MaxNumberOfTargetsAtOnce);

	if (GameModeActorStruct.IsBeatGridMode)
	{
		InitBeatGrid();
	}
}
