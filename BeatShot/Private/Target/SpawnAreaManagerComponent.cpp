// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Target/SpawnAreaManagerComponent.h"
#include <stack>
#include "GlobalConstants.h"
#include "Algo/RandomShuffle.h"
#include "Target/MatrixFunctions.h"
#include "Target/TargetManager.h"

/** Preferred SpawnMemory increments */
const TArray PreferredSpawnAreaIncScales = {50, 45, 40, 30, 25, 20, 15, 10, 5};

USpawnAreaManagerComponent::USpawnAreaManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	#if !UE_BUILD_SHIPPING
	bShowDebug_AllSpawnAreas = false;
	bPrintDebug_SpawnAreaStateInfo = false;
	bShowDebug_SpawnableSpawnAreas = false;
	bShowDebug_ActivatableSpawnAreas = false;
	bShowDebug_ActivatedSpawnAreas = false;
	bShowDebug_DeactivatedSpawnAreas = false;
	bShowDebug_RecentSpawnAreas = false;
	bShowDebug_RemovedFromExtremaChange = false;
	bShowDebug_Vertices = false;
	bPrintDebug_Grid = false;
	bShowDebug_NonAdjacent = false;
	#endif

	BSConfig = nullptr;
	Size = FIntVector3();
	SpawnAreaInc = FIntVector3();
	SpawnAreaScale = FVector();
	Origin = FVector();
	StaticExtents = FVector();
	StaticExtrema = FExtrema();
	
	SpawnAreas = TSet<USpawnArea*>();
	AreaKeyMap = TMap<FAreaKey, USpawnArea*>();
	GuidMap = TMap<FGuid, USpawnArea*>();
	CachedExtrema = TSet<USpawnArea*>();
	CachedManaged = TSet<USpawnArea*>();
	CachedActivated = TSet<USpawnArea*>();
	CachedRecent = TSet<USpawnArea*>();
	MostRecentGridBlock = TSet<USpawnArea*>();
	
	MostRecentSpawnArea = nullptr;
	OriginSpawnArea = nullptr;
	bShouldAskRLCForSpawnAreas = false;
}

void USpawnAreaManagerComponent::DestroyComponent(bool bPromoteChildren)
{
	BSConfig.Reset();
	Super::DestroyComponent(bPromoteChildren);
}

void USpawnAreaManagerComponent::Init(const TSharedPtr<FBSConfig>& InConfig, const FVector& InOrigin,
	const FVector& InStaticExtents, const FExtrema& InStaticExtrema)
{
	Clear();

	BSConfig = InConfig;
	Origin = InOrigin;
	StaticExtents = InStaticExtents;
	StaticExtrema = InStaticExtrema;

	SetAppropriateSpawnMemoryValues(SpawnAreaInc, SpawnAreaScale, BSConfig.Get(), StaticExtents);
	InitializeSpawnAreas();

	OriginSpawnArea = FindSpawnArea(Origin);

	#if !UE_BUILD_SHIPPING
	UE_LOG(LogTargetManager, Display, TEXT("Origin: %s "), *Origin.ToCompactString());
	UE_LOG(LogTargetManager, Display, TEXT("StaticExtents: %s "), *StaticExtents.ToCompactString());
	UE_LOG(LogTargetManager, Display, TEXT("StaticExtrema Min: %s Max: %s"), *StaticExtrema.Min.ToCompactString(),
		*StaticExtrema.Max.ToCompactString());
	UE_LOG(LogTargetManager, Display, TEXT("SpawnMemoryScaleY: %.4f SpawnMemoryScaleZ: %.4f"), SpawnAreaScale.Y,
		SpawnAreaScale.Z);
	UE_LOG(LogTargetManager, Display, TEXT("SpawnAreaIncY: %d SpawnAreaIncZ: %d"), SpawnAreaInc.Y, SpawnAreaInc.Z);
	UE_LOG(LogTargetManager, Display, TEXT("SpawnCounterSize: %d Allocated Size: %llu"), SpawnAreas.Num(),
		SpawnAreas.GetAllocatedSize());
	#endif
}

void USpawnAreaManagerComponent::SetAppropriateSpawnMemoryValues(FIntVector3& OutSpawnAreaInc, FVector& OutSpawnAreaScale,
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
			OutSpawnAreaScale.Z = 1.f;
			OutSpawnAreaInc.Z = 1;
			for (const int32 Scale : PreferredSpawnAreaIncScales)
			{
				if (!bWidthScaleSelected)
				{
					if (HalfWidth % Scale == 0)
					{
						OutSpawnAreaInc.Y = Scale;
						OutSpawnAreaScale.Y = 1.f / Scale;
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
						OutSpawnAreaInc.Y = Scale;
						OutSpawnAreaScale.Y = 1.f / Scale;
						bWidthScaleSelected = true;
					}
				}
				if (!bHeightScaleSelected)
				{
					if (HalfHeight % Scale == 0)
					{
						OutSpawnAreaInc.Z = Scale;
						OutSpawnAreaScale.Z = 1.f / Scale;
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
			OutSpawnAreaInc.Y = InCfg->GridConfig.GridSpacing.X + MaxTargetSize;
			OutSpawnAreaInc.Z = InCfg->GridConfig.GridSpacing.Y + MaxTargetSize;
			OutSpawnAreaScale.Y = 1.f / OutSpawnAreaInc.Y;
			OutSpawnAreaScale.Z = 1.f / OutSpawnAreaInc.Z;
		}
		break;
	}
}

void USpawnAreaManagerComponent::InitializeSpawnAreas()
{
	int SizeY = 0;
	int SizeZ = 0;
	int Index = 0;

	// Add an extra row and column if using grid
	const bool bGrid = TargetConfig().TargetDistributionPolicy == ETargetDistributionPolicy::Grid;

	const float MinY = StaticExtrema.Min.Y;
	const float MaxY = bGrid ? StaticExtrema.Max.Y + 1.f : StaticExtrema.Max.Y;
	const float MinZ = StaticExtrema.Min.Z;
	const float MaxZ = bGrid ? StaticExtrema.Max.Z + 1.f : StaticExtrema.Max.Z;

	const float TotalWidth = FMath::Abs(MaxY - MinY);
	const float TotalHeight = FMath::Abs(MaxZ - MinZ);
	
	Size.Y = FMath::CeilToInt32(TotalWidth / SpawnAreaInc.Y);
	Size.Z = FMath::CeilToInt32(TotalHeight / SpawnAreaInc.Z);
	const int32 TotalSize = Size.Y * Size.Z;

	USpawnArea::SetWidth(SpawnAreaInc.Y);
	USpawnArea::SetHeight(SpawnAreaInc.Z);
	USpawnArea::SetTotalNumHorizontalSpawnAreas(Size.Y);
	USpawnArea::SetTotalNumVerticalSpawnAreas(Size.Z);
	USpawnArea::SetSize(TotalSize);
	
	SpawnAreas.Reserve(TotalSize);
	AreaKeyMap.Reserve(TotalSize);

	for (float Z = MinZ; Z < MaxZ; Z += SpawnAreaInc.Z)
	{
		SizeY = 0;
		for (float Y = MinY; Y < MaxY; Y += SpawnAreaInc.Y)
		{
			const FVector Loc(Origin.X, Y, Z);
			const FSetElementId ID = SpawnAreas.Emplace(NewObject<USpawnArea>());
			
			SpawnAreas[ID]->Init(Index, Loc);
			AreaKeyMap.Add(FAreaKey(Loc, SpawnAreaInc), SpawnAreas[ID]);

			Index++;
			SizeY++;
		}
		SizeZ++;
	}

	ensure(Size.Y == SizeY);
	ensure(Size.Z == SizeZ);

	CachedExtrema = SpawnAreas;
}

void USpawnAreaManagerComponent::Clear()
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	BSConfig.Reset();

	Size = FIntVector3();
	SpawnAreaInc = FIntVector3();
	SpawnAreaScale = FVector();
	Origin = FVector();
	StaticExtents = FVector();
	StaticExtrema = FExtrema();
	
	SpawnAreas.Empty();
	AreaKeyMap.Empty();
	GuidMap.Empty();
	CachedExtrema.Empty();
	CachedManaged.Empty();
	CachedActivated.Empty();
	CachedRecent.Empty();
	MostRecentGridBlock.Empty();
	
	MostRecentSpawnArea = nullptr;
	OriginSpawnArea = nullptr;
	
	RequestRLCSpawnArea.Unbind();
	bShouldAskRLCForSpawnAreas = false;

	#if !UE_BUILD_SHIPPING
	FlushPersistentDebugLines(GetWorld());
	#endif
}

bool USpawnAreaManagerComponent::ShouldConsiderManagedAsInvalid() const
{
	return TargetConfig().TargetSpawningPolicy == ETargetSpawningPolicy::RuntimeOnly;
}

void USpawnAreaManagerComponent::UpdateTotalTrackingDamagePossible(const FVector& InLocation) const
{
	if (USpawnArea* SpawnArea = FindSpawnArea(InLocation))
	{
		SpawnArea->IncrementTotalTrackingDamagePossible();
	}
}

void USpawnAreaManagerComponent::HandleTargetDamageEvent(const FTargetDamageEvent& DamageEvent)
{
	USpawnArea* SpawnArea = FindSpawnArea(DamageEvent.Guid);
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
			USpawnArea* SpawnAreaByLoc = FindSpawnArea(DamageEvent.Transform.GetLocation());
			if (!SpawnAreaByLoc)
			{
				UE_LOG(LogTargetManager, Warning, TEXT("Could not find SpawnArea from Transform: %s."),
					*DamageEvent.Transform.GetLocation().ToString());
				return;
			}

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

	if (DamageEvent.bWillDestroy)
	{
		RemoveManagedFlagFromSpawnArea(DamageEvent.Guid);
	}

	#if !UE_BUILD_SHIPPING
	RefreshDebugBoxes();
	#endif
}

void USpawnAreaManagerComponent::HandleRecentTargetRemoval(USpawnArea* SpawnArea)
{
	FlagSpawnAreaAsRecent(SpawnArea);
	FTimerHandle TimerHandle;

	/* Handle removing recent flag from SpawnArea */
	switch (TargetConfig().RecentTargetMemoryPolicy)
	{
	case ERecentTargetMemoryPolicy::None:
		RemoveRecentFlagFromSpawnArea(SpawnArea);
		break;
	case ERecentTargetMemoryPolicy::CustomTimeBased:
		{
			RemoveFromRecentDelegate.BindUObject(this, &ThisClass::RemoveRecentFlagFromSpawnArea, SpawnArea);
			const float Time = TargetConfig().RecentTargetTimeLength;
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
			RemoveFromRecentDelegate.BindUObject(this, &ThisClass::RemoveRecentFlagFromSpawnArea, SpawnArea);
			const float Time = TargetConfig().TargetSpawnCD;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, RemoveFromRecentDelegate, Time, false);
		}
		break;
	default:
		break;
	}
}

void USpawnAreaManagerComponent::OnExtremaChanged(const FExtrema& Extrema)
{
	#if !UE_BUILD_SHIPPING
	TSet<USpawnArea*> RemovedSpawnAreas;
	#endif

	switch (TargetConfig().TargetDistributionPolicy)
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
				if (USpawnArea* SpawnArea_MinZ = FindSpawnArea(FVector(0, Y, MinZ)))
				{
					Temp.Add(SpawnArea_MinZ);
				}
				if (USpawnArea* SpawnArea_MaxZ = FindSpawnArea(FVector(0, Y, MaxZ)))
				{
					Temp.Add(SpawnArea_MaxZ);
				}
			}

			for (float Z = MinZ; Z <= MaxZ; Z += SpawnAreaInc.Z)
			{
				if (USpawnArea* SpawnArea_MinY = FindSpawnArea(FVector(0, MinY, Z)))
				{
					Temp.Add(SpawnArea_MinY);
				}
				if (USpawnArea* SpawnArea_MaxY = FindSpawnArea(FVector(0, MaxY, Z)))
				{
					Temp.Add(SpawnArea_MaxY);
				}
			}

			#if !UE_BUILD_SHIPPING
			if (bShowDebug_RemovedFromExtremaChange)
			{
				RemovedSpawnAreas = SpawnAreas.Difference(Temp);
			}
			#endif

			CachedExtrema = MoveTemp(Temp);
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
				if (bShowDebug_RemovedFromExtremaChange) RemovedSpawnAreas.Add(SpawnArea);
				#endif
			}
			else
			{
				CachedExtrema.Add(SpawnArea);
			}
		}
		break;
	default:
		break;
	}
	#if !UE_BUILD_SHIPPING
	if (bShowDebug_RemovedFromExtremaChange)
	{
		DebugCached_RemovedFromExtremaChangeSpawnAreas = RemovedSpawnAreas;
	}
	#endif
}

/* ------------------------------- */
/* -- SpawnArea finders/getters -- */
/* ------------------------------- */

USpawnArea* USpawnAreaManagerComponent::GetSpawnArea(const int32 Index) const
{
	return IsSpawnAreaValid(Index) ? SpawnAreas[FSetElementId::FromInteger(Index)] : nullptr;
}

USpawnArea* USpawnAreaManagerComponent::FindSpawnArea(const FVector& InLocation) const
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

USpawnArea* USpawnAreaManagerComponent::FindSpawnArea(const FGuid& TargetGuid) const
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
	return InIndex >= 0 && InIndex < SpawnAreas.Num();
}

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
	return SpawnAreas.Difference(GetManagedActivatedOrRecentSpawnAreas());
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
	
	SpawnArea->SetGuid(TargetGuid);
	SpawnArea->SetIsManaged(true);

	// Add to caches and GuidMap
	GuidMap.Add(TargetGuid, SpawnArea);
	CachedManaged.Add(SpawnArea);
}

void USpawnAreaManagerComponent::FlagSpawnAreaAsActivated(const FGuid TargetGuid, const FVector& TargetScale)
{
	USpawnArea* SpawnArea = FindSpawnArea(TargetGuid);
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

	// Set is activated
	SpawnArea->SetIsActivated(true, TargetConfig().bAllowActivationWhileActivated);

	// Set as the most recently activated SpawnArea
	SetMostRecentSpawnArea(SpawnArea);

	// Update scale
	SpawnArea->SetTargetScale(TargetScale);
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
	USpawnArea* SpawnArea = FindSpawnArea(TargetGuid);
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

	SpawnArea->SetIsManaged(false);
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
	#if !UE_BUILD_SHIPPING
	if (bPrintDebug_SpawnAreaStateInfo)
	{
		PrintDebug_SpawnAreaStateInfo();
	}
	#endif
	
	if (TargetConfig().RecentTargetMemoryPolicy != ERecentTargetMemoryPolicy::NumTargetsBased) return;

	const int32 NumToRemove = CachedRecent.Num() - TargetConfig().MaxNumRecentTargets;
	if (NumToRemove <= 0) return;

	for (int32 CurrentRemoveNum = 0; CurrentRemoveNum < NumToRemove; CurrentRemoveNum++)
	{
		if (USpawnArea* Found = FindOldestRecentSpawnArea())
		{
			RemoveRecentFlagFromSpawnArea(Found);
		}
	}
}

/* --------------------------------------------------- */
/* -- Finding Valid SpawnAreas for Spawn/Activation -- */
/* --------------------------------------------------- */

TSet<USpawnArea*> USpawnAreaManagerComponent::GetActivatableSpawnAreas(const int32 NumToActivate) const
{
	// Assumes that we cannot activate an already activated target

	TSet<USpawnArea*> ValidSpawnAreas = GetManagedDeactivatedNotRecentSpawnAreas();

	/* TODO: Might need to have separate "Recent" for spawning and activation
	 * For game modes like ChargedBeatTrack, there will be 4 managed targets but also 4 recent targets at some point,
	 * which is why this condition exists */
	if (ValidSpawnAreas.IsEmpty())
	{
		ValidSpawnAreas = GetDeactivatedManagedSpawnAreas();
	}

	// ReSharper disable once CppLocalVariableMayBeConst
	USpawnArea* PreviousSpawnArea = GetMostRecentSpawnArea();
	
	switch (TargetConfig().TargetActivationSelectionPolicy)
	{
	case ETargetActivationSelectionPolicy::Bordering:
		{
			TSet<USpawnArea*> Filtered = ValidSpawnAreas;
			RemoveNonAdjacentIndices(Filtered, PreviousSpawnArea);
			if (Filtered.Num() >= NumToActivate)
			{
				ValidSpawnAreas = MoveTemp(Filtered);
			}
		}
		break;
	case ETargetActivationSelectionPolicy::None:
	case ETargetActivationSelectionPolicy::Random:
		break;
	default:
		break;
	}

	TSet<USpawnArea*> ChosenSpawnAreas;
	ChosenSpawnAreas.Reserve(NumToActivate);

	// Main loop for choosing spawn areas
	for (int i = 0; i < NumToActivate; i++)
	{
		if (USpawnArea* Chosen = ChooseActivatableSpawnArea(PreviousSpawnArea, ValidSpawnAreas, ChosenSpawnAreas))
		{
			// Add to the return array
			ChosenSpawnAreas.Add(Chosen);

			// Remove from options available to choose
			ValidSpawnAreas.Remove(Chosen);

			// Set as the previous SpawnArea since it will be activated before any chosen later
			PreviousSpawnArea = Chosen;
		}
	}

	return ChosenSpawnAreas;
}

USpawnArea* USpawnAreaManagerComponent::ChooseActivatableSpawnArea(const USpawnArea* PreviousSpawnArea,
	const TSet<USpawnArea*>& ValidSpawnAreas, const TSet<USpawnArea*>& SelectedSpawnAreas) const
{
	// 1st priority: force activate at origin
	// Requirements: Not the previous SpawnArea, not in selected, and corresponds to a spawned target (Valid Guid).
	// Unique exception that does not check ValidSpawnAreas.
	if (TargetConfig().bSpawnEveryOtherTargetInCenter)
	{
		USpawnArea* Candidate = GetOriginSpawnArea();
		if (Candidate && Candidate != PreviousSpawnArea)
		{
			if (Candidate->GetGuid().IsValid() && !SelectedSpawnAreas.Contains(Candidate))
			{
				return Candidate;
			}
		}
	}
	
	// 2st priority: origin if settings permit
	if (TargetConfig().bSpawnAtOriginWheneverPossible)
	{
		USpawnArea* Candidate = GetOriginSpawnArea();
		if (ValidSpawnAreas.Contains(Candidate) && Candidate->GetGuid().IsValid())
		{
			return Candidate;
		}
	}

	// 3rd priority: Let RLC choose the SpawnArea if settings permit
	if (bShouldAskRLCForSpawnAreas && RequestRLCSpawnArea.IsBound())
	{
		const int32 PreviousIndex = !PreviousSpawnArea ? -1 : PreviousSpawnArea->GetIndex();

		// Transform to array of indices
		TArray<int32> ValidIndices;
		ValidIndices.Reserve(ValidIndices.Num());
		Algo::Transform(ValidSpawnAreas, ValidIndices, [](const USpawnArea* SpawnArea)
		{
			return SpawnArea->GetIndex();
		});

		const int32 CandidateIndex = RequestRLCSpawnArea.Execute(PreviousIndex, ValidIndices);
		USpawnArea* Candidate = GetSpawnArea(CandidateIndex);
		if (Candidate && Candidate->GetGuid().IsValid())
		{
			return Candidate;
		}
	}

	// 4th priority: Randomly select an index from ValidSpawnAreas
	if (!ValidSpawnAreas.IsEmpty())
	{
		const int32 RandomIndex = FMath::RandRange(0, ValidSpawnAreas.Num() - 1);
		USpawnArea* RandomSpawnArea = ValidSpawnAreas.Array()[RandomIndex];
		if (RandomSpawnArea && RandomSpawnArea->GetGuid().IsValid())
		{
			return RandomSpawnArea;
		}
	}

	// No valid spawn area found
	return nullptr;
}

USpawnArea* USpawnAreaManagerComponent::ChooseSpawnableSpawnArea(const USpawnArea* PreviousSpawnArea,
	const TSet<USpawnArea*>& ValidSpawnAreas, const TSet<USpawnArea*>& SelectedSpawnAreas) const
{
	// 1st priority: force spawn at origin
	// Requirements: Not the previous SpawnArea, not managed, and not in selected.
	// Unique exception that does not check ValidSpawnAreas
	if (TargetConfig().bSpawnEveryOtherTargetInCenter)
	{
		USpawnArea* Candidate = GetOriginSpawnArea();
		if (Candidate && !Candidate->IsManaged() && PreviousSpawnArea != Candidate && !SelectedSpawnAreas.Contains(Candidate))
		{
			return Candidate;
		}
	}
	
	// 2st priority: origin if settings permit
	if (TargetConfig().bSpawnAtOriginWheneverPossible)
	{
		USpawnArea* Candidate = GetOriginSpawnArea();
		if (Candidate && ValidSpawnAreas.Contains(Candidate))
		{
			return Candidate;
		}
	}

	// 3rd priority: Let RLC choose the SpawnArea if settings permit
	if (bShouldAskRLCForSpawnAreas && RequestRLCSpawnArea.IsBound())
	{
		const int32 PreviousIndex = PreviousSpawnArea ? PreviousSpawnArea->GetIndex() : -1;

		// Transform to array of indices
		TArray<int32> ValidIndices;
		ValidIndices.Reserve(ValidIndices.Num());
		Algo::Transform(ValidSpawnAreas, ValidIndices, [](const USpawnArea* SpawnArea)
		{
			return SpawnArea->GetIndex();
		});

		const int32 CandidateIndex = RequestRLCSpawnArea.Execute(PreviousIndex, ValidIndices);
		if (USpawnArea* Candidate = GetSpawnArea(CandidateIndex))
		{
			return Candidate;
		}
		UE_LOG(LogTargetManager, Warning, TEXT("Unable to Spawn at SpawnArea suggested by RLAgent."));
	}

	// 4th priority: Randomly select an index from ValidSpawnAreas
	if (!ValidSpawnAreas.IsEmpty())
	{
		return ValidSpawnAreas.Array()[FMath::RandRange(0, ValidSpawnAreas.Num() - 1)];
	}

	// No valid spawn area found
	return nullptr;
}

TSet<USpawnArea*> USpawnAreaManagerComponent::GetSpawnableSpawnAreas(const TArray<FVector>& Scales,
	const int32 NumToSpawn) const
{
	switch (TargetConfig().TargetDistributionPolicy)
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

TSet<USpawnArea*> USpawnAreaManagerComponent::GetSpawnableSpawnAreas_Grid(const TArray<FVector>& Scales,
	const int32 NumToSpawn) const
{
	// Get all SpawnAreas that are managed, deactivated, and not recent
	TSet<USpawnArea*> ValidSpawnAreas = GetUnflaggedSpawnAreas();
	
	#if !UE_BUILD_SHIPPING
	if (bShowDebug_SpawnableSpawnAreas)
	{
		DebugCached_SpawnableValidSpawnAreas = ValidSpawnAreas;
	}
	#endif

	switch (TargetConfig().RuntimeTargetSpawningLocationSelectionMode)
	{
	case ERuntimeTargetSpawningLocationSelectionMode::None:
	case ERuntimeTargetSpawningLocationSelectionMode::Random:
		{
			TArray<USpawnArea*> Temp = ValidSpawnAreas.Array();
			Algo::RandomShuffle(Temp);
			ValidSpawnAreas = TSet(MoveTemp(Temp));
		}
		break;
	case ERuntimeTargetSpawningLocationSelectionMode::Bordering:
		{
			FindAdjacentGridUsingDFS(ValidSpawnAreas, NumToSpawn);
		}
		break;
	case ERuntimeTargetSpawningLocationSelectionMode::RandomGridBlock:
		{
			FindGridBlockUsingLargestRect(ValidSpawnAreas, CreateIndexValidityArray(ValidSpawnAreas), NumToSpawn, false);
		}
		break;
	case ERuntimeTargetSpawningLocationSelectionMode::NearbyGridBlock:
		{
			FindGridBlockUsingLargestRect(ValidSpawnAreas, CreateIndexValidityArray(ValidSpawnAreas), NumToSpawn, true);
		}
		break;
	case ERuntimeTargetSpawningLocationSelectionMode::RandomVertical: // TODO: NYI
		{
		}
		break;
	case ERuntimeTargetSpawningLocationSelectionMode::RandomHorizontal: // TODO: NYI
		{
		}
		break;
	}

	// Make sure number of elements is no more than number to spawn
	check(ValidSpawnAreas.Num() <= NumToSpawn);

	// Set the target scales
	int i = 0;
	for (USpawnArea* SpawnArea : ValidSpawnAreas)
	{
		SpawnArea->SetTargetScale(Scales[i++]);
		if (i >= Scales.Num()) break;
	}

	return ValidSpawnAreas;
}

TSet<USpawnArea*> USpawnAreaManagerComponent::GetSpawnableSpawnAreas_NonGrid(const TArray<FVector>& Scales,
	const int32 NumToSpawn) const
{
	// Start with all SpawnAreas within the current box bounds, RemoveOverlappingSpawnAreas will take care of rest
	TSet<USpawnArea*> ValidSpawnAreas = CachedExtrema;

	// ReSharper disable once CppLocalVariableMayBeConst
	USpawnArea* PreviousSpawnArea = GetMostRecentSpawnArea();
	
	TSet<USpawnArea*> ChosenSpawnAreas;

	// Main loop for choosing spawn areas
	for (int i = 0; i < NumToSpawn; i++)
	{
		TSet<USpawnArea*> ValidSpawnAreasCopy = ValidSpawnAreas;
		// Remove any overlap caused by any managed/activated SpawnAreas or any already chosen SpawnAreas.
		// This has to be done at every iteration because overlapping vertices need to be regenerated if
		// the current scale is larger than the Spawn Area's scale being compared (hence causing overlap).
		RemoveOverlappingSpawnAreas(ValidSpawnAreasCopy, ChosenSpawnAreas, Scales[i]);

		// If multiple are spawning with different scales, one further along might be able to fit
		if (ValidSpawnAreasCopy.IsEmpty()) continue;
		
		#if !UE_BUILD_SHIPPING
		if (bShowDebug_SpawnableSpawnAreas && i == 0)
		{
			DebugCached_SpawnableValidSpawnAreas = ValidSpawnAreas;
		}
		#endif

		if (USpawnArea* Chosen = ChooseSpawnableSpawnArea(PreviousSpawnArea, ValidSpawnAreasCopy, ChosenSpawnAreas))
		{
			// Only choose random sub point if not origin
			if (Chosen->GetIndex() != GetOriginSpawnArea()->GetIndex())
			{
				Chosen->SetRandomChosenPoint();
			}

			// Set the scale for the target to be spawned
			Chosen->SetTargetScale(Scales[i]);

			// Add to the return array
			ChosenSpawnAreas.Add(Chosen);

			// Remove from options available
			ValidSpawnAreas.Remove(Chosen);

			// Set as the previous SpawnArea since it will be spawned before any chosen later
			PreviousSpawnArea = Chosen;
		}
	}

	return ChosenSpawnAreas;
}

/* ---------------------------------------------------------------- */
/* -- Helper functions for Valid SpawnAreas for Spawn/Activation -- */
/* ---------------------------------------------------------------- */

void USpawnAreaManagerComponent::FindAdjacentGridUsingDFS(TSet<USpawnArea*>& ValidSpawnAreas,
	const int32 NumToSpawn) const
{
	if (ValidSpawnAreas.IsEmpty()) return;

	TArray StartNodeCandidates = MostRecentGridBlock.IsEmpty()
		? ValidSpawnAreas.Array()
		: GetAdjacentSpawnAreas(MostRecentGridBlock, AllIndexTypes).Intersect(ValidSpawnAreas).Array();
	
	TSet<USpawnArea*> ValidPath;
	
	while (ValidPath.Num() < NumToSpawn)
	{
		// Explore all candidate paths if necessary
		if (StartNodeCandidates.IsEmpty()) break;
		USpawnArea* StartNode = StartNodeCandidates[FMath::RandRange(0, StartNodeCandidates.Num() - 1)];
		StartNodeCandidates.RemoveSwap(StartNode);

		TSet<USpawnArea*> Visited;
		TSet<USpawnArea*> CurrentPath;
		TArray<USpawnArea*> Stack;
		Stack.Push(StartNode);

		while (!Stack.IsEmpty())
		{
			USpawnArea* Vertex = Stack.Pop(false);
			if (Visited.Contains(Vertex)) continue;
			
			CurrentPath.Add(Vertex);

			// Exit loop as soon as current path is filled with num to spawn elements
			if (CurrentPath.Num() == NumToSpawn)
			{
				ValidPath = MoveTemp(CurrentPath);
				break;
			}
			// If not possible to spawn NumToSpawn, keep track of best attempt
			if (CurrentPath.Num() > ValidPath.Num())
			{
				ValidPath = CurrentPath;
			}
		
			TArray<int32> AdjacentIndices = Vertex->GetAdjacentIndices().Array();
			Algo::RandomShuffle(AdjacentIndices);
	
			for (const int32 Index : AdjacentIndices)
			{
				if (USpawnArea* Adjacent = GetSpawnArea(Index))
				{
					if (!Visited.Contains(Adjacent) && ValidSpawnAreas.Contains(Adjacent))
					{
						Stack.Push(Adjacent);
					}
				}
			}
			Visited.Add(Vertex);
		}
	}
	ValidSpawnAreas = MoveTemp(ValidPath);
	MostRecentGridBlock = ValidSpawnAreas;
}

TSet<USpawnArea*> USpawnAreaManagerComponent::GetAdjacentSpawnAreas(TSet<USpawnArea*>& InSpawnAreas,
	const TSet<EBorderingDirection>& Directions) const
{
	TSet<USpawnArea*> Out;
	for (const USpawnArea* SpawnArea : InSpawnAreas)
	{
		for (const int32 Index : SpawnArea->GetAdjacentIndices(Directions))
		{
			if (USpawnArea* Adjacent = GetSpawnArea(Index))
			{
				Out.Add(Adjacent);
			}
		}
	}
	// Don't return any SpawnAreas in the original input
	return Out.Difference(InSpawnAreas);
}

void USpawnAreaManagerComponent::FindGridBlockUsingLargestRect(TSet<USpawnArea*>& ValidSpawnAreas,
	const TArray<int32>& IndexValidity, const int32 BlockSize, const bool bBordering) const
{
	FLargestRect Rect = FindLargestValidRectangle(IndexValidity, Size.Z, Size.Y);

	Rect.UpdateIndices(Size.Y);
	Rect.SetBlockSize(BlockSize);
	ChooseRectIndices(Rect, bBordering);

	#if !UE_BUILD_SHIPPING
	if (bPrintDebug_Grid)
	{
		PrintDebug_Matrix(IndexValidity, Size.Z, Size.Y);
		PrintDebug_GridLargestRect(Rect, Size.Y);
	}
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
			if (USpawnArea* SpawnArea = GetSpawnArea(Index))
			{
				ValidSpawnAreas.Add(SpawnArea);
				// Update MostRecentGridBlock set
				MostRecentGridBlock.Add(SpawnArea);
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

	if (USpawnArea* SpawnArea = GetSpawnArea(Index))
	{
		ValidSpawnAreas.Add(SpawnArea);
		MostRecentGridBlock.Add(SpawnArea);
	}
}

void USpawnAreaManagerComponent::ChooseRectIndices(FLargestRect& Rect, const bool bBordering) const
{
	// Set defaults for early returns
	Rect.ChosenStartRowIndex = Rect.StartRowIndex;
	Rect.ChosenStartColIndex = Rect.StartColIndex;
	Rect.ChosenEndRowIndex = Rect.EndRowIndex;
	Rect.ChosenEndColIndex = Rect.EndColIndex;

	// The size of the sub block within the rectangle
	FFactor RandomFactor;

	// If the block size is prime, find the min factors of one less and one greater than the ActualBlockSize
	if (IsPrime(Rect.ActualBlockSize))
	{
		const TSet<FFactor> OneLessBlockSize = FindBestFittingFactors(Rect.ActualBlockSize - 1, Rect);
		const TSet<FFactor> OneGreaterBlockSize = FindBestFittingFactors(Rect.ActualBlockSize + 1, Rect);

		if (OneLessBlockSize.IsEmpty() && OneGreaterBlockSize.IsEmpty()) return;

		FFactor MinDifferenceFactor = FFactor(Rect.ActualBlockSize);
		TSet<FFactor> MinFactors;

		for (const FFactor Factor : OneLessBlockSize.Union(OneGreaterBlockSize))
		{
			if (Factor.Distance < MinDifferenceFactor.Distance)
			{
				MinDifferenceFactor = Factor;
				MinFactors.Empty();
			}
			if (Factor.Distance <= MinDifferenceFactor.Distance)
			{
				MinFactors.Add(Factor);
			}
		}
		
		if (MinFactors.IsEmpty()) return;
		
		RandomFactor = MinFactors.Array()[FMath::RandRange(0, MinFactors.Num() - 1)];
		
		// Mark the rect as needing a remainder index (OneLessBlockSize was chosen),
		// will be found later in FindGridBlockUsingLargestRect
		if (RandomFactor.Factor1 * RandomFactor.Factor2 < Rect.ActualBlockSize)
		{
			Rect.bNeedsRemainderIndex = true;
		}
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
		// Panic
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
	if (bPrintDebug_Grid)
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
					if (USpawnArea* SpawnArea = GetSpawnArea(UpLeftIndex))
					{
						Out.Add(SpawnArea);
					}
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
					if (USpawnArea* SpawnArea = GetSpawnArea(UpRightIndex))
					{
						Out.Add(SpawnArea);
					}
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
					if (USpawnArea* SpawnArea = GetSpawnArea(DownLeftIndex))
					{
						Out.Add(SpawnArea);
					}
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
					if (USpawnArea* SpawnArea = GetSpawnArea(DownRightIndex))
					{
						Out.Add(SpawnArea);
					}
				}
			}
		}
		if (Row == MinRow && Directions.Contains(EBorderingDirection::Down))
		{
			const int32 DownIndex = TestIndex - Size.Y;
			if (USpawnArea* SpawnArea = GetSpawnArea(DownIndex))
			{
				Out.Add(SpawnArea);
			}
		}
		if (Row == MaxRow && Directions.Contains(EBorderingDirection::Up))
		{
			const int32 UpIndex = TestIndex + Size.Y;
			if (USpawnArea* SpawnArea = GetSpawnArea(UpIndex))
			{
				Out.Add(SpawnArea);
			}
		}
		if (Col == MinCol && Directions.Contains(EBorderingDirection::Left))
		{
			const int32 LeftIndex = TestIndex - 1;
			const int32 MinRowIndex = FMath::Floor(TestIndex / Size.Y) * Size.Y;
			if (LeftIndex >= MinRowIndex)
			{
				if (USpawnArea* SpawnArea = GetSpawnArea(LeftIndex))
				{
					Out.Add(SpawnArea);
				}
			}
		}
		if (Col == MaxCol && Directions.Contains(EBorderingDirection::Right))
		{
			const int32 RightIndex = TestIndex + 1;
			const int32 MaxRowIndex = FMath::Floor(TestIndex / Size.Y) * Size.Y + Size.Y - 1;
			if (RightIndex <= MaxRowIndex)
			{
				if (USpawnArea* SpawnArea = GetSpawnArea(RightIndex))
				{
					Out.Add(SpawnArea);
				}
			}
		}
	}
	return Out;
}

void USpawnAreaManagerComponent::RemoveOverlappingSpawnAreas(TSet<USpawnArea*>& ValidSpawnAreas,
	const TSet<USpawnArea*>& ChosenSpawnAreas, const FVector& NewScale) const
{
	// TODO: This is terribly expensive if going from small target scale to large target scale
	TSet<FVector> Invalid;
	TSet<USpawnArea*> InvalidSpawnAreas;
	
	if (ShouldConsiderManagedAsInvalid())
	{
		// Only consider Managed Targets to be invalid if runtime
		InvalidSpawnAreas = GetManagedActivatedOrRecentSpawnAreas();
	}
	else
	{
		InvalidSpawnAreas = GetActivatedOrRecentSpawnAreas();
	}
	
	// Add any chosen SpawnAreas that are queued up to be spawned
	InvalidSpawnAreas.Append(ChosenSpawnAreas);

	for (USpawnArea* SpawnArea : InvalidSpawnAreas)
	{
		if (SpawnArea->GetTargetScale().Length() > NewScale.Length())
		{
			SpawnArea->SetOccupiedVertices(SpawnArea->MakeOccupiedVertices(GetMinDist(), SpawnArea->GetTargetScale()));
		}
		else
		{
			SpawnArea->SetOccupiedVertices(SpawnArea->MakeOccupiedVertices(GetMinDist(), NewScale));
		}
		Invalid.Append(SpawnArea->GetOccupiedVertices());
	}

	for (const FVector& BotLeft : Invalid)
	{
		if (const USpawnArea* Found = AreaKeyMap.FindRef(FAreaKey(BotLeft, SpawnAreaInc)))
		{
			ValidSpawnAreas.Remove(Found);
		}
	}
}

int32 USpawnAreaManagerComponent::RemoveNonAdjacentIndices(TSet<USpawnArea*>& ValidSpawnAreas,
	const USpawnArea* Current) const
{
	if (!Current) return 0;
	
	const int32 PreviousSize = ValidSpawnAreas.Num();
	TSet<USpawnArea*> BorderingSpawnAreas;

	for (const int32 Index : Current->GetAdjacentIndices())
	{
		USpawnArea* SpawnArea = GetSpawnArea(Index);
		check(SpawnArea);
		if (ValidSpawnAreas.Contains(SpawnArea))
		{
			BorderingSpawnAreas.Add(SpawnArea);
		}
	}
	
	#if !UE_BUILD_SHIPPING
	if (bShowDebug_NonAdjacent)
	{
		DebugCached_FilteredBorderingSpawnAreas = ValidSpawnAreas.Difference(BorderingSpawnAreas);
	}
	#endif
	
	ValidSpawnAreas = MoveTemp(BorderingSpawnAreas);
	return PreviousSize - ValidSpawnAreas.Num();
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
	if (bPrintDebug_Grid)
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
	if (bPrintDebug_Grid)
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
	
	// UE_LOG(LogTargetManager, Display, TEXT("MinDistance between factors of %d: %.2f"), Number, MinDistance);
	
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

constexpr bool USpawnAreaManagerComponent::IsPrime(const int32 Number)
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

	#if !UE_BUILD_SHIPPING
	int32 TotalSpawnsValueRef = 0;
	int32 TotalHitsValueRef = 0;
	int32 TotalSpawnsValueTest = 0;
	int32 TotalHitsValueTest = 0;
	#endif

	const bool bHitDamage = TargetConfig().TargetDamageType == ETargetDamageType::Hit;
	// For now only handle separate Hit and Tracking Damage
	int32 (USpawnArea::*TotalFunc)() const = bHitDamage
		? &USpawnArea::GetTotalSpawns
		: &USpawnArea::GetTotalTrackingDamagePossible;
	int32 (USpawnArea::*HitFunc)() const = bHitDamage ? &USpawnArea::GetTotalHits : &USpawnArea::GetTotalTrackingDamage;

	for (USpawnArea* SpawnArea : SpawnAreas)
	{
		const int32 Index = SpawnArea->GetIndex();
		const int32 SpawnsValue = (SpawnArea->*TotalFunc)();
		const int32 HitsValue = (SpawnArea->*HitFunc)();

		TotalSpawns[Index] = SpawnsValue;
		TotalHits[Index] = HitsValue;

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

int32 USpawnAreaManagerComponent::CalcManhattanDist(const int32 Index1, const int32 Index2, const int32 NumCols)
{
	const int32 Row1 = Index1 / NumCols;
	const int32 Col1 = Index1 % NumCols;
	const int32 Row2 = Index2 / NumCols;
	const int32 Col2 = Index2 % NumCols;

	// Calculate Manhattan distance (sum of horizontal and vertical distances)
	return FMath::Abs(Row2 - Row1) + FMath::Abs(Col2 - Col1);
}

TArray<int32> USpawnAreaManagerComponent::FilterIndices(TArray<USpawnArea*>& ValidSpawnAreas,
	bool (USpawnArea::*FilterFunc)() const)
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
	return Removed;
}

/* ----------- */
/* -- Debug -- */
/* ----------- */
#if !UE_BUILD_SHIPPING

void USpawnAreaManagerComponent::RefreshDebugBoxes() const
{
	FlushPersistentDebugLines(GetWorld());
	
	const TSet<USpawnArea*> Activated = GetActivatedSpawnAreas();
	const TSet<USpawnArea*> Deactivated = GetDeactivatedManagedSpawnAreas();
	const TSet<USpawnArea*> Recent = GetRecentSpawnAreas();
	TSet<USpawnArea*> ValidActivatable = GetManagedDeactivatedNotRecentSpawnAreas();
	if (ValidActivatable.IsEmpty())
	{
		ValidActivatable = GetDeactivatedManagedSpawnAreas();
	}
	const TSet<USpawnArea*> ValidSpawnable = DebugCached_SpawnableValidSpawnAreas;
	const TSet<USpawnArea*> RemovedExtrema = DebugCached_RemovedFromExtremaChangeSpawnAreas;
	const TSet<USpawnArea*> RemovedBordering = DebugCached_FilteredBorderingSpawnAreas;

	if (bShowDebug_AllSpawnAreas)
	{
		DrawDebug_Boxes(SpawnAreas, DebugColor_AllSpawnAreas, 6, 0, true);
	}
	if (bShowDebug_RemovedFromExtremaChange)
	{
		DrawDebug_Boxes(RemovedExtrema, DebugColor_RemovedFromExtremaChange, 6, 0, true);
	}
	if (bShowDebug_NonAdjacent)
	{
		DrawDebug_Boxes(RemovedBordering, DebugColor_NonAdjacent, 6, 0, true);
	}
	if (bShowDebug_SpawnableSpawnAreas)
	{
		DrawDebug_Boxes(ValidSpawnable, DebugColor_SpawnableSpawnAreas, 6, 0, true);
	}
	if (bShowDebug_ActivatableSpawnAreas)
	{
		DrawDebug_Boxes(ValidActivatable, DebugColor_ActivatableSpawnAreas, 6, 0, true);
	}
	if (bShowDebug_DeactivatedSpawnAreas)
	{
		DrawDebug_Boxes(Deactivated, DebugColor_DeactivatedSpawnAreas, 6, 0, true);
	}
	if (bShowDebug_RecentSpawnAreas)
	{
		DrawDebug_Boxes(Recent, DebugColor_RecentSpawnAreas, 6, 0, true);
	}
	if (bShowDebug_ActivatedSpawnAreas)
	{
		DrawDebug_Boxes(Activated, DebugColor_ActivatedSpawnAreas, 6, 0, true);
	}
	if (bShowDebug_Vertices)
	{
		DrawDebug_Vertices(Activated, 12.f, 12.f, 0);
	}
}

void USpawnAreaManagerComponent::DrawDebug_Boxes(const TSet<USpawnArea*>& InSpawnAreas, const FColor& Color,
	const int32 Thickness, const int32 Priority, const bool bPersistent) const
{
	const float Time = bPersistent ? -1.f : TargetConfig().TargetSpawnCD;
	const float PriorityOffset = Priority * Thickness;
	const FVector HalfInc = { 0, (GetSpawnAreaInc().Y - PriorityOffset ) * 0.5f, (GetSpawnAreaInc().Z - PriorityOffset) * 0.5f };
	const UWorld* World = GetWorld();
	for (const USpawnArea* SpawnArea : InSpawnAreas)
	{
		DrawDebugBox(World, SpawnArea->GetMiddleVertex(), HalfInc, Color, bPersistent, Time, 0, Thickness);
	}
}

void USpawnAreaManagerComponent::DrawDebug_Vertices(const TSet<USpawnArea*>& InSpawnAreas, const float InvalidSize,
	const float ValidSize, const int32 Priority) const
{
	const FVector InvalidOffset = FVector(Priority * InvalidSize, 0.f, 0.f);
	const FVector ValidOffset = FVector(Priority * ValidSize, 0.f, 0.f);
	for (const USpawnArea* SpawnArea : InSpawnAreas)
	{
		const float ScaledRadius = SpawnArea->GetTargetScale().X * SphereTargetRadius;
		
		// Multiply by two so that any point outside the sphere will not be overlapping
		float Radius = ScaledRadius * 2.f + (GetMinDist()* 0.5f);

		// Radius can never be less than MinOverlapRadius
		Radius = FMath::Max(Radius, SpawnArea->GetMinOverlapRadius());

		// Add max of height/width to account for random spawning within a SpawnArea
		Radius += FMath::Max(SpawnArea->Width, SpawnArea->Height);
		
		DrawDebugSphere(GetWorld(), SpawnArea->ChosenPoint, Radius, 10, FColor::Magenta, true);
		
		for (const FVector& Vertex : SpawnArea->GetOccupiedVertices())
		{
			DrawDebugPoint(GetWorld(), Vertex + InvalidOffset, InvalidSize, FColor::Red, true);
		}
		for (const FVector& Vertex: SpawnArea->GetUnoccupiedVertices(GetMinDist(), SpawnArea->GetTargetScale()))
        {
			DrawDebugPoint(GetWorld(), Vertex + ValidOffset, ValidSize, FColor::Green, true);
        }
	}
}

void USpawnAreaManagerComponent::PrintDebug_SpawnAreaStateInfo() const
{
	const int NumRecent = GetRecentSpawnAreas().Num();
	const int NumAct = GetActivatedSpawnAreas().Num();
	const int NumManaged = GetManagedSpawnAreas().Num();
	UE_LOG(LogTargetManager, Display, TEXT("NumRecent: %d NumActivated: %d NumManaged: %d"), NumRecent, NumAct,
		NumManaged);
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
	
	for (const int32 Border : SpawnArea->GetAdjacentIndices())
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

	const FString Xs = NumRows * NumCols >= 100 ? "___" : "__";

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
#endif