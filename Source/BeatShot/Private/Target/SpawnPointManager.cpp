// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Target/SpawnPointManager.h"
#include "GlobalConstants.h"

FSpawnPoint::FSpawnPoint()
{
	Point = FVector(-1);
	ActualChosenPoint = FVector(-1);
	Scale = FVector(1);
	TotalSpawns = INDEX_NONE;
	TotalHits = 0;
	IncrementY = 0.f;
	IncrementZ = 0.f;
	Index = INDEX_NONE;
	IndexType = EGridIndexType::None;
	BorderingIndices = TArray<int32>();
	OverlappingPoints = TArray<FVector>();
	TargetGuid = FGuid();
	bIsActivated = false;
	bIsRecent = false;
	TimeSetRecent = DBL_MAX;
}

FSpawnPoint::FSpawnPoint(const int32 NewIndex, const FVector& NewPoint, const float IncY, const float IncZ, const int32 Width, const int32 Size)
{
	Index = NewIndex;
	Point = NewPoint;
	IncrementY = IncY;
	IncrementZ = IncZ;
	IndexType = FindIndexType(NewIndex, Width, Size);
	BorderingIndices = FindBorderingIndices(IndexType, NewIndex, Width);
	ActualChosenPoint = FVector(-1);
	Scale = FVector(1);
	TotalSpawns = INDEX_NONE;
	TotalHits = 0;
	Center = FVector(Point.X, roundf(Point.Y + IncrementY / 2.f), roundf(Point.Z + IncrementZ / 2.f));
	OverlappingPoints = TArray<FVector>();
	TargetGuid = FGuid();
	bIsActivated = false;
	bIsRecent = false;
	TimeSetRecent = DBL_MAX;
}

FSpawnPoint::FSpawnPoint(const int32 NewIndex)
{
	Point = FVector(-1);
	ActualChosenPoint = FVector(-1);
	Scale = FVector(1);
	TotalSpawns = INDEX_NONE;
	TotalHits = 0;
	IncrementY = 0.f;
	IncrementZ = 0.f;
	Index = NewIndex;
	IndexType = EGridIndexType::None;
	BorderingIndices = TArray<int32>();
	OverlappingPoints = TArray<FVector>();
	TargetGuid = FGuid();
	bIsActivated = false;
	bIsRecent = false;
	TimeSetRecent = DBL_MAX;
}

FSpawnPoint::FSpawnPoint(const FVector& NewPoint)
{
	Point = NewPoint;
	ActualChosenPoint = FVector(-1);
	Scale = FVector(1);
	TotalSpawns = INDEX_NONE;
	TotalHits = 0;
	IncrementY = 0.f;
	IncrementZ = 0.f;
	Index = INDEX_NONE;
	IndexType = EGridIndexType::None;
	BorderingIndices = TArray<int32>();
	OverlappingPoints = TArray<FVector>();
	TargetGuid = FGuid();
	bIsActivated = false;
	bIsRecent = false;
	TimeSetRecent = DBL_MAX;
}

bool FSpawnPoint::IsCornerIndex() const
{
	if (IndexType == EGridIndexType::Corner_TopLeft || IndexType == EGridIndexType::Corner_TopRight || IndexType == EGridIndexType::Corner_BottomRight || IndexType ==
	EGridIndexType::Corner_BottomLeft)
	{
		return true;
	}
	return false;
}

bool FSpawnPoint::IsBorderIndex() const
{
	if (IndexType == EGridIndexType::Border_Top || IndexType == EGridIndexType::Border_Right || IndexType == EGridIndexType::Border_Bottom || IndexType == EGridIndexType::Border_Left)
	{
		return true;
	}
	return false;
}

TArray<int32> FSpawnPoint::FindBorderingIndices(const EGridIndexType InGridIndexType, const int32 InIndex, const int32 InWidth)
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

FVector FSpawnPoint::GetRandomSubPoint(const TArray<EBorderingDirection>& BlockedDirections) const
{
	float MinY = Point.Y;
	float MaxY = Point.Y + IncrementY;
	float MinZ = Point.Z;
	float MaxZ = Point.Z + IncrementZ;
	if (BlockedDirections.Contains(EBorderingDirection::Left))
	{
		MinY = Center.Y;
	}
	if (BlockedDirections.Contains(EBorderingDirection::Right))
	{
		MaxY = Center.Y;
	}
	if (BlockedDirections.Contains(EBorderingDirection::Down))
	{
		MinZ = Center.Z;
	}
	if (BlockedDirections.Contains(EBorderingDirection::Up))
	{
		MaxZ = Center.Z;
	}

	const float Y = roundf(FMath::FRandRange(MinY, MaxY - 1.f));
	const float Z = roundf(FMath::FRandRange(MinZ, MaxZ - 1.f));
	return FVector(Point.X, Y, Z);
}

EGridIndexType FSpawnPoint::FindIndexType(const int32 InIndex, const int32 InSize, const int32 InWidth)
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

void FSpawnPoint::SetIsRecent(const bool bSetIsRecent)
{
	bIsRecent = bSetIsRecent;
	if (bSetIsRecent)
	{
		TimeSetRecent = FPlatformTime::Seconds();
	}
	else
	{
		TimeSetRecent = DBL_MAX;
		EmptyOverlappingPoints();
	}
}

TArray<FVector>& FSpawnPoint::SetOverlappingPoints(const float InMinTargetDistance, const float InMinOverlapRadius, const FVector& InScale,
	const FVector& InOrigin, const FVector& InNegativeExtents, const FVector& InPositiveExtents)
{
	float Radius = InScale.X * SphereTargetDiameter + InMinTargetDistance / 2.f;
	Radius = FMath::Max(Radius, InMinOverlapRadius);
	const FSphere Sphere = FSphere(Center, Radius);
	int Count = 0;
	for (float Z = InNegativeExtents.Z; Z < InPositiveExtents.Z; Z += IncrementZ)
	{
		for (float Y = InNegativeExtents.Y; Y < InPositiveExtents.Y; Y += IncrementY)
		{
			Count++;
			if (FVector Loc = FVector(InOrigin.X, Y, Z); Sphere.IsInside(Loc))
			{
				OverlappingPoints.AddUnique(Loc);
			}
		}
	}
	//UE_LOG(LogTargetManager, Display, TEXT("GetOverlappingPoints Count %d"), Count);
	//DrawDebugSphere(GetWorld(), Center, Scale * SphereTargetRadius * 2 + (BSConfig.MinDistanceBetweenTargets / 2.f), 32, FColor::Magenta, false, 0.5f);
	//UE_LOG(LogTargetManager, Display, TEXT("BlockedPoints: %d"), BlockedPoints.Num());
	return OverlappingPoints;
}

// ---------------------- //
// - USpawnPointManager - //
// ---------------------- //

void USpawnPointManager::InitSpawnPointManager(const FBSConfig& InBSConfig, const FVector& InOrigin, const FVector& InStaticExtents)
{
	BSConfig = InBSConfig;
	Origin = InOrigin;
	StaticExtents = InStaticExtents;
}

TArray<FVector> USpawnPointManager::InitializeSpawnPoints(const FVector& NegativeExtents, const FVector& PositiveExtents)
{
	StaticNegativeExtents = NegativeExtents;
	StaticPositiveExtents = PositiveExtents;
	
	SetAppropriateSpawnMemoryValues();
	
	TArray<FVector> AllSpawnLocations;
	
	int TempWidth = 0;
	int TempHeight = 0;
	int Index = 0;
	
	for (float Z = NegativeExtents.Z; Z < PositiveExtents.Z; Z += SpawnMemoryIncZ)
	{
		TempWidth = 0;
		for (float Y = NegativeExtents.Y; Y < PositiveExtents.Y; Y += SpawnMemoryIncY)
		{
			FVector Loc(Origin.X, Y, Z);
			AllSpawnLocations.Add(Loc);
			SpawnPoints.Emplace(
					Index,
					Loc,
					SpawnMemoryIncY,
					SpawnMemoryIncZ,
					BSConfig.GridConfig.NumHorizontalGridTargets,
					BSConfig.GridConfig.NumVerticalGridTargets * BSConfig.GridConfig.NumHorizontalGridTargets);
			SpawnPoints[Index].ActualChosenPoint = Loc;
			Index++;
			TempWidth++;
		}
		TempHeight++;
	}
	Width = TempWidth;
	Height = TempHeight;
	UE_LOG(LogTemp, Display, TEXT("SpawnMemoryScaleY %f SpawnMemoryScaleZ %f"), SpawnMemoryScaleY, SpawnMemoryScaleZ);
	UE_LOG(LogTemp, Display, TEXT("SpawnMemoryIncY %d SpawnMemoryIncZ %d"), SpawnMemoryIncY, SpawnMemoryIncZ);
	UE_LOG(LogTemp, Display, TEXT("SpawnCounterSize: %d %llu"), SpawnPoints.Num(), SpawnPoints.GetAllocatedSize());
	return AllSpawnLocations;
}

// SpawnPoint getters

FSpawnPoint* USpawnPointManager::FindSpawnPointFromIndex(const int32 InIndex)
{
	return SpawnPoints.FindByPredicate([&InIndex] (const FSpawnPoint& SpawnPoint)
	{
		if (SpawnPoint.Index == InIndex)
		{
			return true;
		}
		return false;
	});
}

FSpawnPoint* USpawnPointManager::FindSpawnPointFromLocation(const FVector& InLocation)
{
	return SpawnPoints.FindByPredicate([&InLocation](const FSpawnPoint& SpawnPoint)
	{
		if (InLocation.Y >= SpawnPoint.Point.Y && InLocation.Y < SpawnPoint.Point.Y + SpawnPoint.IncrementY &&
			(InLocation.Z >= SpawnPoint.Point.Z && InLocation.Z < SpawnPoint.Point.Z + SpawnPoint.IncrementZ))
		{
			return true;
		}
		return false;
	});
}

FSpawnPoint* USpawnPointManager::FindSpawnPointFromGuid(const FGuid& InGuid)
{
	return SpawnPoints.FindByPredicate([&InGuid](const FSpawnPoint& SpawnPoint)
	{
		if (SpawnPoint.TargetGuid == InGuid)
		{
			return true;
		}
		return false;
	});
}

FSpawnPoint* USpawnPointManager::FindOldestRecentSpawnPoint() const
{
	TArray<FSpawnPoint> RecentSpawnPoints = GetRecentSpawnPoints();
	
	if (RecentSpawnPoints.IsEmpty())
	{
		return nullptr;
	}
	
	FSpawnPoint* MostRecent = &RecentSpawnPoints.Top();
	for (FSpawnPoint& SpawnPoint : RecentSpawnPoints)
	{
		if (SpawnPoint.GetTimeSetRecent() < MostRecent->GetTimeSetRecent())
		{
			MostRecent = &SpawnPoint;
		}
	}
	return MostRecent;
}

// SpawnPoint array getters

TArray<FSpawnPoint> USpawnPointManager::GetRecentSpawnPoints() const
{
	return SpawnPoints.FilterByPredicate([] (const FSpawnPoint& SpawnPoint)
	{
		if (SpawnPoint.IsRecent())
		{
			return true;
		}
		return false;
	});
}

TArray<FSpawnPoint> USpawnPointManager::GetActivatedSpawnPoints() const
{
	return SpawnPoints.FilterByPredicate([] (const FSpawnPoint& SpawnPoint)
	{
		if (SpawnPoint.IsActivated())
		{
			return true;
		}
		return false;
	});
}

TArray<FSpawnPoint> USpawnPointManager::GetActivatedOrRecentSpawnPoints() const
{
	return SpawnPoints.FilterByPredicate([] (const FSpawnPoint& SpawnPoint)
	{
		if (SpawnPoint.IsActivated() || SpawnPoint.IsRecent())
		{
			return true;
		}
		return false;
	});
}

// SpawnPoint overlaps

void USpawnPointManager::SetOverlappingPoints(FSpawnPoint& Point, const FVector& Scale) const
{
	Point.SetOverlappingPoints(BSConfig.SpatialConfig.MinDistanceBetweenTargets, MinOverlapRadius, Scale, Origin, StaticNegativeExtents, StaticPositiveExtents);
}

void USpawnPointManager::RemoveOverlappingPointsFromSpawnLocations(TArray<FVector>& SpawnLocations, const FVector& Scale) const
{
	TArray<FVector> OverlappingPoints;
	/* Resizing Overlapping Points if necessary */
	for (FSpawnPoint& Point : GetActivatedOrRecentSpawnPoints())
	{
		TArray<FVector> CurrentBlockedLocations;
		if (Scale.Length() > Point.Scale.Length())
		{
			SetOverlappingPoints(Point, Scale);
		}
		CurrentBlockedLocations = Point.GetOverlappingPoints();
		OverlappingPoints.Append(CurrentBlockedLocations);
		if (bShowDebug_SpawnMemory)
		{
			for (FVector Vector : Point.GetOverlappingPoints())
			{
				DrawDebugPoint(GetWorld(), Vector, 10, FColor::Red, false, 0.35f);
			}
		}
	}
	SpawnLocations = SpawnLocations.FilterByPredicate([&] (const FVector& Location)
	{
		if (OverlappingPoints.Contains(Location))
		{
			OverlappingPoints.Remove(Location);
			return false;
		}
		return true;
	});
}

// flags

void USpawnPointManager::FlagSpawnPointAsRecent(const FGuid SpawnPointGuid)
{
	if (FSpawnPoint* Point = FindSpawnPointFromGuid(SpawnPointGuid))
	{
		if (Point->IsActivated())
		{
			RemoveActivatedFlagFromSpawnPoint(SpawnPointGuid);
		}
		Point->SetIsRecent(true);
	}
}

void USpawnPointManager::FlagSpawnPointAsActivated(const FGuid SpawnPointGuid)
{
	if (FSpawnPoint* Point = FindSpawnPointFromGuid(SpawnPointGuid))
	{
		if (Point->IsRecent())
		{
			RemoveRecentFlagFromSpawnPoint(SpawnPointGuid);
		}
		Point->SetIsActivated(true);
		SetOverlappingPoints(*Point, Point->Scale);
	}
}

void USpawnPointManager::RemoveRecentFlagFromSpawnPoint(const FGuid SpawnPointGuid)
{
	if (FSpawnPoint* Point = FindSpawnPointFromGuid(SpawnPointGuid))
	{
		Point->SetIsRecent(false);
	}
}

void USpawnPointManager::RemoveActivatedFlagFromSpawnPoint(const FGuid SpawnPointGuid)
{
	if (FSpawnPoint* Point = FindSpawnPointFromGuid(SpawnPointGuid))
	{
		Point->SetIsActivated(false);
	}
}

void USpawnPointManager::SetAppropriateSpawnMemoryValues()
{
	if (BSConfig.SpatialConfig.TargetDistributionMethod == ETargetDistributionPolicy::Grid)
	{
		SpawnMemoryIncY = BSConfig.GridConfig.GridSpacing.X + BSConfig.TargetConfig.MaxTargetScale * SphereTargetDiameter;
		SpawnMemoryIncZ = BSConfig.GridConfig.GridSpacing.Y + BSConfig.TargetConfig.MaxTargetScale * SphereTargetDiameter;
	}
	else
	{
		const int32 HalfWidth = StaticExtents.Y;
		const int32 HalfHeight = StaticExtents.Z;
		bool bWidthScaleSelected = false;
		bool bHeightScaleSelected = false;
		for (const int32 Scale : PreferredScales)
		{
			if (!bWidthScaleSelected)
			{
				if (HalfWidth % Scale == 0)
				{
					if (HalfWidth / Scale % 5 == 0)
					{
						SpawnMemoryScaleY = 1.f / static_cast<float>(Scale);
						bWidthScaleSelected = true;
					}
				}
			}
			if (!bHeightScaleSelected)
			{
				if (HalfHeight % Scale == 0)
				{
					if (HalfHeight / Scale % 5 == 0)
					{
						SpawnMemoryScaleZ = 1.f / static_cast<float>(Scale);
						bHeightScaleSelected = true;
					}
				}
			}
			if (bHeightScaleSelected && bWidthScaleSelected)
			{
				break;
			}
		}
		if (!bWidthScaleSelected || !bHeightScaleSelected)
		{
			UE_LOG(LogTemp, Warning, TEXT("Couldn't Find Height/Width"));
			SpawnMemoryScaleY = 1.f / 50.f;
			SpawnMemoryScaleZ = 1.f / 50.f;
		}
		SpawnMemoryIncY = roundf(1.f / SpawnMemoryScaleY);
		SpawnMemoryIncZ = roundf(1.f / SpawnMemoryScaleZ);
	}
	MinOverlapRadius = (SpawnMemoryIncY + SpawnMemoryIncZ) / 2.f;
}

int32 USpawnPointManager::GetOutArrayIndexFromSpawnCounterIndex(const int32 SpawnCounterIndex) const
{
	/* First find the Row and Column number that corresponds to the SpawnCounter index */
	const int32 SpawnCounterRowNum = SpawnCounterIndex / GetSpawnPointsWidth();
	const int32 SpawnCounterColNum = SpawnCounterIndex % GetSpawnPointsWidth();

	const int32 WidthScaleFactor = GetSpawnPointsWidth() / 5;
	const int32 HeightScaleFactor = GetSpawnPointsHeight() / 5;

	/* Scale down the SpawnCounter row and column numbers */
	const int32 Row = SpawnCounterRowNum / HeightScaleFactor;
	const int32 Col = SpawnCounterColNum / WidthScaleFactor;
	const int32 Index = Row * 5 + Col;

	return Index;
}
