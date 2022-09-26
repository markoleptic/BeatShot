// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetSpawner.h"
#include "PlayerHUD.h"
#include "SphereTarget.h"
#include "DefaultGameInstance.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ATargetSpawner::ATargetSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	SpawnBox = CreateDefaultSubobject<UBoxComponent>("SpawnBox");
	RootComponent = SpawnBox;
	SetShouldSpawn(false);
	LastTargetSpawnedCenter = false;
	SingleBeat = false;
	BeatGrid = false;
	LastSpawnLocation = FVector::ZeroVector;
	LastTargetScale = 1.f;
}

void ATargetSpawner::BeginPlay()
{
	Super::BeginPlay();
	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GI)
	{
		GI->RegisterTargetSpawner(this);
	}

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
	if (SingleBeat == true)
	{
		SpawnSingleActor();
	}
	else
	{
		ASphereTarget* SpawnTarget = GetWorld()->SpawnActor<ASphereTarget>(ActorToSpawn, SpawnLocation, SpawnBox->GetComponentRotation());
		if (SpawnTarget)
		{
			// Since scaling is done after spawn, we have access to LAST target's scale value and TargetLocation
			LastSpawnLocation = SpawnLocation;
			LastTargetScale = RandomizeScale(SpawnTarget);

			// Update reference to spawned target in Game Instance
			GI->RegisterSphereTarget(SpawnTarget);

			// Broadcast to GameModeActorBase that a target has spawned
			OnTargetSpawn.Broadcast();

			// Check for collisions for the next spawn location while randomizing
			RandomizeLocation(LastSpawnLocation, LastTargetScale);
		}
	}

}

void ATargetSpawner::SpawnSingleActor()
{
	if (ShouldSpawn == false)
	{
		return;
	}
	ASphereTarget* SpawnTarget = GetWorld()->SpawnActor<ASphereTarget>(ActorToSpawn, SpawnLocation, SpawnBox->GetComponentRotation());
	if (SpawnTarget)
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

		LastTargetScale = RandomizeScale(SpawnTarget);

		// Update reference to spawned target in Game Instance
		GI->RegisterSphereTarget(SpawnTarget);

		// Broadcast to GameModeActorBase that a target has spawned
		OnTargetSpawn.Broadcast();

		// Check for collisions for the next spawn location while randomizing
		RandomizeLocation(LastSpawnLocation, LastTargetScale);

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
	// Gets more complicated if used with regular targets
	// Move at (maybe random / based on BPM?) speed until another beat

	if (TrackingTarget)
	{
		LocationBeforeDirectionChange = TrackingTarget->GetActorLocation();
	}

	if (IsValid(TrackingTarget) == false)
	{
		// Only spawn tracker once in the center if Tracking GameMode
		TrackingTarget = GetWorld()->SpawnActor<ASphereTarget>(ActorToSpawn, FirstSpawnLocation, SpawnBox->GetComponentRotation());
		TrackingTarget->SetMaxHealth(100000.f);
		TrackingTarget->SetLifeSpan(GI->GameModeActorStruct.GameModeLength);
		LocationBeforeDirectionChange = FirstSpawnLocation;

		// Update reference to spawned target in Game Instance
		GI->RegisterSphereTarget(TrackingTarget);

		// Broadcast to GameModeActorBase that a target has spawned
		OnTargetSpawn.Broadcast();
	}

	// update tracking direction and speed if already spawned
	if (TrackingTarget)
	{
		RandomizeScale(TrackingTarget);
		EndLocation = RandomizeTrackerLocation(LocationBeforeDirectionChange);
		TrackingDirection = UKismetMathLibrary::GetDirectionUnitVector(LocationBeforeDirectionChange, EndLocation);
		TrackingSpeed = FMath::FRandRange(GameModeActorStruct.MinTrackingSpeed, GameModeActorStruct.MaxTrackingSpeed);
		//SpawnTarget->OnDestroyed.AddDynamic(this, &ATargetSpawner::OnTargetDestroyed);
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
	//const float HSpacingBetweenTargets = round(GameModeActorStruct.BeatGridSpacing.X + GameModeActorStruct.MinTargetScale * 2 * SphereTargetRadius);
	//const float VSpacingBetweenTargets = round(GameModeActorStruct.BeatGridSpacing.Y + GameModeActorStruct.MinTargetScale * 2 * SphereTargetRadius);
	//const float HTotalSpacing = round(GameModeActorStruct.BeatGridSpacing.X * (sqrt(GameModeActorStruct.BeatGridSize) - 1));
	//const float VTotalSpacing = round(GameModeActorStruct.BeatGridSpacing.Y * (sqrt(GameModeActorStruct.BeatGridSize) - 1));
	//const float HFirstLastTargetSpace = round((2 * HalfWidth) - (2 * OuterSpacing));
	//float TotalHorizontalDistanceTraveled = 0;
	//for (int32 i = 0; i< sqrt(GameModeActorStruct.BeatGridSize); i++)
	//{
	//	for (int32 j = 0; j < sqrt(GameModeActorStruct.BeatGridSize); j++)
	//	{
	//		if (i == 0 && j==0)
	//		{
	//			BeatGridSpawnLocation.Z += (HalfHeight - OuterSpacing);
	//			BeatGridSpawnLocation.Y += (- HalfWidth + OuterSpacing);
	//			TotalHorizontalDistanceTraveled += HSpacingBetweenTargets;
	//		}
	//		else
	//		{
	//			BeatGridSpawnLocation.Y += HSpacingBetweenTargets;
	//			TotalHorizontalDistanceTraveled += HSpacingBetweenTargets;
	//		}
	//		FActorSpawnParameters SpawnParams;
	//		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//		BeatGridTarget = GetWorld()->SpawnActor<ASphereTarget>(ActorToSpawn, BeatGridSpawnLocation, SpawnBox->GetComponentRotation(), SpawnParams);
	//		UE_LOG(LogTemp, Display, TEXT("pos %s"), *BeatGridSpawnLocation.ToString());
	//		if (BeatGridTarget != nullptr)
	//		{
	//			RandomizeScale(BeatGridTarget);
	//		}
	//	}
	//	BeatGridSpawnLocation.Y -= TotalHorizontalDistanceTraveled;
	//	TotalHorizontalDistanceTraveled = 0.f;
	//	//BeatGridSpawnLocation.Y -= (HFirstLastTargetSpace + HSpacingBetweenTargets);
	//	BeatGridSpawnLocation.Z -= VSpacingBetweenTargets;
	//}

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

			ASphereTarget* BeatGridTarget = GetWorld()->SpawnActor<ASphereTarget>(ActorToSpawn, BeatGridSpawnLocation, SpawnBox->GetComponentRotation());
			if (BeatGridTarget)
			{
				BeatGridTarget->SetMaxHealth(1000000);
				BeatGridTarget->SetLifeSpan(GameModeActorStruct.GameModeLength);
				BeatGridTarget->SetCanBeDamaged(false);
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
		const int32 InitialArraySize = SpawnedBeatGridTargets.Num();
		const int32 RandomIndex = FMath::RandRange(0, InitialArraySize - 1);
		ActiveBeatGridTarget = SpawnedBeatGridTargets[RandomIndex];
		LastBeatGridIndex = RandomIndex;
		InitialBeatSpawned = true;
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
		const int32 Height = Width;
		const int32 AdjFor = Width + 1;
		const int32 AdjBack = Width - 1;
		const int32 i = LastBeatGridIndex;

		// corners
		if (i == 0)
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
	// notify GameModeActorBase that target has spawned
	OnTargetSpawn.Broadcast();
	GetWorldTimerManager().SetTimer(ActiveBeatGridTarget->TimeSinceSpawn, this, &ATargetSpawner::OnBeatGridTargetTimeout, GI->GameModeActorStruct.TargetMaxLifeSpan, false);
	ActiveBeatGridTarget->SetCanBeDamaged(true);
	ActiveBeatGridTarget->PlayColorGradient();

	if (GameModeActorStruct.IsSingleBeatMode == true)
	{
		SetShouldSpawn(false);
	}
}

void ATargetSpawner::OnBeatGridTargetTimeout()
{
	GetWorldTimerManager().ClearTimer(ActiveBeatGridTarget->TimeSinceSpawn);
	ActiveBeatGridTarget->SetCanBeDamaged(false);

	if (GameModeActorStruct.IsSingleBeatMode == true)
	{
		SetShouldSpawn(true);
	}
}

void ATargetSpawner::OnTargetDestroyed(AActor* DestroyedActor)
{
	SetShouldSpawn(true);
}

float ATargetSpawner::RandomizeScale(ASphereTarget* Target)
{
	const float RandomScaleValue = FMath::FRandRange(GameModeActorStruct.MinTargetScale, GameModeActorStruct.MaxTargetScale);
	Target->BaseMesh->SetWorldScale3D(FVector(RandomScaleValue, RandomScaleValue, RandomScaleValue));
	return RandomScaleValue;
}

void ATargetSpawner::RandomizeLocation(FVector FLastSpawnLocation, float LastTargetScaleValue)
{
	if (SingleBeat && LastTargetSpawnedCenter == false)
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

		// while loop that spans the size of RecentSpawnBounds and only finishes if
		// SpawnLocation is not inside any of the RecentSpawnBounds spheres
		while (SphereIsInside)
		{
			for (int i = 0; i < RecentSpawnBounds.Num(); i++)
			{
				if (RecentSpawnBounds[i].IsInside(SpawnLocation))
				{
					if (OverloadProtect > 20)
					{
						ShouldSpawn = false;
						exit(0);
					}
					UE_LOG(LogTemp, Display, TEXT("Iterating %s"), *SpawnLocation.ToString());
					SpawnLocation = BoxBounds.Origin;
					SpawnLocation.Y += -BoxBounds.BoxExtent.Y + 2 * BoxBounds.BoxExtent.Y * FMath::FRand();
					SpawnLocation.Z += -BoxBounds.BoxExtent.Z + 2 * BoxBounds.BoxExtent.Z * FMath::FRand();
					OverloadProtect++;
					break;
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

FVector ATargetSpawner::RandomizeTrackerLocation(FVector LocationBeforeChange)
{
	// if just doing tracking only, we don't really care about recent spawn locations
	// **Not currently using LastTargetScaleValue**
	CheckSpawnRadius = SphereTargetRadius * 2 + GameModeActorStruct.MinDistanceBetweenTargets;
	FSphere LastSpawnSphere = FSphere(LocationBeforeChange, CheckSpawnRadius);

	//try to spawn at origin if available
	FVector LocationToReturn = BoxBounds.Origin;

	//So we don't get stuck in infinite loop
	int OverloadProtect = 0;

	while (LastSpawnSphere.IsInside(LocationToReturn))
	{
		if (OverloadProtect > 20)
		{
			ShouldSpawn = false;
			UE_LOG(LogTemp, Warning, TEXT("Overloading Location Randomizer"));
		}

		UE_LOG(LogTemp, Display, TEXT("Iterating %s"), *LocationToReturn.ToString());
		LocationToReturn = BoxBounds.Origin;
		LocationToReturn.Y += -BoxBounds.BoxExtent.Y + 2 * BoxBounds.BoxExtent.Y * FMath::FRand();
		LocationToReturn.Z += -BoxBounds.BoxExtent.Z + 2 * BoxBounds.BoxExtent.Z * FMath::FRand();

		OverloadProtect++;
	}
	return LocationToReturn;
}

void ATargetSpawner::InitializeGameModeActor(FGameModeActorStruct NewGameModeActor)
{
	// Initialize Struct passed by GameModeActorBase
	GameModeActorStruct = NewGameModeActor;

	/*
	 * GameMode specific checking
	 */
	if (GameModeActorStruct.GameModeActorName == EGameModeActorName::NarrowSpreadSingleBeat ||
		GameModeActorStruct.GameModeActorName == EGameModeActorName::WideSpreadSingleBeat ||
		GameModeActorStruct.IsSingleBeatMode == true)
	{
		SingleBeat = true;
	}
	if (GameModeActorStruct.GameModeActorName == EGameModeActorName::BeatGrid ||
		GameModeActorStruct.IsBeatGridMode)
	{
		BeatGrid = true;
	}

	// GameMode menu uses the full width, while box bounds are only half width / half height
	GameModeActorStruct.BoxBounds.X = 0.f;
	GameModeActorStruct.BoxBounds.Y = GameModeActorStruct.BoxBounds.Y / 2.f;
	GameModeActorStruct.BoxBounds.Z = GameModeActorStruct.BoxBounds.Z / 2.f;

	// Set the center of spawn box based on user selection
	if (GameModeActorStruct.HeadshotHeight == true)
	{
		GameModeActorStruct.CenterOfSpawnBox.Z = 160.f;
		GameModeActorStruct.BoxBounds.Z = 0.f;
	}
	else if (GameModeActorStruct.WallCentered == true)
	{
		GameModeActorStruct.CenterOfSpawnBox.Z = 750.f;
	}
	else
	{
		GameModeActorStruct.CenterOfSpawnBox.Z = GameModeActorStruct.BoxBounds.Z + 100.f;
	}

	// Set new location & box extent
	SpawnBox->SetRelativeLocation(GameModeActorStruct.CenterOfSpawnBox);
	SpawnBox->SetBoxExtent(GameModeActorStruct.BoxBounds);
	BoxBounds = SpawnBox->CalcBounds(GetActorTransform());
	FirstSpawnLocation = BoxBounds.Origin;

	/*
	 * Setting max targets at one time for the size of RecentSpawnLocations & RecentSpawnBounds
	 * Only used for MultiBeat
	 */

	MaxNumberOfTargetsAtOnce = ceil(GameModeActorStruct.TargetMaxLifeSpan / GameModeActorStruct.TargetSpawnCD);
	RecentSpawnLocations.Init(BoxBounds.Origin, MaxNumberOfTargetsAtOnce);
	RecentSpawnBounds.Init(FSphere(BoxBounds.Origin, 1), MaxNumberOfTargetsAtOnce);

	if (BeatGrid == true)
	{
		InitBeatGrid();
	}
}

void ATargetSpawner::SetTargetSpawnCD(float NewTargetSpawnCD)
{
	GameModeActorStruct.TargetSpawnCD = NewTargetSpawnCD;
}

