// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Target/SpawnAreaManagerComponent.h"
#include "GlobalConstants.h"
#include "Target/TargetManager.h"

USpawnArea::USpawnArea()
{
	Width = 0.f;
	Height = 0.f;
	Vertex_BottomLeft = FVector(-1);
	CenterPoint = FVector(-1);
	ChosenPoint = FVector(-1);
	TargetScale = FVector(1);
	TotalSpawns = INDEX_NONE;
	TotalHits = 0;
	Index = INDEX_NONE;
	bIsActivated = false;
	bIsCurrentlyManaged = false;
	bIsRecent = false;
	TimeSetRecent = DBL_MAX;
	IndexType = EGridIndexType::None;
	TargetGuid = FGuid();
	AdjacentIndices = TArray<int32>();
	OverlappingVertices = TArray<FVector>();
}

void USpawnArea::Init(const int32 InIndex, const FVector& InBottomLeft, const float IncY, const float IncZ,  const int32 InNumHorizontalTargets, const int32 InSize)
{
	Width = IncY;
	Height = IncZ;
	
	Vertex_BottomLeft = InBottomLeft;
	CenterPoint = Vertex_BottomLeft + FVector(0, IncY / 2.f, IncZ / 2.f);
	Vertex_BottomRight = Vertex_BottomLeft + FVector(0, IncY,0);
	Vertex_TopLeft = Vertex_BottomLeft + FVector(0, 0, IncZ);
	Vertex_TopRight = Vertex_BottomLeft + FVector(0, IncY, IncZ);

	ChosenPoint = FVector(-1);
	TargetScale = FVector(1);
	
	TotalSpawns = INDEX_NONE;
	TotalHits = 0;
	Index = InIndex;
	
	bIsActivated = false;
	bIsRecent = false;
	TimeSetRecent = DBL_MAX;
	IndexType = FindIndexType(InIndex, InSize, InNumHorizontalTargets);
	TargetGuid = FGuid();
	AdjacentIndices = FindAdjacentIndices(FindIndexType(InIndex, InSize, InNumHorizontalTargets), InIndex, InNumHorizontalTargets);
	OverlappingVertices = TArray<FVector>();
}

TArray<int32> USpawnArea::FindAdjacentIndices(const EGridIndexType InGridIndexType, const int32 InIndex, const int32 InWidth)
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

TArray<FVector> USpawnArea::GenerateOverlappingVertices(const float InMinTargetDistance, const float InMinOverlapRadius, const FVector& InScale, TArray<FVector>& DebugVertices, const bool bAddDebugVertices) const
{
	TArray<FVector> OutPoints;

	const float ScaledRadius = InScale.X * SphereTargetRadius;
	// multiply by two so that any point outside the sphere will not be overlapping
	float Radius = ScaledRadius * 2.f + (InMinTargetDistance / 2.f);
	Radius = FMath::Max(Radius, InMinOverlapRadius) + FMath::Max(Width, Height);
	
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

// ------------------------------ //
// - USpawnAreaManagerComponent - //
// ------------------------------ //

USpawnAreaManagerComponent::USpawnAreaManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	BSConfig = nullptr;
	bShowDebug_SpawnMemory = false;
	bShowDebug_OverlappingVertices = false;
	bShowDebug_OverlappingVertices_All = false;
	SpawnAreas = TArray<USpawnArea*>();
	AllBottomLeftVertices = TArray<FVector>();
}

void USpawnAreaManagerComponent::DestroyComponent(bool bPromoteChildren)
{
	BSConfig = nullptr;
	Super::DestroyComponent(bPromoteChildren);
}

void USpawnAreaManagerComponent::Init(FBSConfig* InBSConfig, const FVector& InOrigin, const FVector& InStaticExtents, const FExtrema& InStaticExtrema)
{
	SpawnAreas.Empty();
	AllBottomLeftVertices.Empty();
	
	BSConfig = InBSConfig;
	Origin = InOrigin;
	StaticExtents = InStaticExtents;
	StaticExtrema = InStaticExtrema;
	
	SetAppropriateSpawnMemoryValues();
	AllBottomLeftVertices = InitializeSpawnAreas();
}

TArray<FVector> USpawnAreaManagerComponent::InitializeSpawnAreas()
{
	int TempWidth = 0;
	int TempHeight = 0;
	int Index = 0;

	TArray<FVector> AllSpawnLocations;
	
	for (float Z = StaticExtrema.Min.Z; Z < StaticExtrema.Max.Z; Z += SpawnMemoryIncZ)
	{
		TempWidth = 0;
		for (float Y = StaticExtrema.Min.Y; Y < StaticExtrema.Max.Y; Y += SpawnMemoryIncY)
		{
			FVector Loc(Origin.X, Y, Z);
			AllSpawnLocations.Add(Loc);
			USpawnArea* SpawnArea = NewObject<USpawnArea>();
			SpawnArea->Init(Index,
					Loc,
					SpawnMemoryIncY,
					SpawnMemoryIncZ,
					GetBSConfig()->GridConfig.NumHorizontalGridTargets,
					GetBSConfig()->GridConfig.NumVerticalGridTargets * GetBSConfig()->GridConfig.NumHorizontalGridTargets);
			SpawnArea->ChosenPoint = Loc;
			SpawnAreas.Add(SpawnArea);
			Index++;
			TempWidth++;
		}
		TempHeight++;
	}
	Width = TempWidth;
	Height = TempHeight;
	UE_LOG(LogTemp, Display, TEXT("SpawnMemoryScaleY %f SpawnMemoryScaleZ %f"), SpawnMemoryScaleY, SpawnMemoryScaleZ);
	UE_LOG(LogTemp, Display, TEXT("SpawnMemoryIncY %d SpawnMemoryIncZ %d"), SpawnMemoryIncY, SpawnMemoryIncZ);
	UE_LOG(LogTemp, Display, TEXT("SpawnCounterSize: %d %llu"), SpawnAreas.Num(), SpawnAreas.GetAllocatedSize());
	return AllSpawnLocations;
}

void USpawnAreaManagerComponent::SetAppropriateSpawnMemoryValues()
{
	switch (GetBSConfig()->TargetConfig.TargetDistributionPolicy)
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
				UE_LOG(LogTemp, Warning, TEXT("Couldn't Find Height/Width for StaticExtents: Y:%f Z:%f"), StaticExtents.Y, StaticExtents.Z);
			}
		}
		break;
	case ETargetDistributionPolicy::Grid:
		SpawnMemoryIncY = GetBSConfig()->GridConfig.GridSpacing.X + GetBSConfig()->TargetConfig.MaxSpawnedTargetScale * SphereTargetDiameter;
		SpawnMemoryIncZ = GetBSConfig()->GridConfig.GridSpacing.Y + GetBSConfig()->TargetConfig.MaxSpawnedTargetScale * SphereTargetDiameter;
		break;
	}
	MinOverlapRadius = FMath::Max(SpawnMemoryIncY, SpawnMemoryIncZ) / 2.f;
}

// SpawnArea finders/getters

USpawnArea* USpawnAreaManagerComponent::FindSpawnAreaFromIndex(const int32 InIndex) const
{
	const TArray<USpawnArea*>::ElementType* Area = SpawnAreas.FindByPredicate([&InIndex] (const USpawnArea* SpawnArea)
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
		if ((InLocation.Y >= SpawnArea->Vertex_BottomLeft.Y) && (InLocation.Z >= SpawnArea->Vertex_BottomLeft.Z) && (InLocation.Y < SpawnArea->Vertex_TopRight.Y - 0.01) && (InLocation.Z < SpawnArea->Vertex_TopRight.Z -
			0.01))
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

int32 USpawnAreaManagerComponent::FindSpawnAreaIndexFromLocation(const FVector& InLocation) const
{
	const TArray<USpawnArea*>::ElementType* SpawnArea = SpawnAreas.FindByPredicate([&InLocation](const USpawnArea* SpawnArea)
	{
		if ((InLocation.Y >= SpawnArea->Vertex_BottomLeft.Y) &&
			(InLocation.Z >= SpawnArea->Vertex_BottomLeft.Z) &&
			(InLocation.Y < SpawnArea->Vertex_TopRight.Y - 0.01) &&
			(InLocation.Z < SpawnArea->Vertex_TopRight.Z - 0.01))
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

// SpawnArea array getters

TArray<USpawnArea*> USpawnAreaManagerComponent::GetManagedSpawnAreas() const
{
	return SpawnAreas.FilterByPredicate([] (const USpawnArea* SpawnArea)
	{
		if (SpawnArea->IsCurrentlyManaged())
		{
			return true;
		}
		return false;
	});
}

TArray<USpawnArea*> USpawnAreaManagerComponent::GetDeactivatedManagedSpawnAreas() const
{
	TArray<USpawnArea*> Areas = SpawnAreas.FilterByPredicate([] (const USpawnArea* SpawnArea)
	{
		if (SpawnArea->IsCurrentlyManaged() && !SpawnArea->IsActivated())
		{
			return true;
		}
		return false;
	});
	if (!Areas.IsEmpty())
	{
		Areas.Sort([] (const USpawnArea& SpawnArea1, const USpawnArea& SpawnArea2)
		{
			return SpawnArea1.GetTimeSetRecent() < SpawnArea2.GetTimeSetRecent();
		});
	}
	return Areas;
}

TArray<USpawnArea*> USpawnAreaManagerComponent::GetRecentSpawnAreas() const
{
	return SpawnAreas.FilterByPredicate([] (const USpawnArea* SpawnArea)
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
	return SpawnAreas.FilterByPredicate([] (const USpawnArea* SpawnArea)
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
	return SpawnAreas.FilterByPredicate([] (const USpawnArea* SpawnArea)
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
	return SpawnAreas.FilterByPredicate([] (const USpawnArea* SpawnArea)
{
	if (SpawnArea->IsCurrentlyManaged() || SpawnArea->IsActivated() || SpawnArea->IsRecent())
	{
		return true;
	}
	return false;
});
}

void USpawnAreaManagerComponent::RefreshRecentFlags()
{
	if (const int32 NumToRemove = GetRecentSpawnAreas().Num() - GetBSConfig()->TargetConfig.MaxNumRecentTargets; NumToRemove > 0)
	{
		for (int32 CurrentRemoveNum = 0; CurrentRemoveNum < NumToRemove; CurrentRemoveNum++)
		{
			if (const USpawnArea* Found = FindOldestRecentSpawnArea())
			{
				RemoveRecentFlagFromSpawnArea(Found->GetTargetGuid());
			}
		}
	}
}

// SpawnArea overlaps
void USpawnAreaManagerComponent::RemoveOverlappingSpawnLocations(TArray<FVector>& SpawnLocations, const FVector& Scale) const
{
	TArray<FVector> OverlappingVertices;
	TArray<FVector> DebugVertices;

	// Consider Managed Targets to be overlapping/invalid if runtime targets can be spawned without activation
	TArray<USpawnArea*> InvalidSpawnAreas;
	if (GetBSConfig()->TargetConfig.TargetSpawningPolicy == ETargetSpawningPolicy::RuntimeOnly && GetBSConfig()->TargetConfig.bAllowSpawnWithoutActivation)
	{
		InvalidSpawnAreas = GetManagedActivatedOrRecentSpawnAreas();
	}
	else
	{
		InvalidSpawnAreas = GetActivatedOrRecentSpawnAreas();
	}
	
	for (const USpawnArea* SpawnArea : InvalidSpawnAreas)
	{
		// Regenerate Overlapping vertices if necessary
		if (Scale.Length() > SpawnArea->GetTargetScale().Length())
		{
			TArray<FVector> ScaledOverlappingPoints = SpawnArea->GenerateOverlappingVertices(
				GetBSConfig()->TargetConfig.MinDistanceBetweenTargets, MinOverlapRadius, SpawnArea->GetTargetScale(), DebugVertices, bShowDebug_OverlappingVertices_All);
			for (const FVector& Vector : ScaledOverlappingPoints)
			{
				OverlappingVertices.AddUnique(Vector);
				if (bShowDebug_OverlappingVertices_All)
				{
					DrawDebugPoint(GetWorld(), Vector, 10.f, FColor::Red, false, 0.5f);
				}
			}
			if (bShowDebug_OverlappingVertices_All)
			{
				const float ScaledRadius = Scale.X * SphereTargetRadius;
				float Radius = ScaledRadius * 2.f + (GetBSConfig()->TargetConfig.MinDistanceBetweenTargets / 2.f);
				Radius = FMath::Max(Radius, MinOverlapRadius) + FMath::Max(Width, Height);
				DrawDebugSphere(GetWorld(), SpawnArea->ChosenPoint, Radius, 16, FColor::Magenta, false, 0.5f);
				for (FVector Vertex : DebugVertices)
				{
					DrawDebugPoint(GetWorld(), Vertex, 10.f, FColor::Green, false, 0.5f);
				}
			}
		}
		else
		{
			for (const FVector& Vector : SpawnArea->GetOverlappingVertices())
			{
				OverlappingVertices.AddUnique(Vector);
				if (bShowDebug_OverlappingVertices_All)
				{
					DrawDebugPoint(GetWorld(), Vector, 10.f, FColor::Red, false, 0.5f);
				}
			}
			if (bShowDebug_OverlappingVertices_All)
			{
				const float ScaledRadius = Scale.X * SphereTargetRadius;
				float Radius = ScaledRadius * 2.f + (GetBSConfig()->TargetConfig.MinDistanceBetweenTargets / 2.f);
				Radius = FMath::Max(Radius, MinOverlapRadius) + FMath::Max(Width, Height);
				DrawDebugSphere(GetWorld(), SpawnArea->ChosenPoint, Radius, 16, FColor::Magenta, false, 0.5f);
			}
		}
	}
	SpawnLocations = SpawnLocations.FilterByPredicate([&OverlappingVertices] (const FVector& Location)
	{
		return OverlappingVertices.Contains(Location) ? false : true;
	});
	if (bShowDebug_SpawnMemory)
	{
		DrawDebug_Boxes(OverlappingVertices, FColor::Red, 4, 3);
	}
}

// flags (activation and recent)

void USpawnAreaManagerComponent::FlagSpawnAreaAsManaged(const FGuid TargetGuid) const
{
	if (USpawnArea* SpawnArea = FindSpawnAreaFromGuid(TargetGuid))
	{
		if (!SpawnArea->IsCurrentlyManaged())
		{
			SpawnArea->SetIsCurrentlyManaged(true);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Tried to add an managed flag to a SpawnArea that was already flagged activated"));
		}
		if (SpawnArea->OverlappingVertices.IsEmpty())
		{
			TArray<FVector> DebugVertices;
			TArray<FVector> OverlappingPoints = SpawnArea->GenerateOverlappingVertices(
				GetBSConfig()->TargetConfig.MinDistanceBetweenTargets, MinOverlapRadius, SpawnArea->GetTargetScale(), DebugVertices, bShowDebug_OverlappingVertices);
			SpawnArea->SetOverlappingVertices(OverlappingPoints);
			if (bShowDebug_OverlappingVertices)
			{
				const float ScaledRadius = SpawnArea->GetTargetScale().X * SphereTargetRadius;
				float Radius = ScaledRadius * 2.f + (GetBSConfig()->TargetConfig.MinDistanceBetweenTargets / 2.f);
				Radius = FMath::Max(Radius, MinOverlapRadius) + FMath::Max(Width, Height);
				DrawDebugSphere(GetWorld(), SpawnArea->ChosenPoint, Radius, 16, FColor::Magenta, false, 0.5f);
				
				for (const FVector& Vector : OverlappingPoints)
				{
					DrawDebugPoint(GetWorld(), Vector, 10.f, FColor::Red, false, 0.5f);
				}
				for (FVector Vertex : DebugVertices)
				{
					DrawDebugPoint(GetWorld(), Vertex, 10.f, FColor::Green, false, 0.5f);
				}
			}
		}
	}
}

void USpawnAreaManagerComponent::FlagSpawnAreaAsActivated(const FGuid TargetGuid)
{
	if (USpawnArea* SpawnArea = FindSpawnAreaFromGuid(TargetGuid))
	{
		if (SpawnArea->IsRecent())
		{
			RemoveRecentFlagFromSpawnArea(TargetGuid);
		}
		if (!SpawnArea->IsActivated())
		{
			SpawnArea->SetIsActivated(true);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Tried to add an activated flag to a SpawnArea that was already flagged activated"));
		}
		if (SpawnArea->OverlappingVertices.IsEmpty())
		{
			TArray<FVector> DebugVertices;
			SpawnArea->SetOverlappingVertices(SpawnArea->GenerateOverlappingVertices(
				GetBSConfig()->TargetConfig.MinDistanceBetweenTargets, MinOverlapRadius, SpawnArea->GetTargetScale(), DebugVertices, false));
		}
	}
}

void USpawnAreaManagerComponent::FlagSpawnAreaAsRecent(const FGuid TargetGuid) const
{
	if (USpawnArea* SpawnArea = FindSpawnAreaFromGuid(TargetGuid))
	{
		if (!SpawnArea->IsRecent())
		{
			SpawnArea->SetIsRecent(true);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Tried to add a recent flag to a SpawnArea that was already flagged recent"));
		}
	}
}

void USpawnAreaManagerComponent::HandleRecentTargetRemoval(const ERecentTargetMemoryPolicy& RecentTargetMemoryPolicy, const FTargetDamageEvent& TargetDamageEvent)
{
	RemoveActivatedFlagFromSpawnArea(TargetDamageEvent);
	FlagSpawnAreaAsRecent(TargetDamageEvent.Guid);
	
	FTimerHandle TimerHandle;
	
	/* Handle removing recent flag from SpawnArea */
	switch (RecentTargetMemoryPolicy)
	{
	case ERecentTargetMemoryPolicy::None:
		break;
	case ERecentTargetMemoryPolicy::CustomTimeBased:
		RemoveFromRecentDelegate.BindUObject(this, &USpawnAreaManagerComponent::RemoveRecentFlagFromSpawnArea, TargetDamageEvent.Guid);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, RemoveFromRecentDelegate, GetBSConfig()->TargetConfig.RecentTargetTimeLength, false);
		break;
	case ERecentTargetMemoryPolicy::NumTargetsBased:
		RefreshRecentFlags();
		break;
	case ERecentTargetMemoryPolicy::UseTargetSpawnCD:
		RemoveFromRecentDelegate.BindUObject(this, &USpawnAreaManagerComponent::RemoveRecentFlagFromSpawnArea, TargetDamageEvent.Guid);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, RemoveFromRecentDelegate, GetBSConfig()->TargetConfig.TargetSpawnCD, false);
		break;
	}
}

void USpawnAreaManagerComponent::RemoveManagedFlagFromSpawnArea(const FGuid TargetGuid) const
{
	if (USpawnArea* SpawnArea = FindSpawnAreaFromGuid(TargetGuid))
	{
		if (SpawnArea->IsCurrentlyManaged())
		{
			SpawnArea->SetIsCurrentlyManaged(false);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Tried to remove a managed flag from a SpawnArea that wasn't managed"));
		}
	}
}

void USpawnAreaManagerComponent::RemoveActivatedFlagFromSpawnArea(const FTargetDamageEvent& TargetDamageEvent) const
{
	if (USpawnArea* SpawnArea = FindSpawnAreaFromGuid(TargetDamageEvent.Guid))
	{
		if (SpawnArea->IsActivated())
		{
			SpawnArea->SetIsActivated(false);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Tried to remove an activated flag from a SpawnArea that wasn't activated"));
		}
		SpawnArea->IncrementTotalSpawns();
		if (TargetDamageEvent.TimeAlive != INDEX_NONE)
		{
			SpawnArea->IncrementTotalHits();
		}
	}
}

void USpawnAreaManagerComponent::RemoveRecentFlagFromSpawnArea(const FGuid TargetGuid) 
{
	if (USpawnArea* SpawnArea = FindSpawnAreaFromGuid(TargetGuid))
	{
		if (SpawnArea->IsRecent())
		{
			SpawnArea->SetIsRecent(false);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Tried to remove a recent flag from a SpawnArea that wasn't recent"));
		}
	}
}

// Getting valid spawn locations

TArray<FVector> USpawnAreaManagerComponent::GetValidSpawnLocations(const FVector& Scale, const FExtrema& InCurrentExtrema, const USpawnArea* CurrentSpawnArea) const
{
	TArray<FVector> ValidSpawnLocations;
	
	switch (GetBSConfig()->TargetConfig.TargetDistributionPolicy)
	{
	case ETargetDistributionPolicy::EdgeOnly:
		HandleEdgeOnlySpawnLocations(ValidSpawnLocations, InCurrentExtrema);
		RemoveOverlappingSpawnLocations(ValidSpawnLocations, Scale);
		break;
	case ETargetDistributionPolicy::FullRange:
		HandleFullRangeSpawnLocations(ValidSpawnLocations, InCurrentExtrema);
		RemoveOverlappingSpawnLocations(ValidSpawnLocations, Scale);
		break;
	case ETargetDistributionPolicy::Grid:
		HandleGridSpawnLocations(ValidSpawnLocations, CurrentSpawnArea);
		break;
	case ETargetDistributionPolicy::None:
	case ETargetDistributionPolicy::HeadshotHeightOnly:
	default:
		ValidSpawnLocations = GetAllBottomLeftVertices();
		RemoveOverlappingSpawnLocations(ValidSpawnLocations, Scale);
		break;
	}
	
	if (bShowDebug_SpawnMemory)
	{
		DrawDebug_Boxes(ValidSpawnLocations, FColor::Emerald, 4, 0);
	}
	
	return ValidSpawnLocations;
}

void USpawnAreaManagerComponent::HandleEdgeOnlySpawnLocations(TArray<FVector>& ValidSpawnLocations, const FExtrema& Extrema) const
{
	const float MaxY = Extrema.Max.Y - GetSpawnMemoryIncY();
	const float MaxZ = Extrema.Max.Z - GetSpawnMemoryIncZ();
	const float OriginX = Origin.X;
	for (float Y = Extrema.Min.Y; Y < Extrema.Max.Y; Y += GetSpawnMemoryIncY())
	{
		ValidSpawnLocations.AddUnique(FVector(OriginX, Y, Extrema.Min.Z));
		ValidSpawnLocations.AddUnique(FVector(OriginX, Y, MaxZ));
	}
	for (float Z = Extrema.Min.Z; Z < Extrema.Max.Z; Z += GetSpawnMemoryIncZ())
	{
		ValidSpawnLocations.AddUnique(FVector(OriginX, Extrema.Min.Y, Z));
		ValidSpawnLocations.AddUnique(FVector(OriginX, MaxY, Z));
	}
	ValidSpawnLocations.Add(Origin);
	if (bShowDebug_SpawnMemory)
	{
		const TArray<FVector> RemovedLocations = GetAllBottomLeftVertices().FilterByPredicate([&] (const FVector& Vector)
		{
			return !ValidSpawnLocations.Contains(Vector);
		});
		DrawDebug_Boxes(RemovedLocations, FColor::Red, 4, 3);
	}
}

void USpawnAreaManagerComponent::HandleFullRangeSpawnLocations(TArray<FVector>& ValidSpawnLocations, const FExtrema& Extrema) const
{
	TArray<FVector> RemovedLocations;
	ValidSpawnLocations = GetAllBottomLeftVertices().FilterByPredicate([&](const FVector& Vector)
	{
		if (Vector.Y < Extrema.Min.Y || Vector.Y >= Extrema.Max.Y || Vector.Z < Extrema.Min.Z || Vector.Z >= Extrema.Max.Z)
		{
			if (bShowDebug_SpawnMemory)
			{
				RemovedLocations.Add(Vector);
			}
			return false;
		}
		return true;
	});
	if (bShowDebug_SpawnMemory)
	{
		DrawDebug_Boxes(RemovedLocations, FColor::Red, 4, 3);
	}
}

void USpawnAreaManagerComponent::HandleGridSpawnLocations(TArray<FVector>& ValidSpawnLocations, const USpawnArea* CurrentSpawnArea) const
{
	ValidSpawnLocations = GetAllBottomLeftVertices();
	switch (GetBSConfig()->TargetConfig.TargetActivationSelectionPolicy)
	{
	case ETargetActivationSelectionPolicy::None:
	case ETargetActivationSelectionPolicy::Random:
		HandleFilterActivated(ValidSpawnLocations);
		HandleFilterRecent(ValidSpawnLocations);
		break;
	case ETargetActivationSelectionPolicy::Bordering:
		HandleBorderingSelectionPolicy(ValidSpawnLocations, CurrentSpawnArea);
		break;
	default:
		break;
	}
}

void USpawnAreaManagerComponent::HandleBorderingSelectionPolicy(TArray<FVector>& ValidSpawnLocations, const USpawnArea* CurrentSpawnArea) const
{
	if (CurrentSpawnArea)
	{
		// Filter out non-bordering points
		if (const TArray<int32> BorderingIndices = CurrentSpawnArea->GetBorderingIndices(); !BorderingIndices.IsEmpty())
		{
			ValidSpawnLocations = ValidSpawnLocations.FilterByPredicate([&] (const FVector& Vector)
			{
				if (const USpawnArea* FoundPoint = FindSpawnAreaFromLocation(Vector))
				{
					if (!BorderingIndices.Contains(FoundPoint->GetIndex()))
					{
						return false;
					}
					return true;
				}
				return false;
			});
		}
	}
	
	// Without this condition, ValidSpawnLocations can contain locations that refer to a non-activated already managed target.
	// This means that the SpawnArea corresponding to an already managed target will get overriden with a new target, and the old target will no longer
	// have an associated SpawnArea, causing the game to get stuck with a non-activated target. Only applies for modes that allow spawning targets without activation.
	if (GetBSConfig()->TargetConfig.TargetSpawningPolicy == ETargetSpawningPolicy::RuntimeOnly && GetBSConfig()->TargetConfig.bAllowSpawnWithoutActivation)
	{
		HandleFilterActivated(ValidSpawnLocations);
		if (!ValidSpawnLocations.IsEmpty())
		{
			HandleFilterRecent(ValidSpawnLocations);
		}
		if (!ValidSpawnLocations.IsEmpty())
		{
			HandleFilterManaged(ValidSpawnLocations);
		}
		return;
	}
	
	// First try filtering out activated points
	TArray<FVector> NoActive = ValidSpawnLocations;
	HandleFilterActivated(NoActive);
	
	if (!NoActive.IsEmpty())
	{
		ValidSpawnLocations = NoActive;
		
		// Then try filtering out recent points too
		TArray<FVector> NoRecent = ValidSpawnLocations;
		HandleFilterRecent(NoRecent);

		if (!NoRecent.IsEmpty())
		{
			ValidSpawnLocations = NoRecent;
		}
	}
}

void USpawnAreaManagerComponent::HandleFilterRecent(TArray<FVector>& ValidSpawnLocations) const
{
	TArray<FVector> RemovedLocations;
	ValidSpawnLocations = ValidSpawnLocations.FilterByPredicate([&] (const FVector& Vector)
	{
		if (const USpawnArea* FoundPoint = FindSpawnAreaFromLocation(Vector))
		{
			if (FoundPoint->IsRecent())
			{
				RemovedLocations.Add(Vector);
				return false;
			}
			return true;
		}
		return false;
	});
	if (bShowDebug_SpawnMemory)
	{
		DrawDebug_Boxes(RemovedLocations, FColor::Red, 4, 3);
	}
}

void USpawnAreaManagerComponent::HandleFilterActivated(TArray<FVector>& ValidSpawnLocations) const
{
	TArray<FVector> RemovedLocations;
	ValidSpawnLocations = ValidSpawnLocations.FilterByPredicate([&] (const FVector& Vector)
	{
		if (const USpawnArea* FoundPoint = FindSpawnAreaFromLocation(Vector))
		{
			if (FoundPoint->IsActivated())
			{
				RemovedLocations.Add(Vector);
				return false;
			}
			return true;
		}
		return false;
	});
	if (bShowDebug_SpawnMemory)
	{
		DrawDebug_Boxes(RemovedLocations, FColor::Red, 4, 3);
	}
}

void USpawnAreaManagerComponent::HandleFilterManaged(TArray<FVector>& ValidSpawnLocations) const
{
	TArray<FVector> RemovedLocations;
	ValidSpawnLocations = ValidSpawnLocations.FilterByPredicate([&] (const FVector& Vector)
	{
		if (const USpawnArea* FoundPoint = FindSpawnAreaFromLocation(Vector))
		{
			if (FoundPoint->IsCurrentlyManaged())
			{
				RemovedLocations.Add(Vector);
				return false;
			}
			return true;
		}
		return false;
	});
	if (bShowDebug_SpawnMemory)
	{
		DrawDebug_Boxes(RemovedLocations, FColor::Blue, 4, 3);
	}
}

// Util or debug

int32 USpawnAreaManagerComponent::GetOutArrayIndexFromSpawnAreaIndex(const int32 SpawnAreaIndex) const
{
	/* First find the Row and Column number that corresponds to the SpawnCounter index */
	const int32 SpawnAreaRowNum = SpawnAreaIndex / GetSpawnAreasWidth();
	const int32 SpawnAreaColNum = SpawnAreaIndex % GetSpawnAreasWidth();

	const int32 WidthScaleFactor = GetSpawnAreasWidth() / 5;
	const int32 HeightScaleFactor = GetSpawnAreasHeight() / 5;

	/* Scale down the SpawnCounter row and column numbers */
	const int32 Row = SpawnAreaRowNum / HeightScaleFactor;
	const int32 Col = SpawnAreaColNum / WidthScaleFactor;
	const int32 Index = Row * 5 + Col;

	return Index;
}

void USpawnAreaManagerComponent::DrawDebug_Boxes(const TArray<FVector>& InLocations, const FColor& InColor, const int32 InThickness, const int32 InDepthPriority) const
{
	for (const FVector& Vector : InLocations)
	{
		FVector Loc = FVector(Vector.X, Vector.Y + GetSpawnMemoryIncY() / 2.f, Vector.Z + GetSpawnMemoryIncZ() / 2.f);
		DrawDebugBox(GetWorld(), Loc, FVector(0, GetSpawnMemoryIncY() / 2.f, GetSpawnMemoryIncZ() / 2.f),
			InColor, false, GetBSConfig()->TargetConfig.TargetSpawnCD, 0, InThickness);
	}
}

void USpawnAreaManagerComponent::PrintDebug_SpawnArea(const USpawnArea* SpawnArea) const
{
	UE_LOG(LogTemp, Display, TEXT("SpawnArea:"));
	UE_LOG(LogTemp, Display, TEXT("Index %d IndexType %s"), SpawnArea->Index, *UEnum::GetDisplayValueAsText(SpawnArea->GetIndexType()).ToString());
	UE_LOG(LogTemp, Display, TEXT("Vertex_BottomLeft: %s CenterPoint: %s ChosenPoint: %s"), *SpawnArea->Vertex_BottomLeft.ToString(), *SpawnArea->CenterPoint.ToString(), *SpawnArea->ChosenPoint.ToString());
	FString String;
	for (const int32 Border : SpawnArea->GetBorderingIndices())
	{
		String.Append(" " + FString::FromInt(Border));
	}
	UE_LOG(LogTemp, Display, TEXT("AdjacentIndices %s"), *String);
	UE_LOG(LogTemp, Display, TEXT("IsActivated %hhd IsRecent %hhd"), SpawnArea->IsActivated(), SpawnArea->IsRecent());
	UE_LOG(LogTemp, Display, TEXT("TotalSpawns %d TotalHits %d"), SpawnArea->GetTotalSpawns(), SpawnArea->GetTotalHits());
}
