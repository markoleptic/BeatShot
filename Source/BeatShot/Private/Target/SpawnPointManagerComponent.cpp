// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Target/SpawnPointManagerComponent.h"
#include "GlobalConstants.h"
#include "Target/TargetManager.h"

USpawnPoint::USpawnPoint()
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
	bIsCurrentlyManaged = false;
	bIsRecent = false;
	TimeSetRecent = DBL_MAX;
	IndexType = EGridIndexType::None;
	TargetGuid = FGuid();
	BorderingIndices = TArray<int32>();
	OverlappingPoints = TArray<FVector>();
}

void USpawnPoint::Init(const int32 InIndex, const FVector& InPoint, const bool bIsCornerPoint, const float IncY, const float IncZ,  const int32 InWidth, const int32 InSize)
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

bool USpawnPoint::IsCornerIndex() const
{
	if (IndexType == EGridIndexType::Corner_TopLeft || IndexType == EGridIndexType::Corner_TopRight || IndexType == EGridIndexType::Corner_BottomRight || IndexType ==
	EGridIndexType::Corner_BottomLeft)
	{
		return true;
	}
	return false;
}

bool USpawnPoint::IsBorderIndex() const
{
	if (IndexType == EGridIndexType::Border_Top || IndexType == EGridIndexType::Border_Right || IndexType == EGridIndexType::Border_Bottom || IndexType == EGridIndexType::Border_Left)
	{
		return true;
	}
	return false;
}

TArray<int32> USpawnPoint::FindBorderingIndices(const EGridIndexType InGridIndexType, const int32 InIndex, const int32 InWidth)
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

FVector USpawnPoint::FindCornerPointFromCenterPoint(const FVector& InCenterPoint, const float InIncY, const float InIncZ)
{
	return FVector(InCenterPoint.X, roundf(InCenterPoint.Y - InIncY / 2.f), roundf(InCenterPoint.Z - InIncZ / 2.f));
}

FVector USpawnPoint::FindCenterPointFromCornerPoint(const FVector& InCornerPoint, const float InIncY, const float InIncZ)
{
	return FVector(InCornerPoint.X, roundf(InCornerPoint.Y + InIncY / 2.f), roundf(InCornerPoint.Z + InIncZ / 2.f));
}

FVector USpawnPoint::GenerateRandomSubPoint(const TArray<EBorderingDirection>& BlockedDirections) const
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

void USpawnPoint::SetChosenPointAsRandomSubPoint(const TArray<EBorderingDirection>& BlockedDirections)
{
	ChosenPoint = GenerateRandomSubPoint(BlockedDirections);
}

void USpawnPoint::IncrementTotalSpawns()
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

void USpawnPoint::IncrementTotalHits()
{
	TotalHits++;
}

EGridIndexType USpawnPoint::FindIndexType(const int32 InIndex, const int32 InSize, const int32 InWidth)
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

void USpawnPoint::SetIsRecent(const bool bSetIsRecent)
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

void USpawnPoint::SetOverlappingPoints(const TArray<FVector>& InOverlappingPoints)
{
	OverlappingPoints = InOverlappingPoints;
}

TArray<FVector> USpawnPoint::GenerateOverlappingPoints(const float InMinTargetDistance, const float InMinOverlapRadius, const FVector& InScale,
		const FVector& InOrigin) const
{
	TArray<FVector> OutPoints;
	float Radius = (InScale.X * SphereTargetRadius * 2.f) + InMinTargetDistance / 2.f;
	Radius = FMath::Max(Radius, InMinOverlapRadius);
	const FSphere Sphere = FSphere(CenterPoint, Radius);

	const float MaxSnappedRadiusY = FMath::GridSnap(Radius, IncrementY) + IncrementY;
	const float MaxSnappedRadiusZ = FMath::GridSnap(Radius, IncrementZ) + IncrementZ;
	const float MinY = CornerPoint.Y - MaxSnappedRadiusY;
	const float MaxY = CornerPoint.Y + MaxSnappedRadiusY + IncrementY;
	const float MinZ = CornerPoint.Z - MaxSnappedRadiusZ;
	const float MaxZ = CornerPoint.Z + MaxSnappedRadiusZ + IncrementZ;
	
	int Count = 0;
	for (float Z = MinZ; Z < MaxZ; Z += IncrementZ)
	{
		for (float Y = MinY; Y < MaxY; Y += IncrementY)
		{
			Count++;
			if (FVector Loc = FVector(InOrigin.X, Y, Z); Sphere.IsInside(Loc))
			{
				OutPoints.AddUnique(Loc);
			}
		}
	}
	//UE_LOG(LogTargetManager, Display, TEXT("GetOverlappingPoints Count %d"), Count);
	//DrawDebugSphere(GetWorld(), CenterPoint, Scale * SphereTargetRadius * 2 + (BSConfig.MinDistanceBetweenTargets / 2.f), 32, FColor::Magenta, false, 0.5f);
	//UE_LOG(LogTargetManager, Display, TEXT("BlockedPoints: %d"), BlockedPoints.Num());
	return OutPoints;
}

TArray<EBorderingDirection> USpawnPoint::GetBorderingDirections(const TArray<FVector>& ValidLocations, const FExtrema& InExtrema) const
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

// ------------------------------- //
// - USpawnPointManagerComponent - //
// ------------------------------- //

USpawnPointManagerComponent::USpawnPointManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USpawnPointManagerComponent::InitSpawnPointManager(const FBSConfig& InBSConfig, const FVector& InOrigin, const FVector& InStaticExtents)
{
	BSConfig = InBSConfig;
	Origin = InOrigin;
	StaticExtents = InStaticExtents;
}

TArray<FVector> USpawnPointManagerComponent::InitializeSpawnPoints(const FExtrema& InStaticExtrema)
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
			USpawnPoint* SpawnPoint = NewObject<USpawnPoint>();
			SpawnPoint->Init(Index,
					Loc,
					bLocationsAreCorners,
					SpawnMemoryIncY,
					SpawnMemoryIncZ,
					BSConfig.GridConfig.NumHorizontalGridTargets,
					BSConfig.GridConfig.NumVerticalGridTargets * BSConfig.GridConfig.NumHorizontalGridTargets);
			SpawnPoint->ChosenPoint = Loc;
			SpawnPoints.Add(SpawnPoint);
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

void USpawnPointManagerComponent::SetAppropriateSpawnMemoryValues()
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
							SpawnMemoryIncY = Scale;
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
							SpawnMemoryIncZ = Scale;
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
				SpawnMemoryIncY = 50.f;
				SpawnMemoryIncZ = 50.f;
			}
		}
		break;
	case ETargetDistributionPolicy::Grid:
		bLocationsAreCorners = false;
		SpawnMemoryIncY = BSConfig.GridConfig.GridSpacing.X + BSConfig.TargetConfig.MaxTargetScale * SphereTargetDiameter;
		SpawnMemoryIncZ = BSConfig.GridConfig.GridSpacing.Y + BSConfig.TargetConfig.MaxTargetScale * SphereTargetDiameter;
		break;
	}
	MinOverlapRadius = FMath::Max(SpawnMemoryIncY, SpawnMemoryIncZ) / 2.f;
}

// SpawnPoint finders/getters

USpawnPoint* USpawnPointManagerComponent::FindSpawnPointFromIndex(const int32 InIndex)
{
	USpawnPoint** Point = SpawnPoints.FindByPredicate([&InIndex] (const USpawnPoint* SpawnPoint)
	{
		if (SpawnPoint->Index == InIndex)
		{
			return true;
		}
		return false;
	});
	if (Point)
	{
		return *Point;
	}
	return nullptr;
}

USpawnPoint* USpawnPointManagerComponent::FindSpawnPointFromLocation(const FVector& InLocation)
{
	USpawnPoint** Point = SpawnPoints.FindByPredicate([&InLocation](const USpawnPoint* SpawnPoint)
	{
		if (InLocation.Y >= SpawnPoint->CornerPoint.Y && InLocation.Y < SpawnPoint->CornerPoint.Y + SpawnPoint->IncrementY &&
			(InLocation.Z >= SpawnPoint->CornerPoint.Z && InLocation.Z < SpawnPoint->CornerPoint.Z + SpawnPoint->IncrementZ))
		{
			return true;
		}
		return false;
	});
	if (Point)
	{
		return *Point;
	}
	return nullptr;
}

USpawnPoint* USpawnPointManagerComponent::FindSpawnPointFromGuid(const FGuid& InGuid)
{
	USpawnPoint** Point = SpawnPoints.FindByPredicate([&InGuid](const USpawnPoint* SpawnPoint)
	{
		if (SpawnPoint->GetGuid() == InGuid)
		{
			return true;
		}
		return false;
	});
	if (Point)
	{
		return *Point;
	}
	return nullptr;
}

USpawnPoint* USpawnPointManagerComponent::FindOldestRecentSpawnPoint() const
{
	TArray<USpawnPoint*> RecentSpawnPoints = GetRecentSpawnPoints();
	
	if (RecentSpawnPoints.IsEmpty())
	{
		return nullptr;
	}
	
	USpawnPoint* MostRecent = RecentSpawnPoints.Top();
	for (USpawnPoint* SpawnPoint : RecentSpawnPoints)
	{
		if (SpawnPoint->GetTimeSetRecent() < MostRecent->GetTimeSetRecent())
		{
			MostRecent = SpawnPoint;
		}
	}
	return MostRecent;
}

USpawnPoint* USpawnPointManagerComponent::FindOldestDeactivatedManagedPoint() const
{
	if (TArray<USpawnPoint*> Points = GetDeactivatedManagedPoints(); !Points.IsEmpty())
	{
		return Points.Top();
	}
	return nullptr;
}

int32 USpawnPointManagerComponent::FindSpawnPointIndexFromLocation(const FVector& InLocation) const
{
	if (const USpawnPoint* Found = *SpawnPoints.FindByPredicate([&InLocation](const USpawnPoint* SpawnPoint)
	{
		if (InLocation.Y >= SpawnPoint->CornerPoint.Y && InLocation.Y < SpawnPoint->CornerPoint.Y + SpawnPoint->IncrementY &&
			(InLocation.Z >= SpawnPoint->CornerPoint.Z && InLocation.Z < SpawnPoint->CornerPoint.Z + SpawnPoint->IncrementZ))
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

bool USpawnPointManagerComponent::IsSpawnPointValid(const USpawnPoint* InSpawnPoint) const
{
	if (SpawnPoints.Contains(InSpawnPoint))
	{
		return true;
	}
	return false;
}

// SpawnPoint array getters

TArray<USpawnPoint*> USpawnPointManagerComponent::GetManagedPoints() const
{
	return SpawnPoints.FilterByPredicate([] (const USpawnPoint* SpawnPoint)
	{
		if (SpawnPoint->IsCurrentlyManaged())
		{
			return true;
		}
		return false;
	});
}

TArray<USpawnPoint*> USpawnPointManagerComponent::GetDeactivatedManagedPoints() const
{
	TArray<USpawnPoint*> Points = SpawnPoints.FilterByPredicate([] (const USpawnPoint* SpawnPoint)
	{
		if (SpawnPoint->IsCurrentlyManaged() && !SpawnPoint->IsActivated())
		{
			return true;
		}
		return false;
	});
	if (!Points.IsEmpty())
	{
		Points.Sort([] (const USpawnPoint& SpawnPoint1, const USpawnPoint& SpawnPoint2)
		{
			return SpawnPoint1.GetTimeSetRecent() < SpawnPoint2.GetTimeSetRecent();
		});
	}
	return Points;
}

TArray<USpawnPoint*> USpawnPointManagerComponent::GetRecentSpawnPoints() const
{
	return SpawnPoints.FilterByPredicate([] (const USpawnPoint* SpawnPoint)
	{
		if (SpawnPoint->IsRecent())
		{
			return true;
		}
		return false;
	});
}

TArray<USpawnPoint*> USpawnPointManagerComponent::GetActivatedSpawnPoints() const
{
	return SpawnPoints.FilterByPredicate([] (const USpawnPoint* SpawnPoint)
	{
		if (SpawnPoint->IsActivated())
		{
			return true;
		}
		return false;
	});
}

TArray<USpawnPoint*> USpawnPointManagerComponent::GetActivatedOrRecentSpawnPoints() const
{
	return SpawnPoints.FilterByPredicate([] (const USpawnPoint* SpawnPoint)
	{
		if (SpawnPoint->IsActivated() || SpawnPoint->IsRecent())
		{
			return true;
		}
		return false;
	});
}

void USpawnPointManagerComponent::RefreshRecentTargetFlags()
{
	if (const int32 NumToRemove = GetRecentSpawnPoints().Num() - BSConfig.TargetConfig.MaxNumRecentTargets; NumToRemove > 0)
	{
		for (int32 CurrentRemoveNum = 0; CurrentRemoveNum < NumToRemove; CurrentRemoveNum++)
		{
			if (const USpawnPoint* Found = FindOldestRecentSpawnPoint())
			{
				RemoveRecentFlagFromSpawnPoint(Found->GetGuid());
			}
		}
	}
}

// SpawnPoint overlaps

void USpawnPointManagerComponent::RemoveOverlappingPointsFromSpawnLocations(TArray<FVector>& SpawnLocations, const FVector& Scale, const bool bShowDebug) const
{
	TArray<FVector> OverlappingPoints;
	/* Resizing Overlapping Points if necessary */
	for (const USpawnPoint* Point : GetActivatedOrRecentSpawnPoints())
	{
		if (Scale.Length() > Point->GetScale().Length())
		{
			TArray<FVector> ScaledOverlappingPoints = Point->GenerateOverlappingPoints(BSConfig.TargetConfig.MinDistanceBetweenTargets, MinOverlapRadius, Point->GetScale(), Origin);
			for (const FVector& Vector : ScaledOverlappingPoints)
			{
				OverlappingPoints.AddUnique(Vector);
			}
		}
		else
		{
			for (const FVector& Vector : Point->GetOverlappingPoints())
			{
				OverlappingPoints.AddUnique(Vector);
			}
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
	if (bShowDebug)
	{
		DrawDebug_Boxes(OverlappingPoints, FColor::Red, 4);
	}
}

void USpawnPointManagerComponent::RemoveEdgePoints(TArray<FVector>& In, const FExtrema& Extrema, const bool bShowDebug) const
{
	TArray<FVector> Removed;
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
				if (bShowDebug)
				{
					Removed.Add(Vector);
				}
				return false;
			}
			if (Vector.Z != Extrema.Min.Z && Top.Z < Extrema.Max.Z && !In.Contains(Top))
			{
				if (bShowDebug)
				{
					Removed.Add(Vector);
				}
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
				if (bShowDebug)
				{
					Removed.Add(Vector);
				}
				return false;
			}
			if (Top.Z < Extrema.Max.Z && !In.Contains(Top))
			{
				if (bShowDebug)
				{
					Removed.Add(Vector);
				}
				return false;
			}
			return true;
		});
	}
	if (bShowDebug)
	{
		DrawDebug_Boxes(Removed, FColor::Purple, 4);
	}
}

// flags (activation and recent)

void USpawnPointManagerComponent::FlagSpawnPointAsRecent(const FGuid SpawnPointGuid)
{
	if (USpawnPoint* Point = FindSpawnPointFromGuid(SpawnPointGuid))
	{
		if (!Point->IsRecent())
		{
			Point->SetIsRecent(true);
		}
	}
}

void USpawnPointManagerComponent::FlagSpawnPointAsActivated(const FGuid SpawnPointGuid)
{
	if (USpawnPoint* Point = FindSpawnPointFromGuid(SpawnPointGuid))
	{
		if (Point->IsRecent())
		{
			RemoveRecentFlagFromSpawnPoint(SpawnPointGuid);
		}
		if (!Point->IsActivated())
		{
			Point->SetIsActivated(true);
			Point->SetOverlappingPoints(Point->GenerateOverlappingPoints(BSConfig.TargetConfig.MinDistanceBetweenTargets, MinOverlapRadius, Point->GetScale(), Origin));
		}
	}
}

void USpawnPointManagerComponent::HandleRecentTargetRemoval(const ERecentTargetMemoryPolicy& RecentTargetMemoryPolicy, const FTargetDamageEvent& TargetDamageEvent)
{
	RemoveActivatedFlagFromSpawnPoint(TargetDamageEvent);
	FlagSpawnPointAsRecent(TargetDamageEvent.Guid);
	
	FTimerHandle TimerHandle;
	
	/* Handle removing recent flag from SpawnPoint */
	switch (RecentTargetMemoryPolicy)
	{
	case ERecentTargetMemoryPolicy::None:
		break;
	case ERecentTargetMemoryPolicy::CustomTimeBased:
		RemoveFromRecentDelegate.BindUObject(this, &USpawnPointManagerComponent::RemoveRecentFlagFromSpawnPoint, TargetDamageEvent.Guid);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, RemoveFromRecentDelegate, BSConfig.TargetConfig.RecentTargetTimeLength, false);
		break;
	case ERecentTargetMemoryPolicy::NumTargetsBased:
		RefreshRecentTargetFlags();
		break;
	case ERecentTargetMemoryPolicy::UseTargetSpawnCD:
		RemoveFromRecentDelegate.BindUObject(this, &USpawnPointManagerComponent::RemoveRecentFlagFromSpawnPoint, TargetDamageEvent.Guid);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, RemoveFromRecentDelegate, BSConfig.TargetConfig.TargetSpawnCD, false);
		break;
	}
}

void USpawnPointManagerComponent::RemoveRecentFlagFromSpawnPoint(const FGuid SpawnPointGuid)
{
	if (USpawnPoint* Point = FindSpawnPointFromGuid(SpawnPointGuid))
	{
		if (Point->IsRecent())
		{
			Point->SetIsRecent(false);
		}
	}
}

void USpawnPointManagerComponent::RemoveActivatedFlagFromSpawnPoint(const FTargetDamageEvent& TargetDamageEvent)
{
	if (USpawnPoint* Point = FindSpawnPointFromGuid(TargetDamageEvent.Guid))
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

// Util or debug

int32 USpawnPointManagerComponent::GetOutArrayIndexFromSpawnCounterIndex(const int32 SpawnCounterIndex) const
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

void USpawnPointManagerComponent::DrawDebug_Boxes(const TArray<FVector>& InLocations, const FColor& InColor, const int32 InThickness) const
{
	for (const FVector& Vector : InLocations)
	{
		FVector Loc = FVector(Vector.X, Vector.Y + GetSpawnMemoryIncY() / 2.f, Vector.Z + GetSpawnMemoryIncZ() / 2.f);
		DrawDebugBox(GetWorld(), Loc, FVector(0, GetSpawnMemoryIncY() / 2.f, GetSpawnMemoryIncZ() / 2.f),
			InColor, false, BSConfig.TargetConfig.TargetSpawnCD, 0, InThickness);
	}
}

void USpawnPointManagerComponent::PrintDebug_SpawnPoint(const USpawnPoint* SpawnPoint) const
{
	UE_LOG(LogTemp, Display, TEXT("SpawnPoint:"));
	UE_LOG(LogTemp, Display, TEXT("Index %d IndexType %s"), SpawnPoint->Index, *UEnum::GetDisplayValueAsText(SpawnPoint->GetIndexType()).ToString());
	UE_LOG(LogTemp, Display, TEXT("CornerPoint: %s CenterPoint: %s ChosenPoint: %s"), *SpawnPoint->CornerPoint.ToString(), *SpawnPoint->CenterPoint.ToString(), *SpawnPoint->ChosenPoint.ToString());
	FString String;
	for (const int32 Border : SpawnPoint->GetBorderingIndices())
	{
		String.Append(" " + FString::FromInt(Border));
	}
	UE_LOG(LogTemp, Display, TEXT("BorderingIndices %s"), *String);
	UE_LOG(LogTemp, Display, TEXT("IsActivated %hhd IsRecent %hhd"), SpawnPoint->IsActivated(), SpawnPoint->IsRecent());
	UE_LOG(LogTemp, Display, TEXT("TotalSpawns %d TotalHits %d"), SpawnPoint->GetTotalSpawns(), SpawnPoint->GetTotalHits());
}
