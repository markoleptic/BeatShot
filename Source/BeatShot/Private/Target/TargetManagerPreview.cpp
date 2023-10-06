// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Target/TargetManagerPreview.h"
#include "Target/TargetPreview.h"


ATargetManagerPreview::ATargetManagerPreview()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATargetManagerPreview::InitBoxBoundsWidget(const TObjectPtr<UCustomGameModesWidget_Preview> InGameModePreviewWidget)
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
		for (TObjectPtr<ATarget> Target : GetManagedTargets())
		{
			if (Target)
			{
				Target->Destroy();
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

void ATargetManagerPreview::SetSimulatePlayerDestroyingTargets(const bool bInSimulatePlayerDestroyingTargets, const float InDestroyChance)
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
					TargetPreview->InitTargetWidget(TargetWidget, GetBoxOrigin(), TargetPreview->GetActorLocation(),
						FMath::Max(GetBSConfig()->DynamicSpawnAreaScaling.GetMinExtent().Z, Get3DBoxExtents().Z) * 2.f);
					TargetPreview->SetSimulatePlayerDestroying(bSimulatePlayerDestroyingTargets, DestroyChance);
				}
			}
		}
	}
	return Target;
}

void ATargetManagerPreview::UpdateSpawnVolume() const
{
	Super::UpdateSpawnVolume();
	
	if (!GetBSConfig() || !GameModePreviewWidget)
	{
		return;
	}

	// Set the Current box bounds widget size and position
	const float CurrentY = SpawnBox->GetUnscaledBoxExtent().Y * 2.f;
	const float CurrentZ = SpawnBox->GetUnscaledBoxExtent().Z * 2.f;
	float Height = GetBoxOrigin().Z - SpawnBox->Bounds.BoxExtent.Z + ClampedOverflowAmount;
	GameModePreviewWidget->SetBoxBounds_Current(FVector2d(CurrentY, CurrentZ), Height);
	
	if (GetBSConfig()->TargetConfig.BoundsScalingPolicy == EBoundsScalingPolicy::Dynamic)
	{
		// Set the "Min"/Start box bounds widget size and position
		const float StartZ = FMath::GridSnap<float>(GetBSConfig()->DynamicSpawnAreaScaling.GetMinExtent().Z, SpawnAreaManager->GetSpawnMemoryIncZ()) * 2.f;
		const float StartY = FMath::GridSnap<float>(GetBSConfig()->DynamicSpawnAreaScaling.GetMinExtent().Y, SpawnAreaManager->GetSpawnMemoryIncY()) * 2.f;
		Height = GetBoxOrigin().Z - (StartZ/ 2.f) + ClampedOverflowAmount;
		GameModePreviewWidget->SetBoxBounds_Min(FVector2d(StartY, StartZ), Height);

		// Set the "Max"/End box bounds widget size and position
		const float EndZ = FMath::GridSnap<float>(Get3DBoxExtents().Z, SpawnAreaManager->GetSpawnMemoryIncZ()) * 2.f;
		const float EndY = FMath::GridSnap<float>(Get3DBoxExtents().Y, SpawnAreaManager->GetSpawnMemoryIncY()) * 2.f;
		Height = GetBoxOrigin().Z - (EndZ / 2.f) + ClampedOverflowAmount;
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
	
	GameModePreviewWidget->SetFloorDistanceHeight(FMath::Clamp(GetBSConfig()->TargetConfig.FloorDistance, 110.f, MaxAllowedFloorDistance));
	
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
}

