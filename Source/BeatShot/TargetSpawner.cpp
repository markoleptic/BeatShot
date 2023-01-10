// Fill out your copyright notice in the Description page of Project Settings.

#include "TargetSpawner.h"
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

	/* Set how precise the 2D representation of SpawnArea should be */
	if (GameModeActorStruct.BoxBounds.Y * GameModeActorStruct.BoxBounds.Z <= 250000)
	{
		SpawnAreaScale = 5;
	}
	else
	{
		SpawnAreaScale = 10;
	}

	/* Set new location & box extent */
	SpawnBox->SetRelativeLocation(CenterOfSpawnBox);
	SpawnBox->SetBoxExtent(GameModeActorStruct.BoxBounds);
	BoxBounds = SpawnBox->CalcBounds(GetActorTransform());

	/* Initial target spawn location */
	SpawnLocation = BoxBounds.Origin;

	/* Initial target size */
	TargetScale = GetNextTargetScale();
	
	if (GameModeActorStruct.IsBeatGridMode)
	{
		InitBeatGrid();
	}
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
				const float NewTargetScale = GetNextTargetScale();
				BeatGridTarget->SetActorScale3D(FVector(NewTargetScale, NewTargetScale, NewTargetScale));
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
			
			AddTargetToTargetArrays(SpawnTarget, SpawnLocation, TargetScale);

			/* Broadcast to GameModeActorBase that a target has spawned */
			if (!Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->OnTargetSpawned.ExecuteIfBound())
			{
				UE_LOG(LogTemp, Display, TEXT("OnTargetSpawned not bound."));
			}
		}
	}
	/* Find the next target's location and scale */
	FindNextTargetProperties();
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

			AddTargetToTargetArrays(SpawnTarget, SpawnLocation, TargetScale);

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

void ATargetSpawner::SetNewTrackingDirection()
{
	if (!BeatTrackTarget)
	{
		/** Initial tracking target spawn */
		BeatTrackTarget = GetWorld()->SpawnActor<ASphereTarget>(ActorToSpawn, BoxBounds.Origin,
															   SpawnBox->GetComponentRotation());
		BeatTrackTarget->OnActorEndOverlap.AddDynamic(this, &ATargetSpawner::OnBeatTrackOverlapEnd);
		LocationBeforeDirectionChange = BoxBounds.Origin;

		/** Broadcast to GameModeActorBase and DefaultCharacter that a BeatTrack target has spawned */
		Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->OnBeatTrackTargetSpawned.Broadcast(
			BeatTrackTarget);
	}
	if (BeatTrackTarget)
	{
		LocationBeforeDirectionChange = BeatTrackTarget->GetActorLocation();
		const float NewTargetScale = GetNextTargetScale();
		BeatTrackTarget->SetActorScale3D(FVector(NewTargetScale, NewTargetScale, NewTargetScale));
		BeatTrackTargetSpeed = FMath::FRandRange(GameModeActorStruct.MinTrackingSpeed, GameModeActorStruct.MaxTrackingSpeed);
		EndLocation = GetRandomBeatTrackLocation(LocationBeforeDirectionChange);
		BeatTrackTargetDirection = UKismetMathLibrary::GetDirectionUnitVector(LocationBeforeDirectionChange, EndLocation);
	}
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
	ActiveTargetArray.Remove(DestroyedTarget);
	ActiveTargetArray.Shrink();

	//UE_LOG(LogTemp, Display, TEXT("ActiveTargetArray size: %d"), ActiveTargetArray.Num());
	
	FTimerHandle TimerHandle;
	RemoveFromRecentDelegate.BindUFunction(this, FName("RemoveFromRecentTargetArray"), DestroyedTarget->Guid);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, RemoveFromRecentDelegate, GameModeActorStruct.TargetSpawnCD + 0.01, false);

	UE_LOG(LogTemp, Display, TEXT("RecentTargetArray size: %d"), RecentTargetArray.Num());

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
	bool bDoesContainOrigin = false;
	FVector NewSpawnLocation = BoxBounds.Origin;
	FVector BoxExtent = GameModeActorStruct.BoxBounds;

	if (GameModeActorStruct.IsSingleBeatMode && !LastTargetSpawnedCenter)
	{
		return NewSpawnLocation;
	}

	/* Change the BoxExtent of the SpawnBox if dynamic */
	if (SpreadType == ESpreadType::DynamicRandom || SpreadType == ESpreadType::DynamicEdgeOnly)
	{
		bIsDynamicSpreadType = true;
		const float BoxBoundsScaleFactor = (1 - DynamicScaleFactor / 100.f * 0.5f);
		BoxExtent = FVector(0, roundf(GameModeActorStruct.BoxBounds.Y * BoxBoundsScaleFactor),
		                    roundf(GameModeActorStruct.BoxBounds.Z * BoxBoundsScaleFactor));
		SpawnBox->SetBoxExtent(BoxExtent);
		BoxBounds = SpawnBox->CalcBounds(GetActorTransform());
		DrawDebugBox(GetWorld(), BoxBounds.Origin, BoxExtent, FColor::Orange, false, 1);
	}
	
	/* Spawn at BoxBounds origin whenever possible */
	for (FActiveTargetStruct Struct : GetRecentTargetArray())
	{
		if (Struct.Center == ConvertLocationToPoint(BoxBounds.Origin))
		{
			bDoesContainOrigin = true;
			break;
		}
	}
	if (!bDoesContainOrigin)
	{
		NewSpawnLocation = BoxBounds.Origin;
		return NewSpawnLocation;
	}

	/* Get the valid spawn points based on scale, BoxExtent, and SpreadType */
	TArray<FIntPoint> OpenPoints = GetValidSpawnPoints(NewTargetScale, BoxExtent, bIsDynamicSpreadType);
	if (OpenPoints.Num() == 0)
	{
		bSkipNextSpawn = true;
		return NewSpawnLocation;
	}

	/* Choose a random point */
	const int32 RandomPoint = UKismetMathLibrary::RandomFloatInRange(0, OpenPoints.Num() - 1);
	
	if (GameModeActorStruct.IsSingleBeatMode)
	{
		NewSpawnLocation = ConvertPointToLocationSingleBeat(OpenPoints[RandomPoint]);
		return NewSpawnLocation;
	}
	NewSpawnLocation = ConvertPointToLocation(OpenPoints[RandomPoint]);
	return NewSpawnLocation;
}

FVector ATargetSpawner::GetRandomBeatTrackLocation(const FVector& LocationBeforeChange) const
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
											 BeatTrackTargetSpeed * GameModeActorStruct.TargetSpawnCD,
											 BoxBounds.Origin, BoxBounds.BoxExtent))
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

void ATargetSpawner::AddTargetToTargetArrays(ASphereTarget* Target, const FVector LastSpawnLocation,
											 const float LastTargetScale)
{
	ActiveTargetArray.Emplace(Target);
	TArray<FActiveTargetStruct> CopyOfRecentTargetArray = GetRecentTargetArray();
	CopyOfRecentTargetArray.Emplace(FActiveTargetStruct(
		Target, GetCircleOfPoints(ConvertLocationToPoint(LastSpawnLocation), LastTargetScale), LastTargetScale,
		ConvertLocationToPoint(LastSpawnLocation)));
	RecentTargetArray = CopyOfRecentTargetArray;
}

void ATargetSpawner::RemoveFromRecentTargetArray(const FGuid GuidToRemove)
{
	TArray<FActiveTargetStruct> CopyOfRecentTargetArray = GetRecentTargetArray();
	CopyOfRecentTargetArray.Remove(FActiveTargetStruct(GuidToRemove));
	RecentTargetArray = CopyOfRecentTargetArray;
}

TArray<FIntPoint> ATargetSpawner::GetCircleOfPoints(const FIntPoint Center, const float Scale) const
{
	/* Multiply by 2 so that any point outside of circle is a valid spawn location */
	const int32 Radius = roundf(
		(Scale * SphereTargetRadius * 2 + GameModeActorStruct.MinDistanceBetweenTargets) / SpawnAreaScale);
	const int32 LeftSquare = FMath::Clamp(Center.X - Radius, 0,
										  roundf(GameModeActorStruct.BoxBounds.Y * 2 / SpawnAreaScale));
	const int32 RightSquare = FMath::Clamp(Center.X + Radius, 0,
										   roundf(GameModeActorStruct.BoxBounds.Y * 2 / SpawnAreaScale));
	const int32 BottomSquare = FMath::Clamp(Center.Y - Radius, 0,
											roundf(GameModeActorStruct.BoxBounds.Z * 2 / SpawnAreaScale));
	const int32 TopSquare = FMath::Clamp(Center.Y + Radius, 0,
										 roundf(GameModeActorStruct.BoxBounds.Z * 2 / SpawnAreaScale));

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
				BlockedPoints.Add(FIntPoint(x, y));
			}
		}
	}
	//const float CircleArea = 3.14159 * Radius * Radius;
	//UE_LOG(LogTemp, Display, TEXT("Circle Radius: %d Circle Area: %f BlockedPoints: %d, Center: %s"), Radius,
	//	   CircleArea, BlockedPoints.Num(), *Center.ToString());
	//UE_LOG(LogTemp, Display, TEXT("LeftSquare: %d RightSquare: %d BottomSquare: %d, TopSquare: %d"), LeftSquare,
	//	   RightSquare, BottomSquare, TopSquare);
	return BlockedPoints;
}

TArray<FIntPoint> ATargetSpawner::GetValidSpawnPoints(const float Scale, const FVector& BoxExtent,
													  const bool bIsDynamicSpreadType)
{
	/* An array of valid spawn locations to return */
	TArray<FIntPoint> OpenPoints;

	/* SingleBeat only cares about the current BoxExtent (Border) */
	if (GameModeActorStruct.IsSingleBeatMode)
	{
		const FIntPoint Border = FIntPoint(roundf(BoxExtent.Y * 2 / SpawnAreaScale), roundf(BoxExtent.Z * 2 / SpawnAreaScale));
		/* iterate through columns */
		for (int i = 0; i < Border.Y; i++)
		{
			OpenPoints.Add(FIntPoint(0, i));
			OpenPoints.Add(FIntPoint(Border.X, i));
		}
		/* iterate through rows */
		for (int i = 0; i < Border.X; i++)
		{
			OpenPoints.AddUnique(FIntPoint(i, 0));
			OpenPoints.AddUnique(FIntPoint(i, Border.Y));
		}
		return OpenPoints;
	}

	/* A 2D array representation of the space that the spawn area occupies */
	std::vector SpawnArea = {
		static_cast<unsigned long>(roundf(GameModeActorStruct.BoxBounds.Y * 2 / SpawnAreaScale)),
		std::vector(static_cast<unsigned long>(roundf(GameModeActorStruct.BoxBounds.Z * 2 / SpawnAreaScale)), 0)
	};
	
	/* Resizing the SpawnArea if Dynamic Spread type */
	if (bIsDynamicSpreadType)
	{
		SpawnArea = ResizeSpawnAreaBounds(SpawnArea, BoxExtent);
	}

	/* Resizing Blocked Points if necessary */
	for (FActiveTargetStruct Struct : GetRecentTargetArray())
	{
		if (Struct.TargetScale < Scale)
		{
			SpawnArea = ResizeCircleInSpawnArea(Struct.Center, Scale, SpawnArea);
		}
		for (const FIntPoint Point : Struct.BlockedSpawnPoints)
		{
			SpawnArea[Point.X][Point.Y] = 1;
		}
	}
	
	int32 Count = 0;
	/* Getting all valid spawn points */
	for (std::vector<std::vector<int32>>::iterator Row = SpawnArea.begin(); Row != SpawnArea.end(); ++Row)
	{
		for (std::vector<int32>::iterator Col = Row->begin(); Col != Row->end(); ++Col)
		{
			const int32 X = std::distance(SpawnArea.begin(), Row);
			const int32 Y = std::distance(Row->begin(), Col);
			if (SpawnArea[X][Y] == 0)
			{
				OpenPoints.Add(FIntPoint(X, Y));
			}
			Count++;
		}
	}
	
	UE_LOG(LogTemp, Display, TEXT("Count: %d "), Count);
	UE_LOG(LogTemp, Display, TEXT("Number of Valid Spawn Points: %d "), OpenPoints.Num());
	return OpenPoints;
}

TArray<FActiveTargetStruct> ATargetSpawner::GetRecentTargetArray()
{
	return RecentTargetArray;
}

std::vector<std::vector<int32>> ATargetSpawner::ResizeCircleInSpawnArea(
	const FIntPoint Center, const float Scale, std::vector<std::vector<int32>>& OutSpawnArea) const
{
	/* Multiply by 2 so that any point outside of circle is a valid spawn location */
	const int32 Radius = roundf((Scale * SphereTargetRadius * 2 + GameModeActorStruct.
		MinDistanceBetweenTargets) / SpawnAreaScale);
	const int32 LeftSquare = FMath::Clamp(Center.X - Radius, 0,
	                                      roundf(GameModeActorStruct.BoxBounds.Y * 2 / SpawnAreaScale));
	const int32 RightSquare = FMath::Clamp(Center.X + Radius, 0,
	                                       roundf(GameModeActorStruct.BoxBounds.Y * 2 / SpawnAreaScale));
	const int32 BottomSquare = FMath::Clamp(Center.Y - Radius, 0,
	                                        roundf(GameModeActorStruct.BoxBounds.Z * 2 / SpawnAreaScale));
	const int32 TopSquare = FMath::Clamp(Center.Y + Radius, 0,
	                                     roundf(GameModeActorStruct.BoxBounds.Z * 2 / SpawnAreaScale));

	/* Iterate over a square section with length & width equal to sphere diameter */
	for (int x = LeftSquare; x < RightSquare; x++)
	{
		for (int y = BottomSquare; y < TopSquare; y++)
		{
			/* Only add to BlockedPoints if inside circle radius */
			if ((x - Center.X) * (x - Center.X) + (y - Center.Y) * (y - Center.Y) <=
				Radius * Radius)
			{
				OutSpawnArea[x][y] = 1;
			}
		}
	}
	return OutSpawnArea;
}

std::vector<std::vector<int32>> ATargetSpawner::ResizeSpawnAreaBounds(std::vector<std::vector<int32>>& OutSpawnArea,
                                                                      const FVector& BoxExtent) const
{
	const int32 MaxXValue = roundf((GameModeActorStruct.BoxBounds.Y * 2) / SpawnAreaScale);
	const int32 MaxYValue = roundf((GameModeActorStruct.BoxBounds.Z * 2) / SpawnAreaScale);
	const int32 XValueClip = roundf(abs(BoxExtent.Y - GameModeActorStruct.BoxBounds.Y) / SpawnAreaScale);
	const int32 YValueClip = roundf(abs(BoxExtent.Z - GameModeActorStruct.BoxBounds.Z) / SpawnAreaScale);
	const int32 XValueUpperClip = MaxXValue - XValueClip;
	const int32 YValueUpperClip = MaxYValue - YValueClip;
	int32 NumClips = 0;

	// UE_LOG(LogTemp, Display, TEXT("MaxXValue: %d MaxYValue: %d %d %d %d %d"), MaxXValue, MaxYValue, XValueClip,
	//        YValueClip, XValueUpperClip, YValueUpperClip);
	for (std::vector<std::vector<int32>>::iterator Row = OutSpawnArea.begin(); Row != OutSpawnArea.end(); ++Row)
	{
		for (std::vector<int32>::iterator Col = Row->begin(); Col != Row->end(); ++Col)
		{
			const int32 X = std::distance(OutSpawnArea.begin(), Row);
			const int32 Y = std::distance(Row->begin(), Col);
			if (OutSpawnArea[X][Y] != 1)
			{
				if (X < XValueClip || Y < YValueClip ||
					(X > XValueUpperClip && X <= MaxXValue) ||
					(Y > YValueUpperClip && Y <= MaxYValue))
				{
					OutSpawnArea[X][Y] = 2;
					NumClips++;
				}
				else
				{
					OutSpawnArea[X][Y] = 0;
				}
			}
		}
	}
	//UE_LOG(LogTemp, Display, TEXT("NumClips %d"), NumClips);
	return OutSpawnArea;
}

FIntPoint ATargetSpawner::ConvertLocationToPoint(const FVector Location) const
{
	const int32 CenterXValue = (Location.Y + GameModeActorStruct.BoxBounds.Y) / SpawnAreaScale;
	const int32 CenterYValue = (Location.Z + GameModeActorStruct.BoxBounds.Z - BoxBounds.Origin.Z) / SpawnAreaScale;
	return FIntPoint(CenterXValue, CenterYValue);
}

FVector ATargetSpawner::ConvertPointToLocation(const FIntPoint Point) const
{
	const int32 Y = Point.X * SpawnAreaScale - GameModeActorStruct.BoxBounds.Y;
	const int32 Z = Point.Y * SpawnAreaScale - GameModeActorStruct.BoxBounds.Z + BoxBounds.Origin.Z;
	return FVector(BoxBounds.Origin.X, Y, Z);
}

FVector ATargetSpawner::ConvertPointToLocationSingleBeat(const FIntPoint Point) const
{
	const int32 Y = Point.X * SpawnAreaScale - BoxBounds.BoxExtent.Y;
	const int32 Z = Point.Y * SpawnAreaScale - BoxBounds.BoxExtent.Z + BoxBounds.Origin.Z;
	return FVector(BoxBounds.Origin.X, Y, Z);
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