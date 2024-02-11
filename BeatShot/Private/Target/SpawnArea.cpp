// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Target/SpawnArea.h"
#include "GlobalConstants.h"
#include "Target/SpawnAreaManagerComponent.h"

using namespace Constants;

float USpawnArea::Width = 0.f;
float USpawnArea::Height = 0.f;
int32 USpawnArea::TotalNumHorizontalSpawnAreas = 0;
int32 USpawnArea::TotalNumVerticalSpawnAreas = 0;
int32 USpawnArea::Size = 0;
float USpawnArea::MinDistanceBetweenTargets = 10.f;
FExtrema USpawnArea::TotalSpawnAreaExtrema = FExtrema();

USpawnArea::USpawnArea()
{
	Vertex_BottomLeft = FVector(-1.f);
	CenterPoint = FVector(-1.f);
	ChosenPoint = FVector(-1.f);
	TargetScale = FVector(1.f);
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
	TargetScale = FVector(1.f);

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

void USpawnArea::SetIsManaged(const bool bManaged)
{
	bIsManaged = bManaged;
	if (bIsManaged)
	{
		SetOccupiedVertices(MakeOccupiedVertices(TargetScale));
	}
	else
	{
		OccupiedVertices.Empty();
		#if !UE_BUILD_SHIPPING
		DebugOccupiedVertices.Empty();
		#endif
	}
}

void USpawnArea::SetIsActivated(const bool bActivated, const bool bAllow)
{
	bIsActivated = bActivated;
	bAllowActivationWhileActivated = bAllow;
}

void USpawnArea::SetTargetScale(const FVector& InScale)
{
	TargetScale = InScale;
}

void USpawnArea::SetChosenPoint(const FVector& InLocation)
{
	ChosenPoint = Vertex_BottomLeft + InLocation;
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

TSet<FVector> USpawnArea::MakeOccupiedVertices(const FVector& InScale) const
{
	TSet<FVector> OutInvalid;
	
	const float Radius = CalcTraceRadius(InScale);
	
	const FSphere Sphere = FSphere(Vertex_BottomLeft, Radius);

	const int32 IncY = floor(Radius / Width);
	const int32 IncZ = floor(Radius / Height);
	
	const float MinY = FMath::Max(TotalSpawnAreaExtrema.Min.Y, Vertex_BottomLeft.Y - IncY * Width);
	const float MaxY = FMath::Min(TotalSpawnAreaExtrema.Max.Y - Width, Vertex_BottomLeft.Y + IncY * Width);
	const float MinZ = FMath::Max(TotalSpawnAreaExtrema.Min.Z, Vertex_BottomLeft.Z - IncZ * Height);
	const float MaxZ = FMath::Min(TotalSpawnAreaExtrema.Max.Z - Height, Vertex_BottomLeft.Z + IncZ * Height);
	
	FVector Vertex(Vertex_BottomLeft.X, 0.f, 0.f);
	for (Vertex.Z = MinZ; Vertex.Z <= MaxZ; Vertex.Z += Height)
	{
		for (Vertex.Y = MinY; Vertex.Y <= MaxY; Vertex.Y += Width)
		{
			if (Sphere.IsInside(Vertex))
			{
				OutInvalid.Add(Vertex);
			}
		}
	}
	
	return OutInvalid;
}

TSet<FVector> USpawnArea::MakeUnoccupiedVertices(const FVector& InScale) const
{
	TSet<FVector> OutValid;
	
	const float Radius = CalcTraceRadius(InScale);
	
	const FSphere Sphere = FSphere(Vertex_BottomLeft, Radius);

	const int32 IncY = floor(Radius / Width);
	const int32 IncZ = floor(Radius / Height);
	
	const float MinY = FMath::Max(TotalSpawnAreaExtrema.Min.Y, Vertex_BottomLeft.Y - IncY * Width);
	const float MaxY = FMath::Min(TotalSpawnAreaExtrema.Max.Y - Width, Vertex_BottomLeft.Y + IncY * Width);
	const float MinZ = FMath::Max(TotalSpawnAreaExtrema.Min.Z, Vertex_BottomLeft.Z - IncZ * Height);
	const float MaxZ = FMath::Min(TotalSpawnAreaExtrema.Max.Z - Height, Vertex_BottomLeft.Z + IncZ * Height);
	
	FVector Vertex(Vertex_BottomLeft.X, 0.f, 0.f);
	for (Vertex.Z = MinZ; Vertex.Z <= MaxZ; Vertex.Z += Height)
	{
		for (Vertex.Y = MinY; Vertex.Y <= MaxY; Vertex.Y += Width)
		{
			if (!Sphere.IsInside(Vertex))
			{
				OutValid.Add(Vertex);
			}
		}
	}
	
	return OutValid;
}

void USpawnArea::SetTotalSpawnAreaExtrema(const FExtrema& InExtrema)
{
	TotalSpawnAreaExtrema = InExtrema;
}

FVector USpawnArea::GenerateRandomOffset()
{
	#if !UE_BUILD_SHIPPING
	if (GIsAutomationTesting)
	{
		const int32 RandomNum = FMath::RandRange(0, 3);
		if (RandomNum == 0)
		{
			return FVector(0.f, 0.f, 0.f);
		}
		if (RandomNum == 1)
		{
			return FVector(0.f, Width - 1.f, 0.f);
		}
		if (RandomNum == 2)
		{
			return FVector(0.f, 0.f, Height - 1.f);
		}
		if (RandomNum == 3)
		{
			return FVector(0.f, Width - 1.f, Height - 1.f);
		}
	}
	#endif
	
	const float Y = roundf(FMath::FRandRange(0.f, Width - 1.f));
	const float Z = roundf(FMath::FRandRange(0.f, Height - 1.f));
	return FVector(0.f, Y, Z);
}

float USpawnArea::CalcTraceRadius(const FVector& InScale)
{
	// Radius can never be less than half the max side dimension
	const float MinRadius = FMath::Max(Width, Height) * 0.5f;

	// Make Radius a multiple of the Spawn Area width or height
	const float SnappedRadius = ceil(InScale.X * SphereTargetRadius / MinRadius) * MinRadius;

	// Multiply by two so that any point outside the sphere will not be overlapping
	// Square root of two is used to reach the diagonals from the bottom left vertex
	return SnappedRadius * FMath::Sqrt(2.f) * 2.f;
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

TSet<FVector> USpawnArea::SetMakeDebugOccupiedVertices(const FVector& InScale)
{
	DebugOccupiedVertices = MakeOccupiedVertices(InScale);
	LastOccupiedVerticesTargetScale = InScale;
	return DebugOccupiedVertices;
}
