// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetSpawner.h"
#include <string>
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
	FirstSpawnLocation = BoxBounds.Origin;
	SpawnLocation = FirstSpawnLocation;
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

void ATargetSpawner::SpawnActor()
{
	if (ShouldSpawn == false)
	{
		return;
	}
	// Use different function for SingleBeat game modes
	if (GameModeActorStruct.IsSingleBeatMode == true)
	{
		SpawnSingleActor();
	}
	else
	{
		ASphereTarget* SpawnTarget = GetWorld()->SpawnActor<ASphereTarget>(ActorToSpawn, SpawnLocation, SpawnBox->GetComponentRotation());
		if (SpawnTarget)
		{
			// Since scaling is done after spawn, we have access to LAST target's scale and location
			LastSpawnLocation = SpawnLocation;

			if (GameModeActorStruct.SpreadType == ESpreadType::DynamicRandom ||
				GameModeActorStruct.SpreadType == ESpreadType::DynamicEdgeOnly)
			{
				LastTargetScale = ChangeDynamicScale(SpawnTarget);
				RandomizeDynamicLocation(LastSpawnLocation, LastTargetScale);
			}
			else
			{
				LastTargetScale = RandomizeScale(SpawnTarget);
				RandomizeLocation(LastSpawnLocation, LastTargetScale);
			}

			SpawnTarget->OnLifeSpanExpired.AddDynamic(this, &ATargetSpawner::OnTargetTimeout);

			// Broadcast to GameModeActorBase that a target has spawned
			OnTargetSpawn.Broadcast();
		}
	}

}

void ATargetSpawner::SpawnSingleActor()
{
	if (ShouldSpawn == false)
	{
		return;
	}
	if (ASphereTarget* SpawnTarget = GetWorld()->SpawnActor<ASphereTarget>(ActorToSpawn, SpawnLocation, SpawnBox->GetComponentRotation()))
	{
		// Since scaling is done after spawn, we have access to LAST target's scale value and TargetLocation
		LastSpawnLocation = SpawnLocation;

		if (LastSpawnLocation == BoxBounds.Origin)
		{
			LastTargetSpawnedCenter = true;
		}
		else
		{
			LastTargetSpawnedCenter = false;
		}

		if (GameModeActorStruct.SpreadType != ESpreadType::None)
		{
			LastTargetScale = ChangeDynamicScale(SpawnTarget);
			RandomizeDynamicLocation(LastSpawnLocation, LastTargetScale);
		}
		else
		{
			LastTargetScale = RandomizeScale(SpawnTarget);
			RandomizeLocation(LastSpawnLocation, LastTargetScale);
		}

		// Broadcast to GameModeActorBase that a target has spawned
		OnTargetSpawn.Broadcast();

		SpawnTarget->OnLifeSpanExpired.AddDynamic(this, &ATargetSpawner::OnTargetTimeout);

		// Bind the destruction of target to OnTargetDestroyed to spawn a new target
		SpawnTarget->OnDestroyed.AddDynamic(this, &ATargetSpawner::OnTargetDestroyed);
		SetShouldSpawn(false);
	}
}

void ATargetSpawner::SpawnTracker()
{
	if (ShouldSpawn == false)
	{
		return;
	}

	if (TrackingTarget)
	{
		LocationBeforeDirectionChange = TrackingTarget->GetActorLocation();
	}

	if (!IsValid(TrackingTarget))
	{
		// Initial tracking target spawn
		TrackingTarget = GetWorld()->SpawnActor<ASphereTarget>(ActorToSpawn, FirstSpawnLocation, SpawnBox->GetComponentRotation());
		TrackingTarget->OnActorEndOverlap.AddDynamic(this, &ATargetSpawner::OnBeatTrackOverlapEnd);
		LocationBeforeDirectionChange = FirstSpawnLocation;

		// Broadcast to GameModeActorBase that a target has spawned
		OnTargetSpawn.Broadcast();
	}

	// update tracking direction and speed if already spawned
	if (IsValid(TrackingTarget))
	{
		RandomizeScale(TrackingTarget);
		TrackingSpeed = FMath::FRandRange(GameModeActorStruct.MinTrackingSpeed, GameModeActorStruct.MaxTrackingSpeed);
		EndLocation = RandomizeTrackerLocation(LocationBeforeDirectionChange);
		TrackingDirection = UKismetMathLibrary::GetDirectionUnitVector(LocationBeforeDirectionChange, EndLocation);
	}
}

bool ATargetSpawner::GetShouldSpawn()
{
	return ShouldSpawn;
}

void ATargetSpawner::SetShouldSpawn(bool bShouldSpawn)
{
	ShouldSpawn = bShouldSpawn;
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
	InitialBeatSpawned = false;

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
	if (ShouldSpawn == false || GameModeActorStruct.IsBeatGridMode == false)
	{
		return;
	}

	if (InitialBeatSpawned == false)
	{
		if (const int32 InitialArraySize = SpawnedBeatGridTargets.Num(); InitialArraySize > 0)
		{
			const int32 RandomIndex = FMath::RandRange(0, InitialArraySize - 1);
			ActiveBeatGridTarget = SpawnedBeatGridTargets[RandomIndex];
			LastBeatGridIndex = RandomIndex;
			InitialBeatSpawned = true;
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
			SpawnCandidates = { 1, Width, AdjFor };
		}
		else if (i == AdjBack)
		{
			SpawnCandidates = { i - 1, i + AdjBack, i + Width };
		}
		else if (i == (Width * AdjBack))
		{
			SpawnCandidates = { i - Width, i - AdjBack, i + 1 };
		}
		else if (i == MaxIndex)
		{
			SpawnCandidates = { i - AdjFor, i - Width, i - 1 };
		}
		// top
		else if (i > 0 && i < AdjBack)
		{
			SpawnCandidates = { i - 1, i + 1, i + AdjBack, i + Width, i + AdjFor };
		}
		//left
		else if (i % Width == 0 && i < Width * AdjBack)
		{
			SpawnCandidates = { i - Width, i - AdjBack, i + 1, i + AdjFor, i + Width };
		}
		//bottom
		else if (i > Width * AdjBack && i < MaxIndex)
		{
			SpawnCandidates = { i - AdjFor, i - Width, i - AdjBack,  i - 1, i + 1 };
		}
		//right
		else if ((i + 1) % Width == 0 && i < MaxIndex)
		{
			SpawnCandidates = { i - AdjFor, i - Width, i - 1, i + AdjBack, i + Width };
		}
		//middle
		else
		{
			SpawnCandidates = { i - AdjFor, i - Width, i - AdjBack, i - 1, i + 1, i + AdjBack, i + Width, i + AdjFor };
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
	// only "spawn" target if it hasn't been destroyed by GameModeActorBase
	if (ActiveBeatGridTarget)
	{
		// notify GameModeActorBase that target has "spawned"
		OnTargetSpawn.Broadcast();
		ActiveBeatGridTarget->StartBeatGridTimer(GI->GameModeActorStruct.TargetMaxLifeSpan);
		//GetWorldTimerManager().SetTimer(ActiveBeatGridTarget->TimeSinceSpawn, ActiveBeatGridTarget, &ATargetSpawner::OnBeatGridTargetTimeout, GI->GameModeActorStruct.TargetMaxLifeSpan, false);
		//UE_LOG(LogTemp, Display, TEXT("lifespan: %f"), GI->GameModeActorStruct.TargetMaxLifeSpan);
		//UE_LOG(LogTemp, Display, TEXT("Timein %s"), ActiveBeatGridTarget->GetName());
		//ActiveBeatGridTarget->SetCanBeDamaged(true);
		//ActiveBeatGridTarget->PlayColorGradient();
	}

	if (GameModeActorStruct.IsSingleBeatMode == true)
	{
		SetShouldSpawn(false);
	}
}

void ATargetSpawner::OnTargetDestroyed(AActor* DestroyedActor)
{
	SetShouldSpawn(true);
}

float ATargetSpawner::RandomizeScale(ASphereTarget* Target)
{
	const float RandomScaleValue = FMath::FRandRange(GameModeActorStruct.MinTargetScale, GameModeActorStruct.MaxTargetScale);
	const FVector NewScale = { RandomScaleValue , RandomScaleValue , RandomScaleValue };
	Target->SetActorScale3D(NewScale);
	return RandomScaleValue;
}

void ATargetSpawner::RandomizeLocation(FVector FLastSpawnLocation, float LastTargetScaleValue)
{
	if (GameModeActorStruct.IsSingleBeatMode == true && LastTargetSpawnedCenter == false)
	{
		SpawnLocation = BoxBounds.Origin;
	}
	else
	{
		// Insert the most recent spawn location into array
		RecentSpawnLocations.Insert(LastSpawnLocation, 0);
		RecentSpawnLocations.SetNum(MaxNumberOfTargetsAtOnce);

		// Insert sphere of CheckSpawnRadius radius into sphere array
		CheckSpawnRadius = SphereTargetRadius * LastTargetScaleValue + GameModeActorStruct.MinDistanceBetweenTargets;
		RecentSpawnBounds.Insert(FSphere(LastSpawnLocation, CheckSpawnRadius), 0);
		RecentSpawnBounds.SetNum(MaxNumberOfTargetsAtOnce);

		// SpawnLocation initially centered inside SpawnBox
		SpawnLocation = BoxBounds.Origin;

		// bool variable to track if the SpawnLocation is inside of any of the active targets
		bool SphereIsInside = true;

		int OverloadProtect = 0;

		/*
		 * spans the size of RecentSpawnBounds and only finishes if
		 * SpawnLocation is not inside any of the RecentSpawnBounds spheres
		 */

		while (SphereIsInside)
		{
			for (int i = 0; i < RecentSpawnBounds.Num(); i++)
			{
				if (RecentSpawnBounds[i].IsInside(SpawnLocation))
				{
					if (OverloadProtect > 20)
					{
						ShouldSpawn = false;
						break;
					}
					UE_LOG(LogTemp, Display, TEXT("Iterating %s"), *SpawnLocation.ToString());
					SpawnLocation = BoxBounds.Origin;
					SpawnLocation.Y += -BoxBounds.BoxExtent.Y + 2 * BoxBounds.BoxExtent.Y * FMath::FRand();
					SpawnLocation.Z += -BoxBounds.BoxExtent.Z + 2 * BoxBounds.BoxExtent.Z * FMath::FRand();
					OverloadProtect++;
				}
				if (i == RecentSpawnBounds.Num() - 1)
				{
					SphereIsInside = false;
					break;
				}
			}
		}
	}
}

void ATargetSpawner::RandomizeDynamicLocation(FVector FLastSpawnLocation, float LastTargetScaleValue)
{
	// start with widest value and gradually lower
	const float NewFactor = 1 - DynamicScaleFactor / 100.f * 0.5f;
	const FVector ScaledBoxBounds = GameModeActorStruct.BoxBounds * NewFactor;
	SpawnBox->SetBoxExtent(ScaledBoxBounds);
	BoxBounds = SpawnBox->CalcBounds(GetActorTransform());

	if (GameModeActorStruct.IsSingleBeatMode == true && LastTargetSpawnedCenter == false)
	{
		SpawnLocation = BoxBounds.Origin;
	}
	else
	{
		// Insert the most recent spawn location into array
		RecentSpawnLocations.Insert(LastSpawnLocation, 0);
		RecentSpawnLocations.SetNum(MaxNumberOfTargetsAtOnce);

		// Insert sphere of CheckSpawnRadius radius into sphere array
		CheckSpawnRadius = SphereTargetRadius * LastTargetScaleValue + GameModeActorStruct.MinDistanceBetweenTargets;
		RecentSpawnBounds.Insert(FSphere(LastSpawnLocation, CheckSpawnRadius), 0);
		RecentSpawnBounds.SetNum(MaxNumberOfTargetsAtOnce);

		// SpawnLocation initially centered inside SpawnBox
		SpawnLocation = BoxBounds.Origin;

		// bool variable to track if the SpawnLocation is inside of any of the active targets
		bool SphereIsInside = true;

		int OverloadProtect = 0;

		/*
		 * spans the size of RecentSpawnBounds and only finishes if
		 * SpawnLocation is not inside any of the RecentSpawnBounds spheres
		 */

		while (SphereIsInside)
		{
			for (int i = 0; i < RecentSpawnBounds.Num(); i++)
			{
				if (RecentSpawnBounds[i].IsInside(SpawnLocation))
				{
					if (OverloadProtect > 20)
					{
						ShouldSpawn = false;
						break;
					}
					if (GameModeActorStruct.SpreadType == ESpreadType::DynamicEdgeOnly)
					{
						SpawnLocation = BoxBounds.Origin;
						// Y is left-right Z is up-down
						const float OffsetX = 0.f;
						float OffsetY;
						float OffsetZ;
						switch (UKismetMathLibrary::RandomIntegerInRange(0, 3))
						{
						case 0:
							// top
							OffsetY = UKismetMathLibrary::RandomFloatInRange(-ScaledBoxBounds.Y, ScaledBoxBounds.Y);
							OffsetZ = ScaledBoxBounds.Z;
						case 1:
							// right
							OffsetY = ScaledBoxBounds.Y;
							OffsetZ = UKismetMathLibrary::RandomFloatInRange(-ScaledBoxBounds.Z, ScaledBoxBounds.Z);
						case 2:
							// left
							OffsetY = -ScaledBoxBounds.Y;
							OffsetZ = UKismetMathLibrary::RandomFloatInRange(-ScaledBoxBounds.Z, ScaledBoxBounds.Z);
						default:
							// bottom
							OffsetY = UKismetMathLibrary::RandomFloatInRange(-ScaledBoxBounds.Y, ScaledBoxBounds.Y);
							OffsetZ = -ScaledBoxBounds.Z;
						}
						FVector Offset = { OffsetX, OffsetY, OffsetZ };
						SpawnLocation = FirstSpawnLocation + Offset;
					}
					else if (GameModeActorStruct.SpreadType == ESpreadType::DynamicRandom)
					{
						SpawnLocation = UKismetMathLibrary::RandomPointInBoundingBox(FirstSpawnLocation, BoxBounds.BoxExtent);
					}
					OverloadProtect++;
				}
				if (i == RecentSpawnBounds.Num() - 1)
				{
					SphereIsInside = false;
					break;
				}
			}
		}
		//DrawDebugSphere(GetWorld(), FirstSpawnLocation, SpawnRadius,12,FColor::Orange, false, 1);
		DrawDebugBox(GetWorld(), FirstSpawnLocation, ScaledBoxBounds, FColor::Orange, false, 1);
	}
}

float ATargetSpawner::ChangeDynamicScale(ASphereTarget* Target)
{
	float Scale;
	// start with widest value and gradually lower
	if (DynamicScaleFactor == 0)
	{
		Scale = GameModeActorStruct.MaxTargetScale;
	}
	else
	{
		const float NewFactor = 1 - DynamicScaleFactor / 100.f;
		Scale = UKismetMathLibrary::Lerp(GameModeActorStruct.MinTargetScale, GameModeActorStruct.MaxTargetScale, NewFactor);
	}
	Target->SetActorScale3D({ Scale, Scale, Scale });
	return Scale;
}

void ATargetSpawner::OnTargetTimeout(bool DidExpire)
{
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

FVector ATargetSpawner::RandomizeTrackerLocation(FVector LocationBeforeChange)
{
	// if just doing tracking only, we don't really care about recent spawn locations
	CheckSpawnRadius = SphereTargetRadius * 2 + GameModeActorStruct.MinDistanceBetweenTargets;
	// FSphere LastSpawnSphere = FSphere(LocationBeforeChange, CheckSpawnRadius);

	// try to spawn at origin if available
	FVector LocationToReturn = BoxBounds.Origin;

	// So we don't get stuck in infinite loop
	int OverloadProtect = 0;
	bool IsInsideBox = false;
	while (IsInsideBox == false)
	{
		if (OverloadProtect > 20)
		{
			UE_LOG(LogTemp, Warning, TEXT("Overloading Location Randomizer"));
			break;
		}
		LocationToReturn = UKismetMathLibrary::RandomPointInBoundingBox(BoxBounds.Origin, BoxBounds.BoxExtent);
		UE_LOG(LogTemp, Display, TEXT("Iterating %s"), *LocationToReturn.ToString());

		float DistanceBetween = TrackingSpeed * GameModeActorStruct.TargetSpawnCD;
		UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), DistanceBetween);

		if (UKismetMathLibrary::IsPointInBox(LocationBeforeChange +
			UKismetMathLibrary::GetDirectionUnitVector(LocationBeforeChange, LocationToReturn) *
			TrackingSpeed * GameModeActorStruct.TargetSpawnCD,
			BoxBounds.Origin, BoxBounds.BoxExtent))
		{
			IsInsideBox = true;
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
		UE_LOG(LogTemp, Display, TEXT("EndOverlap called"));
	}
}

void ATargetSpawner::InitializeGameModeActor(FGameModeActorStruct NewGameModeActor)
{
	// Initialize Struct passed by GameModeActorBase
	GameModeActorStruct = NewGameModeActor;

	// GameMode menu uses the full width, while box bounds are only half width / half height
	GameModeActorStruct.BoxBounds.X = 0.f;
	GameModeActorStruct.BoxBounds.Y = GameModeActorStruct.BoxBounds.Y / 2.f;
	GameModeActorStruct.BoxBounds.Z = GameModeActorStruct.BoxBounds.Z / 2.f;

	// Set the center of spawn box based on user selection
	if (GameModeActorStruct.HeadshotHeight == true)
	{
		GameModeActorStruct.CenterOfSpawnBox.Z = HeadshotHeight;
		GameModeActorStruct.BoxBounds.Z = 0.f;
	}
	else if (GameModeActorStruct.WallCentered == true)
	{
		GameModeActorStruct.CenterOfSpawnBox.Z = CenterBackWallHeight;
	}
	else
	{
		GameModeActorStruct.CenterOfSpawnBox.Z = GameModeActorStruct.BoxBounds.Z + DistanceFromFloor;
	}

	// Set new location & box extent
	SpawnBox->SetRelativeLocation(GameModeActorStruct.CenterOfSpawnBox);
	SpawnBox->SetBoxExtent(GameModeActorStruct.BoxBounds);
	BoxBounds = SpawnBox->CalcBounds(GetActorTransform());
	FirstSpawnLocation = BoxBounds.Origin;
	MaxRadius = GameModeActorStruct.BoxBounds.Z;

	/*
	 * Setting max targets at one time for the size of RecentSpawnLocations & RecentSpawnBounds
	 * Only used for MultiBeat
	 */

	MaxNumberOfTargetsAtOnce = ceil(GameModeActorStruct.TargetMaxLifeSpan / GameModeActorStruct.TargetSpawnCD);
	RecentSpawnLocations.Init(BoxBounds.Origin, MaxNumberOfTargetsAtOnce);
	RecentSpawnBounds.Init(FSphere(BoxBounds.Origin, 1), MaxNumberOfTargetsAtOnce);

	if (GameModeActorStruct.IsBeatGridMode)
	{
		InitBeatGrid();
	}
}

void ATargetSpawner::SetTargetSpawnCD(float NewTargetSpawnCD)
{
	GameModeActorStruct.TargetSpawnCD = NewTargetSpawnCD;
}

