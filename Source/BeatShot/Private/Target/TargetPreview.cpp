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
	return bWasActivated;
}

void ATargetPreview::OnSimulatePlayerDestroyingTimerExpired()
{
	DamageSelf();
}

void ATargetPreview::OnIncomingDamageTaken(const FDamageEventData& InData)
{
	if (InData.NewValue > InData.OldValue)
	{
		return;
	}
	
	FTimerManager& TimerManager = GetWorldTimerManager();
	const float ElapsedTime = TimerManager.GetTimerElapsed(DamageableWindow);

	// Always damaging self in Preview Class, so use different comparison than base class
	const bool bExpired = !TimerManager.IsTimerActive(DamageableWindow);
	TimerManager.ClearTimer(DamageableWindow);
	const float TimeAlive = bExpired ? -1.f : ElapsedTime;
	const bool bOutOfHealth = InData.NewValue <= 0.f;
	
	// Replace self damage with game mode damage type
	FTargetDamageEvent Event(TimeAlive, GetActorTransform(), GetGuid(), Config.TargetDamageType);
	Event.SetDamageDeltaAndHealth(InData.OldValue, InData.NewValue);

	ColorWhenDestroyed = TargetColorChangeMaterial->K2_GetVectorParameterValue("BaseColor");

	HandleDeactivation(bExpired, bOutOfHealth);
	OnTargetDamageEventOrTimeout.Broadcast(Event);
	HandleDestruction(bExpired, bOutOfHealth);
	
	bCanBeReactivated = true;
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

void ATargetPreview::HandleDestruction(const bool bExpired, const bool bOutOfHealth)
{
	if (ShouldDestroy(bExpired, bOutOfHealth))
	{
		TargetWidget->RemoveFromParent();
	}
	Super::HandleDestruction(bExpired, bOutOfHealth);
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
