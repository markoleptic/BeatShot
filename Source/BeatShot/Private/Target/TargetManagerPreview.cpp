// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Target/TargetManagerPreview.h"
#include "Components/SizeBox.h"
#include "Target/TargetPreview.h"


ATargetManagerPreview::ATargetManagerPreview()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATargetManagerPreview::InitBoxBoundsWidget(const TObjectPtr<UBoxBoundsWidget> InCurrent,
	const TObjectPtr<UBoxBoundsWidget> InMin, const TObjectPtr<UBoxBoundsWidget> InMax, const TObjectPtr<USizeBox> InFloorDistance)
{
	BoxBoundsWidget_Current = InCurrent;
	BoxBoundsWidget_Min = InMin;
	BoxBoundsWidget_Max = InMax;
	FloorDistance = InFloorDistance;
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
					TargetPreview->InitTargetWidget(TargetWidget, GetBoxOrigin(), TargetPreview->GetActorLocation());
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
	if (!GetBSConfig())
	{
		return;
	}
	if (BoxBoundsWidget_Current)
	{
		BoxBoundsWidget_Current->SetBoxBounds(FVector2d(SpawnBox->GetUnscaledBoxExtent().Y * 2.f, SpawnBox->GetUnscaledBoxExtent().Z * 2.f));
		BoxBoundsWidget_Current->SetBoxBoundsPosition(GetBoxOrigin().Z - SpawnBox->Bounds.BoxExtent.Z);
	}
	
	if (BoxBoundsWidget_Min)
	{
		if (GetBSConfig()->TargetConfig.BoundsScalingPolicy == EBoundsScalingPolicy::Dynamic)
		{
			BoxBoundsWidget_Min->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			const float MinY = FMath::GridSnap<float>(GetBSConfig()->DynamicSpawnAreaScaling.GetMinExtent().Y, SpawnAreaManager->GetSpawnMemoryIncY()) * 2.f;
			const float MinZ = FMath::GridSnap<float>(GetBSConfig()->DynamicSpawnAreaScaling.GetMinExtent().Z, SpawnAreaManager->GetSpawnMemoryIncZ()) * 2.f;
			BoxBoundsWidget_Min->SetBoxBounds(FVector2d(MinY, MinZ));
			BoxBoundsWidget_Min->SetBoxBoundsPosition(GetBoxOrigin().Z - (MinZ / 2.f));
		}
		else
		{
			BoxBoundsWidget_Min->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	if (BoxBoundsWidget_Max)
	{
		if (GetBSConfig()->TargetConfig.BoundsScalingPolicy == EBoundsScalingPolicy::Dynamic)
		{
			BoxBoundsWidget_Max->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			const float MaxY = FMath::GridSnap<float>(GetBoxExtents_Static().Y, SpawnAreaManager->GetSpawnMemoryIncY()) * 2.f;
			const float MaxZ = FMath::GridSnap<float>(GetBoxExtents_Static().Z, SpawnAreaManager->GetSpawnMemoryIncZ()) * 2.f;
			BoxBoundsWidget_Max->SetBoxBounds(FVector2d(MaxY, MaxZ));
			BoxBoundsWidget_Max->SetBoxBoundsPosition(GetBoxOrigin().Z - (MaxZ / 2.f));
		}
		else
		{
			BoxBoundsWidget_Max->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	if (FloorDistance)
	{
		FloorDistance->SetHeightOverride(BSConfig->TargetConfig.FloorDistance);
	}
}

