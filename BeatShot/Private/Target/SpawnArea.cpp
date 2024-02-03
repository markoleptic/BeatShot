// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Target/SpawnArea.h"
#include "GlobalConstants.h"

using namespace Constants;

float USpawnArea::Width = 0.f;
float USpawnArea::Height = 0.f;
int32 USpawnArea::TotalNumHorizontalSpawnAreas = 0;
int32 USpawnArea::TotalNumVerticalSpawnAreas = 0;
int32 USpawnArea::Size = 0;

USpawnArea::USpawnArea()
{
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
	bIsManaged = false;
	bIsRecent = false;
	TimeSetRecent = DBL_MAX;
	GridIndexType = EGridIndexType::None;
	OccupiedVertices = TSet<FVector>();
	AdjacentIndexMap = TMap<EBorderingDirection, int32>();
	Guid = FGuid();
}

void USpawnArea::Init(const int32 InIndex, const FVector& InBottomLeftVertex)
{
	Vertex_BottomLeft = InBottomLeftVertex;
	CenterPoint = Vertex_BottomLeft + FVector(0, Width * 0.5f, Height * 0.5f);
	Vertex_BottomRight = Vertex_BottomLeft + FVector(0, Width, 0);
	Vertex_TopLeft = Vertex_BottomLeft + FVector(0, 0, Height);
	Vertex_TopRight = Vertex_BottomLeft + FVector(0, Width, Height);

	ChosenPoint = Vertex_BottomLeft;
	TargetScale = FVector(1);

	TotalSpawns = INDEX_NONE;
	TotalHits = 0;
	Index = InIndex;

	bIsActivated = false;
	bIsRecent = false;
	TimeSetRecent = DBL_MAX;
	GridIndexType = FindIndexType(InIndex, Size, TotalNumHorizontalSpawnAreas);
	SetAdjacentIndices(GridIndexType, Index, TotalNumHorizontalSpawnAreas);
	OccupiedVertices = TSet<FVector>();
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

	TArray<int32> Temp;
	AdjacentIndexMap.GenerateValueArray(Temp);
	AdjacentIndices = TSet(MoveTemp(Temp));
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

void USpawnArea::SetIsManaged(const bool bManaged)
{
	bIsManaged = bManaged;
}

void USpawnArea::SetIsActivated(const bool bActivated, const bool bAllow)
{
	bIsActivated = bActivated;
	bAllowActivationWhileActivated = bAllow;
}

float USpawnArea::GetMinOverlapRadius()
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
	}
}

void USpawnArea::SetOccupiedVertices(const TSet<FVector>& InVertices)
{
	OccupiedVertices = InVertices;
}

TSet<FVector> USpawnArea::MakeOccupiedVertices(const float InMinDist, const FVector& InScale) const
{
	TSet<FVector> OutInvalid;

	const float ScaledRadius = InScale.X * SphereTargetRadius;
	// Multiply by two so that any point outside the sphere will not be overlapping
	float Radius = ScaledRadius * 2.f + (InMinDist * 0.5f);

	// Radius can never be less than MinOverlapRadius
	Radius = FMath::Max(Radius, GetMinOverlapRadius());

	// Add max of height/width to account for random spawning within a SpawnArea
	Radius += FMath::Max(Width, Height);

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
	
	for (float Z = MinZ; Z <= MaxZ; Z += Height)
	{
		for (float Y = MinY; Y <= MaxY; Y += Width)
		{
			if (FVector Loc = FVector(ChosenPoint.X, Y, Z); Sphere.IsInside(Loc))
			{
				OutInvalid.Add(Loc);
			}
		}
	}
	return OutInvalid;
}

FVector USpawnArea::GetMiddleVertex() const
{
	return (Vertex_BottomLeft + Vertex_TopRight) / 2.f;
}

bool USpawnArea::IsBorderingIndex(const int32 InIndex) const
{
	return AdjacentIndices.Contains(InIndex);
}

TSet<int32> USpawnArea::GetAdjacentIndices(const TSet<EBorderingDirection>& Directions) const
{
	TSet<int32> Out;
	for (const EBorderingDirection Direction : Directions)
	{
		if (AdjacentIndexMap.Contains(Direction))
		{
			Out.Add(AdjacentIndexMap[Direction]);
		}
	}
	return Out;
}

TSet<FVector> USpawnArea::GetUnoccupiedVertices(const float InMinDist, const FVector& InScale) const
{
	TSet<FVector> OutValid;

	const float ScaledRadius = InScale.X * SphereTargetRadius;
	// Multiply by two so that any point outside the sphere will not be overlapping
	float Radius = ScaledRadius * 2.f + (InMinDist * 0.5f);
	
	// Radius can never be less than MinOverlapRadius
	Radius = FMath::Max(Radius, GetMinOverlapRadius());

	// Add max of height/width to account for random spawning within a SpawnArea
	Radius += FMath::Max(Width, Height);

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
	
	for (float Z = MinZ; Z <= MaxZ; Z += Height)
	{
		for (float Y = MinY; Y <= MaxY; Y += Width)
		{
			if (FVector Loc = FVector(ChosenPoint.X, Y, Z); !Sphere.IsInside(Loc))
			{
				OutValid.Add(Loc);
			}
		}
	}
	return OutValid;
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