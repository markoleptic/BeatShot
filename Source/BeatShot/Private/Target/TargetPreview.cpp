// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Target/TargetPreview.h"
#include "WidgetComponents/TargetWidget.h"

ATargetPreview::ATargetPreview()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ATargetPreview::BeginPlay()
{
	Super::BeginPlay();
}

void ATargetPreview::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ATargetPreview::Init(const FBS_TargetConfig& InTargetConfig)
{
	Super::Init(InTargetConfig);
}

void ATargetPreview::InitTargetWidget(const TObjectPtr<UTargetWidget> InTargetWidget, const FVector2d& NewLocation)
{
	TargetWidget = InTargetWidget;
	SetTargetLocation(NewLocation);
}

void ATargetPreview::OnHealthChanged(AActor* ActorInstigator, const float OldValue, const float NewValue)
{
	Super::OnHealthChanged(ActorInstigator, OldValue, NewValue);
}

void ATargetPreview::OnTargetMaxLifeSpanExpired()
{
	Super::OnTargetMaxLifeSpanExpired();
}

bool ATargetPreview::ShouldDeactivate(const bool bExpired, const float CurrentHealth) const
{
	return Super::ShouldDeactivate(bExpired, CurrentHealth);
}

void ATargetPreview::HandleDeactivationResponses(const bool bExpired)
{
	Super::HandleDeactivationResponses(bExpired);
}

void ATargetPreview::HandleDestruction(const bool bExpired, const float CurrentHealth)
{
	if (ShouldDestroy(bExpired, CurrentHealth))
	{
		TargetWidget->RemoveFromParent();
	}
	Super::HandleDestruction(bExpired, CurrentHealth);
}

void ATargetPreview::SetTargetColor(const FLinearColor& Color)
{
	Super::SetTargetColor(Color);
	if (TargetWidget)
	{
		TargetWidget->SetTargetColor(Color);
	}
}

void ATargetPreview::SetTargetOutlineColor(const FLinearColor& Color)
{
	Super::SetTargetOutlineColor(Color);
	if (TargetWidget)
	{
		TargetWidget->SetTargetOutlineColor(Color);
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

void ATargetPreview::SetUseSeparateOutlineColor(const bool bUseSeparateOutlineColor)
{
	Super::SetUseSeparateOutlineColor(bUseSeparateOutlineColor);

	if (bUseSeparateOutlineColor)
	{
		TargetWidget->SetUseSeparateTargetOutlineColor(1.f);
		return;
	}
	TargetWidget->SetUseSeparateTargetOutlineColor(0.f);
}

void ATargetPreview::SetTargetLocation(const FVector2d& NewLocation) const
{
	TargetWidget->SetTargetPosition(NewLocation);
}

