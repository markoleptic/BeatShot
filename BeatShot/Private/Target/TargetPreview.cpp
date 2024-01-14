// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Target/TargetPreview.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "WidgetComponents/GameModePreviewWidgets/TargetWidget.h"

ATargetPreview::ATargetPreview()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ATargetPreview::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (ProjectileMovementComponent->IsActive())
	{
		if (ProjectileMovementComponent->Velocity.Length() > 0.f)
		{
			SetTargetWidgetLocation(GetActorLocation());
		}
	}
}

void ATargetPreview::Init(const FBS_TargetConfig& InTargetConfig)
{
	Super::Init(InTargetConfig);
}

void ATargetPreview::Destroyed()
{
	if (TargetWidget)
	{
		TargetWidget->RemoveFromParent();
	}
	Super::Destroyed();
}

void ATargetPreview::InitTargetWidget(const TObjectPtr<UTargetWidget> InTargetWidget, const FVector& InBoxBoundsOrigin,
	const FVector& InStartLocation, const float BoundsHeight)
{
	TargetWidget = InTargetWidget;
	BoxBoundsHeight = BoundsHeight;
	BoxBoundsOrigin = InBoxBoundsOrigin;
	SetTargetWidgetLocation(InStartLocation);
	TargetWidget->SetTargetScale(CapsuleComponent->GetRelativeScale3D());
}

void ATargetPreview::SetSimulatePlayerDestroying(const bool bInSimulatePlayerDestroying, const float InDestroyChance)
{
	bSimulatePlayerDestroying = bInSimulatePlayerDestroying;
	DestroyChance = InDestroyChance;
}

void ATargetPreview::SetActorHiddenInGame(bool bNewHidden)
{
	Super::SetActorHiddenInGame(bNewHidden);

	if (TargetWidget)
	{
		if (bNewHidden)
		{
			TargetWidget->SetRenderOpacity(0.2f);
		}
		else
		{
			TargetWidget->SetRenderOpacity(1.f);
		}
	}
}

bool ATargetPreview::ActivateTarget(const float Lifespan)
{
	const bool bWasActivated = Super::ActivateTarget(Lifespan);
	if (bSimulatePlayerDestroying && bWasActivated && Lifespan > 0)
	{
		if (DestroyChance > FMath::FRandRange(0.f, 1.f))
		{
			const float DestroyTime = FMath::FRandRange(Lifespan * 0.25f, Lifespan * 0.75f);
			GetWorldTimerManager().ClearTimer(SimulatePlayerDestroyingTimer);
			GetWorldTimerManager().SetTimer(SimulatePlayerDestroyingTimer, this,
				&ThisClass::OnSimulatePlayerDestroyingTimerExpired, DestroyTime, false);
		}
	}
	else if (bSimulatePlayerDestroying && bWasActivated)
	{
		if (DestroyChance > FMath::FRandRange(0.f, 1.f))
		{
			const float DestroyTime = FMath::FRandRange(0.5f, 1.f);
			GetWorldTimerManager().ClearTimer(SimulatePlayerDestroyingTimer);
			GetWorldTimerManager().SetTimer(SimulatePlayerDestroyingTimer, this,
				&ThisClass::OnSimulatePlayerDestroyingTimerExpired, DestroyTime, false);
		}
	}
	return bWasActivated;
}

void ATargetPreview::OnSimulatePlayerDestroyingTimerExpired()
{
	DamageSelf(true);
}

void ATargetPreview::OnIncomingDamageTaken(const FDamageEventData& InData)
{
	if (InData.NewValue > InData.OldValue)
	{
		return;
	}
	
	FTimerManager& TimerManager = GetWorldTimerManager();
	const float ElapsedTime = TimerManager.GetTimerElapsed(ExpirationTimer);
	TimerManager.ClearTimer(ExpirationTimer);
	
	// Replace self damage with game mode damage type
	FTargetDamageEvent Event(InData, ElapsedTime, this);

	const bool bDeactivate = ShouldDeactivate(Event.bDamagedSelf, Event.CurrentHealth);
	const bool bDestroy = ShouldDestroy(Event.bDamagedSelf, Event.bOutOfHealth);
	
	TArray<ETargetDamageType> VulnerableDamageTypes;
	VulnerableDamageTypes.Add(ETargetDamageType::Self);
	VulnerableDamageTypes.Add(GetTargetDamageType());
	Event.SetTargetData(bDeactivate, bDestroy, VulnerableDamageTypes);
	OnTargetDamageEvent.Broadcast(Event);
	
	ColorWhenDamageTaken = TargetColorChangeMaterial->K2_GetVectorParameterValue("BaseColor");
	if (bDeactivate) HandleDeactivation(Event.bDamagedSelf, Event.bOutOfHealth, bDestroy);
	
	if (bDestroy)
	{
		Destroy();
	}
	else
	{
		CheckForHealthReset(Event.bOutOfHealth);
		bCanBeReactivated = true;
	}
}

void ATargetPreview::OnLifeSpanExpired()
{
	GetWorldTimerManager().ClearTimer(SimulatePlayerDestroyingTimer);
	Super::OnLifeSpanExpired();
}

void ATargetPreview::HandleDeactivationResponses(const bool bExpired)
{
	Super::HandleDeactivationResponses(bExpired);

	// Hide target
	if (Config.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::HideTarget))
	{
		if (TargetWidget)
		{
			TargetWidget->SetRenderOpacity(0.2f);
		}
	}
}

void ATargetPreview::SetTargetColor(const FLinearColor& Color)
{
	Super::SetTargetColor(Color);
	if (TargetWidget)
	{
		TargetWidget->SetTargetColor(Color);
	}
}

void ATargetPreview::SetTargetScale(const FVector& NewScale) const
{
	Super::SetTargetScale(NewScale);
	if (TargetWidget)
	{
		TargetWidget->SetTargetScale(NewScale);
	}
}

void ATargetPreview::SetTargetWidgetLocation(const FVector& NewLocation) const
{
	if (TargetWidget)
	{
		const float X = NewLocation.Y;
		const float Y = NewLocation.Z - BoxBoundsHeight;
		TargetWidget->SetTargetPosition(FVector2d(X, Y));
	}
}
