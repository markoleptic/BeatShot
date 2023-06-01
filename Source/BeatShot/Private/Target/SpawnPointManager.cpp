// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Target/SpawnPointManager.h"
#include "GlobalConstants.h"

FSpawnPoint::FSpawnPoint()
{
	IncrementY = 0.f;
	IncrementZ = 0.f;
	CornerPoint = FVector(-1);
	CenterPoint = FVector(-1);
	ChosenPoint = FVector(-1);
	Scale = FVector(1);
	TotalSpawns = INDEX_NONE;
	TotalHits = 0;
	Index = INDEX_NONE;
	bIsActivated = false;
	bIsRecent = false;
	TimeSetRecent = DBL_MAX;
	IndexType = EGridIndexType::None;
	TargetGuid = FGuid();
	BorderingIndices = TArray<int32>();
	OverlappingPoints = TArray<FVector>();
}

FSpawnPoint::FSpawnPoint(const int32 InIndex, const FVector& InPoint, const bool bIsCornerPoint, const float IncY, const float IncZ,  const int32 InWidth, const int32 InSize)
{
	IncrementY = IncY;
	IncrementZ = IncZ;

	if (bIsCornerPoint)
	{
		CornerPoint = InPoint;
		CenterPoint = FindCenterPointFromCornerPoint(InPoint, IncY, IncZ);
	}
	else
	{
		CenterPoint = InPoint;
		CornerPoint = FindCornerPointFromCenterPoint(InPoint, IncY, IncZ);
	}
	ChosenPoint = FVector(-1);
	Scale = FVector(1);
	
	TotalSpawns = INDEX_NONE;
	TotalHits = 0;
	Index = InIndex;
	
	bIsActivated = false;
	bIsRecent = false;
	TimeSetRecent = DBL_MAX;
	IndexType = FindIndexType(InIndex, InSize, InWidth);
	TargetGuid = FGuid();
	BorderingIndices = FindBorderingIndices(FindIndexType(InIndex, InSize, InWidth), InIndex, InWidth);
	OverlappingPoints = TArray<FVector>();
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

FVector FSpawnPoint::FindCornerPointFromCenterPoint(const FVector& InCenterPoint, const float InIncY, const float InIncZ)
{
	return FVector(InCenterPoint.X, roundf(InCenterPoint.Y - InIncY / 2.f), roundf(InCenterPoint.Z - InIncZ / 2.f));
}

FVector FSpawnPoint::FindCenterPointFromCornerPoint(const FVector& InCornerPoint, const float InIncY, const float InIncZ)
{
	return FVector(InCornerPoint.X, roundf(InCornerPoint.Y + InIncY / 2.f), roundf(InCornerPoint.Z + InIncZ / 2.f));
}

FVector FSpawnPoint::GenerateRandomSubPoint(const TArray<EBorderingDirection>& BlockedDirections) const
{
	float MinY = CornerPoint.Y;
	float MaxY = CornerPoint.Y + IncrementY;
	float MinZ = CornerPoint.Z;
	float MaxZ = CornerPoint.Z + IncrementZ;
	if (BlockedDirections.Contains(EBorderingDirection::Left))
	{
		MinY = CenterPoint.Y;
	}
	if (BlockedDirections.Contains(EBorderingDirection::Right))
	{
		MaxY = CenterPoint.Y;
	}
	if (BlockedDirections.Contains(EBorderingDirection::Down))
	{
		MinZ = CenterPoint.Z;
	}
	if (BlockedDirections.Contains(EBorderingDirection::Up))
	{
		MaxZ = CenterPoint.Z;
	}

	const float Y = roundf(FMath::FRandRange(MinY, MaxY - 1.f));
	const float Z = roundf(FMath::FRandRange(MinZ, MaxZ - 1.f));
	return FVector(CornerPoint.X, Y, Z);
}

void FSpawnPoint::SetChosenPointAsRandomSubPoint(const TArray<EBorderingDirection>& BlockedDirections)
{
	ChosenPoint = GenerateRandomSubPoint(BlockedDirections);
}

void FSpawnPoint::IncrementTotalSpawns()
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

void FSpawnPoint::IncrementTotalHits()
{
	TotalHits++;
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
	const FVector& InOrigin, const FExtrema& InExtrema)
{
	float Radius = InScale.X * SphereTargetDiameter + InMinTargetDistance / 2.f;
	Radius = FMath::Max(Radius, InMinOverlapRadius);
	const FSphere Sphere = FSphere(CenterPoint, Radius);
	int Count = 0;
	for (float Z = InExtrema.Min.Z; Z < InExtrema.Max.Z; Z += IncrementZ)
	{
		for (float Y = InExtrema.Min.Y; Y < InExtrema.Max.Y; Y += IncrementY)
		{
			Count++;
			if (FVector Loc = FVector(InOrigin.X, Y, Z); Sphere.IsInside(Loc))
			{
				OverlappingPoints.AddUnique(Loc);
			}
		}
	}
	//UE_LOG(LogTargetManager, Display, TEXT("GetOverlappingPoints Count %d"), Count);
	//DrawDebugSphere(GetWorld(), CenterPoint, Scale * SphereTargetRadius * 2 + (BSConfig.MinDistanceBetweenTargets / 2.f), 32, FColor::Magenta, false, 0.5f);
	//UE_LOG(LogTargetManager, Display, TEXT("BlockedPoints: %d"), BlockedPoints.Num());
	return OverlappingPoints;
}

TArray<EBorderingDirection> FSpawnPoint::GetBorderingDirections(const TArray<FVector>& ValidLocations, const FExtrema& InExtrema) const
{
	TArray<EBorderingDirection> Directions;
	const FVector Left = CornerPoint + FVector(0, -IncrementY, 0);
	const FVector Right = CornerPoint + FVector(0, IncrementY, 0);
	const FVector Up = CornerPoint + FVector(0, 0, IncrementZ);
	const FVector Down = CornerPoint + FVector(0, 0, -IncrementZ);

	if (Left.Y != InExtrema.Min.Y && !ValidLocations.Contains(Left))
	{
		Directions.Add(EBorderingDirection::Left);
	}
	if (Right.Y != InExtrema.Max.Y && !ValidLocations.Contains(Right))
	{
		Directions.Add(EBorderingDirection::Right);
	}
	if (Up.Z != InExtrema.Max.Z && !ValidLocations.Contains(Up))
	{
		Directions.Add(EBorderingDirection::Up);
	}
	if (Down.Z != InExtrema.Min.Z && !ValidLocations.Contains(Down))
	{
		Directions.Add(EBorderingDirection::Down);
	}
	return Directions;
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

TArray<FVector> USpawnPointManager::InitializeSpawnPoints(const FExtrema& InStaticExtrema)
{
	StaticExtrema = InStaticExtrema;
	
	SetAppropriateSpawnMemoryValues();
	
	TArray<FVector> AllSpawnLocations;
	
	int TempWidth = 0;
	int TempHeight = 0;
	int Index = 0;
	
	for (float Z = InStaticExtrema.Min.Z; Z < InStaticExtrema.Max.Z; Z += SpawnMemoryIncZ)
	{
		TempWidth = 0;
		for (float Y = InStaticExtrema.Min.Y; Y < InStaticExtrema.Max.Y; Y += SpawnMemoryIncY)
		{
			FVector Loc(Origin.X, Y, Z);
			AllSpawnLocations.Add(Loc);
			SpawnPoints.Emplace(
					Index,
					Loc,
					bLocationsAreCorners,
					SpawnMemoryIncY,
					SpawnMemoryIncZ,
					BSConfig.GridConfig.NumHorizontalGridTargets,
					BSConfig.GridConfig.NumVerticalGridTargets * BSConfig.GridConfig.NumHorizontalGridTargets);
			SpawnPoints[Index].ChosenPoint = Loc;
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

void USpawnPointManager::SetAppropriateSpawnMemoryValues()
{
	switch (BSConfig.TargetConfig.TargetDistributionPolicy)
	{
	case ETargetDistributionPolicy::None:
	case ETargetDistributionPolicy::HeadshotHeightOnly:
	case ETargetDistributionPolicy::EdgeOnly:
	case ETargetDistributionPolicy::FullRange:
		{
			bLocationsAreCorners = true;
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
		break;
	case ETargetDistributionPolicy::Grid:
		bLocationsAreCorners = false;
		SpawnMemoryIncY = BSConfig.GridConfig.GridSpacing.X + BSConfig.TargetConfig.MaxTargetScale * SphereTargetDiameter;
		SpawnMemoryIncZ = BSConfig.GridConfig.GridSpacing.Y + BSConfig.TargetConfig.MaxTargetScale * SphereTargetDiameter;
		break;
	}
	MinOverlapRadius = (SpawnMemoryIncY + SpawnMemoryIncZ) / 2.f;
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
		if (InLocation.Y >= SpawnPoint.CornerPoint.Y && InLocation.Y < SpawnPoint.CornerPoint.Y + SpawnPoint.IncrementY &&
			(InLocation.Z >= SpawnPoint.CornerPoint.Z && InLocation.Z < SpawnPoint.CornerPoint.Z + SpawnPoint.IncrementZ))
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
		if (SpawnPoint.GetGuid() == InGuid)
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

int32 USpawnPointManager::FindIndexFromLocation(const FVector& InLocation) const
{
	if (const FSpawnPoint* Found = SpawnPoints.FindByPredicate([&InLocation](const FSpawnPoint& SpawnPoint)
	{
		if (InLocation.Y >= SpawnPoint.CornerPoint.Y && InLocation.Y < SpawnPoint.CornerPoint.Y + SpawnPoint.IncrementY &&
			(InLocation.Z >= SpawnPoint.CornerPoint.Z && InLocation.Z < SpawnPoint.CornerPoint.Z + SpawnPoint.IncrementZ))
		{
			return true;
		}
		return false;
	}))
	{
		return Found->Index;
	}
	return INDEX_NONE;
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
	Point.SetOverlappingPoints(BSConfig.TargetConfig.bMoveTargetsForward, MinOverlapRadius, Scale, Origin, StaticExtrema);
}

TArray<FVector> USpawnPointManager::RemoveOverlappingPointsFromSpawnLocations(TArray<FVector>& SpawnLocations, const FVector& Scale, const FExtrema& Extrema) const
{
	TArray<FVector> OverlappingPoints;
	/* Resizing Overlapping Points if necessary */
	for (FSpawnPoint& Point : GetActivatedOrRecentSpawnPoints())
	{
		if (Scale.Length() > Point.GetScale().Length())
		{
			SetOverlappingPoints(Point, Scale);
		}
		for (const FVector& Vector : Point.GetOverlappingPoints())
		{
			OverlappingPoints.AddUnique(Vector);
		}
	}
	SpawnLocations = SpawnLocations.FilterByPredicate([&] (const FVector& Location)
	{
		if (OverlappingPoints.Contains(Location))
		{
			return false;
		}
		return true;
	});
	return OverlappingPoints;
}

void USpawnPointManager::RemoveEdgePoints(TArray<FVector>& In, const FExtrema& Extrema) const
{
	if (BSConfig.TargetConfig.TargetDistributionPolicy == ETargetDistributionPolicy::EdgeOnly)
	{
		In = In.FilterByPredicate([&](const FVector& Vector)
		{
			if (Vector == Origin)
			{
				return true;
			}
			const FVector Right = Vector + FVector(0, SpawnMemoryIncY, 0);
			const FVector Top = Vector + FVector(0, 0, SpawnMemoryIncZ);
			if (Vector.Y != Extrema.Min.Y && Right.Y < Extrema.Max.Y && !In.Contains(Right))
			{
				return false;
			}
			if (Vector.Z != Extrema.Min.Z && Top.Z < Extrema.Max.Z && !In.Contains(Top))
			{
				return false;
			}
			return true;
		});
	}
	else
	{
		In = In.FilterByPredicate([&](const FVector& Vector)
		{
			const FVector Right = Vector + FVector(0, SpawnMemoryIncY, 0);
			const FVector Top = Vector + FVector(0, 0, SpawnMemoryIncZ);
			if (Right.Y < Extrema.Max.Y && !In.Contains(Right))
			{
				return false;
			}
			if (Top.Z < Extrema.Max.Z && !In.Contains(Top))
			{
				return false;
			}
			return true;
		});
	}
}

// flags

void USpawnPointManager::FlagSpawnPointAsRecent(const FGuid SpawnPointGuid)
{
	if (FSpawnPoint* Point = FindSpawnPointFromGuid(SpawnPointGuid))
	{
		if (!Point->IsRecent())
		{
			Point->SetIsRecent(true);
		}
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
		if (!Point->IsActivated())
		{
			Point->SetIsActivated(true);
		}
		SetOverlappingPoints(*Point, Point->GetScale());
	}
}

void USpawnPointManager::RemoveRecentFlagFromSpawnPoint(const FGuid SpawnPointGuid)
{
	if (FSpawnPoint* Point = FindSpawnPointFromGuid(SpawnPointGuid))
	{
		if (Point->IsRecent())
		{
			Point->SetIsRecent(false);
		}
	}
}

void USpawnPointManager::RemoveActivatedFlagFromSpawnPoint(const FTargetDamageEvent& TargetDamageEvent)
{
	if (FSpawnPoint* Point = FindSpawnPointFromGuid(TargetDamageEvent.Guid))
	{
		if (Point->IsActivated())
		{
			Point->SetIsActivated(false);
		}
		Point->IncrementTotalSpawns();
		if (TargetDamageEvent.TimeAlive != INDEX_NONE)
		{
			Point->IncrementTotalHits();
		}
	}
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

void USpawnPointManager::PrintDebug_SpawnPoint(const FSpawnPoint& SpawnPoint) const
{
	UE_LOG(LogTemp, Display, TEXT("SpawnPoint:"));
	UE_LOG(LogTemp, Display, TEXT("Index %d IndexType %s"), SpawnPoint.Index, *UEnum::GetDisplayValueAsText(SpawnPoint.GetIndexType()).ToString());
	UE_LOG(LogTemp, Display, TEXT("CornerPoint: %s CenterPoint: %s ChosenPoint: %s"), *SpawnPoint.CornerPoint.ToString(), *SpawnPoint.CenterPoint.ToString(), *SpawnPoint.ChosenPoint.ToString());
	FString String;
	for (const int32 Border : SpawnPoint.GetBorderingIndices())
	{
		String.Append(" " + FString::FromInt(Border));
	}
	UE_LOG(LogTemp, Display, TEXT("BorderingIndices %s"), *String);
	UE_LOG(LogTemp, Display, TEXT("IsActivated %hhd IsRecent %hhd"), SpawnPoint.IsActivated(), SpawnPoint.IsRecent());
	UE_LOG(LogTemp, Display, TEXT("TotalSpawns %d TotalHits %d"), SpawnPoint.GetTotalSpawns(), SpawnPoint.GetTotalHits());
}
