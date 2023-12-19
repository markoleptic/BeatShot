// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Target/SpawnAreaManagerComponent.h"
#include <stack>
#include "GlobalConstants.h"
#include "Algo/RandomShuffle.h"
#include "Target/MatrixFunctions.h"
#include "Target/TargetManager.h"

/** Preferred SpawnMemory increments */
const TArray PreferredSpawnAreaIncScales = {50, 45, 40, 30, 25, 20, 15, 10, 5};

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
	CachedRecent = TSet<USpawnArea*>();
	CachedExtrema = TSet<USpawnArea*>();
	MostRecentGridBlock = TSet<USpawnArea*>();
	AllSpawnAreas = TSet<USpawnArea*>();
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

	SetAppropriateSpawnMemoryValues(SpawnAreaInc, SpawnAreaScale, BSConfig, StaticExtents);
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

void USpawnAreaManagerComponent::SetAppropriateSpawnMemoryValues(FIntVector3& InSpawnAreaInc, FVector& InSpawnAreaScale,
	const FBSConfig* InCfg, const FVector& InStaticExtents)
{
	const int32 HalfWidth = InStaticExtents.Y;
	const int32 HalfHeight = InStaticExtents.Z;
	bool bWidthScaleSelected = false;
	bool bHeightScaleSelected = false;

	switch (InCfg->TargetConfig.TargetDistributionPolicy)
	{
	case ETargetDistributionPolicy::HeadshotHeightOnly:
		{
			InSpawnAreaScale.Z = 1.f;
			InSpawnAreaInc.Z = 1;
			for (const int32 Scale : PreferredSpawnAreaIncScales)
			{
				if (!bWidthScaleSelected)
				{
					if (HalfWidth % Scale == 0)
					{
						InSpawnAreaInc.Y = Scale;
						InSpawnAreaScale.Y = 1.f / Scale;
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
					InStaticExtents.Y, InStaticExtents.Z);
			}
		}
		break;
	case ETargetDistributionPolicy::None:
	case ETargetDistributionPolicy::EdgeOnly:
	case ETargetDistributionPolicy::FullRange:
		{
			for (const int32 Scale : PreferredSpawnAreaIncScales)
			{
				if (!bWidthScaleSelected)
				{
					if (HalfWidth % Scale == 0)
					{
						InSpawnAreaInc.Y = Scale;
						InSpawnAreaScale.Y = 1.f / Scale;
						bWidthScaleSelected = true;
					}
				}
				if (!bHeightScaleSelected)
				{
					if (HalfHeight % Scale == 0)
					{
						InSpawnAreaInc.Z = Scale;
						InSpawnAreaScale.Z = 1.f / Scale;
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
					InStaticExtents.Y, InStaticExtents.Z);
			}
		}
		break;
	case ETargetDistributionPolicy::Grid:
		{
			const float MaxTargetSize = InCfg->TargetConfig.MaxSpawnedTargetScale * SphereTargetDiameter;
			InSpawnAreaInc.Y = InCfg->GridConfig.GridSpacing.X + MaxTargetSize;
			InSpawnAreaInc.Z = InCfg->GridConfig.GridSpacing.Y + MaxTargetSize;
			InSpawnAreaScale.Y = 1.f / InSpawnAreaInc.Y;
			InSpawnAreaScale.Z = 1.f / InSpawnAreaInc.Z;
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
			AllSpawnAreas.Add(SpawnArea);
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
	CachedRecent.Empty();
	CachedExtrema.Empty();
	MostRecentGridBlock.Empty();
	AllSpawnAreas.Empty();

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
			if (!SpawnAreaByLoc)
			{
				UE_LOG(LogTargetManager, Warning, TEXT("Could not find SpawnArea from Transform: %s."),*DamageEvent.Transform.GetLocation().ToString());
				return;
			}

			// Total Tracking Damage Possible is done on tick in UpdateTotalTrackingDamagePossible

			// Only increment total tracking damage if damage came from player
			if (!DamageEvent.bDamagedSelf && DamageEvent.DamageDelta > 0.f) SpawnAreaByLoc->
				IncrementTotalTrackingDamage();
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

		if (DamageEvent.bWillDestroy)
		{
			RemoveManagedFlagFromSpawnArea(DamageEvent.Guid);
		}
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
	int32 GridY = Origin.Y + SpawnAreaInc.Y * FMath::FloorToInt(RelativeLocation.Y / SpawnAreaInc.Y);
	int32 GridZ = Origin.Z + SpawnAreaInc.Z * FMath::FloorToInt(RelativeLocation.Z / SpawnAreaInc.Z);

	GridY = FMath::Clamp(GridY, StaticExtrema.Min.Y, StaticExtrema.Max.Y - SpawnAreaInc.Y);
	GridZ = FMath::Clamp(GridZ, StaticExtrema.Min.Z, StaticExtrema.Max.Z - SpawnAreaInc.Z);
	
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
	return CachedManaged.Difference(CachedActivated);
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
	return CachedActivated.Union(CachedRecent);
}

TSet<USpawnArea*> USpawnAreaManagerComponent::GetManagedActivatedOrRecentSpawnAreas() const
{
	return CachedManaged.Union(GetActivatedOrRecentSpawnAreas());
}

TSet<USpawnArea*> USpawnAreaManagerComponent::GetManagedDeactivatedNotRecentSpawnAreas() const
{
	return CachedManaged.Difference(GetActivatedOrRecentSpawnAreas());
}

TSet<USpawnArea*> USpawnAreaManagerComponent::GetUnflaggedSpawnAreas() const
{
	return AllSpawnAreas.Difference(GetManagedActivatedOrRecentSpawnAreas());
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
	
	SpawnArea->SetGuid(TargetGuid);
	SpawnArea->SetIsCurrentlyManaged(true);
	GuidMap.Add(TargetGuid, SpawnArea);
	CachedManaged.Add(SpawnArea);

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
	const int32 NumRemovedManaged = CachedManaged.Remove(SpawnArea);

	if (!SpawnArea->IsManaged())
	{
		UE_LOG(LogTargetManager, Warning, TEXT("Tried to remove managed flag from from non-managed SpawnArea."));
		return;
	}

	if (NumRemoved == 0) UE_LOG(LogTargetManager, Warning, TEXT("Failed to remove from TargetGuidToSpawnArea map."));
	if (NumRemovedManaged == 0) UE_LOG(LogTargetManager, Warning, TEXT("Failed to remove from CachedManaged."));

	SpawnArea->SetIsCurrentlyManaged(false);
	SpawnArea->ResetGuid();
}

void USpawnAreaManagerComponent::RemoveActivatedFlagFromSpawnArea(USpawnArea* SpawnArea)
{
	if (!SpawnArea) return;

	// Remove from activated cache
	const int32 NumRemovedFromCache = CachedActivated.Remove(SpawnArea);

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

	const int32 NumToRemove = CachedRecent.Num() - GetTargetCfg().MaxNumRecentTargets;
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
	
	TSet<USpawnArea*> ValidSpawnAreasSet = GetManagedDeactivatedNotRecentSpawnAreas();
	/* TODO: Might need to have separate "Recent" for spawning and activation
	 * For game modes like ChargedBeatTrack, there will be 4 managed targets
	 * but also 4 recent targets at some point, which is why this condition
	 * exists */
	
	if (ValidSpawnAreasSet.IsEmpty())
	{
		ValidSpawnAreasSet = GetDeactivatedManagedSpawnAreas();
	}
	TArray<USpawnArea*> ValidSpawnAreas = ValidSpawnAreasSet.Array();
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

	// Main loop for choosing spawn areas
	for (int i = 0; i < NumToActivate; i++)
	{
		// Add the origin if settings permit
		// TODO: this might not work due to desync w/ spawning ?
		if (GetBSConfig()->TargetConfig.bSpawnAtOriginWheneverPossible && ValidSpawnAreas.
			Contains(GetOriginSpawnArea()))
		{
			if (!GetOriginSpawnArea()->GetGuid().IsValid()) continue;

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
			if (!Chosen->GetGuid().IsValid()) continue;

			ChosenSpawnAreas.Add(Chosen);
			ValidSpawnAreas.Remove(Chosen);
			PreviousSpawnArea = Chosen;
		}
		else
		{
			const int32 RandomIndex = FMath::RandRange(0, ValidSpawnAreas.Num() - 1);
			if (!ValidSpawnAreas.IsValidIndex(RandomIndex)) continue;
			
			USpawnArea* Chosen = ValidSpawnAreas[RandomIndex];
			if (!Chosen->GetGuid().IsValid()) continue;
			
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

TArray<USpawnArea*> USpawnAreaManagerComponent::GetSpawnableSpawnAreas_Grid(const TArray<FVector>& Scales,
	const int32 NumToSpawn) const
{
	// Get all SpawnAreas that are managed, deactivated, and not recent
	const TSet<USpawnArea*> ValidSet = GetUnflaggedSpawnAreas();
	TArray<USpawnArea*> OutArray = ValidSet.Array();

	// Always shuffle the SpawnAreas
	Algo::RandomShuffle(OutArray);

	switch (GetTargetCfg().RuntimeTargetSpawningLocationSelectionMode)
	{
	case ERuntimeTargetSpawningLocationSelectionMode::None:
	case ERuntimeTargetSpawningLocationSelectionMode::Random:
		{
			const int32 Num = FMath::Min(OutArray.Num(), NumToSpawn);
			OutArray.SetNum(Num);
		}
		break;
	case ERuntimeTargetSpawningLocationSelectionMode::Bordering:
		{
			FindRandomBorderingGrid(OutArray, NumToSpawn);
		}
		break;
	case ERuntimeTargetSpawningLocationSelectionMode::RandomGridBlock:
		{
			FindGridBlockUsingLargestRect(OutArray, CreateIndexValidityArray(ValidSet), NumToSpawn);
		}
		break;
	case ERuntimeTargetSpawningLocationSelectionMode::NearbyGridBlock:
		{
			FindGridBlockUsingLargestRect(OutArray, CreateIndexValidityArray(ValidSet), NumToSpawn, true);
		}
		break;
	case ERuntimeTargetSpawningLocationSelectionMode::RandomVertical:
		{
			FindGridBlockUsingDFS(OutArray, CreateIndexValidityArray(ValidSet), VerticalIndexTypes, NumToSpawn);
		}
		break;
	case ERuntimeTargetSpawningLocationSelectionMode::RandomHorizontal:
		{
			FindGridBlockUsingDFS(OutArray, CreateIndexValidityArray(ValidSet), HorizontalIndexTypes, NumToSpawn);
		}
		break;
	}

	// Make sure number of elements is no more than number to spawn
	if (OutArray.Num() > NumToSpawn) OutArray.SetNum(NumToSpawn);

	// Set the target scales
	for (int i = 0; i < OutArray.Num(); i++)
	{
		if (OutArray.IsValidIndex(i)) OutArray[i]->SetTargetScale(Scales[i]);
	}
	return OutArray;
}

TArray<USpawnArea*> USpawnAreaManagerComponent::GetSpawnableSpawnAreas_NonGrid(const TArray<FVector>& Scales,
	const int32 NumToSpawn) const
{
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
	TArray<USpawnArea*> ValidSpawnAreas = CachedExtrema.Array();

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

void USpawnAreaManagerComponent::FindGridBlockUsingLargestRect(TArray<USpawnArea*>& ValidSpawnAreas,
	const TArray<int32>& IndexValidity, const int32 BlockSize, const bool bBordering) const
{
	FLargestRect Rect = FindLargestValidRectangle(IndexValidity, Size.Z, Size.Y);

	Rect.UpdateIndices(Size.Y);
	Rect.SetBlockSize(BlockSize);
	ChooseRectIndices(Rect, bBordering);

	#if !UE_BUILD_SHIPPING
	PrintDebug_Matrix(IndexValidity, Size.Z, Size.Y);
	PrintDebug_GridLargestRect(Rect, Size.Y);
	#endif

	// Empty the array since we have all the indices now
	ValidSpawnAreas.Empty();
	MostRecentGridBlock.Empty();
	
	int32 StartI = Rect.ChosenStartRowIndex;
	int32 StartJ = Rect.ChosenStartColIndex;
	int32 EndI = Rect.ChosenEndRowIndex;
	int32 EndJ = Rect.ChosenEndColIndex;

	bool bIAsRow = true;
	bool bIncrement = true;

	// Randomize the start indices if it will get chopped off
	if (Rect.ChosenBlockSize > Rect.ActualBlockSize)
	{
		bIAsRow = FMath::RandBool();
		bIncrement = FMath::RandBool();

		// Swap rows and columns
		if (!bIAsRow)
		{
			Swap(StartI, StartJ);
			Swap(EndI, EndJ);
		}
		
		// Swap start and end
		if (!bIncrement)
		{
			Swap(StartI, EndI);
			Swap(StartJ, EndJ);
		}
	}
	
	// Iterate through the chosen block, adding the SpawnAreas to the array
	for (int32 i = StartI; (bIncrement && i <= EndI) || (!bIncrement && i >= EndI); (bIncrement ? ++i : --i))
	{
		for (int32 j = StartJ; (bIncrement && j <= EndJ) || (!bIncrement && j >= EndJ); (bIncrement ? ++j : --j))
		{
			const int32 Index = bIAsRow ? i * Size.Y + j : j * Size.Y + i;
			if (SpawnAreas.IsValidIndex(Index))
			{
				ValidSpawnAreas.Add(SpawnAreas[Index]);
				// Update MostRecentGridBlock set
				MostRecentGridBlock.Add(SpawnAreas[Index]);
			}
			else UE_LOG(LogTargetManager, Warning, TEXT("Invalid Index: %d"), Index);
			if (ValidSpawnAreas.Num() >= Rect.ActualBlockSize) break;
		}
		if (ValidSpawnAreas.Num() >= Rect.ActualBlockSize) break;
	}
	
	// Choose a remainder index if ActualBlockSize is prime and a smaller grid is chosen
	if (!Rect.bNeedsRemainderIndex) return;

	const TSet<USpawnArea*> Bordering = GetBorderingGridBlockSpawnAreas(MostRecentGridBlock, GridBlockIndexTypes);
	TArray<std::pair<int32, int32>> BorderingInRect = FindBorderingIndicesInRect(Bordering, Rect, Size.Y);
	if (BorderingInRect.IsEmpty()) return;

	const auto [Row, Col] = BorderingInRect[FMath::RandRange(0, BorderingInRect.Num() - 1)];
	const int32 Index = Row * Size.Y + Col;
	if (!SpawnAreas.IsValidIndex(Index)) return;

	ValidSpawnAreas.Add(SpawnAreas[Index]);
	MostRecentGridBlock.Add(SpawnAreas[Index]);
}

void USpawnAreaManagerComponent::ChooseRectIndices(FLargestRect& Rect, const bool bBordering) const
{
	// Set defaults for early returns
	Rect.ChosenStartRowIndex = Rect.StartRowIndex;
	Rect.ChosenStartColIndex = Rect.StartColIndex;
	Rect.ChosenEndRowIndex = Rect.EndRowIndex;
	Rect.ChosenEndColIndex = Rect.EndColIndex;

	// The of size of the sub block within the rectangle
	FFactor RandomFactor;

	if (IsPrime(Rect.ActualBlockSize))
	{
		TSet<FFactor> OneLessBlockSize = FindBestFittingFactors(Rect.ActualBlockSize - 1, Rect);
		const TSet<FFactor> OneGreaterBlockSize = FindBestFittingFactors(Rect.ActualBlockSize + 1, Rect);
		
		if (OneLessBlockSize.IsEmpty() && OneGreaterBlockSize.IsEmpty()) return;
		
		FFactor MinDifferenceFactor = FFactor(Rect.ActualBlockSize);
		TSet<FFactor> MinFactors;
		
		for (const FFactor Factor : OneLessBlockSize.Union(OneGreaterBlockSize))
		{
			if (Factor.Distance <= MinDifferenceFactor.Distance) MinDifferenceFactor = Factor;
		}
		for (const FFactor Factor : OneLessBlockSize.Union(OneGreaterBlockSize))
		{
			if (Factor.Distance <= MinDifferenceFactor.Distance) MinFactors.Add(Factor);
		}

		TArray<FFactor> Factors = MinFactors.Array();
		RandomFactor = Factors[FMath::RandRange(0, Factors.Num() - 1)];

		// Mark the rect as needing a remainder index, will be found later in FindGridBlockUsingLargestRect
		if (RandomFactor.Factor1 * RandomFactor.Factor2 < Rect.ActualBlockSize) Rect.bNeedsRemainderIndex = true;
	}
	else
	{
		TArray<FFactor> Factors = FindBestFittingFactors(Rect.ActualBlockSize, Rect).Array();
		if (Factors.IsEmpty()) return;

		RandomFactor = Factors[FMath::RandRange(0, Factors.Num() - 1)];
	}

	const bool bF1FitsRows = RandomFactor.Factor1 <= Rect.NumRowsAvailable;
	const bool bF2FitsRows = RandomFactor.Factor2 <= Rect.NumRowsAvailable;
	const bool bF1FitsCols = RandomFactor.Factor1 <= Rect.NumColsAvailable;
	const bool bF2FitsCols = RandomFactor.Factor2 <= Rect.NumColsAvailable;

	int32 SubRowSize;
	int32 SubColSize;

	// All fit, choose random
	if (bF1FitsRows && bF2FitsRows && bF1FitsCols && bF2FitsCols)
	{
		const bool bRandom = FMath::RandBool();
		SubRowSize = bRandom ? RandomFactor.Factor1 : RandomFactor.Factor2;
		SubColSize = bRandom ? RandomFactor.Factor2 : RandomFactor.Factor1;
	}
	else if (bF1FitsRows && bF2FitsCols)
	{
		SubRowSize = RandomFactor.Factor1;
		SubColSize = RandomFactor.Factor2;
	}
	else if (bF2FitsRows && bF1FitsCols)
	{
		SubRowSize = RandomFactor.Factor2;
		SubColSize = RandomFactor.Factor1;
	}
	else
	{
		UE_LOG(LogTargetManager, Warning, TEXT("BOTH FACTORS > ROWS OR COLS AVAILABLE SOMETHING WRONG"));
		return;
	}
	
	const int32 MaxAllowedStartRowIndex = Rect.EndRowIndex - SubRowSize + 1;
	const int32 MaxAllowedStartColIndex = Rect.EndColIndex - SubColSize + 1;

	// See if a recent bordering index can be included in the next grid block
	if (bBordering && FindBorderingRectIndices(Rect, SubRowSize, SubColSize)) return;

	// To choose a random sub-block within the largest rectangle, choose a random start index
	const int32 RandomRowStartIndex = FMath::RandRange(Rect.StartRowIndex, MaxAllowedStartRowIndex);
	const int32 RandomColStartIndex = FMath::RandRange(Rect.StartColIndex, MaxAllowedStartColIndex);
	
	Rect.ChosenBlockSize = SubRowSize * SubColSize;
	Rect.ChosenStartRowIndex = RandomRowStartIndex;
	Rect.ChosenStartColIndex = RandomColStartIndex;
	Rect.ChosenEndRowIndex = Rect.ChosenStartRowIndex + SubRowSize - 1;
	Rect.ChosenEndColIndex = Rect.ChosenStartColIndex + SubColSize - 1;

	#if !UE_BUILD_SHIPPING
	if (bDebug_Grid)
	{
		UE_LOG(LogTargetManager, Display, TEXT("NumRowsAvail: %d NumColsAvail: %d F1: %d F2: %d"),
			Rect.NumRowsAvailable, Rect.NumColsAvailable, RandomFactor.Factor1, RandomFactor.Factor2);
		UE_LOG(LogTargetManager, Display, TEXT("MaxAllowedStartIndex: %d"),
			MaxAllowedStartRowIndex * Size.Y + MaxAllowedStartColIndex);
	}

	if (Rect.ChosenStartRowIndex < Rect.StartRowIndex || Rect.ChosenStartColIndex < Rect.StartColIndex)
	{
		UE_LOG(LogTargetManager, Warning, TEXT("ChosenStartIndex < Start!!!"));
	}
	if (Rect.ChosenEndRowIndex > Rect.EndRowIndex || Rect.ChosenEndColIndex > Rect.EndColIndex)
	{
		UE_LOG(LogTargetManager, Warning, TEXT("ChosenEndIndex > End!!!"));
	}
	#endif
}

bool USpawnAreaManagerComponent::FindBorderingRectIndices(FLargestRect& Rect, const int32 SubRowSize,
	const int32 SubColSize) const
{
	// An array of indices that were in the previous grid block and are within the current largest rectangle
	const TSet<USpawnArea*> Bordering = GetBorderingGridBlockSpawnAreas(MostRecentGridBlock, AllIndexTypes);
	TArray<std::pair<int32, int32>> BorderingInRect = FindBorderingIndicesInRect(Bordering, Rect, Size.Y);

	if (BorderingInRect.IsEmpty()) return false;

	// Choose a random bordering index to include in the block
	const auto [Row, Col] = BorderingInRect[FMath::RandRange(0, BorderingInRect.Num() - 1)];

	// Array of indices that include the bordering index and fit in the largest rectangle
	TArray<std::pair<int32, int32>> ValidIndices;

	// Iterate through all possible sub-block configurations
	for (int i = Rect.StartRowIndex; i <= Rect.EndRowIndex; ++i)
	{
		for (int j = Rect.StartColIndex; j <= Rect.EndColIndex; ++j)
		{
			const int32 BlockRowEnd = i + SubRowSize - 1;
			const int32 BlockColEnd = j + SubColSize - 1;

			if (BlockRowEnd <= Rect.EndRowIndex && BlockColEnd <= Rect.EndColIndex)
			{
				if (Row >= i && Col >= j && Row <= BlockRowEnd && Col <= BlockColEnd)
				{
					ValidIndices.Add({i, j});
				}
			}
		}
	}

	if (ValidIndices.IsEmpty()) return false;

	// Choose a random valid index
	const auto [ChosenRow, ChosenCol] = ValidIndices[FMath::RandRange(0, ValidIndices.Num() - 1)];

	Rect.ChosenStartRowIndex = ChosenRow;
	Rect.ChosenStartColIndex = ChosenCol;
	Rect.ChosenEndRowIndex = Rect.ChosenStartRowIndex + SubRowSize - 1;
	Rect.ChosenEndColIndex = Rect.ChosenStartColIndex + SubColSize - 1;

	return true;
}

TSet<USpawnArea*> USpawnAreaManagerComponent::GetBorderingGridBlockSpawnAreas(const TSet<USpawnArea*>& GridBlock,
	const TSet<EBorderingDirection>& Directions) const
{
	TArray<std::pair<int32, int32>> RowColBorders;
	TSet<USpawnArea*> Out;
	int32 MinRow = Size.Z - 1;
	int32 MinCol = Size.Y - 1;
	int32 MaxRow = 0;
	int32 MaxCol = 0;

	for (const USpawnArea* SpawnArea : GridBlock)
	{
		std::pair Pair = {SpawnArea->GetIndex() / Size.Y, SpawnArea->GetIndex() % Size.Y};

		// Update min and max values
		MinRow = FMath::Min(MinRow, Pair.first);
		MaxRow = FMath::Max(MaxRow, Pair.first);
		MinCol = FMath::Min(MinCol, Pair.second);
		MaxCol = FMath::Max(MaxCol, Pair.second);

		RowColBorders.Add(Pair);
	}
	for (auto [Row, Col] : RowColBorders)
	{
		const int32 TestIndex = Row * Size.Y + Col;

		if (Row == MaxRow && Col == MinCol && Directions.Contains(EBorderingDirection::UpLeft))
		{
			const int32 UpLeftIndex = TestIndex + Size.Y - 1;
			if (UpLeftIndex < SpawnAreas.Num())
			{
				const int32 MinUpRowIndex = FMath::Floor(TestIndex / Size.Y) * Size.Y + Size.Y;
				if (UpLeftIndex >= MinUpRowIndex)
				{
					Out.Add(SpawnAreas[UpLeftIndex]);
				}
			}
		}
		if (Row == MaxRow && Col == MaxCol && Directions.Contains(EBorderingDirection::UpRight))
		{
			const int32 UpRightIndex = TestIndex + Size.Y + 1;
			if (UpRightIndex < SpawnAreas.Num())
			{
				const int32 MaxUpRowIndex = FMath::Floor(TestIndex / Size.Y) * Size.Y + Size.Y + Size.Y - 1;
				if (UpRightIndex <= MaxUpRowIndex)
				{
					Out.Add(SpawnAreas[UpRightIndex]);
				}
			}
		}
		if (Row == MinRow && Col == MinCol && Directions.Contains(EBorderingDirection::DownLeft))
		{
			const int32 DownLeftIndex = TestIndex - Size.Y - 1;
			if (DownLeftIndex >= 0)
			{
				const int32 MinDownRowIndex = FMath::Floor(TestIndex / Size.Y) * Size.Y - Size.Y;
				if (DownLeftIndex >= MinDownRowIndex)
				{
					Out.Add(SpawnAreas[DownLeftIndex]);
				}
			}
		}
		if (Row == MinRow && Col == MaxCol && Directions.Contains(EBorderingDirection::DownRight))
		{
			const int32 DownRightIndex = TestIndex - Size.Y + 1;
			if (DownRightIndex >= 0)
			{
				const int32 MaxDownRowIndex = FMath::Floor(TestIndex / Size.Y) * Size.Y - 1;
				if (DownRightIndex <= MaxDownRowIndex)
				{
					Out.Add(SpawnAreas[DownRightIndex]);
				}
			}
		}
		if (Row == MinRow && Directions.Contains(EBorderingDirection::Down))
		{
			const int32 DownIndex = TestIndex - Size.Y;
			if (DownIndex >= 0)
			{
				Out.Add(SpawnAreas[DownIndex]);
			}
		}
		if (Row == MaxRow && Directions.Contains(EBorderingDirection::Up))
		{
			const int32 UpIndex = TestIndex + Size.Y;
			if (UpIndex < SpawnAreas.Num())
			{
				Out.Add(SpawnAreas[UpIndex]);
			}
		}
		if (Col == MinCol && Directions.Contains(EBorderingDirection::Left))
		{
			const int32 LeftIndex = TestIndex - 1;
			const int32 MinRowIndex = FMath::Floor(TestIndex / Size.Y) * Size.Y;
			if (LeftIndex >= MinRowIndex)
			{
				Out.Add(SpawnAreas[LeftIndex]);
			}
		}
		if (Col == MaxCol && Directions.Contains(EBorderingDirection::Right))
		{
			const int32 RightIndex = TestIndex + 1;
			const int32 MaxRowIndex = FMath::Floor(TestIndex / Size.Y) * Size.Y + Size.Y - 1;
			if (RightIndex <= MaxRowIndex)
			{
				Out.Add(SpawnAreas[RightIndex]);
			}
		}
	}
	return Out;
}

void USpawnAreaManagerComponent::FindGridBlockUsingDFS(TArray<USpawnArea*>& ValidSpawnAreas,
	const TArray<int32>& IndexValidity, const TSet<EBorderingDirection>& Directions, const int32 BlockSize) const
{
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
		PrintDebug_GridDFS(LoopParams);
	}
	#endif
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
				if (USpawnArea* SpawnArea_MinZ = FindSpawnAreaFromLocation(FVector(0, Y, MinZ)))
					Temp.Add(SpawnArea_MinZ);

				if (USpawnArea* SpawnArea_MaxZ = FindSpawnAreaFromLocation(FVector(0, Y, MaxZ)))
					Temp.Add(SpawnArea_MaxZ);
			}

			for (float Z = MinZ; Z <= MaxZ; Z += SpawnAreaInc.Z)
			{
				if (USpawnArea* SpawnArea_MinY = FindSpawnAreaFromLocation(FVector(0, MinY, Z)))
					Temp.Add(SpawnArea_MinY);
				if (USpawnArea* SpawnArea_MaxY = FindSpawnAreaFromLocation(FVector(0, MaxY, Z)))
					Temp.Add(SpawnArea_MaxY);
			}

			#if !UE_BUILD_SHIPPING
			if (bDebug_Removed)
			{
				RemovedSpawnAreas = SpawnAreas;
				for (USpawnArea* SpawnArea : Temp) RemovedSpawnAreas.Remove(SpawnArea);
			}
			#endif

			CachedExtrema = Temp;
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

	#if !UE_BUILD_SHIPPING
	if (bDebug_Grid)
	{
		PrintDebug_Matrix(IndexValidity, Size.Z, Size.Y);
	}
	#endif

	return IndexValidity;
}

TArray<int32> USpawnAreaManagerComponent::CreateIndexValidityArray(const TSet<USpawnArea*>& ValidSpawnAreas) const
{
	TArray<int32> IndexValidity;
	IndexValidity.Init(0, SpawnAreas.Num());
	for (const USpawnArea* SpawnArea : ValidSpawnAreas) IndexValidity[SpawnArea->GetIndex()] = 1;

	#if !UE_BUILD_SHIPPING
	if (bDebug_Grid)
	{
		PrintDebug_Matrix(IndexValidity, Size.Z, Size.Y);
	}
	#endif

	return IndexValidity;
}

FLargestRect USpawnAreaManagerComponent::FindLargestValidRectangle(const TArray<int32>& IndexValidity,
	const int32 NumRows, const int32 NumCols)
{
	FLargestRect LargestRect;

	// Create a histogram of column heights initialized with zeros
	TArray<int32> Heights;
	Heights.Init(0, NumCols);

	for (int32 Row = 0; Row < NumRows; Row++)
	{
		// Update the column heights based on the current row's validity
		for (int32 Col = 0; Col < NumCols; Col++)
		{
			const int32 Index = Row * NumCols + Col;
			// If the cell is valid, increment the column height; otherwise, reset to 0
			Heights[Col] = IndexValidity[Index] ? Heights[Col] + 1 : 0;
		}

		UpdateLargestRectangle(Heights, LargestRect, Row);
	}
	return LargestRect;
}

void USpawnAreaManagerComponent::UpdateLargestRectangle(TArray<int32>& Heights, FLargestRect& LargestRect,
	const int32 CurrentRow)
{
	std::stack<FRectInfo> Stack;
	const int32 NumCols = Heights.Num();

	// Iterate through the columns to identify potential rectangles
	for (int32 i = 0; i < Heights.Num(); ++i)
	{
		int32 StartColIndex = i;

		// Compare the current column's height with the height of the column at the top of the stack
		// If the current height is less than the stack's height, it suggests the potential end of a rectangle
		while (!Stack.empty() && Stack.top().Height > Heights[i])
		{
			const int32 CurrentColIndex = Stack.top().ColIndex;

			// Area of the potential rectangle with the current column as the right boundary
			const int32 Height = Stack.top().Height;

			if (Height > 0)
			{
				const int32 Area = (i - CurrentColIndex) * Height;
				const int32 StartIndex = Stack.top().Index - NumCols * (Height - 1);
				const int32 EndIndex = Stack.top().Index + ((Area / Height) - 1);
				LargestRect.TestNewMaxArea(Area, StartIndex, EndIndex);
			}

			// Update StartColIndex to the column index at the top of the stack since we measure the width from there to i
			StartColIndex = CurrentColIndex;

			Stack.pop();
		}
		Stack.push({CurrentRow * NumCols + StartColIndex, StartColIndex, Heights[i]});
	}
	// After processing all columns, check if there are remaining elements in the stack
	while (!Stack.empty())
	{
		const int32 CurrentColIndex = Stack.top().ColIndex;
		const int32 Height = Stack.top().Height;
		if (Height > 0)
		{
			const int32 Area = (NumCols - CurrentColIndex) * Height;
			const int32 StartIndex = Stack.top().Index - NumCols * (Height - 1);
			const int32 EndIndex = Stack.top().Index + ((Area / Height) - 1);
			LargestRect.TestNewMaxArea(Area, StartIndex, EndIndex);
		}
		Stack.pop();
	}
}

TSet<FFactor> USpawnAreaManagerComponent::FindAllFactors(const int32 Number)
{
	TSet<FFactor> Out;
	if (Number == 0) return Out;
	if (Number == 1)
	{
		Out.Add(FFactor(1, 1));
		return Out;
	}
	for (int i = 1; i <= Number / 2; i++)
	{
		if (Number % i == 0)
		{
			Out.Add(FFactor(i, Number / i));
		}
	}
	return Out;
}

FFactor USpawnAreaManagerComponent::FindLargestFactors(const int32 Number)
{
	FFactor Factor(-1, -1);
	float MinDistance = Number;
	for (int i = 2; i <= Number / 2; i++)
	{
		if (Number % i == 0)
		{
			const int32 OtherNumber = Number / i;
			const float Distance = abs(i - OtherNumber);
			if (Distance < MinDistance)
			{
				MinDistance = Distance;
				Factor.Factor1 = i;
				Factor.Factor2 = OtherNumber;
			}
		}
	}
	return Factor;
}

TSet<FFactor> USpawnAreaManagerComponent::FindBestFittingFactors(const int32 Number, const int32 Constraint1,
	const int32 Constraint2)
{
	TSet<FFactor> FittingFactors;
	float MinDistance = Number;
	// Add any factors that that fit within constraints, and record the minimum distance between them
	for (const FFactor& Factor : FindAllFactors(Number))
	{
		const float Dist = abs(Factor.Factor1 - Factor.Factor2);
		if ((Factor.Factor1 <= Constraint1 && Factor.Factor2 <= Constraint2) || (Factor.Factor2 <= Constraint1 && Factor
			.Factor1 <= Constraint2))
		{
			FittingFactors.Add(Factor);
			if (Dist < MinDistance)
			{
				MinDistance = Dist;
			}
		}
	}

	TSet<FFactor> BestFactors;
	// Add the factors with the minimum distance, since there might be more than one
	for (const FFactor& Factor : FittingFactors)
	{
		if (abs(Factor.Factor1 - Factor.Factor2) == MinDistance)
		{
			BestFactors.Add(Factor);
		}
	}

	UE_LOG(LogTargetManager, Display, TEXT("MinDistance between factors of %d: %.2f"), Number, MinDistance);

	return BestFactors;
}

TArray<std::pair<int32, int32>> USpawnAreaManagerComponent::FindBorderingIndicesInRect(
	const TSet<USpawnArea*>& Bordering, const FLargestRect& Rect, const int32 NumCols)
{
	TArray<std::pair<int32, int32>> BorderingInRect;

	for (const USpawnArea* SpawnArea : Bordering)
	{
		const int32 RowIndex = SpawnArea->GetIndex() / NumCols;
		const int32 ColIndex = SpawnArea->GetIndex() % NumCols;

		const bool bGrEqualToStart = RowIndex >= Rect.StartRowIndex && ColIndex >= Rect.StartColIndex;
		const bool bLeEqualToEnd = RowIndex <= Rect.EndRowIndex && ColIndex <= Rect.EndColIndex;

		// Don't consider any indices not within the largest rectangle
		if (!bGrEqualToStart || !bLeEqualToEnd) continue;

		BorderingInRect.Add({RowIndex, ColIndex});
	}

	return BorderingInRect;
}

bool USpawnAreaManagerComponent::IsPrime(const int32 Number)
{
	if (Number <= 1) return false;
	if (Number == 2 || Number == 3) return true;
	if (Number % 2 == 0 || Number % 3 == 0) return false;

	// All prime numbers > 3 can be expressed by 6k ± 1, where k is a positive integer
	int i = 5;

	while (i * i <= Number)
	{
		// Check divisibility from (5, sqrt(number)), or by an integer that is 2 greater
		if (Number % i == 0 || Number % (i + 2) == 0) return false;
		i += 6;
	}

	return true;
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

void USpawnAreaManagerComponent::FindValidIndexCombinationsDFS(const int32 StartIndex, FDFSLoopParams& Params) const
{
	if (Params.bFound) return;

	// Test to see if block total distance <= Best Total Distance
	if (Params.CurrentBlock.Num() == Params.BlockSize)
	{
		float TotalDist = 0.0f;
		for (int32 i = 0; i < Params.CurrentBlock.Num(); ++i)
			for (int32 j = i + 1; j < Params.CurrentBlock.Num(); ++j)
				TotalDist += CalcManhattanDist(Params.CurrentBlock.Indices[i], Params.CurrentBlock.Indices[j],
					Params.NumCols);;

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

void USpawnAreaManagerComponent::PrintDebug_GridDFS(const FDFSLoopParams& LoopParams)
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

void USpawnAreaManagerComponent::PrintDebug_GridLargestRect(const FLargestRect& LargestRect, const int32 NumCols)
{
	const int32 StartRowIndex = LargestRect.StartIndex / NumCols;
	const int32 StartColumnIndex = LargestRect.StartIndex % NumCols;
	const int32 EndRowIndex = LargestRect.EndIndex / NumCols;
	const int32 EndColumnIndex = LargestRect.EndIndex % NumCols;

	UE_LOG(LogTargetManager, Display, TEXT("LargestRect MaxArea: %d"), LargestRect.MaxArea);
	UE_LOG(LogTargetManager, Display, TEXT("LargestRect StartIndices: [%d, %d](%d), EndIndices: [%d, %d](%d)"),
		StartRowIndex, StartColumnIndex, LargestRect.StartIndex, EndRowIndex, EndColumnIndex, LargestRect.EndIndex);
	UE_LOG(LogTargetManager, Display, TEXT("LargestRect ChosenStart: [%d, %d](%d), ChosenEnd: [%d, %d](%d)"),
		LargestRect.ChosenStartRowIndex, LargestRect.ChosenStartColIndex,
		LargestRect.ChosenStartRowIndex * NumCols + LargestRect.ChosenStartColIndex, LargestRect.ChosenEndRowIndex,
		LargestRect.ChosenEndColIndex, LargestRect.ChosenEndRowIndex * NumCols + LargestRect.ChosenEndColIndex);
}

void USpawnAreaManagerComponent::PrintDebug_Matrix(const TArray<int32>& Matrix, const int32 NumRows,
	const int32 NumCols)
{
	FString Line;
	TArray<FString> Lines;
	Lines.Init("", NumRows);

	const FString Xs = NumRows * NumCols >= 100 ? "XXX" : "XX";

	FNumberFormattingOptions Options;
	Options.MinimumIntegralDigits = NumRows * NumCols >= 100 ? 3 : 2;
	Options.MaximumIntegralDigits = NumRows * NumCols >= 100 ? 3 : 2;

	FNumberFormattingOptions RowNumberOptions;
	RowNumberOptions.MinimumIntegralDigits = NumRows >= 10 ? 2 : 1;
	RowNumberOptions.MaximumIntegralDigits = NumRows >= 10 ? 2 : 1;

	int32 CurrentIndex = 0;
	int32 CurrentRow = NumRows - 1;

	for (const int32 i : Matrix)
	{
		FString Number = FText::AsNumber(CurrentIndex, &Options).ToString();
		Line += (i == 1 ? Number : Xs) + " ";
		if ((CurrentIndex + 1) % NumCols == 0 && (CurrentIndex + 1) >= NumCols)
		{
			Lines[CurrentRow] = Line;
			CurrentRow--;
			Line.Empty();
		}
		CurrentIndex++;
	}
	Line.Empty();

	FString DashedLine;
	FString ColumnNumberLine = "Cols";

	for (int i = 0; i < RowNumberOptions.MinimumIntegralDigits; i++) ColumnNumberLine += " ";

	ColumnNumberLine += "| ";

	for (int i = 0; i < NumCols; i++)
	{
		ColumnNumberLine += FText::AsNumber(i, &Options).ToString() + " ";
	}

	for (int i = 0; i < ColumnNumberLine.Len() - 1; i++) DashedLine += "-";

	UE_LOG(LogTargetManager, Display, TEXT("Index Validity:"));
	UE_LOG(LogTargetManager, Display, TEXT("%s"), *ColumnNumberLine);
	UE_LOG(LogTargetManager, Display, TEXT("%s"), *DashedLine);
	CurrentRow = NumRows - 1;
	for (auto String : Lines)
	{
		FString CurrentRowString = FText::AsNumber(CurrentRow, &RowNumberOptions).ToString();
		UE_LOG(LogTargetManager, Display, TEXT("Row %s| %s"), *CurrentRowString, *String);
		CurrentRow--;
	}
}
