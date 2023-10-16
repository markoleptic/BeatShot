// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Target/TargetManagerPreview.h"
#include "Target/TargetPreview.h"


ATargetManagerPreview::ATargetManagerPreview()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATargetManagerPreview::InitBoxBoundsWidget(
	const TObjectPtr<UCustomGameModesWidget_Preview> InGameModePreviewWidget)
{
	GameModePreviewWidget = InGameModePreviewWidget;
}

void ATargetManagerPreview::RestartSimulation()
{
	Init(BSConfig, PlayerSettings);
}

void ATargetManagerPreview::FinishSimulation()
{
	if (!GetManagedTargets().IsEmpty())
	{
		for (const TPair<FGuid, ATarget*> Pair : GetManagedTargets())
		{
			if (Pair.Value)
			{
				Pair.Value->Destroy();
			}
		}
		ManagedTargets.Empty();
	}
}

float ATargetManagerPreview::GetSimulation_TargetSpawnCD() const
{
	if (BSConfig)
	{
		return BSConfig->TargetConfig.TargetSpawnCD;
	}
	return -1.f;
}

void ATargetManagerPreview::SetSimulatePlayerDestroyingTargets(const bool bInSimulatePlayerDestroyingTargets,
	const float InDestroyChance)
{
	bSimulatePlayerDestroyingTargets = bInSimulatePlayerDestroyingTargets;
	DestroyChance = InDestroyChance;
}

ATarget* ATargetManagerPreview::SpawnTarget(USpawnArea* InSpawnArea)
{
	ATarget* Target = Super::SpawnTarget(InSpawnArea);
	if (Target)
	{
		if (ATargetPreview* TargetPreview = Cast<ATargetPreview>(Target))
		{
			if (CreateTargetWidget.IsBound())
			{
				
				if (UTargetWidget* TargetWidget = CreateTargetWidget.Execute())
				{
					const float Height = StaticExtents.Z + (GetBSConfig()->TargetConfig.FloorDistance - ClampedOverflowAmount) * 0.5f;
					TargetPreview->InitTargetWidget(TargetWidget, GetSpawnBoxOrigin(), TargetPreview->GetActorLocation(), Height);
					TargetPreview->SetSimulatePlayerDestroying(bSimulatePlayerDestroyingTargets, DestroyChance);
				}
			}
		}
	}
	return Target;
}

void ATargetManagerPreview::UpdateSpawnVolume(const float Factor) const
{
	Super::UpdateSpawnVolume(Factor);

	if (!GetBSConfig() || !GameModePreviewWidget)
	{
		return;
	}
	
	if (GetBSConfig()->TargetConfig.FloorDistance > MaxAllowedFloorDistance)
	{
		ClampedOverflowAmount = MaxAllowedFloorDistance - GetBSConfig()->TargetConfig.FloorDistance;
		if (!bIsExceedingMaxFloorDistance)
		{
			GameModePreviewWidget->SetText_FloorDistance(FloorDistanceExceededText);
		}
		bIsExceedingMaxFloorDistance = true;
	}
	else
	{
		ClampedOverflowAmount = 0.f;
		if (bIsExceedingMaxFloorDistance)
		{
			GameModePreviewWidget->SetText_FloorDistance(FloorDistanceText);
		}
		bIsExceedingMaxFloorDistance = false;
	}
	
	GameModePreviewWidget->SetFloorDistanceHeight(FMath::Clamp(GetBSConfig()->TargetConfig.FloorDistance, 110.f,
		MaxAllowedFloorDistance));

	const FVector Origin = GetSpawnBoxOrigin();
	const FVector SBExtents = GetSpawnBoxExtents();

	// Set the Current box bounds widget size and position
	const float CurrentY = SBExtents.Y * 2.f;
	const float CurrentZ = SBExtents.Z * 2.f;
	float Height = Origin.Z - SBExtents.Z + ClampedOverflowAmount;
	GameModePreviewWidget->SetBoxBounds_Current(FVector2d(CurrentY, CurrentZ), Height);

	if (GetBSConfig()->TargetConfig.BoundsScalingPolicy == EBoundsScalingPolicy::Dynamic)
	{
		const FVector StartExtents = GetBSConfig()->DynamicSpawnAreaScaling.GetStartExtents();
		const FIntVector3 Inc = SpawnAreaManager->GetSpawnAreaInc();
		
		// Set the Min/Start box bounds widget size and position
		const float StartZ = FMath::GridSnap(StartExtents.Z, Inc.Z) * 2.f;
		const float StartY = FMath::GridSnap(StartExtents.Y, Inc.Y) * 2.f;
		Height = Origin.Z - (StartZ / 2.f) + ClampedOverflowAmount;
		GameModePreviewWidget->SetBoxBounds_Min(FVector2d(StartY, StartZ), Height);

		// Set the Max/End box bounds widget size and position
		const float EndZ = FMath::GridSnap(StaticExtents.Z, Inc.Z) * 2.f;
		const float EndY = FMath::GridSnap(StaticExtents.Y, Inc.Y) * 2.f;
		Height = Origin.Z - (EndZ / 2.f) + ClampedOverflowAmount;
		GameModePreviewWidget->SetBoxBounds_Max(FVector2d(EndY, EndZ), Height);

		// Adjust visibility and Preview Widget height
		GameModePreviewWidget->SetBoxBoundsVisibility_Min(ESlateVisibility::SelfHitTestInvisible);
		GameModePreviewWidget->SetBoxBoundsVisibility_Max(ESlateVisibility::SelfHitTestInvisible);
		GameModePreviewWidget->SetStaticBoundsHeight(FMath::Max(StartZ, EndZ));
	}
	else
	{
		GameModePreviewWidget->SetBoxBoundsVisibility_Min(ESlateVisibility::Collapsed);
		GameModePreviewWidget->SetBoxBoundsVisibility_Max(ESlateVisibility::Collapsed);
		GameModePreviewWidget->SetStaticBoundsHeight(CurrentZ);
	}
}
