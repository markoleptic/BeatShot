// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Target/TargetManagerPreview.h"

#include "Target/TargetPreview.h"


// Sets default values
ATargetManagerPreview::ATargetManagerPreview()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void ATargetManagerPreview::Init(const FBSConfig& InBSConfig, const FPlayerSettings_Game& InPlayerSettings)
{
	Super::Init(InBSConfig, InPlayerSettings);
}

void ATargetManagerPreview::InitTargetManagerPreview(const TObjectPtr<UBoxBoundsWidget> InTargetWidget, FBSConfig* InBSConfig)
{
	BoxBoundsWidget = InTargetWidget;
	BoxBoundsWidget->SetBoxBounds(FVector2d(BSConfig.TargetConfig.BoxBounds.Y, BSConfig.TargetConfig.BoxBounds.Z));
	ConfigPtr = InBSConfig;
}

void ATargetManagerPreview::SetShouldSpawn(const bool bShouldSpawn)
{
	Super::SetShouldSpawn(bShouldSpawn);
}

void ATargetManagerPreview::OnPlayerStopTrackingTarget()
{
	Super::OnPlayerStopTrackingTarget();
}

void ATargetManagerPreview::OnAudioAnalyzerBeat()
{
	Super::OnAudioAnalyzerBeat();
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
					TargetPreview->InitTargetWidget(TargetWidget, GetWidgetPositionFromWorldPosition(TargetPreview->GetActorLocation()));
				}
			}
		}
	}
	return Target;
}

bool ATargetManagerPreview::ActivateTarget(ATarget* InTarget) const
{
	return Super::ActivateTarget(InTarget);
}

void ATargetManagerPreview::HandleRuntimeSpawnAndActivation()
{
	Super::HandleRuntimeSpawnAndActivation();
}

int32 ATargetManagerPreview::GetNumberOfRuntimeTargetsToSpawn() const
{
	return Super::GetNumberOfRuntimeTargetsToSpawn();
}

int32 ATargetManagerPreview::GetNumberOfTargetsToActivate(const int32 MaxPossibleToActivate) const
{
	return Super::GetNumberOfTargetsToActivate(MaxPossibleToActivate);
}

void ATargetManagerPreview::HandleActivateExistingTargets()
{
	Super::HandleActivateExistingTargets();
}

void ATargetManagerPreview::HandlePermanentlyActiveTargetActivation() const
{
	Super::HandlePermanentlyActiveTargetActivation();
}

void ATargetManagerPreview::SpawnUpfrontOnlyTargets()
{
	Super::SpawnUpfrontOnlyTargets();
}

void ATargetManagerPreview::OnTargetHealthChangedOrExpired(const FTargetDamageEvent& TargetDamageEvent)
{
	Super::OnTargetHealthChangedOrExpired(TargetDamageEvent);
}

void ATargetManagerPreview::UpdateConsecutiveTargetsHit(const float TimeAlive)
{
	Super::UpdateConsecutiveTargetsHit(TimeAlive);
}

void ATargetManagerPreview::UpdateDynamicSpawnScale(const float TimeAlive)
{
	Super::UpdateDynamicSpawnScale(TimeAlive);
}

void ATargetManagerPreview::HandleTargetExpirationDelegate(const ETargetDamageType& DamageType, const FTargetDamageEvent& TargetDamageEvent) const
{
	Super::HandleTargetExpirationDelegate(DamageType, TargetDamageEvent);
}

void ATargetManagerPreview::HandleManagedTargetRemoval(const TArray<ETargetDestructionCondition>& TargetDestructionConditions, const FTargetDamageEvent& TargetDamageEvent)
{
	Super::HandleManagedTargetRemoval(TargetDestructionConditions, TargetDamageEvent);
}

void ATargetManagerPreview::FindNextTargetProperties()
{
	Super::FindNextTargetProperties();
}

FVector ATargetManagerPreview::GetNextTargetScale() const
{
	return Super::GetNextTargetScale();
}

USpawnArea* ATargetManagerPreview::GetNextSpawnArea(EBoundsScalingPolicy BoundsScalingPolicy, const FVector& NewTargetScale) const
{
	return Super::GetNextSpawnArea(BoundsScalingPolicy, NewTargetScale);
}

FVector ATargetManagerPreview::GetRandomMovingTargetEndLocation(const FVector& LocationBeforeChange, const float TargetSpeed, const bool bLastDirectionChangeHorizontal) const
{
	return Super::GetRandomMovingTargetEndLocation(LocationBeforeChange, TargetSpeed, bLastDirectionChangeHorizontal);
}

void ATargetManagerPreview::UpdateSpawnVolume() const
{
	Super::UpdateSpawnVolume();
	if (BoxBoundsWidget && ConfigPtr)
	{
		BoxBoundsWidget->SetBoxBounds(FVector2d(ConfigPtr->TargetConfig.BoxBounds.Y, ConfigPtr->TargetConfig.BoxBounds.Z));
	}
}

void ATargetManagerPreview::UpdateTotalPossibleDamage()
{
	Super::UpdateTotalPossibleDamage();
}

bool ATargetManagerPreview::TrackingTargetIsDamageable() const
{
	return Super::TrackingTargetIsDamageable();
}

ATarget* ATargetManagerPreview::FindManagedTargetByGuid(const FGuid Guid) const
{
	return Super::FindManagedTargetByGuid(Guid);
}

FVector ATargetManagerPreview::GetBoxExtents_Static() const
{
	return Super::GetBoxExtents_Static();
}

FVector ATargetManagerPreview::GetBoxOrigin() const
{
	return Super::GetBoxOrigin();
}

FExtrema ATargetManagerPreview::GetBoxExtrema(const bool bDynamic) const
{
	return Super::GetBoxExtrema(bDynamic);
}

FExtrema ATargetManagerPreview::GenerateBoxExtremaGrid() const
{
	return Super::GenerateBoxExtremaGrid();
}

int32 ATargetManagerPreview::AddToManagedTargets(ATarget* SpawnTarget)
{
	return Super::AddToManagedTargets(SpawnTarget);
}

void ATargetManagerPreview::RemoveFromManagedTargets(const FGuid GuidToRemove)
{
	Super::RemoveFromManagedTargets(GuidToRemove);
}

void ATargetManagerPreview::SetBoxExtents_Dynamic() const
{
	Super::SetBoxExtents_Dynamic();
}

void ATargetManagerPreview::UpdatePlayerSettings(const FPlayerSettings_Game& InPlayerSettings)
{
	Super::UpdatePlayerSettings(InPlayerSettings);
}

USpawnArea* ATargetManagerPreview::TryGetSpawnAreaFromReinforcementLearningComponent(const TArray<FVector>& OpenLocations) const
{
	return Super::TryGetSpawnAreaFromReinforcementLearningComponent(OpenLocations);
}

FVector2d ATargetManagerPreview::GetWidgetPositionFromWorldPosition(const FVector& InPosition) const
{
	const float X = InPosition.Y;
	const float Y = InPosition.Z - GetBoxOrigin().Z;
	return FVector2d(X, Y);
}

