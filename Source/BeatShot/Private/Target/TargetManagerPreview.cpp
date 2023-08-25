// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Target/TargetManagerPreview.h"
#include "Target/TargetPreview.h"


ATargetManagerPreview::ATargetManagerPreview()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATargetManagerPreview::InitBoxBoundsWidget(const TObjectPtr<UBoxBoundsWidget> InBoxBoundsWidget)
{
	BoxBoundsWidget = InBoxBoundsWidget;
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
	if (BoxBoundsWidget && GetBSConfig())
	{
		BoxBoundsWidget->SetBoxBounds(FVector2d(SpawnBox->GetUnscaledBoxExtent().Y * 2.f, SpawnBox->GetUnscaledBoxExtent().Z * 2.f));
	}
}

