// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Target/SpawnAreaManagerComponent.h"
#include "GlobalConstants.h"
#include "Target/MatrixFunctions.h"
#include "Target/TargetManager.h"

/* ---------------- */
/* -- USpawnArea -- */
/* ---------------- */


USpawnArea::USpawnArea()
{
	Width = 0.f;
	Height = 0.f;
	Vertex_BottomLeft = FVector(-1);
	CenterPoint = FVector(-1);
	ChosenPoint = FVector(-1);
	TargetScale = FVector(1);
	TotalHits = 0;
	TotalSpawns = INDEX_NONE;
	TotalTrackingDamage = 0;
	TotalTrackingDamagePossible = INDEX_NONE;
	Index = INDEX_NONE;
	bIsActivated = false;
	bAllowActivationWhileActivated = false;
	bIsCurrentlyManaged = false;
	bIsRecent = false;
	TimeSetRecent = DBL_MAX;
	IndexType = EGridIndexType::None;
	TargetGuid = FGuid();
	OverlappingVertices = TArray<FVector>();
	AdjacentIndexMap = TMap<EBorderingDirection, int32>();
	Size = 0;
}

void USpawnArea::Init(const FSpawnAreaParams& InParams)
{
	Width = InParams.IncY;
	Height = InParams.IncZ;

	Vertex_BottomLeft = InParams.BottomLeft;
	CenterPoint = Vertex_BottomLeft + FVector(0, InParams.IncY * 0.5f, InParams.IncZ * 0.5f);
	Vertex_BottomRight = Vertex_BottomLeft + FVector(0, InParams.IncY, 0);
	Vertex_TopLeft = Vertex_BottomLeft + FVector(0, 0, InParams.IncZ);
	Vertex_TopRight = Vertex_BottomLeft + FVector(0, InParams.IncY, InParams.IncZ);

	ChosenPoint = Vertex_BottomLeft;
	TargetScale = FVector(1);

	TotalSpawns = INDEX_NONE;
	TotalHits = 0;
	Index = InParams.Index;
	Size = InParams.NumVerticalTargets * InParams.NumHorizontalTargets;

	bIsActivated = false;
	bIsRecent = false;
	TimeSetRecent = DBL_MAX;
	IndexType = FindIndexType(InParams.Index, Size, InParams.NumHorizontalTargets);
	TargetGuid = FGuid();
	SetAdjacentIndices(IndexType, Index, InParams.NumHorizontalTargets);
	OverlappingVertices = TArray<FVector>();
}

void USpawnArea::SetAdjacentIndices(const EGridIndexType InGridIndexType, const int32 InIndex,
	const int32 InWidth)
{
	const int32 UpLeft = InIndex + InWidth - 1;
	const int32 Up = InIndex + InWidth;
	const int32 UpRight = InIndex + InWidth + 1;
	const int32 Right = InIndex + 1;
	const int32 DownRight = InIndex - InWidth + 1;
	const int32 Down = InIndex - InWidth;
	const int32 DownLeft = InIndex - InWidth - 1;
	const int32 Left = InIndex - 1;

	switch (InGridIndexType)
	{
	case EGridIndexType::None:
		break;
	case EGridIndexType::Corner_TopLeft:
		AdjacentIndexMap.Add(EBorderingDirection::Right, Right);
		AdjacentIndexMap.Add(EBorderingDirection::Down, Down);
		AdjacentIndexMap.Add(EBorderingDirection::DownRight, DownRight);
		break;
	case EGridIndexType::Corner_TopRight:
		AdjacentIndexMap.Add(EBorderingDirection::Left, Left);
		AdjacentIndexMap.Add(EBorderingDirection::Down, Down);
		AdjacentIndexMap.Add(EBorderingDirection::DownLeft, DownLeft);
		break;
	case EGridIndexType::Corner_BottomRight:
		AdjacentIndexMap.Add(EBorderingDirection::Left, Left);
		AdjacentIndexMap.Add(EBorderingDirection::Up, Up);
		AdjacentIndexMap.Add(EBorderingDirection::UpLeft, UpLeft);
		break;
	case EGridIndexType::Corner_BottomLeft:
		AdjacentIndexMap.Add(EBorderingDirection::Right, Right);
		AdjacentIndexMap.Add(EBorderingDirection::Up, Up);
		AdjacentIndexMap.Add(EBorderingDirection::UpRight, UpRight);
		break;
	case EGridIndexType::Border_Top:
		AdjacentIndexMap.Add(EBorderingDirection::Left, Left);
		AdjacentIndexMap.Add(EBorderingDirection::Right, Right);
		AdjacentIndexMap.Add(EBorderingDirection::DownRight, DownRight);
		AdjacentIndexMap.Add(EBorderingDirection::Down, Down);
		AdjacentIndexMap.Add(EBorderingDirection::DownLeft, DownLeft);
		break;
	case EGridIndexType::Border_Right:
		AdjacentIndexMap.Add(EBorderingDirection::UpLeft, UpLeft);
		AdjacentIndexMap.Add(EBorderingDirection::Up, Up);
		AdjacentIndexMap.Add(EBorderingDirection::Left, Left);
		AdjacentIndexMap.Add(EBorderingDirection::Down, Down);
		AdjacentIndexMap.Add(EBorderingDirection::DownLeft, DownLeft);
		break;
	case EGridIndexType::Border_Bottom:
		AdjacentIndexMap.Add(EBorderingDirection::UpLeft, UpLeft);
		AdjacentIndexMap.Add(EBorderingDirection::Up, Up);
		AdjacentIndexMap.Add(EBorderingDirection::UpRight, UpRight);
		AdjacentIndexMap.Add(EBorderingDirection::Right, Right);
		AdjacentIndexMap.Add(EBorderingDirection::Left, Left);
		break;
	case EGridIndexType::Border_Left:
		AdjacentIndexMap.Add(EBorderingDirection::Up, Up);
		AdjacentIndexMap.Add(EBorderingDirection::UpRight, UpRight);
		AdjacentIndexMap.Add(EBorderingDirection::Right, Right);
		AdjacentIndexMap.Add(EBorderingDirection::DownRight, DownRight);
		AdjacentIndexMap.Add(EBorderingDirection::Down, Down);
		break;
	case EGridIndexType::Middle:
		AdjacentIndexMap.Add(EBorderingDirection::UpLeft, UpLeft);
		AdjacentIndexMap.Add(EBorderingDirection::Up, Up);
		AdjacentIndexMap.Add(EBorderingDirection::UpRight, UpRight);
		AdjacentIndexMap.Add(EBorderingDirection::Right, Right);
		AdjacentIndexMap.Add(EBorderingDirection::DownRight, DownRight);
		AdjacentIndexMap.Add(EBorderingDirection::Down, Down);
		AdjacentIndexMap.Add(EBorderingDirection::DownLeft, DownLeft);
		AdjacentIndexMap.Add(EBorderingDirection::Left, Left);
		break;
	}
}

EGridIndexType USpawnArea::FindIndexType(const int32 InIndex, const int32 InSize, const int32 InWidth)
{
	const int32 MaxIndex = InSize - 1;
	const int32 BottomRowFirstIndex = InWidth - 1;
	const int32 TopRowFirstIndex = InSize - InWidth;
	if (InIndex == 0)
	{
		return EGridIndexType::Corner_BottomLeft;
	}
	if (InIndex == BottomRowFirstIndex)
	{
		return EGridIndexType::Corner_BottomRight;
	}
	if (InIndex == MaxIndex)
	{
		return EGridIndexType::Corner_TopRight;
	}
	if (InIndex == TopRowFirstIndex)
	{
		return EGridIndexType::Corner_TopLeft;
	}

	// top
	if (InIndex > 0 && InIndex < BottomRowFirstIndex)
	{
		return EGridIndexType::Border_Bottom;
	}
	// right
	if ((InIndex + 1) % InWidth == 0 && InIndex < MaxIndex)
	{
		return EGridIndexType::Border_Right;
	}
	// bottom
	if (InIndex > TopRowFirstIndex && InIndex < MaxIndex)
	{
		return EGridIndexType::Border_Top;
	}
	// left	
	if (InIndex % InWidth == 0 && InIndex < TopRowFirstIndex)
	{
		return EGridIndexType::Border_Left;
	}
	return EGridIndexType::Middle;
}

FVector USpawnArea::GenerateRandomPointInSpawnArea() const
{
	const float Y = roundf(FMath::FRandRange(Vertex_BottomLeft.Y, Vertex_BottomRight.Y - 1.f));
	const float Z = roundf(FMath::FRandRange(Vertex_BottomLeft.Z, Vertex_TopLeft.Z - 1.f));
	return FVector(Vertex_BottomLeft.X, Y, Z);
}

TArray<int32> USpawnArea::GetBorderingIndices() const
{
	TArray<int32> Out;
	AdjacentIndexMap.GenerateValueArray(Out);
	return Out;
}

float USpawnArea::GetMinOverlapRadius() const
{
	return FMath::Max(Width, Height) * 0.5f;
}

void USpawnArea::SetRandomChosenPoint()
{
	ChosenPoint = GenerateRandomPointInSpawnArea();
}

void USpawnArea::SetIsRecent(const bool bSetIsRecent)
{
	bIsRecent = bSetIsRecent;
	if (bSetIsRecent)
	{
		TimeSetRecent = FPlatformTime::Seconds();
	}
	else
	{
		TimeSetRecent = DBL_MAX;
		EmptyOverlappingVertices();
	}
}

void USpawnArea::SetOverlappingVertices(const TArray<FVector>& InOverlappingVertices)
{
	OverlappingVertices = InOverlappingVertices;
}

TArray<FVector> USpawnArea::GenerateOverlappingVertices(const float InMinTargetDistance, const FVector& InScale,
	TArray<FVector>& DebugVertices, const bool bAddDebugVertices) const
{
	TArray<FVector> OutPoints;

	const float ScaledRadius = InScale.X * SphereTargetRadius;
	// multiply by two so that any point outside the sphere will not be overlapping
	float Radius = ScaledRadius * 2.f + (InMinTargetDistance * 0.5f);
	Radius = FMath::Max(Radius, GetMinOverlapRadius()) + FMath::Max(Width, Height);

	const FSphere Sphere = FSphere(ChosenPoint, Radius);
	int32 IncrementsInYRadius = 1;
	int32 IncrementsInZRadius = 1;

	if (Radius > Width)
	{
		IncrementsInYRadius = floor(Radius / Width);
	}
	if (Radius > Height)
	{
		IncrementsInZRadius = floor(Radius / Height);
	}

	const float MinY = Vertex_BottomLeft.Y - IncrementsInYRadius * Width;
	const float MaxY = Vertex_TopRight.Y + IncrementsInYRadius * Width;
	const float MinZ = Vertex_BottomLeft.Z - IncrementsInZRadius * Height;
	const float MaxZ = Vertex_TopRight.Z + IncrementsInZRadius * Height;

	int Count = 0;
	for (float Z = MinZ; Z <= MaxZ; Z += Height)
	{
		for (float Y = MinY; Y <= MaxY; Y += Width)
		{
			if (FVector Loc = FVector(ChosenPoint.X, Y, Z); Sphere.IsInside(Loc))
			{
				OutPoints.AddUnique(Loc);
			}
			else
			{
				if (bAddDebugVertices)
				{
					DebugVertices.Add(FVector(ChosenPoint.X, Y, Z));
				}
			}
			Count++;
		}
	}
	return OutPoints;
}

void USpawnArea::IncrementTotalSpawns()
{
	if (TotalSpawns == INDEX_NONE)
	{
		TotalSpawns = 1;
	}
	else
	{
		TotalSpawns++;
	}
}

void USpawnArea::IncrementTotalHits()
{
	TotalHits++;
}

void USpawnArea::IncrementTotalTrackingDamagePossible()
{
	if (TotalTrackingDamagePossible == INDEX_NONE)
	{
		TotalTrackingDamagePossible = 1;
	}
	else
	{
		TotalTrackingDamagePossible++;
	}
}

void USpawnArea::IncrementTotalTrackingDamage()
{
	TotalTrackingDamage++;
}

/* -------------------------------- */
/* -- USpawnAreaManagerComponent -- */
/* -------------------------------- */

USpawnAreaManagerComponent::USpawnAreaManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	BSConfig = nullptr;

	bShowDebug_ValidSpawnLocations = false;
	bShowDebug_FilteredRecentIndices = false;
	bShowDebug_FilteredActivatedIndices = false;
	bShowDebug_FilteredManagedIndices = false;
	bShowDebug_RemovedSpawnLocations = false;
	bShowDebug_OverlappingVertices_OnFlaggedManaged = false;
	bShowDebug_OverlappingVertices_OnFlaggedActivated = false;
	bShowDebug_OverlappingVertices_Dynamic = false;

	SpawnAreas = TArray<USpawnArea*>();
	MostRecentSpawnArea = nullptr;
	OriginSpawnArea = nullptr;
	AllBottomLeftVertices = TArray<FVector>();
	Size = FIntVector3();
	SpawnAreaInc = FIntVector3();
	SpawnAreaScale = FVector();
}

void USpawnAreaManagerComponent::DestroyComponent(bool bPromoteChildren)
{
	BSConfig = nullptr;
	Super::DestroyComponent(bPromoteChildren);
}

void USpawnAreaManagerComponent::Init(FBSConfig* InBSConfig, const FVector& InOrigin, const FVector& InStaticExtents,
	const FExtrema& InStaticExtrema)
{
	Clear();

	BSConfig = InBSConfig;
	Origin = InOrigin;
	StaticExtents = InStaticExtents;
	StaticExtrema = InStaticExtrema;

	SetAppropriateSpawnMemoryValues();
	AllBottomLeftVertices = InitializeSpawnAreas();

	OriginSpawnArea = FindSpawnAreaFromLocation(Origin);

	UE_LOG(LogTargetManager, Display, TEXT("Origin: %s "), *Origin.ToCompactString());
	UE_LOG(LogTargetManager, Display, TEXT("StaticExtents: %s "), *StaticExtents.ToCompactString());
	UE_LOG(LogTargetManager, Display, TEXT("StaticExtrema Min: %s Max: %s"), *StaticExtrema.Min.ToCompactString(),
		*StaticExtrema.Max.ToCompactString());
	UE_LOG(LogTargetManager, Display, TEXT("SpawnMemoryScaleY: %.4f SpawnMemoryScaleZ: %.4f"), SpawnAreaScale.Y,
		SpawnAreaScale.Z);
	UE_LOG(LogTargetManager, Display, TEXT("SpawnAreaIncY: %d SpawnAreaIncZ: %d"), SpawnAreaInc.Y, SpawnAreaInc.Z);
	UE_LOG(LogTargetManager, Display, TEXT("SpawnCounterSize: %d Allocated Size: %llu"), SpawnAreas.Num(),
		SpawnAreas.GetAllocatedSize());
}

void USpawnAreaManagerComponent::Clear()
{
	BSConfig = nullptr;

	bShowDebug_ValidSpawnLocations = false;
	bShowDebug_FilteredRecentIndices = false;
	bShowDebug_FilteredActivatedIndices = false;
	bShowDebug_FilteredManagedIndices = false;
	bShowDebug_RemovedSpawnLocations = false;
	bShowDebug_OverlappingVertices_OnFlaggedManaged = false;
	bShowDebug_OverlappingVertices_Dynamic = false;

	SpawnAreas = TArray<USpawnArea*>();
	MostRecentSpawnArea = nullptr;
	OriginSpawnArea = nullptr;
	AllBottomLeftVertices = TArray<FVector>();
	Size = FIntVector3();
	SpawnAreaInc = FIntVector3();
	SpawnAreaScale = FVector();
}

void USpawnAreaManagerComponent::UpdateTotalTrackingDamagePossible(const FVector& InLocation) const
{
	if (USpawnArea* SpawnArea = FindSpawnAreaFromLocation(InLocation))
	{
		SpawnArea->IncrementTotalTrackingDamagePossible();
	}
}

TArray<FVector> USpawnAreaManagerComponent::InitializeSpawnAreas()
{
	int SizeY = 0;
	int SizeZ = 0;
	int Index = 0;

	TArray<FVector> AllSpawnLocations;
	FSpawnAreaParams Params;
	Params.NumHorizontalTargets = GetBSConfig()->GridConfig.NumHorizontalGridTargets;
	Params.NumVerticalTargets = GetBSConfig()->GridConfig.NumVerticalGridTargets;
	Params.bGrid = GetTc().TargetDistributionPolicy == ETargetDistributionPolicy::Grid;
	Params.IncY = SpawnAreaInc.Y;
	Params.IncZ = SpawnAreaInc.Z;

	float MaxY = StaticExtrema.Max.Y;
	float MaxZ = StaticExtrema.Max.Z;

	if (Params.bGrid)
	{
		MaxY += 1.f;
		MaxZ += 1.f;
	}

	for (float Z = StaticExtrema.Min.Z; Z < MaxZ; Z += SpawnAreaInc.Z)
	{
		SizeY = 0;
		for (float Y = StaticExtrema.Min.Y; Y < MaxY; Y += SpawnAreaInc.Y)
		{
			FVector Loc(Origin.X, Y, Z);
			USpawnArea* SpawnArea = NewObject<USpawnArea>();

			SpawnArea->Init(Params.NextIndex(Index, Loc));

			AllSpawnLocations.Add(Loc);
			SpawnAreas.Add(SpawnArea);

			Index++;
			SizeY++;
		}
		SizeZ++;
	}
	Size.Y = SizeY;
	Size.Z = SizeZ;

	return AllSpawnLocations;
}

bool USpawnAreaManagerComponent::ShouldConsiderManagedAsInvalid() const
{
	return GetTc().TargetSpawningPolicy == ETargetSpawningPolicy::RuntimeOnly;
}

void USpawnAreaManagerComponent::SetAppropriateSpawnMemoryValues()
{
	const int32 HalfWidth = StaticExtents.Y;
	const int32 HalfHeight = StaticExtents.Z;
	bool bWidthScaleSelected = false;
	bool bHeightScaleSelected = false;

	switch (GetTc().TargetDistributionPolicy)
	{
	case ETargetDistributionPolicy::HeadshotHeightOnly:
		{
			SpawnAreaScale.Z = 1.f;
			SpawnAreaInc.Z = 1;
			for (const int32 Scale : PreferredScales)
			{
				if (!bWidthScaleSelected)
				{
					if (HalfWidth % Scale == 0)
					{
						SpawnAreaInc.Y = Scale;
						SpawnAreaScale.Y = 1.f / Scale;
						bWidthScaleSelected = true;
					}
				}
				if (bWidthScaleSelected)
				{
					break;
				}
			}
			if (!bWidthScaleSelected)
			{
				UE_LOG(LogTargetManager, Warning, TEXT("Couldn't Find Width for StaticExtents: Y:%f Z:%f"),
					StaticExtents.Y, StaticExtents.Z);
			}
		}
		break;
	case ETargetDistributionPolicy::None:
	case ETargetDistributionPolicy::EdgeOnly:
	case ETargetDistributionPolicy::FullRange:
		{
			for (const int32 Scale : PreferredScales)
			{
				if (!bWidthScaleSelected)
				{
					if (HalfWidth % Scale == 0)
					{
						SpawnAreaInc.Y = Scale;
						SpawnAreaScale.Y = 1.f / Scale;
						bWidthScaleSelected = true;
					}
				}
				if (!bHeightScaleSelected)
				{
					if (HalfHeight % Scale == 0)
					{
						SpawnAreaInc.Z = Scale;
						SpawnAreaScale.Z = 1.f / Scale;
						bHeightScaleSelected = true;
					}
				}
				if (bHeightScaleSelected && bWidthScaleSelected)
				{
					break;
				}
			}
			if (!bWidthScaleSelected || !bHeightScaleSelected)
			{
				UE_LOG(LogTargetManager, Warning, TEXT("Couldn't Find Height/Width for StaticExtents: Y:%f Z:%f"),
					StaticExtents.Y, StaticExtents.Z);
			}
		}
		break;
	case ETargetDistributionPolicy::Grid:
		{
			const float MaxTargetSize = GetTc().MaxSpawnedTargetScale * SphereTargetDiameter;
			SpawnAreaInc.Y = GetBSConfig()->GridConfig.GridSpacing.X + MaxTargetSize;
			SpawnAreaInc.Z = GetBSConfig()->GridConfig.GridSpacing.Y + MaxTargetSize;
			SpawnAreaScale.Y = 1.f / SpawnAreaInc.Y;
			SpawnAreaScale.Z = 1.f / SpawnAreaInc.Z;
		}
		break;
	}
}

void USpawnAreaManagerComponent::HandleTargetDamageEvent(const FTargetDamageEvent& DamageEvent)
{
	USpawnArea* SpawnArea = FindSpawnAreaFromGuid(DamageEvent.Guid);
	if (!SpawnArea)
	{
		UE_LOG(LogTargetManager, Warning, TEXT("Could not find SpawnArea from DamageEvent Guid."));
		return;
	}

	switch (DamageEvent.DamageType)
	{
	case ETargetDamageType::Tracking:
		{
			// Instead of using the spawn area where the target started, use the current location
			USpawnArea* SpawnAreaByLoc = FindSpawnAreaFromLocation(DamageEvent.Transform.GetLocation());
			if (!SpawnAreaByLoc) return;

			// Total Tracking Damage Possible is done on tick in UpdateTotalTrackingDamagePossible

			// Only increment total tracking damage if damage came from player
			if (!DamageEvent.bDamagedSelf && DamageEvent.DamageDelta > 0.f)
				SpawnAreaByLoc->IncrementTotalTrackingDamage();
		}
		break;
	case ETargetDamageType::Hit:
		{
			// Always increment total spawns for Hit Damage Events
			SpawnArea->IncrementTotalSpawns();

			// Only increment total hits if damage came from player
			if (!DamageEvent.bDamagedSelf && DamageEvent.DamageDelta > 0.f) SpawnArea->IncrementTotalHits();
		}
		break;
	case ETargetDamageType::Self:
		{
			if (DamageEvent.VulnerableToDamageTypes.Contains(ETargetDamageType::Hit))
			{
				// Always increment total spawns for Hit Damage Events
				SpawnArea->IncrementTotalSpawns();
			}
			if (DamageEvent.VulnerableToDamageTypes.Contains(ETargetDamageType::Tracking))
			{
				// Nothing to do
			}
		}
		break;
	case ETargetDamageType::None:
	case ETargetDamageType::Combined: UE_LOG(LogTargetManager, Warning,
			TEXT("DamageEvent with DamageType None or Combined."));
		break;
	}

	// Applies to any damage type
	if (DamageEvent.bWillDeactivate || DamageEvent.bWillDestroy)
	{
		RemoveActivatedFlagFromSpawnArea(SpawnArea);
		HandleRecentTargetRemoval(SpawnArea);
	}
}

void USpawnAreaManagerComponent::HandleRecentTargetRemoval(USpawnArea* SpawnArea)
{
	FlagSpawnAreaAsRecent(SpawnArea);
	FTimerHandle TimerHandle;

	/* Handle removing recent flag from SpawnArea */
	switch (GetTc().RecentTargetMemoryPolicy)
	{
	case ERecentTargetMemoryPolicy::None:
		RemoveRecentFlagFromSpawnArea(SpawnArea);
		break;
	case ERecentTargetMemoryPolicy::CustomTimeBased:
		{
			RemoveFromRecentDelegate.BindUObject(this, &USpawnAreaManagerComponent::RemoveRecentFlagFromSpawnArea,
				SpawnArea);
			const float Time = GetTc().RecentTargetTimeLength;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, RemoveFromRecentDelegate, Time, false);
		}
		break;
	case ERecentTargetMemoryPolicy::NumTargetsBased:
		{
			RefreshRecentFlags();
		}
		break;
	case ERecentTargetMemoryPolicy::UseTargetSpawnCD:
		{
			RemoveFromRecentDelegate.BindUObject(this, &USpawnAreaManagerComponent::RemoveRecentFlagFromSpawnArea,
				SpawnArea);
			const float Time = GetTc().TargetSpawnCD;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, RemoveFromRecentDelegate, Time, false);
		}
		break;
	}
}

bool USpawnAreaManagerComponent::ShouldForceSpawnAtOrigin() const
{
	if (GetTc().bSpawnEveryOtherTargetInCenter)
	{
		if (GetMostRecentSpawnArea() != GetOriginSpawnArea()) return true;
	}
	return false;
}

void USpawnAreaManagerComponent::SetMostRecentSpawnArea(USpawnArea* SpawnArea)
{
	MostRecentSpawnArea = SpawnArea;
}

/* ------------------------------- */
/* -- SpawnArea finders/getters -- */
/* ------------------------------- */

USpawnArea* USpawnAreaManagerComponent::GetMostRecentSpawnArea() const
{
	return MostRecentSpawnArea;
}

USpawnArea* USpawnAreaManagerComponent::GetOriginSpawnArea() const
{
	return OriginSpawnArea;
}

USpawnArea* USpawnAreaManagerComponent::FindSpawnAreaFromIndex(const int32 InIndex) const
{
	const TArray<USpawnArea*>::ElementType* Area = SpawnAreas.FindByPredicate([&InIndex](const USpawnArea* SpawnArea)
	{
		if (SpawnArea->Index == InIndex)
		{
			return true;
		}
		return false;
	});
	return Area ? *Area : nullptr;
}

USpawnArea* USpawnAreaManagerComponent::FindSpawnAreaFromLocation(const FVector& InLocation) const
{
	const TArray<USpawnArea*>::ElementType* Area = SpawnAreas.FindByPredicate([&InLocation](const USpawnArea* SpawnArea)
	{
		if ((InLocation.Y >= SpawnArea->Vertex_BottomLeft.Y) && (InLocation.Z >= SpawnArea->Vertex_BottomLeft.Z) && (
			InLocation.Y < SpawnArea->Vertex_TopRight.Y - 0.01) && (InLocation.Z < SpawnArea->Vertex_TopRight.Z - 0.01))
		{
			return true;
		}
		return false;
	});
	return Area ? *Area : nullptr;
}

USpawnArea* USpawnAreaManagerComponent::FindSpawnAreaFromGuid(const FGuid& TargetGuid) const
{
	const TArray<USpawnArea*>::ElementType* Area = SpawnAreas.FindByPredicate([&TargetGuid](const USpawnArea* SpawnArea)
	{
		if (SpawnArea->GetTargetGuid() == TargetGuid)
		{
			return true;
		}
		return false;
	});
	return Area ? *Area : nullptr;
}

USpawnArea* USpawnAreaManagerComponent::FindOldestRecentSpawnArea() const
{
	TArray<USpawnArea*> RecentSpawnAreas = GetRecentSpawnAreas();

	if (RecentSpawnAreas.IsEmpty())
	{
		return nullptr;
	}

	USpawnArea* MostRecent = RecentSpawnAreas.Top();

	for (USpawnArea* SpawnArea : RecentSpawnAreas)
	{
		if (SpawnArea->GetTimeSetRecent() < MostRecent->GetTimeSetRecent())
		{
			MostRecent = SpawnArea;
		}
	}
	return MostRecent;
}

USpawnArea* USpawnAreaManagerComponent::FindOldestDeactivatedManagedSpawnArea() const
{
	TArray<USpawnArea*> Areas = GetDeactivatedManagedSpawnAreas();
	return Areas.IsEmpty() ? nullptr : Areas.Top();
}

USpawnArea* USpawnAreaManagerComponent::FindBorderingDeactivatedManagedSpawnArea(const USpawnArea* InSpawnArea) const
{
	TArray<USpawnArea*> Areas = GetDeactivatedManagedSpawnAreas();
	if (!InSpawnArea) { return nullptr; }
	for (USpawnArea* Area : Areas)
	{
		if (Area->GetBorderingIndices().Contains(InSpawnArea->GetIndex()))
		{
			return Area;
		}
	}
	return nullptr;
}

int32 USpawnAreaManagerComponent::FindSpawnAreaIndexFromLocation(const FVector& InLocation) const
{
	const TArray<USpawnArea*>::ElementType* SpawnArea = SpawnAreas.FindByPredicate(
		[&InLocation](const USpawnArea* SpawnArea)
		{
			if ((InLocation.Y >= SpawnArea->Vertex_BottomLeft.Y) && (InLocation.Z >= SpawnArea->Vertex_BottomLeft.Z) &&
				(InLocation.Y < SpawnArea->Vertex_TopRight.Y - 0.01) && (InLocation.Z < SpawnArea->Vertex_TopRight.Z -
					0.01))
			{
				return true;
			}
			return false;
		});
	return SpawnArea ? (*SpawnArea)->Index : INDEX_NONE;
}

bool USpawnAreaManagerComponent::IsSpawnAreaValid(const USpawnArea* InSpawnArea) const
{
	if (SpawnAreas.Contains(InSpawnArea))
	{
		return true;
	}
	return false;
}

bool USpawnAreaManagerComponent::IsSpawnAreaValid(const int32 InIndex) const
{
	return SpawnAreas.IsValidIndex(InIndex);
}

/* -------------------------------------- */
/* -- TArray SpawnArea finders/getters -- */
/* -------------------------------------- */

TArray<USpawnArea*> USpawnAreaManagerComponent::GetManagedSpawnAreas() const
{
	return SpawnAreas.FilterByPredicate([](const USpawnArea* SpawnArea)
	{
		if (SpawnArea->IsManaged())
		{
			return true;
		}
		return false;
	});
}

TArray<USpawnArea*> USpawnAreaManagerComponent::GetDeactivatedManagedSpawnAreas() const
{
	TArray<USpawnArea*> Areas = SpawnAreas.FilterByPredicate([](const USpawnArea* SpawnArea)
	{
		if (SpawnArea->IsManaged() && !SpawnArea->IsActivated())
		{
			return true;
		}
		return false;
	});
	if (!Areas.IsEmpty())
	{
		Areas.Sort([](const USpawnArea& SpawnArea1, const USpawnArea& SpawnArea2)
		{
			return SpawnArea1.GetTimeSetRecent() < SpawnArea2.GetTimeSetRecent();
		});
	}
	return Areas;
}

TArray<USpawnArea*> USpawnAreaManagerComponent::GetRecentSpawnAreas() const
{
	return SpawnAreas.FilterByPredicate([](const USpawnArea* SpawnArea)
	{
		if (SpawnArea->IsRecent())
		{
			return true;
		}
		return false;
	});
}

TArray<USpawnArea*> USpawnAreaManagerComponent::GetActivatedSpawnAreas() const
{
	return SpawnAreas.FilterByPredicate([](const USpawnArea* SpawnArea)
	{
		if (SpawnArea->IsActivated())
		{
			return true;
		}
		return false;
	});
}

TArray<USpawnArea*> USpawnAreaManagerComponent::GetActivatedOrRecentSpawnAreas() const
{
	return SpawnAreas.FilterByPredicate([](const USpawnArea* SpawnArea)
	{
		if (SpawnArea->IsActivated() || SpawnArea->IsRecent())
		{
			return true;
		}
		return false;
	});
}

TArray<USpawnArea*> USpawnAreaManagerComponent::GetManagedActivatedOrRecentSpawnAreas() const
{
	return SpawnAreas.FilterByPredicate([](const USpawnArea* SpawnArea)
	{
		if (SpawnArea->IsManaged() || SpawnArea->IsActivated() || SpawnArea->IsRecent())
		{
			return true;
		}
		return false;
	});
}

/* ------------------------ */
/* -- SpawnArea flagging -- */
/* ------------------------ */

void USpawnAreaManagerComponent::FlagSpawnAreaAsManaged(const FGuid TargetGuid) const
{
	USpawnArea* SpawnArea = FindSpawnAreaFromGuid(TargetGuid);
	if (!SpawnArea) return;

	if (!SpawnArea->IsManaged()) SpawnArea->SetIsCurrentlyManaged(true);
	else UE_LOG(LogTargetManager, Warning, TEXT("Tried to flag an already managed SpawnArea as managed."));

	// Don't generate new OverlappingVertices if they're already generated
	if (!SpawnArea->OverlappingVertices.IsEmpty()) return;

	TArray<FVector> DebugVertices;
	TArray<FVector> OverlappingPoints = SpawnArea->GenerateOverlappingVertices(GetTc().MinDistanceBetweenTargets,
		SpawnArea->GetTargetScale(), DebugVertices, bShowDebug_OverlappingVertices_OnFlaggedManaged);
	SpawnArea->SetOverlappingVertices(OverlappingPoints);

	#if !UE_BUILD_SHIPPING
	if (!bShowDebug_OverlappingVertices_OnFlaggedManaged) return;

	const float ScaledRadius = SpawnArea->GetTargetScale().X * SphereTargetRadius;
	float Radius = ScaledRadius * 2.f + (GetTc().MinDistanceBetweenTargets / 2.f);
	Radius = FMath::Max(Radius, SpawnArea->GetMinOverlapRadius()) + FMath::Max(Size.Y, Size.Z);
	DrawDebugSphere(GetWorld(), SpawnArea->ChosenPoint, Radius, 16, FColor::Magenta, false, 0.5f);

	for (const FVector& Vector : OverlappingPoints)
	{
		DrawDebugPoint(GetWorld(), Vector, 10.f, FColor::Red, false, 0.5f);
	}
	for (FVector Vertex : DebugVertices)
	{
		DrawDebugPoint(GetWorld(), Vertex, 10.f, FColor::Green, false, 0.5f);
	}
	#endif
}

void USpawnAreaManagerComponent::FlagSpawnAreaAsActivated(const FGuid TargetGuid,
	const bool bCanActivateWhileActivated) const
{
	USpawnArea* SpawnArea = FindSpawnAreaFromGuid(TargetGuid);
	if (!SpawnArea) return;

	if (SpawnArea->CanActivateWhileActivated() && SpawnArea->IsActivated()) return;

	if (SpawnArea->IsRecent()) RemoveRecentFlagFromSpawnArea(SpawnArea);

	if (!SpawnArea->IsActivated()) SpawnArea->SetIsActivated(true, bCanActivateWhileActivated);
	else UE_LOG(LogTargetManager, Warning, TEXT("Tried to flag as Activated when already Activated."));

	// Don't generate new OverlappingVertices if they're already generated
	if (!SpawnArea->OverlappingVertices.IsEmpty()) return;

	TArray<FVector> DebugVertices;
	const TArray<FVector> OverlappingPoints = SpawnArea->GenerateOverlappingVertices(GetTc().MinDistanceBetweenTargets,
		SpawnArea->GetTargetScale(), DebugVertices, bShowDebug_OverlappingVertices_OnFlaggedActivated);
	SpawnArea->SetOverlappingVertices(OverlappingPoints);
}

void USpawnAreaManagerComponent::FlagSpawnAreaAsRecent(USpawnArea* SpawnArea)
{
	if (!SpawnArea) return;
	if (!SpawnArea->IsRecent()) SpawnArea->SetIsRecent(true);
	else UE_LOG(LogTargetManager, Warning, TEXT("Tried to flag as Recent when already Recent."));
}

void USpawnAreaManagerComponent::RemoveManagedFlagFromSpawnArea(const FGuid TargetGuid) const
{
	USpawnArea* SpawnArea = FindSpawnAreaFromGuid(TargetGuid);
	if (!SpawnArea) return;

	if (SpawnArea->IsManaged()) SpawnArea->SetIsCurrentlyManaged(false);
	else UE_LOG(LogTargetManager, Warning, TEXT("Tried to remove managed flag from from non-managed SpawnArea."));
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void USpawnAreaManagerComponent::RemoveActivatedFlagFromSpawnArea(USpawnArea* SpawnArea) const
{
	if (!SpawnArea) return;

	if (SpawnArea->IsActivated()) SpawnArea->SetIsActivated(false);
	else UE_LOG(LogTargetManager, Warning, TEXT("Tried to remove an activated flag from non-activated SpawnArea."));
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void USpawnAreaManagerComponent::RemoveRecentFlagFromSpawnArea(USpawnArea* SpawnArea) const
{
	if (!SpawnArea) return;

	if (SpawnArea->IsRecent()) SpawnArea->SetIsRecent(false);
	else UE_LOG(LogTargetManager, Warning, TEXT("Tried to remove a recent flag from non-recent SpawnArea."));
}

void USpawnAreaManagerComponent::RefreshRecentFlags() const
{
	if (GetTc().RecentTargetMemoryPolicy != ERecentTargetMemoryPolicy::NumTargetsBased) return;

	const int32 NumToRemove = GetRecentSpawnAreas().Num() - GetTc().MaxNumRecentTargets;
	if (NumToRemove <= 0) return;

	for (int32 CurrentRemoveNum = 0; CurrentRemoveNum < NumToRemove; CurrentRemoveNum++)
	{
		if (USpawnArea* Found = FindOldestRecentSpawnArea())
		{
			RemoveRecentFlagFromSpawnArea(Found);
		}
	}
}

/* ----------------------------------- */
/* -- Finding Valid Spawn Locations -- */
/* ----------------------------------- */

TArray<USpawnArea*> USpawnAreaManagerComponent::GetActivatableSpawnAreas(const int32 NumToActivate) const
{
	// Assumes that we cannot activate an already activated target
	TArray<USpawnArea*> Filtered = GetDeactivatedManagedSpawnAreas();
	TArray<USpawnArea*> Out = Filtered;
	
	switch (GetTc().TargetActivationSelectionPolicy)
	{
	case ETargetActivationSelectionPolicy::Bordering:
		{
			FilterBorderingIndices(Filtered, MostRecentSpawnArea);
			if (Filtered.Num() >= NumToActivate) Out = Filtered;
		}
		break;
	case ETargetActivationSelectionPolicy::None:
	case ETargetActivationSelectionPolicy::Random:
		break;
	}
	FilterRecentIndices(Filtered);
	if (Filtered.Num() > NumToActivate) Out = Filtered;
	
	return Out;
}

TArray<USpawnArea*> USpawnAreaManagerComponent::GetSpawnableSpawnAreas(const FVector& Scale, const FExtrema& Extrema,
	const int32 NumToSpawn, const ERuntimeTargetSpawningLocationSelectionMode Mode) const
{
	// TODO: SpawnAreas returned can contain SpawnAreas right beside each other.
	// TODO: Either remove that or have TargetManager do something

	TArray<USpawnArea*> Out;

	switch (GetTc().TargetDistributionPolicy)
	{
	case ETargetDistributionPolicy::EdgeOnly:
		FilterByEdgeOnly(Out, Extrema);
		break;
	case ETargetDistributionPolicy::None:
	case ETargetDistributionPolicy::FullRange:
		FilterByExtrema(Out, Extrema);
		break;
	case ETargetDistributionPolicy::Grid:
		{
			Out = GetSpawnAreas();
			
			// Always filter managed
			TArray<int32> Filtered = FilterManagedIndices(Out);
			const int32 NumManaged = Filtered.Num();

			// Always filter activated
			Filtered.Append(FilterActivatedIndices(Out));
			const int32 NumActivated = Filtered.Num();

			// TODO Maybe not always filter recent
			Filtered.Append(FilterRecentIndices(Out));
			const int32 NumRecent = Filtered.Num();
			
			UE_LOG(LogTargetManager, Display, TEXT("Filtered: Managed: %d Activated: %d Recent: %d"), NumManaged, NumActivated, NumRecent);

			switch (Mode)
			{
			case ERuntimeTargetSpawningLocationSelectionMode::Random:
				break;
			case ERuntimeTargetSpawningLocationSelectionMode::Bordering:
				{
					TArray<USpawnArea*> Best;
					for (int j = 0; j < Out.Num(); j++)
					{
						TArray<USpawnArea*> Current;
						TArray<USpawnArea*> Temp = Out;
						TArray<USpawnArea*> TempFilter = Out;

						// Filter the starting index
						FilterBorderingIndices(TempFilter, Out[j]);

						if (TempFilter.IsEmpty()) continue;

						Current.Add(Out[j]);
						Temp.Remove(Out[j]);

						// loop through the rest of the indices
						for (int i = 0; i < Out.Num() - 1; i++)
						{
							if (j == i) continue;
							TempFilter = Temp;

							FilterBorderingIndices(TempFilter, Out[i]);
							if (TempFilter.IsEmpty()) continue;

							Current.Add(Out[i]);
							Temp.Remove(Out[i]);
						}

						if (Current.Num() == NumToSpawn) return Current;
						if (Current.Num() > Best.Num()) Best = Current;
					}
					return Best;
				}
			case ERuntimeTargetSpawningLocationSelectionMode::RandomGridBlock:
			case ERuntimeTargetSpawningLocationSelectionMode::NearbyGridBlock:
				{
					const TArray<int32> IndexValidity = CreateIndexValidityArray(Filtered);
					FindRandomGridBlock(Out, IndexValidity, NumToSpawn);
					return Out;
				}
			// TODO: NYI
			case ERuntimeTargetSpawningLocationSelectionMode::None:
			case ERuntimeTargetSpawningLocationSelectionMode::RandomVertical:
			case ERuntimeTargetSpawningLocationSelectionMode::RandomHorizontal:
				break;
			}
		}
		break;
	case ETargetDistributionPolicy::HeadshotHeightOnly:
		Out = GetSpawnAreas();
		break;
	}

	RemoveOverlappingSpawnAreas(Out, Scale);

	if (bShowDebug_ValidSpawnLocations)
	{
		DrawDebug_Boxes(Out, FColor::Emerald, 4, 0);
	}

	return Out;
}

void USpawnAreaManagerComponent::FindRandomGridBlock(TArray<USpawnArea*>& ValidSpawnAreas,
	const TArray<int32>& IndexValidity, const int32 BlockSize) const
{
	const TSet IndexTypes = {EBorderingDirection::Left, EBorderingDirection::Right,
		EBorderingDirection::Up, EBorderingDirection::Down};

	FDFSLoopParams LoopParams = FDFSLoopParams(IndexValidity, IndexTypes, BlockSize);
	for (const USpawnArea* SpawnArea : ValidSpawnAreas)
	{
		const int32 CurrentIndex = SpawnArea->GetIndex();
		if (IndexValidity[CurrentIndex] == 1)
		{
			LoopParams.NextIter(CurrentIndex);
			FindValidIndexCombinationsDFS(CurrentIndex, LoopParams);
		}
	}

	if (!LoopParams.Blocks.IsEmpty())
	{
		ValidSpawnAreas.Empty();

		TArray<FBlock> BlocksArr = LoopParams.Blocks.Array();
		const int32 RandomBlockIndex = FMath::RandRange(0, BlocksArr.Num() - 1);

		for (const int32 BlockIndex : BlocksArr[RandomBlockIndex].Indices)
		{
			ValidSpawnAreas.Add(SpawnAreas[BlockIndex]);
		}
	}

	//FString Line;
	UE_LOG(LogTargetManager, Display, TEXT("Total Blocks Found: %d"), LoopParams.Blocks.Num());
	/*UE_LOG(LogTargetManager, Display, TEXT("Blocks:"));
	for (FBlock& Block : LoopParams.Blocks)
	{
		for (const int32 Index : Block.Indices)
		{
			Line += FString::FromInt(Index) + " ";
		}
		UE_LOG(LogTemp, Display, TEXT("%s"), *Line);
		Line.Empty();
	}*/
}

void USpawnAreaManagerComponent::FindValidIndexCombinationsDFS(const int32 StartIndex, FDFSLoopParams& Params) const
{
	if (Params.CurrentDepth == Params.BlockSize)
	{
		if (Params.CurrentBlock.BlockSize == Params.BlockSize)
		{
			// Check if the current block is of the desired size before adding it
			Params.Blocks.Add(Params.CurrentBlock);
		}
		return;
	}

	for (const TPair<EBorderingDirection, int32>& Pair : SpawnAreas[StartIndex]->GetAdjacentIndexMap())
	{
		const int32 SpawnAreaIndex = Pair.Value;
		if (Params.Valid[SpawnAreaIndex] == 1 && !Params.Visited.Contains(SpawnAreaIndex) && !Params.InitialVisited.
			Contains(SpawnAreaIndex) && Params.IndexTypes.Contains(Pair.Key))
		{
			// Mark the spawn area as visited at this depth
			Params.Visited.Add(SpawnAreaIndex);

			// Add the current index to the current block
			Params.CurrentBlock.AddBlockIndex(SpawnAreaIndex);

			// Call the function recursively for the next depth level
			Params.CurrentDepth = Params.CurrentDepth + 1;
			FindValidIndexCombinationsDFS(SpawnAreaIndex, Params);

			// Unmark the spawn area as visited (backtrack)
			Params.Visited.Remove(SpawnAreaIndex);

			// Remove the current index from the current block (backtrack)
			Params.CurrentBlock.RemoveBlockIndex(SpawnAreaIndex);

			// Decrement the depth when backtracking
			Params.CurrentDepth = Params.CurrentDepth - 1;
		}
	}
}

void USpawnAreaManagerComponent::RemoveOverlappingSpawnAreas(TArray<USpawnArea*>& ValidSpawnAreas,
	const FVector& Scale) const
{
	TArray<FVector> OverlappingVertices;
	TArray<FVector> DebugVertices;

	// Consider Managed Targets to be invalid if runtime
	TArray<USpawnArea*> InvalidSpawnAreas = ShouldConsiderManagedAsInvalid()
		? GetManagedActivatedOrRecentSpawnAreas()
		: GetActivatedOrRecentSpawnAreas();


	for (const USpawnArea* SpawnArea : InvalidSpawnAreas)
	{
		// Regenerate Overlapping vertices if necessary
		if (Scale.Length() > SpawnArea->GetTargetScale().Length())
		{
			TArray<FVector> ScaledOverlappingPoints = SpawnArea->GenerateOverlappingVertices(
				GetTc().MinDistanceBetweenTargets, SpawnArea->GetTargetScale(), DebugVertices,
				bShowDebug_OverlappingVertices_Dynamic);
			for (const FVector& Vector : ScaledOverlappingPoints)
			{
				OverlappingVertices.AddUnique(Vector);
				#if !UE_BUILD_SHIPPING
				if (bShowDebug_OverlappingVertices_Dynamic)
				{
					DrawDebugPoint(GetWorld(), Vector, 10.f, FColor::Red, false, 0.5f);
				}
				#endif
			}
			#if !UE_BUILD_SHIPPING
			if (bShowDebug_OverlappingVertices_Dynamic) DrawOverlappingVertices(SpawnArea, Scale, DebugVertices);
			#endif
		}
		else
		{
			for (const FVector& Vector : SpawnArea->GetOverlappingVertices())
			{
				OverlappingVertices.AddUnique(Vector);
				#if !UE_BUILD_SHIPPING
				if (bShowDebug_OverlappingVertices_Dynamic)
				{
					DrawDebugPoint(GetWorld(), Vector, 10.f, FColor::Red, false, 0.5f);
				}
				#endif
			}
			#if !UE_BUILD_SHIPPING
			if (bShowDebug_OverlappingVertices_Dynamic) DrawOverlappingVertices(SpawnArea, Scale, DebugVertices);
			#endif
		}
	}
	ValidSpawnAreas = ValidSpawnAreas.FilterByPredicate([&OverlappingVertices](const USpawnArea* SpawnArea)
	{
		return OverlappingVertices.Contains(SpawnArea->GetBottomLeftVertex()) ? false : true;
	});

	#if !UE_BUILD_SHIPPING
	if (bShowDebug_OverlappingVertices_Dynamic)
	{
		DrawDebug_Boxes(OverlappingVertices, FColor::Red, 4, 3);
	}
	#endif
}

void USpawnAreaManagerComponent::FilterByExtrema(TArray<USpawnArea*>& ValidSpawnAreas, const FExtrema& Extrema) const
{
	TArray<const USpawnArea*> RemovedSpawnAreas;
	ValidSpawnAreas = SpawnAreas.FilterByPredicate([&](const USpawnArea* SpawnArea)
	{
		const FVector Vector = SpawnArea->GetBottomLeftVertex();
		if (Vector.Y < Extrema.Min.Y || Vector.Y >= Extrema.Max.Y || Vector.Z < Extrema.Min.Z || Vector.Z >= Extrema.Max
			.Z)
		{
			if (bShowDebug_RemovedSpawnLocations)
			{
				RemovedSpawnAreas.Add(SpawnArea);
			}
			return false;
		}
		return true;
	});

	if (bShowDebug_RemovedSpawnLocations)
	{
		DrawDebug_Boxes(RemovedSpawnAreas, FColor::Red, 4, 3);
	}
}

void USpawnAreaManagerComponent::FilterByEdgeOnly(TArray<USpawnArea*>& ValidSpawnAreas, const FExtrema& Extrema) const
{
	const float MaxY = Extrema.Max.Y - SpawnAreaInc.Y;
	const float MaxZ = Extrema.Max.Z - SpawnAreaInc.Z;

	const float MinY = Extrema.Min.Y;
	const float MinZ = Extrema.Min.Z;

	TArray<const USpawnArea*> RemovedSpawnAreas;
	ValidSpawnAreas = SpawnAreas.FilterByPredicate([&](const USpawnArea* SpawnArea)
	{
		const FVector Vector = SpawnArea->GetBottomLeftVertex();
		if (FMath::IsNearlyEqual(Vector.Y, MinY, 0.1f) || FMath::IsNearlyEqual(Vector.Y, MaxY, 0.1f))
		{
			if (FMath::IsNearlyEqual(Vector.Z, MinZ, 0.1f) || FMath::IsNearlyEqual(Vector.Z, MaxZ, 0.1f))
			{
				return true;
			}
		}
		else if (FMath::IsNearlyEqual(Vector.Z, MinZ, 0.1f) || FMath::IsNearlyEqual(Vector.Z, MaxZ, 0.1f))
		{
			if (FMath::IsNearlyEqual(Vector.Y, MinY, 0.1f) || FMath::IsNearlyEqual(Vector.Y, MaxY, 0.1f))
			{
				return true;
			}
		}
		if (bShowDebug_RemovedSpawnLocations) RemovedSpawnAreas.Add(SpawnArea);
		return false;
	});
	ValidSpawnAreas.AddUnique(GetOriginSpawnArea());

	#if !UE_BUILD_SHIPPING
	if (bShowDebug_RemovedSpawnLocations)
	{
		DrawDebug_Boxes(RemovedSpawnAreas, FColor::Red, 4, 3);
	}
	#endif
}

TArray<int32> USpawnAreaManagerComponent::FilterManagedIndices(TArray<USpawnArea*>& ValidSpawnAreas) const
{
	return FilterIndices(ValidSpawnAreas, &USpawnArea::IsManaged, bShowDebug_FilteredManagedIndices, FColor::Blue);
}

TArray<int32> USpawnAreaManagerComponent::FilterActivatedIndices(TArray<USpawnArea*>& ValidSpawnAreas) const
{
	return FilterIndices(ValidSpawnAreas, &USpawnArea::IsActivated, bShowDebug_FilteredActivatedIndices, FColor::Cyan);
}

TArray<int32> USpawnAreaManagerComponent::FilterBorderingIndices(TArray<USpawnArea*>& ValidSpawnAreas,
	const USpawnArea* Current) const
{
	TArray<int32> Removed;
	if (!Current) return Removed;
	
	const TArray<int32> BorderingIndices = Current->GetBorderingIndices();
	ValidSpawnAreas = ValidSpawnAreas.FilterByPredicate([&] (const USpawnArea* SpawnArea)
	{
		const int32 CurrentIndex = SpawnArea->GetIndex();
		if (!BorderingIndices.Contains(SpawnArea->GetIndex()))
		{
			Removed.Add(CurrentIndex);
			return false;
		}
		return true;
	});

	return Removed;
}

TArray<int32> USpawnAreaManagerComponent::FilterRecentIndices(TArray<USpawnArea*>& ValidSpawnAreas) const
{
	return FilterIndices(ValidSpawnAreas, &USpawnArea::IsRecent, bShowDebug_FilteredRecentIndices, FColor::Turquoise);
}

TArray<int32> USpawnAreaManagerComponent::FilterIndices(TArray<USpawnArea*>& ValidSpawnAreas,
	bool (USpawnArea::*FilterFunc)() const, const bool bShowDebug, const FColor& DebugColor) const
{
	TArray<int32> Removed;
	ValidSpawnAreas = ValidSpawnAreas.FilterByPredicate([&](const USpawnArea* SpawnArea)
	{
		if ((SpawnArea->*FilterFunc)())
		{
			Removed.Add(SpawnArea->GetIndex());
			return false;
		}
		return true;
	});
	#if !UE_BUILD_SHIPPING
	if (bShowDebug)
	{
		DrawDebug_Boxes(Removed, DebugColor, 4, 3);
	}
	#endif
	return Removed;
}

/* ---------------- */
/* -- Util/Debug -- */
/* ---------------- */

TArray<int32> USpawnAreaManagerComponent::CreateIndexValidityArray(const TArray<int32>& RemovedIndices) const
{
	TArray<int32> IndexValidity;
	IndexValidity.Init(1, SpawnAreas.Num());
	for (const int32 RemovedIndex : RemovedIndices) IndexValidity[RemovedIndex] = 0;

	FString Line;
	UE_LOG(LogTargetManager, Display, TEXT("Index Validity:"));
	for (int i = 0; i < IndexValidity.Num(); i++)
	{
		Line += FString::FromInt(IndexValidity[i]) + " ";
		if ((i + 1) % Size.Y == 0 && i + 1 >= Size.Y)
		{
			UE_LOG(LogTargetManager, Display, TEXT("%s"), *Line);
			Line.Empty();
		}
	}
	return IndexValidity;
}

FAccuracyData USpawnAreaManagerComponent::GetLocationAccuracy()
{
	TArray<int32> TotalSpawns;
	TArray<int32> TotalHits;

	TotalSpawns.Init(-1, SpawnAreas.Num());
	TotalHits.Init(0, SpawnAreas.Num());

	int32 TotalSpawnsValueRef = 0;
	int32 TotalHitsValueRef = 0;
	int32 TotalSpawnsValueTest = 0;
	int32 TotalHitsValueTest = 0;

	const bool bHitDamage = GetTc().TargetDamageType == ETargetDamageType::Hit;
	// For now only handle separate Hit and Tracking Damage
	int32 (USpawnArea::*TotalFunc)() const = bHitDamage
		? &USpawnArea::GetTotalSpawns
		: &USpawnArea::GetTotalTrackingDamagePossible;
	int32 (USpawnArea::*HitFunc)() const = bHitDamage ? &USpawnArea::GetTotalHits : &USpawnArea::GetTotalTrackingDamage;

	for (int i = 0; i < SpawnAreas.Num(); i++)
	{
		const int32 SpawnsValue = (SpawnAreas[i]->*TotalFunc)();
		const int32 HitsValue = (SpawnAreas[i]->*HitFunc)();

		TotalSpawns[i] = SpawnsValue;
		TotalHits[i] = HitsValue;

		#if !UE_BUILD_SHIPPING
		if (SpawnsValue >= 0)
		{
			TotalSpawnsValueRef += SpawnsValue;
		}
		if (HitsValue > 0)
		{
			TotalHitsValueRef += HitsValue;
		}
		#endif
	}

	FAccuracyData OutData = GetAveragedAccuracyData(TotalSpawns, TotalHits, Size.Z, Size.Y);

	OutData.SpawnAreaSize = FVector(Size.X, Size.Y, Size.Z);
	OutData.CalculateAccuracy();

	if (Size.Y < 5 || Size.Z < 5)
	{
		OutData.ModifyForSmallerInput();
	}

	#if !UE_BUILD_SHIPPING
	for (int i = 0; i < OutData.AccuracyRows.Num(); i++)
	{
		for (int j = 0; j < OutData.AccuracyRows[i].Size; j++)
		{
			const int32 TotalSpawnsValue = OutData.AccuracyRows[i].TotalSpawns[j];
			const int32 TotalHitsValue = OutData.AccuracyRows[i].TotalHits[j];

			if (TotalSpawnsValue > 0)
			{
				TotalSpawnsValueTest += TotalSpawnsValue;
			}
			if (TotalHitsValue > 0)
			{
				TotalHitsValueTest += TotalHitsValue;
			}
		}
	}
	UE_LOG(LogTargetManager, Display, TEXT("Total Spawns/Hits Ref: %d %d"), TotalSpawnsValueRef, TotalHitsValueRef);
	UE_LOG(LogTargetManager, Display, TEXT("Total Spawns/Hits Test: %d %d"), TotalSpawnsValueTest, TotalHitsValueTest);
	#endif

	return OutData;
}

void USpawnAreaManagerComponent::DrawDebug_AllSpawnAreas() const
{
	DrawDebug_Boxes(GetAllBottomLeftVertices(), FColor::Cyan, 4, 0, true);
}

void USpawnAreaManagerComponent::ClearDebug_AllSpawnAreas() const
{
	FlushPersistentDebugLines(GetWorld());
}

void USpawnAreaManagerComponent::DrawDebug_Boxes(const TArray<int32>& InIndices, const FColor& InColor,
	const int32 InThickness, const int32 InDepthPriority, const bool bPersistantLines) const
{
	const float Time = bPersistantLines ? -1.f : GetTc().TargetSpawnCD;
	const FVector HalfInc = FVector(0, GetSpawnAreaInc().Y * 0.5f, GetSpawnAreaInc().Z * 0.5f);
	for (const int32 Index : InIndices)
	{
		const USpawnArea* SpawnArea = FindSpawnAreaFromIndex(Index);
		if (!SpawnArea) continue;
		FVector Loc = SpawnArea->GetBottomLeftVertex() + HalfInc;
		DrawDebugBox(GetWorld(), Loc, FVector(0, HalfInc.Y, HalfInc.Z), InColor, bPersistantLines, Time,
			InDepthPriority, InThickness);
	}
}

void USpawnAreaManagerComponent::DrawDebug_Boxes(const TArray<FVector>& InLocations, const FColor& InColor,
	const int32 InThickness, const int32 InDepthPriority, const bool bPersistantLines) const
{
	const float Time = bPersistantLines ? -1.f : GetTc().TargetSpawnCD;
	const FVector HalfInc = FVector(0, GetSpawnAreaInc().Y * 0.5f, GetSpawnAreaInc().Z * 0.5f);
	for (const FVector& Vector : InLocations)
	{
		FVector Loc = FVector(Vector.X, Vector.Y + HalfInc.Y, Vector.Z + HalfInc.Z);
		DrawDebugBox(GetWorld(), Loc, FVector(0, HalfInc.Y, HalfInc.Z), InColor, bPersistantLines, Time,
			InDepthPriority, InThickness);
	}
}

void USpawnAreaManagerComponent::DrawDebug_Boxes(const TArray<const USpawnArea*>& InSpawnAreas, const FColor& InColor,
	const int32 InThickness, const int32 InDepthPriority, const bool bPersistantLines) const
{
	const float Time = bPersistantLines ? -1.f : GetTc().TargetSpawnCD;
	const FVector HalfInc = FVector(0, GetSpawnAreaInc().Y * 0.5f, GetSpawnAreaInc().Z * 0.5f);
	for (const USpawnArea* SpawnArea : InSpawnAreas)
	{
		FVector Loc = SpawnArea->GetBottomLeftVertex() + HalfInc;
		DrawDebugBox(GetWorld(), Loc, FVector(0, HalfInc.Y, HalfInc.Z), InColor, bPersistantLines, Time,
			InDepthPriority, InThickness);
	}
}

void USpawnAreaManagerComponent::DrawDebug_Boxes(const TArray<USpawnArea*>& InSpawnAreas, const FColor& InColor,
	const int32 InThickness, const int32 InDepthPriority, const bool bPersistantLines) const
{
	const float Time = bPersistantLines ? -1.f : GetTc().TargetSpawnCD;
	const FVector HalfInc = FVector(0, GetSpawnAreaInc().Y * 0.5f, GetSpawnAreaInc().Z * 0.5f);
	for (const USpawnArea* SpawnArea : InSpawnAreas)
	{
		FVector Loc = SpawnArea->GetBottomLeftVertex() + HalfInc;
		DrawDebugBox(GetWorld(), Loc, FVector(0, HalfInc.Y, HalfInc.Z), InColor, bPersistantLines, Time,
			InDepthPriority, InThickness);
	}
}

void USpawnAreaManagerComponent::DrawOverlappingVertices(const USpawnArea* SpawnArea, const FVector& Scale,
	const TArray<FVector>& DebugVertices) const
{
	const float ScaledRadius = Scale.X * SphereTargetRadius;
	float Radius = ScaledRadius * 2.f + (GetTc().MinDistanceBetweenTargets / 2.f);
	Radius = FMath::Max(Radius, SpawnArea->GetMinOverlapRadius()) + FMath::Max(Size.Y, Size.Z);
	DrawDebugSphere(GetWorld(), SpawnArea->ChosenPoint, Radius, 16, FColor::Magenta, false, 0.5f);

	for (FVector Vertex : DebugVertices)
	{
		DrawDebugPoint(GetWorld(), Vertex, 10.f, FColor::Green, false, 0.5f);
	}
}

void USpawnAreaManagerComponent::PrintDebug_SpawnArea(const USpawnArea* SpawnArea)
{
	UE_LOG(LogTargetManager, Display, TEXT("SpawnArea:"));
	UE_LOG(LogTargetManager, Display, TEXT("Index %d IndexType %s"), SpawnArea->Index,
		*UEnum::GetDisplayValueAsText(SpawnArea->GetIndexType()).ToString());
	UE_LOG(LogTargetManager, Display, TEXT("Vertex_BottomLeft: %s CenterPoint: %s ChosenPoint: %s"),
		*SpawnArea->Vertex_BottomLeft.ToCompactString(), *SpawnArea->CenterPoint.ToCompactString(),
		*SpawnArea->ChosenPoint.ToCompactString());
	FString String;
	for (const int32 Border : SpawnArea->GetBorderingIndices())
	{
		String.Append(" " + FString::FromInt(Border));
	}

	UE_LOG(LogTargetManager, Display, TEXT("AdjacentIndices %s"), *String);
	UE_LOG(LogTargetManager, Display, TEXT("IsActivated %hhd IsRecent %hhd"), SpawnArea->IsActivated(),
		SpawnArea->IsRecent());
	UE_LOG(LogTargetManager, Display, TEXT("TotalSpawns %d TotalHits %d"), SpawnArea->GetTotalSpawns(),
		SpawnArea->GetTotalHits());
}

void USpawnAreaManagerComponent::PrintDebug_SpawnAreaDist(const USpawnArea* SpawnArea) const
{
	const float MaxAllowedDistance = SpawnArea->GetTargetScale().X * SphereTargetRadius;
	for (const USpawnArea* ActivatedOrRecent : GetActivatedOrRecentSpawnAreas())
	{
		const double Distance = FVector::Distance(ActivatedOrRecent->ChosenPoint, SpawnArea->GetChosenPoint());

		if (Distance < MaxAllowedDistance)
		{
			UE_LOG(LogTemp, Display, TEXT("Distance between targets (%.2f) less than max allowed: %.2f"), Distance,
				MaxAllowedDistance);
		}
	}
}
