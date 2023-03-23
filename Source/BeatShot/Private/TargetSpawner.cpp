// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "TargetSpawner.h"
#include "SphereTarget.h"
#include "BSGameMode.h"
#include "RLBase.h"
#include "Components/BoxComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


/* -------BEGIN------- */
/*    RLProject Code   */
/* ------------------- */

void ATargetSpawner::WriteTargetPairToFile()
{
	FPythonPair Pair;
	PythonTargetPairs.Peek(Pair);
	FString StringToWrite = FString::FromInt(static_cast<int>(Pair.Previous.X)) + ",";
	StringToWrite.Append(FString::FromInt(static_cast<int>(Pair.Previous.Y)) + ",");
	StringToWrite.Append(FString::FromInt(static_cast<int>(Pair.Current.X)) + ",");
	StringToWrite.Append(FString::FromInt(static_cast<int>(Pair.Current.Y)) + ",");
	StringToWrite.Append(FString::FromInt(static_cast<int>(Pair.Current.Z)));
	PythonTargetPairs.Pop();
	FFileHelper::SaveStringToFile(StringToWrite, *WriteLocationFilePath);
	UE_LOG(LogTemp, Display, TEXT("Saved to File: Previous: (%d,%d) Current: (%d,%d) Reward: %d"),static_cast<int>(Pair.Previous.X), static_cast<int>(Pair.Previous.Y), static_cast<int>(Pair.Current.X),
		static_cast<int>(Pair.Current.Y), static_cast<int>(Pair.Current.Z));
}

FVector ATargetSpawner::TryGetSpawnLocationFromFile() const
{
	TArray<FString> StringFromFile;
	FFileHelper::LoadFileToStringArray(StringFromFile, *ReadLocationFilePath);
	const int32 Row = FCString::Atof(*StringFromFile[0]);
	const int32 Col = FCString::Atof(*StringFromFile[1]);
	UE_LOG(LogTemp, Display, TEXT("Location Read from File: (%d, %d)"), Row, Col);
	/* Convert a (Row, Col) into an index for SpawnCounter, where the world location can be found */
	if (const int32 Index = ((4 - Col) * 11) + Row; Index < SpawnCounter.Num())
	{
		return SpawnCounter[Index].Point;
	}
	UE_LOG(LogTemp, Display, TEXT("Invalid Location"));
	return FVector();
}

void ATargetSpawner::UpdatePythonTargetReward(const FVector& WorldLocation, const bool bHit)
{
	const int32 Index = ActivePythonTargetPairs.Find(ConvertUnrealLocToPythonLoc(WorldLocation, false));
	
	if (Index == INDEX_NONE)
	{
		UE_LOG(LogTemp, Display, TEXT("Location not found in ActivePythonTargetPairs %s"), *ConvertUnrealLocToPythonLoc(WorldLocation, false).ToString());
		return;
	}
	FPythonPair FoundPair = ActivePythonTargetPairs[Index];
	
	/* Update reward */
	if (!bHit)
	{
		FoundPair.Current = FVector(FoundPair.Current.X, FoundPair.Current.Y, 1);
	}
	
	ActivePythonTargetPairs.Remove(FoundPair);
	PythonTargetPairs.Enqueue(FoundPair);
}

void ATargetSpawner::AddToActivePythonTargetPairs(const FVector& PreviousWorldLocation, const FVector& NextWorldLocation)
{
	ActivePythonTargetPairs.Emplace(ConvertUnrealLocToPythonLoc(PreviousWorldLocation, false),
		ConvertUnrealLocToPythonLoc(NextWorldLocation, false));
}

FVector ATargetSpawner::ConvertUnrealLocToPythonLoc(const FVector& WorldLocation, const bool bHit) const
{
	if (const int32 Index = SpawnCounter.Find(FVectorCounter(WorldLocation)); Index != INDEX_NONE)
	{
		/* Convert an index into (Row, Col) */
		const int32 RowValue = Index % 11;
		const int32 ColValue = 4 - (static_cast<float>(Index - RowValue) / 11);
		if (bHit)
		{
			return FVector(RowValue, ColValue, 1);
		}
		return FVector(RowValue, ColValue, 0);
	}
	return FVector();
}

/* --------END-------- */
/*    RLProject Code   */
/* ------------------- */

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
	ActiveTargetPairs.Remove(FoundTargetPair);
	TargetPairs.Enqueue(FoundTargetPair);
}

void ATargetSpawner::AddToActiveTargetPairs(const FVector& PreviousWorldLocation, const FVector& NextWorldLocation)
{
	ActiveTargetPairs.Emplace(PreviousWorldLocation ,NextWorldLocation);
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

	if (Action2Index >= SpawnCounter.Num())
	{
		return FVector::ZeroVector;
	}
	
	const FAlgoInput In(StateIndex, State2Index, ActionIndex, Action2Index, TargetPair.Reward);
	UpdateRLAgent(In);

	UE_LOG(LogTemp, Display, TEXT("MaxActionIndex for Index %d: %d"), State2Index, Action2Index);
	return SpawnCounter[Action2Index].Point;
}

void ATargetSpawner::UpdateRLAgent(const FAlgoInput& In) const
{
	RLBase->UpdateEpisodeRewards(In.Reward);
	RLBase->UpdateQTable(In);
}

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

	SpawnMemoryScaleY = 1.f / 100.f;
	SpawnMemoryScaleZ = 1.f / 50.f;

	/* -------BEGIN------- */
	/*    RLProject Code   */
	/* ------------------- */
	
	// if (GameModeActorStruct.CustomGameModeName.Equals("RLProject"))
	// {
	// 	GameModeActorStruct.BoxBounds.Y = 2000;
	// 	GameModeActorStruct.BoxBounds.Z = 800;
	// 	SpawnMemoryScaleY = 1.f / 200.f;
	// 	SpawnMemoryScaleZ = 1.f / 200.f;
	// }
	
	/* --------END-------- */
	/*    RLProject Code   */
	/* ------------------- */

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
	InitializeSpawnCounter();
	
	RLBase = NewObject<URLBase>();
	RLBase->Init(GameModeActorStruct.GameModeActorName, GameModeActorStruct.CustomGameModeName, GetSpawnCounter().Num(), 1.f, 0.9f, 0.9f);
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
		if (Counter.Point.Y == GetBoxExtents_Unscaled_Static().Y)
		{
			OutArray.Emplace(CurrentRow);
			// AppendStringLocAccRow(CurrentRow, StringToWrite);
			// Don't average the middle row
			if (Counter.Point.Z == GetBoxOrigin_Unscaled().Z)
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
			
			/* -------BEGIN------- */
			/*    RLProject Code   */
			/* ------------------- */

			// /* Add pair to ActivePythonTargetPairs array */
			// AddToActivePythonTargetPairs(PreviousSpawnLocation, SpawnLocation);
			//
			// /* If the queue has a pair inside of it, write it to file so Python can make a new spawn location */
			// if (!PythonTargetPairs.IsEmpty())
			// {
			// 	/* This is placed in the actual spawn target function so that it is called at regular intervals (at most every 0.35 seconds) */
			// 	WriteTargetPairToFile();
			// }
			// PreviousSpawnLocation = SpawnLocation;

			/* --------END-------- */
			/*    RLProject Code   */
			/* ------------------- */
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
	
	/* Update reward value and add to the queue */
	// UpdatePythonTargetReward(Location, !DidExpire);

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

		UpdateRLAgentReward(Location, !DidExpire);
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
		return GetBoxOrigin_Unscaled();
	}
	/* Change the BoxExtent of the SpawnBox if dynamic */
	if (IsDynamicSpreadType(SpreadType))
	{
		SetBoxExtents_Dynamic();
	}
	
	TArray<FVector> OpenLocations = GetValidSpawnLocations(NewTargetScale);
	if (OpenLocations.IsEmpty())
	{
		bSkipNextSpawn = true;
		return GetBoxOrigin_Unscaled();
	}
	
	/* -------BEGIN------- */
	/*    RLProject Code   */
	/* ------------------- */

	// /* Get Location generated by Python from file, and make sure the location from Python was successfully translated to world location */
	// if (const FVector Location = TryGetSpawnLocationFromFile(); !Location.Equals(FVector()))
	// {
	// 	/* If the the location is a valid spawn location, return it */
	// 	if (OpenLocations.ContainsByPredicate([&Location](const FVector Element)
	// 	{
	// 		if (Element.Equals(Location))
	// 		{
	// 			return true;
	// 		}
	// 		return false;
	// 	}))
	// 	{
	// 		return Location;
	// 	}
	// }
	//
	// UE_LOG(LogTemp, Display, TEXT("Location from Python not available to spawn, choosing a random location instead."));
	
	/* --------END-------- */
	/*    RLProject Code   */
	/* ------------------- */

	if (const FVector Location = TryGetSpawnLocationFromRLAgent(); Location != FVector::ZeroVector)
	{
		if (OpenLocations.ContainsByPredicate([&Location](const FVector Element)
		{
			if (Element.Equals(Location))
			{
				return true;
			}
			return false;
		}))
		{
			UE_LOG(LogTemp, Display, TEXT("Successful Spawn Location suggested by RLAgent."));
			return Location;
		}
		UE_LOG(LogTemp, Display, TEXT("Unable to Spawn Location suggested by RLAgent. %s"), *Location.ToString());
	}
	
	if (const FVector Origin = GetBoxOrigin_Unscaled(); OpenLocations.ContainsByPredicate([&Origin](const FVector Element)
	{
		if (Element.Equals(Origin))
		{
			return true;
		}
		return false;
	}))
	{
		return Origin;
	}
	
	const int32 RandomPoint = UKismetMathLibrary::RandomIntegerInRange(0, OpenLocations.Num() - 1);
	return OpenLocations[RandomPoint];
	//FVectorCounter Found = GetVectorCounterFromPoint(OpenLocations[RandomPoint]);
	//Found.ActualChosenPoint = Found.GetRandomSubPoint();
	//FVector Center = Found.Point + FVector(0, Found.IncrementY / 2.f, Found.IncrementZ / 2.f);
	//DrawDebugBox(GetWorld(), Center, FVector(0, Found.IncrementY, Found.IncrementZ), FColor::Orange, false, 1.f);
	//return Found.GetRandomSubPoint();
	
}

FVector ATargetSpawner::GetRandomBeatTrackLocation(const FVector& LocationBeforeChange) const
{
	const FVector NewExtent = FVector(0, GetBoxExtents_Unscaled_Static().Y * 0.5, GetBoxExtents_Unscaled_Static().Z * 0.5);

	const FVector BotLeft = UKismetMathLibrary::RandomPointInBoundingBox(SpawnBox->Bounds.Origin + FVector(0, -GetBoxExtents_Unscaled_Static().Y * 0.5, -GetBoxExtents_Unscaled_Static().Z * 0.5),
	                                                                     NewExtent);

	const FVector BotRight = UKismetMathLibrary::RandomPointInBoundingBox(SpawnBox->Bounds.Origin + FVector(0, GetBoxExtents_Unscaled_Static().Y * 0.5, -GetBoxExtents_Unscaled_Static().Z * 0.5),
	                                                                      NewExtent);

	const FVector TopLeft = UKismetMathLibrary::RandomPointInBoundingBox(SpawnBox->Bounds.Origin + FVector(0, -GetBoxExtents_Unscaled_Static().Y * 0.5, GetBoxExtents_Unscaled_Static().Z * 0.5),
	                                                                     NewExtent);

	const FVector TopRight = UKismetMathLibrary::RandomPointInBoundingBox(SpawnBox->Bounds.Origin + FVector(0, GetBoxExtents_Unscaled_Static().Y * 0.5, GetBoxExtents_Unscaled_Static().Z * 0.5),
	                                                                      NewExtent);

	TArray<FVector> PossibleLocations;
	PossibleLocations.Add(BotLeft);
	PossibleLocations.Add(BotRight);
	PossibleLocations.Add(TopLeft);
	PossibleLocations.Add(TopRight);

	if (LocationBeforeChange.Y < 0)
	{
		if (LocationBeforeChange.Z < GetBoxOrigin_Unscaled().Z)
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
		if (LocationBeforeChange.Z < GetBoxOrigin_Unscaled().Z)
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

TArray<FVector> ATargetSpawner::GetValidSpawnLocations(const float Scale) const
{
	const FVector Extents = GetBoxExtents_Scaled_Current();
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
		for (float Y = roundf(-Extents.Y); Y <= roundf(Extents.Y) + 0.01; Y++)
		{
			AllPoints.AddUnique(FVector(GetBoxOrigin_Unscaled().X, Y * (1 / SpawnMemoryScaleY), GetBoxOrigin_Unscaled().Z + -Extents.Z * (1 / SpawnMemoryScaleZ)));
			AllPoints.AddUnique(FVector(GetBoxOrigin_Unscaled().X, Y * (1 / SpawnMemoryScaleY), GetBoxOrigin_Unscaled().Z + Extents.Z * (1 / SpawnMemoryScaleZ)));
		}
		for (float Z = roundf(-Extents.Z); Z <= roundf(Extents.Z) + 0.01; Z++)
		{
			AllPoints.AddUnique(FVector(GetBoxOrigin_Unscaled().X, -Extents.Y * (1 / SpawnMemoryScaleY), GetBoxOrigin_Unscaled().Z + Z * (1 / SpawnMemoryScaleZ)));
			AllPoints.AddUnique(FVector(GetBoxOrigin_Unscaled().X, Extents.Y * (1 / SpawnMemoryScaleY), GetBoxOrigin_Unscaled().Z + Z * (1 / SpawnMemoryScaleZ)));
		}
	}
	else
	{
		for (float Z = -Extents.Z; Z <= Extents.Z + 0.01; Z++)
		{
			for (float Y = -Extents.Y; Y <= Extents.Y + 0.01; Y++)
			{
				AllPoints.Emplace(FVector(GetBoxOrigin_Unscaled().X, Y * (1 / SpawnMemoryScaleY), GetBoxOrigin_Unscaled().Z + Z * (1 / SpawnMemoryScaleZ)));
			}
		}
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
	AllPoints.Shrink();
	return AllPoints;
}

FVector ATargetSpawner::GetBoxExtents_Unscaled_Static() const
{
	return FVector(0, GameModeActorStruct.BoxBounds.Y, GameModeActorStruct.BoxBounds.Z);
}

FVector ATargetSpawner::GetBoxExtents_Scaled_Current() const
{
	return FVector(0, SpawnBox->Bounds.BoxExtent.Y * SpawnMemoryScaleY, SpawnBox->Bounds.BoxExtent.Z * SpawnMemoryScaleZ);
}

FVector ATargetSpawner::GetBoxExtents_Scaled_Static() const
{
	return FVector(0, SpawnMemoryScaleY * GameModeActorStruct.BoxBounds.Y, SpawnMemoryScaleZ * GameModeActorStruct.BoxBounds.Z);
}

FVector ATargetSpawner::GetBoxOrigin_Unscaled() const
{
	return SpawnBox->Bounds.Origin;
}

void ATargetSpawner::SetBoxExtents_Dynamic() const
{
	const float NewFactor = DynamicSpawnCurve->GetFloatValue(DynamicSpawnScale);
	const float LerpY = UKismetMathLibrary::Lerp(GetBoxExtents_Unscaled_Static().Y, GetBoxExtents_Unscaled_Static().Y * 0.5f, NewFactor);
	const float LerpZ = UKismetMathLibrary::Lerp(GetBoxExtents_Unscaled_Static().Z, GetBoxExtents_Unscaled_Static().Z * 0.5f, NewFactor);
	const float Y = FMath::GridSnap(LerpY, 1 / SpawnMemoryScaleY);
	const float Z = FMath::GridSnap(LerpZ, 1 / SpawnMemoryScaleZ);
	SpawnBox->SetBoxExtent(FVector(0, Y, Z));
}

TArray<FVector> ATargetSpawner::GetOverlappingPoints(const FVector Center, const float Scale) const
{
	const FVector NewCenter = FVector(0, Center.Y * SpawnMemoryScaleY, (Center.Z - GetBoxOrigin_Unscaled().Z) * SpawnMemoryScaleZ);
	const float Radius = ((Scale * SphereTargetRadius * 2) + (GameModeActorStruct.MinDistanceBetweenTargets / 2.f)) * SpawnMemoryScaleZ;
	const FSphere Sphere = FSphere(NewCenter, Radius);
	const FVector Extents = GetBoxExtents_Scaled_Static();
	TArray<FVector> BlockedPoints;
	for (float Z = -Extents.Z; Z <= Extents.Z + 0.01; Z++)
	{
		for (float Y = -Extents.Y; Y <= Extents.Y + 0.01; Y++)
		{
			if (FVector Loc = FVector(0, Y, Z); Sphere.IsInside(Loc))
			{
				BlockedPoints.AddUnique(FVector(GetBoxOrigin_Unscaled().X, Loc.Y * (1 / SpawnMemoryScaleY), GetBoxOrigin_Unscaled().Z + Loc.Z * (1 / SpawnMemoryScaleZ)));
			}
		}
	}
	//UE_LOG(LogTemp, Display, TEXT("Count: %d"), Count);
	//UE_LOG(LogTemp, Display, TEXT("BlockedPoints: %d"), BlockedPoints.Num());
	return BlockedPoints;
}

void ATargetSpawner::InitializeSpawnCounter()
{
	const FVector Extents = GetBoxExtents_Unscaled_Static();
	const float ZInc = 1 / SpawnMemoryScaleZ;
	const float YInc = 1 / SpawnMemoryScaleY;
	//int BigCount = 0;
	int ZCount = 0;
	for (float Z = -Extents.Z; Z <= Extents.Z + 0.01; Z += ZInc)
	{
		int YCount = 0;
		for (float Y = -Extents.Y; Y <= Extents.Y + 0.01; Y += YInc)
		{
			SpawnCounter.Emplace(FVectorCounter(FVector(GetBoxOrigin_Unscaled().X, Y, GetBoxOrigin_Unscaled().Z + Z), YInc, ZInc));
			//TArray<FVector> SubPoints;
			// if (Z == Extents.Z || Y == Extents.Y)
			// {○
			// 	continue;
			// }
			// int SmallCount = 0;
			// const float ZSubInc = Z + ZInc;
			// const float YSubInc = Y + YInc;
			// for (float SubZ = Z; SubZ < ZSubInc; SubZ+=1.f)
			// {
			// 	for (float SubY = Y; SubY< YSubInc; SubY+=1.f)
			// 	{
			// 		SubPoints.Emplace(FVector(GetBoxOrigin_Unscaled().X, SubY, GetBoxOrigin_Unscaled().Z + SubZ));
			// 		BigCount++;
			// 		SmallCount++;
			// 	}
			// }
			// float SubZ = Z;
			// float SubY = Y;
			// UE_LOG(LogTemp, Display, TEXT("SubZ %f"), SubZ);
			// UE_LOG(LogTemp, Display, TEXT("SubZ %f"), SubZ + ZInc);
			// UE_LOG(LogTemp, Display, TEXT("SubY %f"), SubY);
			// UE_LOG(LogTemp, Display, TEXT("SubY %f"), SubY + YInc);
			// UE_LOG(LogTemp, Display, TEXT("Small Count %d"), SmallCount);
			// UE_LOG(LogTemp, Display, TEXT("%s"), *FVector(GetBoxOrigin_Unscaled().X, Y, GetBoxOrigin_Unscaled().Z + Z).ToString());
			YCount++;
		}
		UE_LOG(LogTemp, Display, TEXT("YCount %d"), YCount);
		ZCount++;
	}
	UE_LOG(LogTemp, Display, TEXT("ZCount %d "), ZCount);
	UE_LOG(LogTemp, Display, TEXT("SpawnCounterSize: %d %llu"), SpawnCounter.Num(), SpawnCounter.GetAllocatedSize());
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

FVectorCounter ATargetSpawner::GetVectorCounterFromPoint(const FVector Point)
{
	TArray<FVectorCounter> SpawnCounterCopy = GetSpawnCounter();
	for (const FVectorCounter VectorCounter: SpawnCounterCopy)
	{
		if (VectorCounter.Point.Y == Point.Y && VectorCounter.Point.Z == Point.Z)
		{
			return VectorCounter;
		}
	}
	UE_LOG(LogTemp, Display, TEXT("Point Not Found in SpawnCounter"));
	return FVectorCounter();
}