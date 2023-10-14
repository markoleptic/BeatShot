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
	AdjacentIndices = TArray<int32>();
	OverlappingVertices = TArray<FVector>();
	Size = 0;
}

void USpawnArea::Init(const FSpawnAreaParams& InParams)
{
	Width = InParams.IncY;
	Height = InParams.IncZ;

	Vertex_BottomLeft = InParams.BottomLeft;
	CenterPoint = Vertex_BottomLeft + FVector(0, InParams.IncY / 2.f, InParams.IncZ / 2.f);
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
	AdjacentIndices = FindAdjacentIndices(FindIndexType(InParams.Index, Size, InParams.NumHorizontalTargets),
		InParams.Index, InParams.NumHorizontalTargets);
	OverlappingVertices = TArray<FVector>();
}

TArray<int32> USpawnArea::FindAdjacentIndices(const EGridIndexType InGridIndexType, const int32 InIndex,
	const int32 InWidth)
{
	TArray<int32> ReturnArray = TArray<int32>();

	const int32 TopLeft = InIndex + InWidth - 1;
	const int32 Top = InIndex + InWidth;
	const int32 TopRight = InIndex + InWidth + 1;
	const int32 Right = InIndex + 1;
	const int32 BottomRight = InIndex - InWidth + 1;
	const int32 Bottom = InIndex - InWidth;
	const int32 BottomLeft = InIndex - InWidth - 1;
	const int32 Left = InIndex - 1;

	switch (InGridIndexType)
	{
	case EGridIndexType::None:
		break;
	case EGridIndexType::Corner_TopLeft:
		ReturnArray.Add(Right);
		ReturnArray.Add(Bottom);
		ReturnArray.Add(BottomRight);
		break;
	case EGridIndexType::Corner_TopRight:
		ReturnArray.Add(Left);
		ReturnArray.Add(Bottom);
		ReturnArray.Add(BottomLeft);
		break;
	case EGridIndexType::Corner_BottomRight:
		ReturnArray.Add(Left);
		ReturnArray.Add(Top);
		ReturnArray.Add(TopLeft);
		break;
	case EGridIndexType::Corner_BottomLeft:
		ReturnArray.Add(Right);
		ReturnArray.Add(Top);
		ReturnArray.Add(TopRight);
		break;
	case EGridIndexType::Border_Top:
		ReturnArray.Add(Left);
		ReturnArray.Add(Right);
		ReturnArray.Add(BottomRight);
		ReturnArray.Add(Bottom);
		ReturnArray.Add(BottomLeft);
		break;
	case EGridIndexType::Border_Right:
		ReturnArray.Add(TopLeft);
		ReturnArray.Add(Top);
		ReturnArray.Add(Left);
		ReturnArray.Add(BottomLeft);
		ReturnArray.Add(Bottom);
		break;
	case EGridIndexType::Border_Bottom:
		ReturnArray.Add(TopLeft);
		ReturnArray.Add(Top);
		ReturnArray.Add(TopRight);
		ReturnArray.Add(Right);
		ReturnArray.Add(Left);
		break;
	case EGridIndexType::Border_Left:
		ReturnArray.Add(Top);
		ReturnArray.Add(TopRight);
		ReturnArray.Add(Right);
		ReturnArray.Add(BottomRight);
		ReturnArray.Add(Bottom);
		break;
	case EGridIndexType::Middle:
		ReturnArray.Add(TopLeft);
		ReturnArray.Add(Top);
		ReturnArray.Add(TopRight);
		ReturnArray.Add(Right);
		ReturnArray.Add(BottomRight);
		ReturnArray.Add(Bottom);
		ReturnArray.Add(BottomLeft);
		ReturnArray.Add(Left);
		break;
	}
	return ReturnArray;
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
	
	switch (DamageEvent.DamageType) {
	case ETargetDamageType::Tracking:
		{
			// Instead of using the spawn area where the target started, use the current location
			USpawnArea* SpawnAreaByLoc = FindSpawnAreaFromLocation(DamageEvent.Transform.GetLocation());
			if (!SpawnAreaByLoc) return;

			// Total Tracking Damage Possible is done on tick in UpdateTotalTrackingDamagePossible
			
			// Only increment total tracking damage if damage came from player
			if (!DamageEvent.bDamagedSelf && DamageEvent.DamageDelta > 0.f) SpawnAreaByLoc->IncrementTotalTrackingDamage();
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
	case ETargetDamageType::Combined:
		UE_LOG(LogTargetManager, Warning, TEXT("DamageEvent with DamageType None or Combined."));
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
	TArray<FVector> OverlappingPoints = SpawnArea->GenerateOverlappingVertices(
		GetTc().MinDistanceBetweenTargets,
		SpawnArea->GetTargetScale(),
		DebugVertices,
		bShowDebug_OverlappingVertices_OnFlaggedManaged);
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

void USpawnAreaManagerComponent::FlagSpawnAreaAsActivated(const FGuid TargetGuid, const bool bCanActivateWhileActivated) const
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
	const TArray<FVector> OverlappingPoints = SpawnArea->GenerateOverlappingVertices(
		GetTc().MinDistanceBetweenTargets,
		SpawnArea->GetTargetScale(),
		DebugVertices,
		bShowDebug_OverlappingVertices_OnFlaggedActivated);
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
	TArray<USpawnArea*> Out;
	TArray<USpawnArea*> Managed = GetManagedSpawnAreas();
	
	// Assumes that we cannot activate an already activated target
	switch (GetTc().TargetActivationSelectionPolicy) {
	// Activated -> Bordering -> Recent
	case ETargetActivationSelectionPolicy::Bordering:
		{
			FilterActivatedIndices(Managed);
			Out = Managed;
			if (Managed.Num() > NumToActivate)
			{
				FilterBorderingIndices(Managed);
				if (Managed.Num() > NumToActivate)
				{
					Out = Managed;
					FilterRecentIndices(Managed);
					if (Managed.Num() > NumToActivate) Out = Managed;
				}
			}

		}
		break;
	case ETargetActivationSelectionPolicy::None:
	case ETargetActivationSelectionPolicy::Random:
		{
			FilterActivatedIndices(Managed);
			Out = Managed;
			if (Managed.Num() > NumToActivate)
			{
				FilterRecentIndices(Out);
				if (Managed.Num() > NumToActivate) Out = Managed;
			}
		}
		break;
	}
	return Out;
}

TArray<USpawnArea*> USpawnAreaManagerComponent::GetSpawnableSpawnAreas(const FVector& Scale,
	const FExtrema& Extrema, const int32 NumToSpawn, const ERuntimeTargetSpawningLocationSelectionMode Mode) const
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
			// Always filter managed and activated for spawning
			FilterManagedIndices(Out);
			FilterActivatedIndices(Out);
			
			// Try filtering out recent
			//TArray<USpawnArea*> Temp = Out;
			//FilterRecentIndices(Temp);
			//if (Temp.Num() < NumToSpawn) break;
			//Out = Temp;

			switch (Mode) {
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
					// TODO: This just results in a straight line in the same place over and over
					// TODO: also prob need to include filterrecenttargets
					TArray<int32> Indices;
					Indices.Init(0, Size.Y * Size.Z);
					for (const USpawnArea* Area : Out)
					{
						for (const int32 Index: Area->GetBorderingIndices())
						{
							Indices[Index]++;
						}
						UE_LOG(LogTemp, Display, TEXT("Index: %d NuM: %d"), Area->GetIndex(), Area->GetBorderingIndices().Num());
					}
					Out.Empty();
					for (int i = 0; i < Size.Z; i++)
					{
						FString Line;
						for (int j = 0; j < Size.Y; j++)
						{
							Line += FString::FromInt(Indices[i * j + i]) + " ";
						}
						UE_LOG(LogTemp, Display, TEXT("%s"), *Line);
					}
					/*Indices.Sort([&] (const int32 Index1, const int32 Index2)
					{
						return Index1 > Index2;
					});*/
					for (int i = 0; i < NumToSpawn; i++)
					{
						if (Indices.IsValidIndex(i) && SpawnAreas.IsValidIndex(i))
						{
							Out.Add(SpawnAreas[i]);
						}
					}
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

void USpawnAreaManagerComponent::HandleGridSpawnLocations(TArray<USpawnArea*>& ValidSpawnAreas, const int32 NumToSpawn,
	const bool bConsiderManagedInvalid) const
{
	ValidSpawnAreas = GetSpawnAreas();
	
	/* Without this condition, ValidSpawnAreas can contain SpawnAreas that refer to a non-activated already
	 * managed target. This means that the SpawnArea corresponding to an already managed target will get
	 * overriden with a new target, and the old target will no longer have an associated SpawnArea, causing the
	 * game to get stuck with a non-activated target. Only applies for modes that allow spawning targets
	 * without activation. */

	if (bConsiderManagedInvalid) FilterManagedIndices(ValidSpawnAreas);

	// Always filter activated
	FilterActivatedIndices(ValidSpawnAreas);
	if (ValidSpawnAreas.Num() < NumToSpawn) return;
	
	TArray<USpawnArea*> Temp = ValidSpawnAreas;
	
	switch (GetTc().TargetActivationSelectionPolicy)
	{
	case ETargetActivationSelectionPolicy::None:
	case ETargetActivationSelectionPolicy::Random:
		{
			// Try filtering out recent
			FilterRecentIndices(Temp);
			if (Temp.Num() < NumToSpawn) return;
			ValidSpawnAreas = Temp;
		}
		break;
	case ETargetActivationSelectionPolicy::Bordering:
		{
			// Try filtering out recent
			FilterRecentIndices(Temp);
			if (Temp.Num() < NumToSpawn) return;
			ValidSpawnAreas = Temp;

			// Try filtering out bordering
			FilterBorderingIndices(Temp);
			if (Temp.Num() < NumToSpawn) return;
			ValidSpawnAreas = Temp;
		}
		break;
	default:
		break;
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
				GetTc().MinDistanceBetweenTargets, SpawnArea->GetTargetScale(),
				DebugVertices, bShowDebug_OverlappingVertices_Dynamic);
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
			if (bShowDebug_OverlappingVertices_Dynamic)  DrawOverlappingVertices(SpawnArea, Scale, DebugVertices);
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

void USpawnAreaManagerComponent::FilterByExtrema(TArray<USpawnArea*>& ValidSpawnAreas,const FExtrema& Extrema) const
{
	TArray<const USpawnArea*> RemovedSpawnAreas;
	ValidSpawnAreas = SpawnAreas.FilterByPredicate([&](const USpawnArea* SpawnArea)
	{
		const FVector Vector = SpawnArea->GetBottomLeftVertex();
		if (Vector.Y < Extrema.Min.Y || Vector.Y >= Extrema.Max.Y || Vector.Z < Extrema.Min.Z || Vector.Z >= Extrema.Max.Z)
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

void USpawnAreaManagerComponent::FilterByEdgeOnly(TArray<USpawnArea*>& ValidSpawnAreas,
	const FExtrema& Extrema) const
{
	
	const float MaxY = Extrema.Max.Y - SpawnAreaInc.Y;
	const float MaxZ = Extrema.Max.Z - SpawnAreaInc.Z;
	
	const float MinY = Extrema.Min.Y;
	const float MinZ = Extrema.Min.Z;
	
	/*const float OriginX = Origin.X;
	
	for (float Y = Extrema.Min.Y; Y < Extrema.Max.Y; Y += SpawnAreaInc.Y)
	{
		if (USpawnArea* Found = FindSpawnAreaFromLocation(FVector(OriginX, Y, Extrema.Min.Z)))
		{
			ValidSpawnAreas.AddUnique(Found);
		}
		if (USpawnArea* Found = FindSpawnAreaFromLocation(FVector(OriginX, Y, MaxZ)))
		{
			ValidSpawnAreas.AddUnique(Found);
		}
	}
	for (float Z = Extrema.Min.Z; Z < Extrema.Max.Z; Z += SpawnAreaInc.Z)
	{
		if (USpawnArea* Found = FindSpawnAreaFromLocation(FVector(OriginX, Extrema.Min.Y, Z)))
		{
			ValidSpawnAreas.AddUnique(Found);
		}
		if (USpawnArea* Found = FindSpawnAreaFromLocation(FVector(OriginX, MaxY, Z)))
		{
			ValidSpawnAreas.AddUnique(Found);
		}
	}*/
	
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
	/*const TArray<USpawnArea*> RemovedAreas = SpawnAreas.FilterByPredicate([&](const USpawnArea* SpawnArea)
	{
		return !ValidSpawnAreas.Contains(SpawnArea);
	});
	DrawDebug_Boxes(RemovedAreas, FColor::Red, 4, 3);*/
	#endif
}

void USpawnAreaManagerComponent::FilterBorderingIndices(TArray<USpawnArea*>& ValidSpawnAreas) const
{
	if (!MostRecentSpawnArea) return;
	
	// Filter out non-bordering points
	const TArray<int32> BorderingIndices = MostRecentSpawnArea->GetBorderingIndices();
	if (BorderingIndices.IsEmpty()) return;
	
	ValidSpawnAreas = ValidSpawnAreas.FilterByPredicate([&](const USpawnArea* SpawnArea)
	{
		if (!BorderingIndices.Contains(SpawnArea->GetIndex())) return false;
		
		return true;
	});
}

void USpawnAreaManagerComponent::FilterBorderingIndices(TArray<USpawnArea*>& ValidSpawnAreas, USpawnArea* Current) const
{
	if (!Current) return;
	const TArray<int32> BorderingIndices = Current->GetBorderingIndices();
	if (BorderingIndices.IsEmpty()) return;
	
	ValidSpawnAreas = ValidSpawnAreas.FilterByPredicate([&](const USpawnArea* SpawnArea)
	{
		if (!BorderingIndices.Contains(SpawnArea->GetIndex())) return false;
		return true;
	});
}

void USpawnAreaManagerComponent::FilterRecentIndices(TArray<USpawnArea*>& ValidSpawnAreas) const
{
	FilterIndices(ValidSpawnAreas, &USpawnArea::IsRecent,
		bShowDebug_FilteredRecentIndices, FColor::Turquoise);
}

void USpawnAreaManagerComponent::FilterActivatedIndices(TArray<USpawnArea*>& ValidSpawnAreas) const
{
	FilterIndices(ValidSpawnAreas, &USpawnArea::IsActivated,
	bShowDebug_FilteredActivatedIndices, FColor::Cyan);
}

void USpawnAreaManagerComponent::FilterManagedIndices(TArray<USpawnArea*>& ValidSpawnAreas) const
{
	FilterIndices(ValidSpawnAreas, &USpawnArea::IsManaged,
		bShowDebug_FilteredManagedIndices, FColor::Blue);
}

void USpawnAreaManagerComponent::FilterIndices(TArray<USpawnArea*>& ValidSpawnAreas,
	bool(USpawnArea::* FilterFunc)() const, const bool bShowDebug, const FColor& DebugColor) const
{
	TArray<const USpawnArea*> Removed;
	ValidSpawnAreas = ValidSpawnAreas.FilterByPredicate([&](const USpawnArea* SpawnArea)
	{
		if ((SpawnArea->*FilterFunc)())
		{
			#if !UE_BUILD_SHIPPING
			Removed.Add(SpawnArea);
			#endif
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
	int32 (USpawnArea::*TotalFunc)() const = bHitDamage ? &USpawnArea::GetTotalSpawns : &USpawnArea::GetTotalTrackingDamagePossible;
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

/* ---------------- */
/* -- Util/Debug -- */
/* ---------------- */

void USpawnAreaManagerComponent::DrawDebug_AllSpawnAreas() const
{
	DrawDebug_Boxes(GetAllBottomLeftVertices(), FColor::Cyan, 4, 0, true);
}

void USpawnAreaManagerComponent::ClearDebug_AllSpawnAreas() const
{
	FlushPersistentDebugLines(GetWorld());
}

void USpawnAreaManagerComponent::DrawDebug_Boxes(const TArray<FVector>& InLocations, const FColor& InColor,
	const int32 InThickness, const int32 InDepthPriority, const bool bPersistantLines) const
{
	const float Time = bPersistantLines ? -1.f : GetTc().TargetSpawnCD;
	const FVector HalfInc = FVector(0, GetSpawnAreaInc().Y * 0.5f, GetSpawnAreaInc().Z * 0.5f);
	UE_LOG(LogTemp, Display, TEXT("HalfInc: %s"), *HalfInc.ToString());
	for (const FVector& Vector : InLocations)
	{
		FVector Loc = FVector(Vector.X, Vector.Y + HalfInc.Y, Vector.Z + HalfInc.Z);
		DrawDebugBox(GetWorld(), Loc, FVector(0, HalfInc.Y, HalfInc.Z), InColor, bPersistantLines, Time,
			InDepthPriority, InThickness);
	}
}

void USpawnAreaManagerComponent::DrawDebug_Boxes(const TArray<const USpawnArea*>& InSpawnAreas, const FColor& InColor,
	const int32 InThickness, const int32 InDepthPriority, bool bPersistantLines) const
{
	const float Time = bPersistantLines ? -1.f : GetTc().TargetSpawnCD;
	const FVector HalfInc = FVector(0, GetSpawnAreaInc().Y * 0.5f, GetSpawnAreaInc().Z * 0.5f);
	UE_LOG(LogTemp, Display, TEXT("HalfInc: %s"), *HalfInc.ToString());
	for (const USpawnArea* SpawnArea : InSpawnAreas)
	{
		FVector Loc = SpawnArea->GetBottomLeftVertex() + HalfInc;
		DrawDebugBox(GetWorld(), Loc, FVector(0, HalfInc.Y, HalfInc.Z), InColor, bPersistantLines, Time,
			InDepthPriority, InThickness);
	}
}

void USpawnAreaManagerComponent::DrawDebug_Boxes(const TArray<USpawnArea*>& InSpawnAreas, const FColor& InColor,
	const int32 InThickness, const int32 InDepthPriority, bool bPersistantLines) const
{
	const float Time = bPersistantLines ? -1.f : GetTc().TargetSpawnCD;
	const FVector HalfInc = FVector(0, GetSpawnAreaInc().Y * 0.5f, GetSpawnAreaInc().Z * 0.5f);
	UE_LOG(LogTemp, Display, TEXT("HalfInc: %s"), *HalfInc.ToString());
	for (const USpawnArea* SpawnArea : InSpawnAreas)
	{
		FVector Loc = SpawnArea->GetBottomLeftVertex() + HalfInc;
		DrawDebugBox(GetWorld(), Loc, FVector(0, HalfInc.Y, HalfInc.Z), InColor, bPersistantLines, Time,
			InDepthPriority, InThickness);
	}
}

void USpawnAreaManagerComponent::DrawOverlappingVertices(const USpawnArea* SpawnArea, const FVector& Scale, const TArray<FVector>& DebugVertices) const
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
