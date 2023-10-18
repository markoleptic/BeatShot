// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Target/SpawnAreaManagerComponent.h"
#include <stack>
#include "GlobalConstants.h"
#include "Algo/RandomShuffle.h"
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
	GridIndexType = EGridIndexType::None;
	OverlappingVertices = TSet<FVector>();
	AdjacentIndexMap = TMap<EBorderingDirection, int32>();
	Size = 0;
	Guid = FGuid();
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
	GridIndexType = FindIndexType(InParams.Index, Size, InParams.NumHorizontalTargets);
	SetAdjacentIndices(GridIndexType, Index, InParams.NumHorizontalTargets);
	OverlappingVertices = TSet<FVector>();
}

void USpawnArea::SetAdjacentIndices(const EGridIndexType InGridIndexType, const int32 InIndex, const int32 InWidth)
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

void USpawnArea::SetIsCurrentlyManaged(const bool bSetIsCurrentlyManaged)
{
	bIsCurrentlyManaged = bSetIsCurrentlyManaged;
}

void USpawnArea::SetIsActivated(const bool bActivated, const bool bAllow)
{
	bIsActivated = bActivated;
	bAllowActivationWhileActivated = bAllow;
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

void USpawnArea::SetTargetScale(const FVector& InScale)
{
	TargetScale = InScale;
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

void USpawnArea::SetInvalidVerts(const TSet<FVector>& InVerts)
{
	OverlappingVertices = InVerts;
}

TSet<FVector> USpawnArea::MakeInvalidVerts(const float InMinDist, const FVector& InScale, TSet<FVector>& OutValid,
	const bool bAddValidVertices) const
{
	TSet<FVector> OutInvalid;

	const float ScaledRadius = InScale.X * SphereTargetRadius;
	// multiply by two so that any point outside the sphere will not be overlapping
	float Radius = ScaledRadius * 2.f + (InMinDist * 0.5f);
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
				OutInvalid.Add(Loc);
			}
			else
			{
				if (bAddValidVertices)
				{
					OutValid.Emplace(FVector(ChosenPoint.X, Y, Z));
				}
			}
			Count++;
		}
	}
	return OutInvalid;
}

void USpawnArea::EmptyOverlappingVertices()
{
	OverlappingVertices.Empty();
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

	bDebug_Valid = false;
	bDebug_FilterRecent = false;
	bDebug_FilterActivated = false;
	bDebug_FilterManaged = false;
	bDebug_Removed = false;
	bDebug_ManagedVerts = false;
	bDebug_ActivatedVerts = false;
	bDebug_AllVerts = false;
	bDebug_Grid = false;
	bDebug_FilterBordering = false;

	SpawnAreas = TArray<USpawnArea*>();
	AreaKeyMap = TMap<FAreaKey, USpawnArea*>();
	GuidMap = TMap<FGuid, USpawnArea*>();
	CachedManaged = TSet<USpawnArea*>();
	CachedActivated = TSet<USpawnArea*>();
	CachedDeactivated = TSet<USpawnArea*>();
	CachedRecent = TSet<USpawnArea*>();
	CachedExtrema = TSet<USpawnArea*>();
	CachedEdgeOnly = TSet<USpawnArea*>();
	MostRecentSpawnArea = nullptr;
	OriginSpawnArea = nullptr;
	Size = FIntVector3();
	SpawnAreaInc = FIntVector3();
	SpawnAreaScale = FVector();
	bShouldAskRLCForSpawnAreas = false;
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
	InitializeSpawnAreas();

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

void USpawnAreaManagerComponent::SetAppropriateSpawnMemoryValues()
{
	const int32 HalfWidth = StaticExtents.Y;
	const int32 HalfHeight = StaticExtents.Z;
	bool bWidthScaleSelected = false;
	bool bHeightScaleSelected = false;

	switch (GetTargetCfg().TargetDistributionPolicy)
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
			const float MaxTargetSize = GetTargetCfg().MaxSpawnedTargetScale * SphereTargetDiameter;
			SpawnAreaInc.Y = GetBSConfig()->GridConfig.GridSpacing.X + MaxTargetSize;
			SpawnAreaInc.Z = GetBSConfig()->GridConfig.GridSpacing.Y + MaxTargetSize;
			SpawnAreaScale.Y = 1.f / SpawnAreaInc.Y;
			SpawnAreaScale.Z = 1.f / SpawnAreaInc.Z;
		}
		break;
	}
}

void USpawnAreaManagerComponent::InitializeSpawnAreas()
{
	int SizeY = 0;
	int SizeZ = 0;
	int Index = 0;

	FSpawnAreaParams Params;
	Params.NumHorizontalTargets = GetBSConfig()->GridConfig.NumHorizontalGridTargets;
	Params.NumVerticalTargets = GetBSConfig()->GridConfig.NumVerticalGridTargets;
	Params.bGrid = GetTargetCfg().TargetDistributionPolicy == ETargetDistributionPolicy::Grid;
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

			AreaKeyMap.Emplace(FAreaKey(Loc, SpawnAreaInc), SpawnArea);
			SpawnAreas.Add(SpawnArea);
			CachedExtrema.Add(SpawnArea);
			Index++;
			SizeY++;
		}
		SizeZ++;
	}
	Size.Y = SizeY;
	Size.Z = SizeZ;
}

void USpawnAreaManagerComponent::Clear()
{
	BSConfig = nullptr;

	bDebug_Valid = false;
	bDebug_FilterRecent = false;
	bDebug_FilterActivated = false;
	bDebug_FilterManaged = false;
	bDebug_Removed = false;
	bDebug_ManagedVerts = false;
	bDebug_AllVerts = false;
	bDebug_Grid = false;
	bDebug_FilterBordering = false;

	SpawnAreas.Empty();
	AreaKeyMap.Empty();
	GuidMap.Empty();
	CachedManaged.Empty();
	CachedActivated.Empty();
	CachedDeactivated.Empty();
	CachedRecent.Empty();
	CachedExtrema.Empty();
	CachedEdgeOnly.Empty();

	MostRecentSpawnArea = nullptr;
	OriginSpawnArea = nullptr;
	Size = FIntVector3();
	SpawnAreaInc = FIntVector3();
	SpawnAreaScale = FVector();
	bShouldAskRLCForSpawnAreas = false;
}

bool USpawnAreaManagerComponent::ShouldConsiderManagedAsInvalid() const
{
	return GetTargetCfg().TargetSpawningPolicy == ETargetSpawningPolicy::RuntimeOnly;
}

bool USpawnAreaManagerComponent::ShouldForceSpawnAtOrigin() const
{
	if (GetTargetCfg().bSpawnEveryOtherTargetInCenter)
	{
		if (GetMostRecentSpawnArea() != GetOriginSpawnArea()) return true;
	}
	return false;
}

void USpawnAreaManagerComponent::UpdateTotalTrackingDamagePossible(const FVector& InLocation) const
{
	if (USpawnArea* SpawnArea = FindSpawnAreaFromLocation(InLocation))
	{
		SpawnArea->IncrementTotalTrackingDamagePossible();
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
	switch (GetTargetCfg().RecentTargetMemoryPolicy)
	{
	case ERecentTargetMemoryPolicy::None:
		RemoveRecentFlagFromSpawnArea(SpawnArea);
		break;
	case ERecentTargetMemoryPolicy::CustomTimeBased:
		{
			RemoveFromRecentDelegate.BindUObject(this, &USpawnAreaManagerComponent::RemoveRecentFlagFromSpawnArea,
				SpawnArea);
			const float Time = GetTargetCfg().RecentTargetTimeLength;
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
			const float Time = GetTargetCfg().TargetSpawnCD;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, RemoveFromRecentDelegate, Time, false);
		}
		break;
	}
}

/* ------------------------------- */
/* -- SpawnArea finders/getters -- */
/* ------------------------------- */

USpawnArea* USpawnAreaManagerComponent::FindSpawnAreaFromLocation(const FVector& InLocation) const
{
	// Adjust for the SpawnAreaInc being aligned to the BoxBounds Origin
	const FVector RelativeLocation = InLocation - Origin;

	// Snap to lowest SpawnAreaInc (49.9 -> 0 if SpawnAreaInc value is 50)
	const int32 GridY = Origin.Y + SpawnAreaInc.Y * FMath::FloorToInt(RelativeLocation.Y / SpawnAreaInc.Y);
	const int32 GridZ = Origin.Z + SpawnAreaInc.Z * FMath::FloorToInt(RelativeLocation.Z / SpawnAreaInc.Z);

	const auto Found = AreaKeyMap.Find(FAreaKey(FVector(0, GridY, GridZ), SpawnAreaInc));
	return Found ? *Found : nullptr;
}

USpawnArea* USpawnAreaManagerComponent::FindSpawnAreaFromGuid(const FGuid& TargetGuid) const
{
	const auto Found = GuidMap.Find(TargetGuid);
	return Found ? *Found : nullptr;
}

USpawnArea* USpawnAreaManagerComponent::FindOldestRecentSpawnArea() const
{
	TSet<USpawnArea*> RecentSpawnAreas = GetRecentSpawnAreas();

	if (RecentSpawnAreas.IsEmpty())
	{
		return nullptr;
	}

	USpawnArea* MostRecent = nullptr;

	for (USpawnArea* SpawnArea : RecentSpawnAreas)
	{
		if (!MostRecent)
		{
			MostRecent = SpawnArea;
			continue;
		}
		if (SpawnArea->GetTimeSetRecent() < MostRecent->GetTimeSetRecent())
		{
			MostRecent = SpawnArea;
		}
	}
	return MostRecent;
}

USpawnArea* USpawnAreaManagerComponent::FindOldestDeactivatedManagedSpawnArea() const
{
	TSet<USpawnArea*> Areas = GetDeactivatedManagedSpawnAreas();

	USpawnArea* MostRecent = nullptr;

	for (USpawnArea* SpawnArea : GetDeactivatedManagedSpawnAreas())
	{
		if (!MostRecent)
		{
			MostRecent = SpawnArea;
			continue;
		}
		if (SpawnArea->GetTimeSetRecent() < MostRecent->GetTimeSetRecent())
		{
			MostRecent = SpawnArea;
		}
	}
	return MostRecent;
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

/* ------------------------------------ */
/* -- TSet SpawnArea finders/getters -- */
/* ------------------------------------ */

TSet<USpawnArea*> USpawnAreaManagerComponent::GetManagedSpawnAreas() const
{
	return CachedManaged;
}

TSet<USpawnArea*> USpawnAreaManagerComponent::GetDeactivatedManagedSpawnAreas() const
{
	return CachedDeactivated;
}

TSet<USpawnArea*> USpawnAreaManagerComponent::GetRecentSpawnAreas() const
{
	return CachedRecent;
}

TSet<USpawnArea*> USpawnAreaManagerComponent::GetActivatedSpawnAreas() const
{
	return CachedActivated;
}

TSet<USpawnArea*> USpawnAreaManagerComponent::GetActivatedOrRecentSpawnAreas() const
{
	TSet<USpawnArea*> Out = CachedActivated;
	Out.Append(CachedRecent);
	return Out;
}

TSet<USpawnArea*> USpawnAreaManagerComponent::GetManagedActivatedOrRecentSpawnAreas() const
{
	TSet<USpawnArea*> Out = CachedManaged;
	Out.Append(CachedActivated);
	Out.Append(CachedRecent);
	return Out;
}

/* ------------------------ */
/* -- SpawnArea flagging -- */
/* ------------------------ */

void USpawnAreaManagerComponent::FlagSpawnAreaAsManaged(USpawnArea* SpawnArea, const FGuid TargetGuid)
{
	if (!SpawnArea) return;

	if (SpawnArea->IsManaged())
	{
		UE_LOG(LogTargetManager, Warning, TEXT("Tried to flag an already managed SpawnArea as managed."));
		return;
	}

	// Add to caches and GuidMap
	GuidMap.Add(TargetGuid, SpawnArea);
	CachedManaged.Add(SpawnArea);
	CachedDeactivated.Add(SpawnArea);
	SpawnArea->SetGuid(TargetGuid);
	SpawnArea->SetIsCurrentlyManaged(true);

	// Don't generate new OverlappingVertices if they're already generated
	if (!SpawnArea->OverlappingVertices.IsEmpty()) return;

	TSet<FVector> Valid;
	const FVector Scale = SpawnArea->GetTargetScale();
	const TSet<FVector> Invalid = SpawnArea->MakeInvalidVerts(GetMinDist(), Scale, Valid, bDebug_ManagedVerts);
	SpawnArea->SetInvalidVerts(Invalid);

	#if !UE_BUILD_SHIPPING
	if (bDebug_ManagedVerts) DrawVerticesOverlap(SpawnArea, Scale, Valid, Invalid);
	#endif
}

void USpawnAreaManagerComponent::FlagSpawnAreaAsActivated(const FGuid TargetGuid, const bool bCanActivateWhileActivated)
{
	USpawnArea* SpawnArea = FindSpawnAreaFromGuid(TargetGuid);
	if (!SpawnArea)
	{
		UE_LOG(LogTargetManager, Warning, TEXT("Failed to find target from Guid to activate."));
		return;
	}

	// Ignore already activated target that can be reactivated
	if (SpawnArea->CanActivateWhileActivated() && SpawnArea->IsActivated()) return;

	// Should no longer be considered recent if activated
	if (SpawnArea->IsRecent()) RemoveRecentFlagFromSpawnArea(SpawnArea);

	if (SpawnArea->IsActivated())
	{
		UE_LOG(LogTargetManager, Warning, TEXT("Tried to flag as Activated when already Activated."));
		return;
	}

	// Add to activated cache
	CachedActivated.Add(SpawnArea);

	// Remove from deactivated cache
	const int32 NumRemoved = CachedDeactivated.Remove(SpawnArea);
	if (NumRemoved == 0) UE_LOG(LogTemp, Display, TEXT("Failed to remove from CachedDeactivated."));

	SpawnArea->SetIsActivated(true, bCanActivateWhileActivated);

	// Don't generate new OverlappingVertices if they're already generated
	if (!SpawnArea->OverlappingVertices.IsEmpty()) return;

	TSet<FVector> Valid;
	const FVector Scale = SpawnArea->GetTargetScale();
	const TSet<FVector> Invalid = SpawnArea->MakeInvalidVerts(GetMinDist(), Scale, Valid, bDebug_ActivatedVerts);
	SpawnArea->SetInvalidVerts(Invalid);

	#if !UE_BUILD_SHIPPING
	if (bDebug_ActivatedVerts) DrawVerticesOverlap(SpawnArea, Scale, Valid, Invalid);
	#endif
}

void USpawnAreaManagerComponent::FlagSpawnAreaAsRecent(USpawnArea* SpawnArea)
{
	if (!SpawnArea) return;

	if (SpawnArea->IsRecent())
	{
		UE_LOG(LogTargetManager, Warning, TEXT("Tried to flag as Recent when already Recent."));
		return;
	}

	// Add to cache
	CachedRecent.Add(SpawnArea);

	SpawnArea->SetIsRecent(true);
}

void USpawnAreaManagerComponent::RemoveManagedFlagFromSpawnArea(const FGuid TargetGuid)
{
	USpawnArea* SpawnArea = FindSpawnAreaFromGuid(TargetGuid);
	if (!SpawnArea)
	{
		UE_LOG(LogTargetManager, Warning, TEXT("Failed to find target by Guid to remove from managed."));
		return;
	}

	const int32 NumRemoved = GuidMap.Remove(TargetGuid);
	const int32 NumRemovedDeactivated = CachedDeactivated.Remove(SpawnArea);
	const int32 NumRemovedManaged = CachedManaged.Remove(SpawnArea);

	if (!SpawnArea->IsManaged())
	{
		UE_LOG(LogTargetManager, Warning, TEXT("Tried to remove managed flag from from non-managed SpawnArea."));
		return;
	}

	if (NumRemoved == 0) UE_LOG(LogTargetManager, Warning, TEXT("Failed to remove from TargetGuidToSpawnArea map."));
	if (NumRemovedDeactivated == 0) UE_LOG(LogTargetManager, Warning, TEXT("Failed to remove from CachedDeactivated."));
	if (NumRemovedManaged == 0) UE_LOG(LogTargetManager, Warning, TEXT("Failed to remove from CachedManaged."));

	SpawnArea->SetIsCurrentlyManaged(false);
	SpawnArea->ResetGuid();
}

void USpawnAreaManagerComponent::RemoveActivatedFlagFromSpawnArea(USpawnArea* SpawnArea)
{
	if (!SpawnArea) return;

	// Remove from activated cache
	const int32 NumRemovedFromCache = CachedActivated.Remove(SpawnArea);

	// Add to deactivated cache
	CachedDeactivated.Add(SpawnArea);

	if (!SpawnArea->IsActivated())
	{
		UE_LOG(LogTargetManager, Warning, TEXT("Tried to remove an activated flag from non-activated SpawnArea."));
		return;
	}

	if (NumRemovedFromCache == 0) UE_LOG(LogTargetManager, Warning, TEXT("Failed to remove from CachedActivated set."));

	SpawnArea->SetIsActivated(false);
}

void USpawnAreaManagerComponent::RemoveRecentFlagFromSpawnArea(USpawnArea* SpawnArea)
{
	if (!SpawnArea) return;

	const int32 NumRemovedFromCache = CachedRecent.Remove(SpawnArea);

	if (!SpawnArea->IsRecent())
	{
		UE_LOG(LogTargetManager, Warning, TEXT("Tried to remove a recent flag from non-recent SpawnArea."));
		return;
	}

	if (NumRemovedFromCache == 0) UE_LOG(LogTargetManager, Warning, TEXT("Failed to remove from CachedRecent set."));

	SpawnArea->SetIsRecent(false);
}

void USpawnAreaManagerComponent::RefreshRecentFlags()
{
	if (GetTargetCfg().RecentTargetMemoryPolicy != ERecentTargetMemoryPolicy::NumTargetsBased) return;

	const int32 NumToRemove = GetRecentSpawnAreas().Num() - GetTargetCfg().MaxNumRecentTargets;
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
	TArray<USpawnArea*> ChosenSpawnAreas;
	TArray<USpawnArea*> ValidSpawnAreas = GetDeactivatedManagedSpawnAreas().Array();
	TArray<USpawnArea*> Filtered = ValidSpawnAreas;
	USpawnArea* PreviousSpawnArea = GetMostRecentSpawnArea();

	// Can skip a lot if forcing every other target in center
	if (ShouldForceSpawnAtOrigin() && NumToActivate == 1)
	{
		USpawnArea* SpawnArea = GetOriginSpawnArea();
		ChosenSpawnAreas.Add(SpawnArea);
		return ChosenSpawnAreas;
	}

	switch (GetTargetCfg().TargetActivationSelectionPolicy)
	{
	case ETargetActivationSelectionPolicy::Bordering:
		{
			FilterBorderingIndices(Filtered, PreviousSpawnArea);
			if (Filtered.Num() >= NumToActivate) ValidSpawnAreas = Filtered;
		}
		break;
	case ETargetActivationSelectionPolicy::None:
	case ETargetActivationSelectionPolicy::Random:
		break;
	}

	// Always try to filter recent indices
	FilterRecentIndices(Filtered);
	if (Filtered.Num() > NumToActivate) ValidSpawnAreas = Filtered;

	// Main loop for choosing spawn areas
	for (int i = 0; i < NumToActivate; i++)
	{
		// Add the origin if settings permit
		if (GetBSConfig()->TargetConfig.bSpawnAtOriginWheneverPossible && ValidSpawnAreas.
			Contains(GetOriginSpawnArea()))
		{
			// Add to the return array
			ChosenSpawnAreas.Add(GetOriginSpawnArea());

			// Remove from options available to choose and push back the start index
			ValidSpawnAreas.Remove(GetOriginSpawnArea());

			// Set as the previous SpawnArea since it will be spawned before any chosen later
			PreviousSpawnArea = GetOriginSpawnArea();
		}
		// Let RLC choose the SpawnArea if settings permit
		else if (bShouldAskRLCForSpawnAreas && RequestRLCSpawnArea.IsBound())
		{
			USpawnArea* Chosen = RequestRLCSpawnArea.Execute(ValidSpawnAreas, PreviousSpawnArea);
			if (!Chosen)
			{
				UE_LOG(LogTargetManager, Warning, TEXT("Unable to Spawn at SpawnArea suggested by RLAgent."));
				continue;
			}

			ChosenSpawnAreas.Add(Chosen);
			ValidSpawnAreas.Remove(Chosen);
			PreviousSpawnArea = Chosen;
		}
		else
		{
			const int32 RandomIndex = FMath::RandRange(0, ValidSpawnAreas.Num() - 1);
			if (!ValidSpawnAreas.IsValidIndex(i)) continue;

			USpawnArea* Chosen = ValidSpawnAreas[RandomIndex];
			ChosenSpawnAreas.Add(Chosen);
			ValidSpawnAreas.Remove(Chosen);
			PreviousSpawnArea = Chosen;
		}
	}

	return ChosenSpawnAreas;
}

TArray<USpawnArea*> USpawnAreaManagerComponent::GetSpawnableSpawnAreas(const TArray<FVector>& Scales,
	const int32 NumToSpawn) const
{
	switch (GetTargetCfg().TargetDistributionPolicy)
	{
	case ETargetDistributionPolicy::Grid:
		return GetSpawnableSpawnAreas_Grid(Scales, NumToSpawn);
	case ETargetDistributionPolicy::None:
	case ETargetDistributionPolicy::FullRange:
	case ETargetDistributionPolicy::EdgeOnly:
	case ETargetDistributionPolicy::HeadshotHeightOnly: default:
		return GetSpawnableSpawnAreas_NonGrid(Scales, NumToSpawn);
	}
}

// TODO Implement RLC
TArray<USpawnArea*> USpawnAreaManagerComponent::GetSpawnableSpawnAreas_Grid(const TArray<FVector>& Scales,
	const int32 NumToSpawn) const
{
	TArray<USpawnArea*> ValidSpawnAreas = SpawnAreas;

	// Always filter managed
	TArray<int32> Filtered = FilterManagedIndices(ValidSpawnAreas);
	const int32 NumManaged = Filtered.Num();

	// Always filter activated
	Filtered.Append(FilterActivatedIndices(ValidSpawnAreas));
	const int32 NumActivated = Filtered.Num();

	// Filter recent if possible
	TArray<USpawnArea*> FilteredRecent = ValidSpawnAreas;
	const TArray<int32> FilteredRecentIndices = FilterRecentIndices(FilteredRecent);
	int32 NumRecent = 0;
	if (FilteredRecent.Num() >= NumToSpawn)
	{
		Filtered.Append(FilteredRecentIndices);
		NumRecent = Filtered.Num();
		ValidSpawnAreas = FilteredRecent;
	}

	// Always shuffle the SpawnAreas
	Algo::RandomShuffle(ValidSpawnAreas);

	switch (GetTargetCfg().RuntimeTargetSpawningLocationSelectionMode)
	{
	case ERuntimeTargetSpawningLocationSelectionMode::None:
	case ERuntimeTargetSpawningLocationSelectionMode::Random:
		{
			const int32 Num = FMath::Min(ValidSpawnAreas.Num(), NumToSpawn);
			ValidSpawnAreas.SetNum(Num);
		}
		break;
	case ERuntimeTargetSpawningLocationSelectionMode::Bordering:
		{
			FindRandomBorderingGrid(ValidSpawnAreas, NumToSpawn);
		}
		break;
	case ERuntimeTargetSpawningLocationSelectionMode::RandomGridBlock:
		{
			// Shuffle the SpawnAreas since we take the first valid block found
			Algo::RandomShuffle(ValidSpawnAreas);
			FindRandomGridBlock(ValidSpawnAreas, CreateIndexValidityArray(Filtered), GridBlockIndexTypes, NumToSpawn);
		}
		break;
	case ERuntimeTargetSpawningLocationSelectionMode::NearbyGridBlock:
		{
			FilterBorderingIndices(ValidSpawnAreas, GetMostRecentSpawnArea());
			FindRandomGridBlock(ValidSpawnAreas, CreateIndexValidityArray(Filtered), GridBlockIndexTypes, NumToSpawn);
		}
		break;
	case ERuntimeTargetSpawningLocationSelectionMode::RandomVertical:
		{
			// Shuffle the SpawnAreas since we take the first valid block found
			Algo::RandomShuffle(ValidSpawnAreas);
			FindRandomGridBlock(ValidSpawnAreas, CreateIndexValidityArray(Filtered), VerticalIndexTypes, NumToSpawn);
		}
		break;
	case ERuntimeTargetSpawningLocationSelectionMode::RandomHorizontal:
		{
			// Shuffle the SpawnAreas since we take the first valid block found
			Algo::RandomShuffle(ValidSpawnAreas);
			FindRandomGridBlock(ValidSpawnAreas, CreateIndexValidityArray(Filtered), HorizontalIndexTypes, NumToSpawn);
		}
		break;
	}

	#if !UE_BUILD_SHIPPING
	if (bDebug_Grid) UE_LOG(LogTargetManager, Display, TEXT("Filtered: Managed: %d Activated: %d Recent: %d"),
		NumManaged, NumActivated, NumRecent);
	#endif

	for (int i = 0; i < ValidSpawnAreas.Num(); ++i)
	{
		if (Scales.IsValidIndex(i)) ValidSpawnAreas[i]->SetTargetScale(Scales[i]);
	}
	return ValidSpawnAreas;
}

TArray<USpawnArea*> USpawnAreaManagerComponent::GetSpawnableSpawnAreas_NonGrid(const TArray<FVector>& Scales,
	const int32 NumToSpawn) const
{
	TArray<USpawnArea*> ValidSpawnAreas;
	TArray<USpawnArea*> ChosenSpawnAreas;

	// Can skip a lot if forcing every other target in center
	if (ShouldForceSpawnAtOrigin() && NumToSpawn == 1)
	{
		USpawnArea* SpawnArea = GetOriginSpawnArea();
		SpawnArea->SetTargetScale(Scales[0]);
		ChosenSpawnAreas.Add(SpawnArea);
		return ChosenSpawnAreas;
	}

	// Get the cached SpawnAreas within the current BoxBounds
	if (GetTargetCfg().TargetDistributionPolicy == ETargetDistributionPolicy::FullRange || GetTargetCfg().
		TargetDistributionPolicy == ETargetDistributionPolicy::HeadshotHeightOnly)
	{
		ValidSpawnAreas = CachedExtrema.Array();
	}
	else if (GetTargetCfg().TargetDistributionPolicy == ETargetDistributionPolicy::EdgeOnly)
	{
		ValidSpawnAreas = CachedEdgeOnly.Array();
	}

	USpawnArea* PreviousSpawnArea = GetMostRecentSpawnArea();

	// Main loop for choosing spawn areas
	for (int i = 0; i < NumToSpawn; i++)
	{
		// Remove any overlap caused by any managed/activated SpawnAreas or any already chosen SpawnAreas
		RemoveOverlappingSpawnAreas(ValidSpawnAreas, ChosenSpawnAreas, Scales[i]);

		#if !UE_BUILD_SHIPPING
		if (bDebug_Valid && i == 0)
		{
			DrawDebug_Boxes(ValidSpawnAreas, FColor::Emerald, 4, 0);
		}
		#endif

		// Add the origin if settings permit
		if (i == 0 && GetBSConfig()->TargetConfig.bSpawnAtOriginWheneverPossible && ValidSpawnAreas.Contains(
			GetOriginSpawnArea()))
		{
			GetOriginSpawnArea()->SetTargetScale(Scales[0]);

			// Add to the return array
			ChosenSpawnAreas.Add(GetOriginSpawnArea());

			// Remove from options available
			ValidSpawnAreas.Remove(GetOriginSpawnArea());

			// Set as the previous SpawnArea since it will be spawned before any chosen later
			PreviousSpawnArea = GetOriginSpawnArea();
		}
		// Let RLC choose the SpawnArea if settings permit
		else if (bShouldAskRLCForSpawnAreas && RequestRLCSpawnArea.IsBound())
		{
			USpawnArea* Chosen = RequestRLCSpawnArea.Execute(ValidSpawnAreas, PreviousSpawnArea);
			if (!Chosen)
			{
				UE_LOG(LogTargetManager, Warning, TEXT("Unable to Spawn at SpawnArea suggested by RLAgent."));
				continue;
			}

			Chosen->SetRandomChosenPoint();
			Chosen->SetTargetScale(Scales[i]);
			ChosenSpawnAreas.Add(Chosen);
			ValidSpawnAreas.Remove(Chosen);
			PreviousSpawnArea = Chosen;
		}
		else
		{
			const int32 RandomIndex = FMath::RandRange(0, ValidSpawnAreas.Num() - 1);
			if (!ValidSpawnAreas.IsValidIndex(RandomIndex)) continue;

			USpawnArea* Chosen = ValidSpawnAreas[RandomIndex];
			Chosen->SetRandomChosenPoint();
			Chosen->SetTargetScale(Scales[i]);
			ChosenSpawnAreas.Add(Chosen);
			ValidSpawnAreas.Remove(Chosen);
			PreviousSpawnArea = Chosen;
		}
	}

	return ChosenSpawnAreas;
}

void USpawnAreaManagerComponent::FindRandomBorderingGrid(TArray<USpawnArea*>& ValidSpawnAreas, int32 NumToSpawn) const
{
	TArray<USpawnArea*> Best;
	for (int j = 0; j < ValidSpawnAreas.Num(); j++)
	{
		TArray<USpawnArea*> Current;
		TArray<USpawnArea*> Temp = ValidSpawnAreas;
		TArray<USpawnArea*> TempFilter = ValidSpawnAreas;

		// Filter the starting index
		FilterBorderingIndices(TempFilter, ValidSpawnAreas[j]);

		if (TempFilter.IsEmpty()) continue;

		Current.Add(ValidSpawnAreas[j]);
		Temp.Remove(ValidSpawnAreas[j]);

		// loop through the rest of the indices
		for (int i = 0; i < ValidSpawnAreas.Num() - 1; i++)
		{
			if (j == i) continue;

			TempFilter = Temp;

			FilterBorderingIndices(TempFilter, ValidSpawnAreas[i]);

			if (TempFilter.IsEmpty()) continue;

			Current.Add(ValidSpawnAreas[i]);
			Temp.Remove(ValidSpawnAreas[i]);
		}

		if (Current.Num() == NumToSpawn) return;
		if (Current.Num() > Best.Num()) Best = Current;
	}
}

void USpawnAreaManagerComponent::FindRandomGridBlock(TArray<USpawnArea*>& ValidSpawnAreas,
	const TArray<int32>& IndexValidity, const TSet<EBorderingDirection>& Directions, const int32 BlockSize) const
{
	//TODO: The indices are flipped or something
	if (BlockSize >= 20)
	{
		const FLargestRect LargestRect = FindLargestValidRectangle(IndexValidity, Size.Z, Size.Y);
		
		const int32 StartRowIndex = LargestRect.StartIndex / Size.Y;
		const int32 StartColumnIndex = LargestRect.StartIndex % Size.Y;
		
		const int32 EndRowIndex = LargestRect.EndIndex / Size.Y;
		const int32 EndColumnIndex = LargestRect.EndIndex % Size.Y;

		const int32 StartTestRowIndex = (Size.Z - 1) - LargestRect.StartIndex / Size.Y;
		const int32 StartTestColumnIndex = (Size.Y - 1) - LargestRect.StartIndex % Size.Y;
		
		const int32 EndTestRowIndex = (Size.Z - 1) - LargestRect.EndIndex / Size.Y;
		const int32 EndTestColumnIndex = (Size.Y - 1) - LargestRect.EndIndex % Size.Y;

		UE_LOG(LogTemp, Display, TEXT("StartRowIndex: %d StartColumnIndex: %d"), StartRowIndex, StartColumnIndex);
		UE_LOG(LogTemp, Display, TEXT("EndRowIndex: %d EndColumnIndex: %d"), EndRowIndex, EndColumnIndex);
		UE_LOG(LogTemp, Display, TEXT("StartTestRowIndex: %d StartTestColumnIndex: %d"), StartTestRowIndex, StartTestColumnIndex);
		UE_LOG(LogTemp, Display, TEXT("EndTestRowIndex: %d EndTestColumnIndex: %d"), EndTestRowIndex, EndTestColumnIndex);
		
		const int32 SquaredSize = FMath::Floor(FMath::Sqrt(static_cast<float>(BlockSize)));
		UE_LOG(LogTemp, Display, TEXT("SquaredSize: %d"), SquaredSize);
		PrintDebug_Grid(LargestRect);
		ValidSpawnAreas.Empty();
		for (int32 i = EndTestRowIndex; i <= StartTestRowIndex; ++i)
		{
			for (int32 j = EndTestColumnIndex; j <= StartTestColumnIndex; ++j)
			{
				if (ValidSpawnAreas.Num() >= BlockSize) return;
				ValidSpawnAreas.Add(SpawnAreas[i * Size.Y + j]);
			}
		}

		return;
	}
	
	FDFSLoopParams LoopParams = FDFSLoopParams(IndexValidity, Directions, BlockSize, Size.Y);
	EstimateDistances(LoopParams);

	for (const USpawnArea* SpawnArea : ValidSpawnAreas)
	{
		if (LoopParams.bFound) break;
		if (IndexValidity[SpawnArea->GetIndex()] == 1)
		{
			LoopParams.NextIter(SpawnArea->GetIndex());
			FindValidIndexCombinationsDFS(SpawnArea->GetIndex(), LoopParams);
		}
	}

	if (LoopParams.bFound)
	{
		ValidSpawnAreas.Empty();
		TArray<FBlock> BlocksArr = LoopParams.Blocks.Array();
		for (const int32 BlockIndex : BlocksArr[0].Indices)
		{
			ValidSpawnAreas.Add(SpawnAreas[BlockIndex]);
		}
	}
	#if !UE_BUILD_SHIPPING
	if (bDebug_Grid)
	{
		PrintDebug_Grid(LoopParams);
	}
	#endif
}

void USpawnAreaManagerComponent::FindValidIndexCombinationsDFS(const int32 StartIndex, FDFSLoopParams& Params) const
{
	if (Params.bFound) return;

	// Test to see if block total distance <= Best Total Distance
	if (Params.CurrentBlock.Num() == Params.BlockSize)
	{
		float TotalDist = 0.0f;
		for (int32 i = 0; i < Params.CurrentBlock.Num(); ++i)
			for (int32 j = i + 1; j < Params.CurrentBlock.Num(); ++j) TotalDist += CalcManhattanDist(
				Params.CurrentBlock.Indices[i], Params.CurrentBlock.Indices[j], Params.NumCols);;

		if (TotalDist <= Params.BestTotalDistance)
		{
			//Params.Blocks.Empty();
			//Params.BestTotalDistance = TotalDist;
			//Params.BestMaxDistance = MaxDist;
			Params.Blocks.Add(Params.CurrentBlock);
			Params.bFound = true;
		}
		return;
	}

	Params.TotalIterations++;

	// Initialize MaxDist for the current block, the max distance between any two points in the block
	float MaxDist = 0;
	for (int32 i = 0; i < Params.CurrentBlock.Num(); ++i)
	{
		const int32 Dist = CalcManhattanDist(StartIndex, Params.CurrentBlock.Indices[i], Params.NumCols);
		if (Dist > MaxDist) MaxDist = Dist;
	}

	for (const TPair<EBorderingDirection, int32>& Pair : SpawnAreas[StartIndex]->GetAdjacentIndexMap())
	{
		if (Params.bFound) return;
		if (MaxDist > Params.BestMaxDistance) return;
		if (Params.Valid[Pair.Value] == 1 && !Params.Visited.Contains(Pair.Value) && !Params.InitialVisited.
			Contains(Pair.Value) && Params.IndexTypes.Contains(Pair.Key) /*&& MaxDist <= Params.BestMaxDistance*/)
		{
			Params.TotalRecursions++;

			// Mark the spawn area as visited at this depth
			Params.Visited.Add(Pair.Value);

			// Add the current index to the current block, increasing the depth level
			Params.CurrentBlock.AddBlockIndex(Pair.Value);

			// Call the function recursively for the next depth level
			FindValidIndexCombinationsDFS(Pair.Value, Params);

			// Unmark the spawn area as visited (backtrack)
			Params.Visited.Remove(Pair.Value);

			// Remove the current index from the current block (backtrack), decreasing the depth level
			Params.CurrentBlock.RemoveBlockIndex(Pair.Value);
		}
		else Params.SkippedRecursions++;
	}
}

void USpawnAreaManagerComponent::RemoveOverlappingSpawnAreas(TArray<USpawnArea*>& ValidSpawnAreas,
	TArray<USpawnArea*>& ChosenSpawnAreas, const FVector& Scale) const
{
	TSet<FVector> Invalid;
	TSet<FVector> Valid;

	// Consider Managed Targets to be invalid if runtime
	TSet<USpawnArea*> InvalidSpawnAreas = ShouldConsiderManagedAsInvalid()
		? GetManagedActivatedOrRecentSpawnAreas()
		: GetActivatedOrRecentSpawnAreas();

	// Add any chosen SpawnAreas that aren't yet managed/activated
	InvalidSpawnAreas.Append(ChosenSpawnAreas);

	for (USpawnArea* SpawnArea : InvalidSpawnAreas)
	{
		// Regenerate if necessary
		const FVector CurrentScale = SpawnArea->GetTargetScale();
		if (Scale.Length() != SpawnArea->GetTargetScale().Length())
		{
			TSet<FVector> New = SpawnArea->MakeInvalidVerts(GetMinDist(), CurrentScale, Valid, bDebug_AllVerts);
			SpawnArea->SetInvalidVerts(New);
		}

		for (const FVector& Vector : SpawnArea->GetOverlappingVertices())
		{
			Invalid.Add(Vector);
		}
	}
	ValidSpawnAreas = ValidSpawnAreas.FilterByPredicate([&Invalid](const USpawnArea* SpawnArea)
	{
		return !Invalid.Contains(SpawnArea->GetBottomLeftVertex());
	});

	#if !UE_BUILD_SHIPPING
	if (bDebug_AllVerts) DrawVerticesOverlap(InvalidSpawnAreas, Scale, Valid, Invalid);
	#endif
}

void USpawnAreaManagerComponent::OnExtremaChanged(const FExtrema& Extrema)
{
	TArray<USpawnArea*> RemovedSpawnAreas;

	switch (GetTargetCfg().TargetDistributionPolicy)
	{
	case ETargetDistributionPolicy::None:
	case ETargetDistributionPolicy::Grid:
		return;
	case ETargetDistributionPolicy::EdgeOnly:
		{
			const float MaxY = Extrema.Max.Y - SpawnAreaInc.Y;
			const float MaxZ = Extrema.Max.Z - SpawnAreaInc.Z;

			const float MinY = Extrema.Min.Y;
			const float MinZ = Extrema.Min.Z;

			TSet<USpawnArea*> Temp;

			for (float Y = MinY; Y <= MaxY; Y += SpawnAreaInc.Y)
			{
				if (USpawnArea* SpawnArea_MinZ = FindSpawnAreaFromLocation(FVector(0, Y, MinZ))) Temp.Add(
					SpawnArea_MinZ);

				if (USpawnArea* SpawnArea_MaxZ = FindSpawnAreaFromLocation(FVector(0, Y, MaxZ))) Temp.Add(
					SpawnArea_MaxZ);
			}

			for (float Z = MinZ; Z <= MaxZ; Z += SpawnAreaInc.Z)
			{
				if (USpawnArea* SpawnArea_MinY = FindSpawnAreaFromLocation(FVector(0, MinY, Z))) Temp.Add(
					SpawnArea_MinY);
				if (USpawnArea* SpawnArea_MaxY = FindSpawnAreaFromLocation(FVector(0, MaxY, Z))) Temp.Add(
					SpawnArea_MaxY);
			}

			#if !UE_BUILD_SHIPPING
			if (bDebug_Removed)
			{
				RemovedSpawnAreas = SpawnAreas;
				for (USpawnArea* SpawnArea : Temp) RemovedSpawnAreas.Remove(SpawnArea);
			}
			#endif

			CachedEdgeOnly = Temp;
		}
		break;
	case ETargetDistributionPolicy::HeadshotHeightOnly:
	case ETargetDistributionPolicy::FullRange:
		for (USpawnArea* SpawnArea : SpawnAreas)
		{
			const FVector Location = SpawnArea->GetBottomLeftVertex();
			if (Location.Y < Extrema.Min.Y || Location.Y >= Extrema.Max.Y || Location.Z < Extrema.Min.Z || Location.Z >=
				Extrema.Max.Z)
			{
				CachedExtrema.Remove(SpawnArea);

				#if !UE_BUILD_SHIPPING
				if (bDebug_Removed) RemovedSpawnAreas.Add(SpawnArea);
				#endif
			}
			else
			{
				CachedExtrema.Add(SpawnArea);
			}
		}
		break;
	}
	#if !UE_BUILD_SHIPPING
	if (bDebug_Removed) DrawDebug_Boxes(RemovedSpawnAreas, FColor::Red, 4, 3);
	#endif
}

TArray<int32> USpawnAreaManagerComponent::FilterManagedIndices(TArray<USpawnArea*>& ValidSpawnAreas) const
{
	return FilterIndices(ValidSpawnAreas, &USpawnArea::IsManaged, bDebug_FilterManaged, FColor::Blue);
}

TArray<int32> USpawnAreaManagerComponent::FilterActivatedIndices(TArray<USpawnArea*>& ValidSpawnAreas) const
{
	return FilterIndices(ValidSpawnAreas, &USpawnArea::IsActivated, bDebug_FilterActivated, FColor::Cyan);
}

TArray<int32> USpawnAreaManagerComponent::FilterBorderingIndices(TArray<USpawnArea*>& ValidSpawnAreas,
	const USpawnArea* Current) const
{
	TArray<int32> Removed;
	if (!Current) return Removed;

	const TArray<int32> BorderingIndices = Current->GetBorderingIndices();
	ValidSpawnAreas = ValidSpawnAreas.FilterByPredicate([&](const USpawnArea* SpawnArea)
	{
		const int32 CurrentIndex = SpawnArea->GetIndex();
		if (!BorderingIndices.Contains(SpawnArea->GetIndex()))
		{
			Removed.Add(CurrentIndex);
			return false;
		}
		return true;
	});

	#if !UE_BUILD_SHIPPING
	if (bDebug_FilterBordering) DrawDebug_Boxes(Removed, FColor::Yellow, 4, 3);
	#endif

	return Removed;
}

TArray<int32> USpawnAreaManagerComponent::FilterRecentIndices(TArray<USpawnArea*>& ValidSpawnAreas) const
{
	return FilterIndices(ValidSpawnAreas, &USpawnArea::IsRecent, bDebug_FilterRecent, FColor::Turquoise);
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

/* ------------- */
/* -- Utility -- */
/* ------------- */

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

int32 USpawnAreaManagerComponent::CalcManhattanDist(const int32 Index1, const int32 Index2, const int32 NumCols)
{
	const int32 Row1 = Index1 / NumCols;
	const int32 Col1 = Index1 % NumCols;
	const int32 Row2 = Index2 / NumCols;
	const int32 Col2 = Index2 % NumCols;

	// Calculate Manhattan distance (sum of horizontal and vertical distances)
	return FMath::Abs(Row2 - Row1) + FMath::Abs(Col2 - Col1);
}

void USpawnAreaManagerComponent::EstimateDistances(FDFSLoopParams& Params)
{
	Params.BestTotalDistance = 0;
	Params.BestMaxDistance = 0;

	// Temp array simulating a block of indices
	TArray<int32> TestIndices;

	// Fill the array with indices corresponding to the BlockSize & NumCols
	for (int32 i = 0; i < FMath::Sqrt(static_cast<float>(Params.BlockSize)) * Params.NumCols; i += Params.NumCols)
	{
		for (int32 j = 0; j < FMath::Sqrt(static_cast<float>(Params.BlockSize)); ++j)
		{
			TestIndices.Add(i + j);
		}
	}

	// Limit size to BlockSize since it will be greater if not a square block
	TestIndices.SetNum(Params.BlockSize);

	// Calculate the sum of distance between every index and every other index
	for (int32 i = 0; i < TestIndices.Num(); ++i)
	{
		for (int32 j = i + 1; j < TestIndices.Num(); ++j)
		{
			const int32 IndexDist = CalcManhattanDist(TestIndices[i], TestIndices[j], Params.NumCols);
			Params.BestTotalDistance += IndexDist;
			if (IndexDist > Params.BestMaxDistance) Params.BestMaxDistance = IndexDist;
		}
	}
}

FLargestRect USpawnAreaManagerComponent::FindLargestValidRectangle(const TArray<int32>& IndexValidity,
	const int32 NumRows, const int32 NumCols)
{
	FLargestRect LargestRect;

	// Histogram of column heights
	TArray<int32> Heights;
	Heights.Init(0, NumCols);

	for (int32 Row = 0; Row < NumRows; Row++)
	{
		for (int32 Col = 0; Col < NumCols; Col++)
		{
			const int32 Index = Row * NumCols + Col;
			// Increment the height of a column if 1, otherwise reset to 0
			Heights[Col] = IndexValidity[Index] ? Heights[Col] + 1 : 0;
		}
		// At the end of each row, replace with new max if greater
		LargestRect.TestNewMaxArea(UpdateLargestRectangle(Heights));
	}
	return LargestRect;
}

FLargestRect USpawnAreaManagerComponent::UpdateLargestRectangle(TArray<int32>& Heights)
{
	FLargestRect LargestRect;

	std::stack<std::pair<int32, int32>> Stack;
	const int32 N = Heights.Num();

	for (int32 i = 0; i < Heights.Num(); ++i)
	{
		int32 Start = i;

		while (!Stack.empty() && Stack.top().second > Heights[i])
		{
			const int32 Index = Stack.top().first;
			const int32 Height = Stack.top().second;
			const int32 TestMax = (i - Index) * Height;
			const int32 EndIndex = (i - Index - 1) + (Height - 1) * N + Index;

			Stack.pop();

			LargestRect.TestNewMaxArea(TestMax, Index, EndIndex);

			Start = Index;
		}
		Stack.push({Start, Heights[i]});
	}

	// Goes along the columns in reverse
	while (!Stack.empty())
	{
		const int32 Index = Stack.top().first;
		const int32 Height = Stack.top().second;
		const int32 TestMax = (N - Index) * Height;
		const int32 EndIndex = (N - Index - 1) + (Height - 1) * N + Index;

		Stack.pop();

		LargestRect.TestNewMaxArea(TestMax, Index, EndIndex);
	}
	return LargestRect;
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

	const bool bHitDamage = GetTargetCfg().TargetDamageType == ETargetDamageType::Hit;
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

/* ----------- */
/* -- Debug -- */
/* ----------- */

void USpawnAreaManagerComponent::DrawDebug_AllSpawnAreas() const
{
	DrawDebug_Boxes(SpawnAreas, FColor::Cyan, 4, 0, true);
}

void USpawnAreaManagerComponent::ClearDebug_AllSpawnAreas() const
{
	FlushPersistentDebugLines(GetWorld());
}

void USpawnAreaManagerComponent::DrawDebug_Boxes(const TArray<int32>& InIndices, const FColor& InColor,
	const int32 InThickness, const int32 InDepthPriority, const bool bPersistantLines) const
{
	const float Time = bPersistantLines ? -1.f : GetTargetCfg().TargetSpawnCD;
	const FVector HalfInc = FVector(0, GetSpawnAreaInc().Y * 0.5f, GetSpawnAreaInc().Z * 0.5f);
	for (const int32 Index : InIndices)
	{
		const USpawnArea* SpawnArea = SpawnAreas[Index];
		if (!SpawnArea) continue;
		FVector Loc = SpawnArea->GetBottomLeftVertex() + HalfInc;
		DrawDebugBox(GetWorld(), Loc, FVector(0, HalfInc.Y, HalfInc.Z), InColor, bPersistantLines, Time,
			InDepthPriority, InThickness);
	}
}

void USpawnAreaManagerComponent::DrawDebug_Boxes(const TArray<const USpawnArea*>& InSpawnAreas, const FColor& InColor,
	const int32 InThickness, const int32 InDepthPriority, const bool bPersistantLines) const
{
	const float Time = bPersistantLines ? -1.f : GetTargetCfg().TargetSpawnCD;
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
	const float Time = bPersistantLines ? -1.f : GetTargetCfg().TargetSpawnCD;
	const FVector HalfInc = FVector(0, GetSpawnAreaInc().Y * 0.5f, GetSpawnAreaInc().Z * 0.5f);
	for (const USpawnArea* SpawnArea : InSpawnAreas)
	{
		FVector Loc = SpawnArea->GetBottomLeftVertex() + HalfInc;
		DrawDebugBox(GetWorld(), Loc, FVector(0, HalfInc.Y, HalfInc.Z), InColor, bPersistantLines, Time,
			InDepthPriority, InThickness);
	}
}

void USpawnAreaManagerComponent::DrawVerticesOverlap(const USpawnArea* SpawnArea, const FVector& Scale,
	const TSet<FVector>& Valid, const TSet<FVector>& Invalid) const
{
	const float ScaledRadius = Scale.X * SphereTargetRadius;
	float Radius = ScaledRadius * 2.f + (GetTargetCfg().MinDistanceBetweenTargets / 2.f);
	Radius = FMath::Max(Radius, SpawnArea->GetMinOverlapRadius()) + FMath::Max(Size.Y, Size.Z);
	DrawDebugSphere(GetWorld(), SpawnArea->ChosenPoint, Radius, 16, FColor::Magenta, false, 0.5f);

	for (FVector Vertex : Valid)
	{
		DrawDebugPoint(GetWorld(), Vertex, 10.f, FColor::Green, false, 0.5f);
	}

	for (FVector Vertex : Invalid)
	{
		DrawDebugPoint(GetWorld(), Vertex, 10.f, FColor::Red, false, 0.5f);
	}
}

void USpawnAreaManagerComponent::DrawVerticesOverlap(const TSet<USpawnArea*>& InSpawnAreas, const FVector& Scale,
	const TSet<FVector>& Valid, const TSet<FVector>& Invalid) const
{
	for (const USpawnArea* SpawnArea : InSpawnAreas)
	{
		const float ScaledRadius = Scale.X * SphereTargetRadius;
		float Radius = ScaledRadius * 2.f + (GetMinDist() * 0.5f);
		Radius = FMath::Max(Radius, SpawnArea->GetMinOverlapRadius()) + FMath::Max(Size.Y, Size.Z);
		DrawDebugSphere(GetWorld(), SpawnArea->ChosenPoint, Radius, 16, FColor::Magenta, false, 0.5f);
	}

	for (FVector Vertex : Valid)
	{
		DrawDebugPoint(GetWorld(), Vertex, 10.f, FColor::Green, false, 0.5f);
	}

	for (FVector Vertex : Invalid)
	{
		DrawDebugPoint(GetWorld(), Vertex, 10.f, FColor::Red, false, 0.5f);
	}
}

void USpawnAreaManagerComponent::PrintDebug_SpawnArea(const USpawnArea* SpawnArea)
{
	UE_LOG(LogTargetManager, Display, TEXT("SpawnArea:"));
	UE_LOG(LogTargetManager, Display, TEXT("Index %d GridIndexType %s"), SpawnArea->Index,
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
			UE_LOG(LogTargetManager, Display, TEXT("Distance between targets (%.2f) less than max allowed: %.2f"),
				Distance, MaxAllowedDistance);
		}
	}
}

void USpawnAreaManagerComponent::PrintDebug_Grid(const FDFSLoopParams& LoopParams)
{
	UE_LOG(LogTargetManager, Display, TEXT("Valid Block Distances:"));
	FString Line;
	for (const FBlock& Block : LoopParams.Blocks)
	{
		Line.Empty();
		for (int32 i = 0; i < Block.Indices.Num(); ++i)
		{
			for (int32 j = i + 1; j < Block.Indices.Num(); ++j)
			{
				Line += FString::FromInt(CalcManhattanDist(Block.Indices[i], Block.Indices[j], LoopParams.NumCols)) +
					" ";
			}
		}
		UE_LOG(LogTargetManager, Display, TEXT("%s"), *Line);
	}

	UE_LOG(LogTargetManager, Display, TEXT("Skipped Blocks Distances:"));
	for (const FBlock& Block : LoopParams.Blocks)
	{
		Line.Empty();
		for (int32 i = 0; i < Block.Indices.Num(); ++i)
		{
			for (int32 j = i + 1; j < Block.Indices.Num(); ++j)
			{
				Line += FString::FromInt(CalcManhattanDist(Block.Indices[i], Block.Indices[j], LoopParams.NumCols)) +
					" ";
			}
		}
		UE_LOG(LogTargetManager, Display, TEXT("%s"), *Line);
	}

	UE_LOG(LogTargetManager, Display, TEXT("Total Blocks Found: %d"), LoopParams.Blocks.Num());
	UE_LOG(LogTargetManager, Display, TEXT("Total Iterations: %d"), LoopParams.TotalIterations);
	UE_LOG(LogTargetManager, Display, TEXT("Total Recursions: %d"), LoopParams.TotalRecursions);
	UE_LOG(LogTargetManager, Display, TEXT("Skipped Recursions: %d"), LoopParams.SkippedRecursions);
	UE_LOG(LogTargetManager, Display, TEXT("BestTotalDistance: %d"), LoopParams.BestTotalDistance);
	UE_LOG(LogTargetManager, Display, TEXT("BestMaxDistance: %d"), LoopParams.BestMaxDistance);
}

void USpawnAreaManagerComponent::PrintDebug_Grid(const FLargestRect& LargestRect)
{
	UE_LOG(LogTargetManager, Display, TEXT("MaxArea %d"), LargestRect.MaxArea);
	UE_LOG(LogTargetManager, Display, TEXT("LargestRect start: %d end: %d"), LargestRect.StartIndex,
		LargestRect.EndIndex);
}
