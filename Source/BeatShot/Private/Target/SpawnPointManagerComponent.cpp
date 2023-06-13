// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Target/SpawnPointManagerComponent.h"
#include "GlobalConstants.h"
#include "Target/TargetManager.h"

USpawnPoint::USpawnPoint()
{
	Width = 0.f;
	Height = 0.f;
	BottomLeft = FVector(-1);
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

void USpawnPoint::Init(const int32 InIndex, const FVector& InBottomLeft, const float IncY, const float IncZ,  const int32 InNumHorizontalTargets, const int32 InSize)
{
	Width = IncY;
	Height = IncZ;
	
	BottomLeft = InBottomLeft;
	CenterPoint = BottomLeft + FVector(0, IncY / 2.f, IncZ / 2.f);
	BottomRight = BottomLeft + FVector(0, IncY,0);
	TopLeft = BottomLeft + FVector(0, 0, IncZ);
	TopRight = BottomLeft + FVector(0, IncY, IncZ);

	ChosenPoint = FVector(-1);
	Scale = FVector(1);
	
	TotalSpawns = INDEX_NONE;
	TotalHits = 0;
	Index = InIndex;
	
	bIsActivated = false;
	bIsRecent = false;
	TimeSetRecent = DBL_MAX;
	IndexType = FindIndexType(InIndex, InSize, InNumHorizontalTargets);
	TargetGuid = FGuid();
	BorderingIndices = FindBorderingIndices(FindIndexType(InIndex, InSize, InNumHorizontalTargets), InIndex, InNumHorizontalTargets);
	OverlappingPoints = TArray<FVector>();
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

FVector USpawnPoint::GenerateRandomSubPoint() const
{
	const float Y = roundf(FMath::FRandRange(BottomLeft.Y, BottomRight.Y - 1.f));
	const float Z = roundf(FMath::FRandRange(BottomLeft.Z, TopLeft.Z - 1.f));
	return FVector(BottomLeft.X, Y, Z);
}

void USpawnPoint::SetRandomChosenPoint()
{
	ChosenPoint = GenerateRandomSubPoint();
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

TArray<FVector> USpawnPoint::GenerateOverlappingPoints(const float InMinTargetDistance, const float InMinOverlapRadius, const FVector& InScale) const
{
	TArray<FVector> OutPoints;

	const float ScaledRadius = InScale.X * SphereTargetRadius;
	// multiply by two so that any point outside the sphere will not be overlapping
	float Radius = ScaledRadius * 2.f + (InMinTargetDistance / 2.f);
	Radius = FMath::Max(Radius, InMinOverlapRadius);
	
	const FSphere Sphere = FSphere(ChosenPoint, Radius);
	
	int32 IncrementsInYRadius = 1;
	int32 IncrementsInZRadius = 1;
	
	if (Radius > Width)
	{
		IncrementsInYRadius = ceil(Radius / Width);
	}
	if (Radius > Height)
	{
		IncrementsInZRadius = ceil(Radius / Height);
	}

	const float MinY = BottomLeft.Y - (IncrementsInYRadius + 1) * Width;
	const float MaxY = BottomRight.Y + (IncrementsInYRadius + 1) * Width;
	const float MinZ = BottomLeft.Z - (IncrementsInZRadius + 1) * Height;
	const float MaxZ = TopLeft.Z + (IncrementsInZRadius + 1) * Height;
	
	int Count = 0;
	for (float Z = MinZ; Z < MaxZ; Z += Height)
	{
		for (float Y = MinY; Y < MaxY; Y += Width)
		{
			Count++;
			if (FVector Loc = FVector(ChosenPoint.X, Y, Z); Sphere.IsInside(Loc))
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
			const int32 HalfWidth = StaticExtents.Y;
			const int32 HalfHeight = StaticExtents.Z;
			bool bWidthScaleSelected = false;
			bool bHeightScaleSelected = false;
			for (const int32 Scale : PreferredScales)
			{
				if (!bWidthScaleSelected)
				{
					if ((HalfWidth % Scale == 0) && (HalfWidth / Scale % 5 == 0))
					{
						SpawnMemoryScaleY = 1.f / static_cast<float>(Scale);
						SpawnMemoryIncY = Scale;
						bWidthScaleSelected = true;
					}
				}
				if (!bHeightScaleSelected)
				{
					if ((HalfHeight % Scale == 0) && (HalfHeight / Scale % 5 == 0))
					{
						SpawnMemoryScaleZ = 1.f / static_cast<float>(Scale);
						SpawnMemoryIncZ = Scale;
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
				UE_LOG(LogTemp, Error, TEXT("Couldn't Find Height/Width"));
			}
		}
		break;
	case ETargetDistributionPolicy::Grid:
		SpawnMemoryIncY = BSConfig.GridConfig.GridSpacing.X + BSConfig.TargetConfig.MaxTargetScale * SphereTargetDiameter;
		SpawnMemoryIncZ = BSConfig.GridConfig.GridSpacing.Y + BSConfig.TargetConfig.MaxTargetScale * SphereTargetDiameter;
		break;
	}
	MinOverlapRadius = FMath::Max(SpawnMemoryIncY, SpawnMemoryIncZ) / 2.f;
}

// SpawnPoint finders/getters

USpawnPoint* USpawnPointManagerComponent::FindSpawnPointFromIndex(const int32 InIndex) const
{
	const TArray<USpawnPoint*>::ElementType* Point = SpawnPoints.FindByPredicate([&InIndex] (const USpawnPoint* SpawnPoint)
	{
		if (SpawnPoint->Index == InIndex)
		{
			return true;
		}
		return false;
	});
	return Point ? *Point : nullptr;
}

USpawnPoint* USpawnPointManagerComponent::FindSpawnPointFromLocation(const FVector& InLocation) const
{
	const TArray<USpawnPoint*>::ElementType* Point = SpawnPoints.FindByPredicate([&InLocation](const USpawnPoint* SpawnPoint)
	{
		if ((InLocation.Y >= SpawnPoint->BottomLeft.Y) && (InLocation.Z >= SpawnPoint->BottomLeft.Z) && (InLocation.Y < SpawnPoint->TopRight.Y - 0.01) && (InLocation.Z < SpawnPoint->TopRight.Z -
			0.01))
		{
			return true;
		}
		return false;
	});
	return Point ? *Point : nullptr;
}

USpawnPoint* USpawnPointManagerComponent::FindSpawnPointFromGuid(const FGuid& InGuid) const
{
	const TArray<USpawnPoint*>::ElementType* Point = SpawnPoints.FindByPredicate([&InGuid](const USpawnPoint* SpawnPoint)
	{
		if (SpawnPoint->GetGuid() == InGuid)
		{
			return true;
		}
		return false;
	});
	return Point ? *Point : nullptr;
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
	TArray<USpawnPoint*> Points = GetDeactivatedManagedPoints();
	return Points.IsEmpty() ? nullptr : Points.Top();
}

int32 USpawnPointManagerComponent::FindSpawnPointIndexFromLocation(const FVector& InLocation) const
{
	const TArray<USpawnPoint*>::ElementType* Point = SpawnPoints.FindByPredicate([&InLocation](const USpawnPoint* SpawnPoint)
	{
		if ((InLocation.Y >= SpawnPoint->BottomLeft.Y) &&
			(InLocation.Z >= SpawnPoint->BottomLeft.Z) &&
			(InLocation.Y < SpawnPoint->TopRight.Y - 0.01) &&
			(InLocation.Z < SpawnPoint->TopRight.Z - 0.01))
		{
			return true;
		}
		return false;
	});
	return Point ? (*Point)->Index : INDEX_NONE;
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
			TArray<FVector> ScaledOverlappingPoints = Point->GenerateOverlappingPoints(BSConfig.TargetConfig.MinDistanceBetweenTargets, MinOverlapRadius, Point->GetScale());
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
			Point->SetOverlappingPoints(Point->GenerateOverlappingPoints(BSConfig.TargetConfig.MinDistanceBetweenTargets, MinOverlapRadius, Point->GetScale()));
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
	UE_LOG(LogTemp, Display, TEXT("BottomLeft: %s CenterPoint: %s ChosenPoint: %s"), *SpawnPoint->BottomLeft.ToString(), *SpawnPoint->CenterPoint.ToString(), *SpawnPoint->ChosenPoint.ToString());
	FString String;
	for (const int32 Border : SpawnPoint->GetBorderingIndices())
	{
		String.Append(" " + FString::FromInt(Border));
	}
	UE_LOG(LogTemp, Display, TEXT("BorderingIndices %s"), *String);
	UE_LOG(LogTemp, Display, TEXT("IsActivated %hhd IsRecent %hhd"), SpawnPoint->IsActivated(), SpawnPoint->IsRecent());
	UE_LOG(LogTemp, Display, TEXT("TotalSpawns %d TotalHits %d"), SpawnPoint->GetTotalSpawns(), SpawnPoint->GetTotalHits());
}
