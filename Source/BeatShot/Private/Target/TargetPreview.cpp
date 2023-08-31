// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Target/TargetPreview.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "WidgetComponents/TargetWidget.h"

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
	Super::Destroyed();
	if (TargetWidget)
	{
		TargetWidget->RemoveFromParent();
	}
}

void ATargetPreview::InitTargetWidget(const TObjectPtr<UTargetWidget> InTargetWidget, const FVector& InBoxBoundsOrigin, const FVector& InStartLocation)
{
	TargetWidget = InTargetWidget;
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
			GetWorldTimerManager().SetTimer(SimulatePlayerDestroyingTimer, this, &ThisClass::OnSimulatePlayerDestroyingTimerExpired, DestroyTime, false);
		}
	}
	return bWasActivated;
}

void ATargetPreview::OnSimulatePlayerDestroyingTimerExpired()
{
	DamageSelf(Config.ExpirationHealthPenalty);
}

void ATargetPreview::OnHealthChanged(AActor* ActorInstigator, const float OldValue, const float NewValue)
{
	FTimerManager& TimerManager = GetWorldTimerManager();
	float TimeAlive;
	if (TimerManager.IsTimerActive(DamageableWindow))
	{
		TimeAlive = TimerManager.GetTimerElapsed(DamageableWindow);
	}
	else
	{
		TimeAlive = -1.f;
	}
	TimerManager.ClearTimer(DamageableWindow);
	
	const FTargetDamageEvent TargetDamageEvent(TimeAlive, NewValue, GetActorTransform(), GetGuid(), abs(OldValue - NewValue));
	ColorWhenDestroyed = TargetColorChangeMaterial->K2_GetVectorParameterValue("BaseColor");
	HandleDeactivation(TimeAlive < 0.f, NewValue);
	OnTargetDamageEventOrTimeout.Broadcast(TargetDamageEvent);
	bCanBeReactivated = true;
	HandleDestruction(TimeAlive < 0.f, NewValue);
}

void ATargetPreview::OnTargetMaxLifeSpanExpired()
{
	GetWorldTimerManager().ClearTimer(SimulatePlayerDestroyingTimer);
	Super::OnTargetMaxLifeSpanExpired();
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

void ATargetPreview::HandleDestruction(const bool bExpired, const float CurrentHealth)
{
	if (ShouldDestroy(bExpired, CurrentHealth))
	{
		TargetWidget->RemoveFromParent();
	}
	Super::HandleDestruction(bExpired, CurrentHealth);
}

FVector2d ATargetPreview::GetWidgetPositionFromWorldPosition(const FVector& InPosition) const
{
	const float X = InPosition.Y;
	const float Y = InPosition.Z - (1000.f + Config.FloorDistance) / 2.f;
	return FVector2d(X, Y);
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
		TargetWidget->SetTargetPosition(GetWidgetPositionFromWorldPosition(NewLocation));
	}
}

