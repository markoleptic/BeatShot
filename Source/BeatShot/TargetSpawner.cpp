// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetSpawner.h"
#include "PlayerHUD.h"
#include "SphereTarget.h"
#include "DefaultGameInstance.h"
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
		ChangeTrackingTargetDirection();
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
	if (ASphereTarget* SpawnTarget = Cast<ASphereTarget>(GetWorld()->SpawnActor(ActorToSpawn, &SpawnLocation, &FRotator::ZeroRotator, SpawnParams)))
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

		/* Find the next target's location and scale */
		FindNextTargetProperties(SpawnLocation, TargetScale);

		// Don't continue spawning for SingleBeat
		SetShouldSpawn(false);
	}
}

void ATargetSpawner::SpawnMultiBeatTarget()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
	if (ASphereTarget* SpawnTarget = Cast<ASphereTarget>(GetWorld()->SpawnActor(ActorToSpawn, &SpawnLocation, &FRotator::ZeroRotator, SpawnParams)))
	{
		/* Setting the current target's scale that was previously calculated */
		SpawnTarget->SetActorScale3D(FVector(TargetScale, TargetScale, TargetScale));
		SpawnTarget->OnLifeSpanExpired.AddDynamic(this, &ATargetSpawner::OnTargetTimeout);

		/* Broadcast to GameModeActorBase that a target has spawned */
		OnTargetSpawn.Broadcast();

		/* LAST target's location and scale */
		LastSpawnLocation = SpawnLocation;
		LastTargetScale = TargetScale;

		/* Find the next target's location and scale */
		FindNextTargetProperties(LastSpawnLocation, LastTargetScale);
	}
}

void ATargetSpawner::ChangeTrackingTargetDirection()
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
		TrackingTarget->OnActorEndOverlap.AddDynamic(this, &ATargetSpawner::OnBeatTrackOverlapEnd);
		LocationBeforeDirectionChange = BoxBounds.Origin;

		// Broadcast to GameModeActorBase that a target has spawned
		OnTargetSpawn.Broadcast();
	}

	if (IsValid(TrackingTarget))
	{
		RandomizeScale(TrackingTarget);
		TrackingSpeed = FMath::FRandRange(GameModeActorStruct.MinTrackingSpeed, GameModeActorStruct.MaxTrackingSpeed);
		EndLocation = RandomizeTrackerLocation(LocationBeforeDirectionChange);
		TrackingDirection = UKismetMathLibrary::GetDirectionUnitVector(LocationBeforeDirectionChange, EndLocation);
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
				RandomizeScale(BeatGridTarget);
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

float ATargetSpawner::RandomizeScale(ASphereTarget* Target)
{
	if (GameModeActorStruct.UseDynamicSizing)
	{
		/* start with widest value and gradually lower */
		if (DynamicScaleFactor == 0)
		{
			TargetScale = GameModeActorStruct.MaxTargetScale;
		}
		else
		{
			const float NewFactor = 1 - DynamicScaleFactor / 100.f;
			TargetScale = UKismetMathLibrary::Lerp(GameModeActorStruct.MinTargetScale,
												   GameModeActorStruct.MaxTargetScale, NewFactor);
		}
	}
	else
	{
		TargetScale = FMath::FRandRange(GameModeActorStruct.MinTargetScale, GameModeActorStruct.MaxTargetScale);
	}
	return TargetScale;
}

void ATargetSpawner::FindNextTargetProperties(const FVector FLastSpawnLocation, const float LastTargetScaleValue)
{
	/* Insert sphere of CheckSpawnRadius radius into sphere array */
	RecentSpawnBounds.Insert(FSphere(FLastSpawnLocation, LastTargetScaleValue * SphereTargetRadius), 0);
	RecentSpawnBounds.SetNum(MaxNumberOfTargetsAtOnce);
	DrawDebugSphere(GetWorld(), FLastSpawnLocation, LastTargetScaleValue * SphereTargetRadius, 20, FColor::Blue, false,
					MaxNumberOfTargetsAtOnce);

	/* for (FSphere Sphere : RecentSpawnBounds)
	{
		DrawDebugSphere(GetWorld(), Sphere.Center, Sphere.W, 10, FColor::Blue, false, 0.5);
	} */
	
	/* set the scale before we find the location */
	if (GameModeActorStruct.UseDynamicSizing)
	{
		/* start with widest value and gradually lower */
		if (DynamicScaleFactor == 0)
		{
			TargetScale = GameModeActorStruct.MaxTargetScale;
		}
		else
		{
			const float NewFactor = 1 - DynamicScaleFactor / 100.f;
			TargetScale = UKismetMathLibrary::Lerp(GameModeActorStruct.MinTargetScale,
			                                       GameModeActorStruct.MaxTargetScale, NewFactor);
		}
	}
	else
	{
		TargetScale = FMath::FRandRange(GameModeActorStruct.MinTargetScale, GameModeActorStruct.MaxTargetScale);
	}

	/* find location */
	/* SpawnLocation initially centered inside SpawnBox */
	SpawnLocation = BoxBounds.Origin;

	/* If single beat needs to spawn at center, exit early */
	if (GameModeActorStruct.IsSingleBeatMode == true && LastTargetSpawnedCenter == false)
	{
		return;
	}

	// bool variable to track if the SpawnLocation is inside of any of the active targets
	bool bSphereIsColliding = true;
	int OverloadProtect = 0;
	const float CollisionSphereRadius = TargetScale * SphereTargetRadius + GameModeActorStruct.MinDistanceBetweenTargets;
	FSphere NewTarget = FSphere(SpawnLocation, CollisionSphereRadius);

	/* dynamic */
	if (GameModeActorStruct.SpreadType == ESpreadType::DynamicRandom ||
		GameModeActorStruct.SpreadType == ESpreadType::DynamicEdgeOnly)
	{
		/* start with widest value and gradually lower */
		const float NewFactor = 1 - DynamicScaleFactor / 100.f * 0.5f;
		const FVector ScaledBoxBounds = GameModeActorStruct.BoxBounds * NewFactor;
		SpawnBox->SetBoxExtent(ScaledBoxBounds);
		BoxBounds = SpawnBox->CalcBounds(GetActorTransform());
		while (bSphereIsColliding)
		{
			for (int i = 0; i < RecentSpawnBounds.Num(); i++)
			{
				if (RecentSpawnBounds[i].Intersects(NewTarget))
				{
					if (GameModeActorStruct.SpreadType == ESpreadType::DynamicEdgeOnly)
					{
						SpawnLocation = BoxBounds.Origin;
						// Y is left-right Z is up-down
						FVector Offset = FVector();
						switch (UKismetMathLibrary::RandomIntegerInRange(0, 3))
						{
						case 0:
							{
								// top
								Offset.Y = UKismetMathLibrary::RandomFloatInRange(
									-ScaledBoxBounds.Y, ScaledBoxBounds.Y);
								Offset.Z = ScaledBoxBounds.Z;
							}
						case 1:
							{
								// right
								Offset.Y = ScaledBoxBounds.Y;
								Offset.Z = UKismetMathLibrary::RandomFloatInRange(
									-ScaledBoxBounds.Z, ScaledBoxBounds.Z);
							}
						case 2:
							{
								// left
								Offset.Y = -ScaledBoxBounds.Y;
								Offset.Z = UKismetMathLibrary::RandomFloatInRange(
									-ScaledBoxBounds.Z, ScaledBoxBounds.Z);
							}
						default:
							{
								// bottom
								Offset.Y = UKismetMathLibrary::RandomFloatInRange(
									-ScaledBoxBounds.Y, ScaledBoxBounds.Y);
								Offset.Z = -ScaledBoxBounds.Z;
							}
						}
						SpawnLocation = SpawnLocation + Offset;
						NewTarget = FSphere(SpawnLocation, CollisionSphereRadius);
					}
					/* Dynamic Random */
					else
					{
						SpawnLocation = UKismetMathLibrary::RandomPointInBoundingBox(
							BoxBounds.Origin, BoxBounds.BoxExtent);
						NewTarget = FSphere(SpawnLocation, CollisionSphereRadius);
					}
					OverloadProtect++;
					i = -1;
				}
				if (i == RecentSpawnBounds.Num() - 1)
				{
					bSphereIsColliding = false;
					break;
				}
				if (OverloadProtect > 15)
				{
					ShouldSpawn = false;
					break;
				}
			}
		}
		//DrawDebugSphere(GetWorld(), SpawnLocation, TargetScale,12, FColor::Orange, false, 3);
		DrawDebugBox(GetWorld(), BoxBounds.Origin, ScaledBoxBounds, FColor::Orange, false, 1);
	}
	else
	{
		while (bSphereIsColliding)
		{
			for (int i = 0; i < RecentSpawnBounds.Num(); i++)
			{
				if (RecentSpawnBounds[i].Intersects(NewTarget))
				{
					SpawnLocation = BoxBounds.Origin;
					SpawnLocation.Y += -BoxBounds.BoxExtent.Y + 2 * BoxBounds.BoxExtent.Y * FMath::FRand();
					SpawnLocation.Z += -BoxBounds.BoxExtent.Z + 2 * BoxBounds.BoxExtent.Z * FMath::FRand();
					NewTarget = FSphere(SpawnLocation, CollisionSphereRadius);
					i = -1;
					OverloadProtect++;
				}
				if (i == RecentSpawnBounds.Num() - 1)
				{
					bSphereIsColliding = false;
					break;
				}
				if (OverloadProtect > 15)
				{
					ShouldSpawn = false;
					break;
				}
				UE_LOG(LogTemp, Display, TEXT("Overload: %d"), OverloadProtect);
			}
		}
	}
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
		Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this))->GameModeActorBaseRef->UpdatePlayerScores(TimeAlive);
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

FVector ATargetSpawner::RandomizeTrackerLocation(const FVector LocationBeforeChange) const
{
	// try to spawn at origin if available
	FVector LocationToReturn = BoxBounds.Origin;

	// So we don't get stuck in infinite loop
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

void ATargetSpawner::OnBeatTrackOverlapEnd(AActor* OverlappedActor, AActor* OtherActor)
{
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
	SpawnLocation = BoxBounds.Origin;
	
	/* Initial target size */
	if (GameModeActorStruct.UseDynamicSizing)
	{
		TargetScale = GameModeActorStruct.MaxTargetScale;
	}
	else if (GameModeActorStruct.MinTargetScale == GameModeActorStruct.MaxTargetScale)
	{
		TargetScale = GameModeActorStruct.MinTargetScale;
	}
	else
	{
		TargetScale = FMath::FRandRange(GameModeActorStruct.MinTargetScale, GameModeActorStruct.MaxTargetScale);
	}
	
	/* Setting max targets at one time for the size of RecentSpawnLocations & RecentSpawnBounds. Only used for MultiBeat */
	MaxNumberOfTargetsAtOnce = ceil(GameModeActorStruct.TargetMaxLifeSpan / GameModeActorStruct.TargetSpawnCD);
	RecentSpawnBounds.Init(FSphere(BoxBounds.Origin, 1), MaxNumberOfTargetsAtOnce);

	if (GameModeActorStruct.IsBeatGridMode)
	{
		InitBeatGrid();
	}
}
