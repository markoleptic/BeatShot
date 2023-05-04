// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Visualizers/SimpleBeamLight.h"

#include "GlobalConstants.h"
#include "NiagaraComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Components/TimelineComponent.h"

ASimpleBeamLight::ASimpleBeamLight()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpotlightBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpotlightBase"));
	RootComponent = SpotlightBase;

	SpotlightLimb = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpotlightLimb"));
	SpotlightLimb->SetupAttachment(SpotlightBase);
	SpotlightLimb->SetRelativeLocation(Constants::DefaultSpotlightLimbOffset);

	SpotlightHead = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpotlightHead"));
	SpotlightHead->SetupAttachment(SpotlightLimb);
	SpotlightHead->SetRelativeLocation(Constants::DefaultSpotlightHeadOffset);
	SpotlightHead->SetRelativeRotation(Constants::DefaultSpotlightHeadRotation);

	Spotlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Spotlight"));
	Spotlight->SetupAttachment(SpotlightHead);
	Spotlight->SetRelativeLocation(Constants::DefaultSpotlightOffset);
	Spotlight->SetAutoActivate(false);
	Spotlight->SetInnerConeAngle(SimpleBeamLightConfig.InnerConeAngle);
	Spotlight->SetOuterConeAngle(SimpleBeamLightConfig.OuterConeAngle);

	LightPositionComponent = CreateDefaultSubobject<USceneComponent>("LightPosition");
	LightPositionComponent->SetupAttachment(SpotlightBase);

	BeamEndLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("BeamEndLight"));
	BeamEndLight->SetupAttachment(LightPositionComponent);
	BeamEndLight->SetAutoActivate(false);

	SimpleBeamComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SimpleBeam"));
	SimpleBeamComponent->SetupAttachment(SpotlightHead);
	SimpleBeamComponent->SetAutoActivate(false);
}

void ASimpleBeamLight::BeginPlay()
{
	Super::BeginPlay();

	SimpleBeamComponent->OnSystemFinished.AddDynamic(this, &ASimpleBeamLight::OnNiagaraBeamFinished);
}

void ASimpleBeamLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!SimpleBeamLightConfig.bIsMovingLight)
	{
		return;
	}
	if (LightPositionTimeline.IsPlaying() || LightPositionTimeline.IsReversing())
	{
		LightPositionTimeline.TickTimeline(DeltaTime);
	}
}

void ASimpleBeamLight::InitSimpleBeamLight(const FSimpleBeamLightConfig& InConfig)
{
	Spotlight->SetIntensity(0.f);
	BeamEndLight->SetIntensity(0.f);

	SimpleBeamLightConfig = InConfig;
	
	if (SimpleBeamComponent)
	{
		SimpleBeamComponent->SetColorParameter(TEXT("User.BeamColor"), SimpleBeamLightConfig.LightColor);
		SimpleBeamComponent->SetFloatParameter(TEXT("User.BeamWidth"), SimpleBeamLightConfig.BeamWidth);
		SimpleBeamComponent->SetFloatParameter(TEXT("User.BeamLength"), SimpleBeamLightConfig.BeamLength);
		SimpleBeamComponent->SetFloatParameter(TEXT("User.Lifetime"), SimpleBeamLightConfig.LightDuration);
	}
	Spotlight->SetLightColor(SimpleBeamLightConfig.LightColor);
	BeamEndLight->SetLightColor(SimpleBeamLightConfig.LightColor);

	EmissiveLightBulb = UMaterialInstanceDynamic::Create(SpotlightHead->GetMaterial(1), this);
	SpotlightHead->SetMaterial(1, EmissiveLightBulb);
	if (EmissiveLightBulb)
	{
		EmissiveLightBulb->SetScalarParameterValue(TEXT("Intensity"), 0.f);
		EmissiveLightBulb->SetVectorParameterValue(TEXT("Color"), SimpleBeamLightConfig.LightColor);
	}

	if (SimpleBeamLightConfig.bAutoCalculateBeamLength)
	{
		LineTrace(SpotlightHead->GetForwardVector() * FVector(Constants::TraceDistance));
	}

	if (SimpleBeamLightConfig.bIsMovingLight)
	{
		TimelineVectorDelegate.BindUFunction(this, FName("UpdateBeamEndLightPosition"));
		LightPositionTimeline.AddInterpVector(LightMovementCurve, TimelineVectorDelegate);
	}
}

void ASimpleBeamLight::ActivateLightComponents()
{
	if (LightPositionTimeline.IsPlaying() || LightPositionTimeline.IsReversing())
	{
		return;
	}
	
	if (SimpleBeamComponent)
	{
		SimpleBeamComponent->Activate();
	}
	if (SimpleBeamLightConfig.bUseSpotlight)
	{
		Spotlight->Activate();
	}
	if (SimpleBeamLightConfig.bUseBeamEndLight)
	{
		BeamEndLight->Activate();
	}

	if (SimpleBeamLightConfig.bIsMovingLight)
	{
		LightPositionTimeline.SetPlayRate(1.f / SimpleBeamLightConfig.LightDuration);
		if (SimpleBeamLightConfig.Index % 2 == 0)
		{
			LightPositionTimeline.ReverseFromEnd();
		}
		else
		{
			LightPositionTimeline.PlayFromStart();
		}
	}
}

void ASimpleBeamLight::DeactivateLightComponents()
{
	if (LightPositionTimeline.IsPlaying() || LightPositionTimeline.IsReversing())
	{
		LightPositionTimeline.Stop();
	}
	
	if (EmissiveLightBulb)
	{
		EmissiveLightBulb->SetScalarParameterValue(TEXT("Intensity"), 0.f);
	}

	if (SimpleBeamLightConfig.bIsMovingLight)
	{
		LightPositionComponent->SetRelativeLocation(FVector(0));
	}
	
	SpotlightLimb->SetRelativeRotation(FRotator(0));
	SpotlightHead->SetRelativeRotation(Constants::DefaultSpotlightHeadRotation);
	
	if (SimpleBeamComponent)
	{
		SimpleBeamComponent->Deactivate();
	}
	if (Spotlight->IsActive())
	{
		Spotlight->SetIntensity(0.f);
		Spotlight->Deactivate();
	}
	if (BeamEndLight->IsActive())
	{
		BeamEndLight->SetIntensity(0.f);
		BeamEndLight->Deactivate();
	}
}

void ASimpleBeamLight::OnNiagaraBeamFinished(UNiagaraComponent* NiagaraComponent)
{
	DeactivateLightComponents();
	OnBeamLightLifetimeCompleted.Broadcast(SimpleBeamLightConfig.Index);
}

void ASimpleBeamLight::LineTrace(const FVector& EndLocation)
{
	const FVector StartLoc = SpotlightHead->GetComponentLocation();
	//DrawDebugLine(GetWorld(), StartLoc, EndLocation, FColor::Red, false, 0.5f);
	if (FHitResult Hit; GetWorld()->LineTraceSingleByChannel(Hit, StartLoc, EndLocation, ECC_Camera, FCollisionQueryParams::DefaultQueryParam))
	{
		const FVector HitLoc = Hit.Location;
		const FVector HitNormal = Hit.Normal;
		if (Hit.bBlockingHit)
		{
			BeamEndLight->SetWorldLocation(HitLoc + SpotlightHead->GetComponentRotation().Vector() * Constants::DefaultBeamEndLightOffset);
			FVector ForwardVector = SpotlightHead->GetForwardVector() * FVector(Constants::TraceDistance);
			double DotProduct = HitNormal.Dot(SpotlightHead->GetForwardVector() * FVector(Constants::TraceDistance));
			FRotator BeamEndRotation = (ForwardVector - DotProduct * 2 * HitNormal).Rotation();
			BeamEndLight->SetWorldRotation(BeamEndRotation);
			if (SimpleBeamLightConfig.bUseSpotlight)
			{
				Spotlight->SetAttenuationRadius(Hit.Distance + 2000);
			}
			SimpleBeamComponent->SetFloatParameter(TEXT("User.BeamLength"), Hit.Distance + 500);
		}
	}
}

void ASimpleBeamLight::UpdateBeamEndLightPosition(const FVector& Position)
{
	LightPositionComponent->SetRelativeLocation(Position);
	const FVector StartLoc = SpotlightHead->GetComponentLocation();
	const FRotator NewLimbHeadRot = (BeamEndLight->GetComponentLocation() + Position - StartLoc).Rotation();
	GimbalRotation = NewLimbHeadRot.Yaw;
	SpotlightHeadRotation = NewLimbHeadRot.Pitch;
	SpotlightLimb->SetRelativeRotation(FRotator(0, GimbalRotation, 0));
	SpotlightHead->SetRelativeRotation(FRotator(SpotlightHeadRotation, 0, 0));
	LineTrace(SpotlightHead->GetForwardVector() * FVector(Constants::TraceDistance));

	if (LightPositionTimeline.IsReversing())
	{
		EmissiveLightBulb->SetScalarParameterValue(TEXT("Intensity"), LightPositionTimeline.GetPlaybackPosition());
		BeamEndLight->SetIntensity(LightPositionTimeline.GetPlaybackPosition() * SimpleBeamLightConfig.MaxBeamEndLightIntensity);
		Spotlight->SetIntensity(LightPositionTimeline.GetPlaybackPosition() * SimpleBeamLightConfig.MaxSpotlightIntensity);
	}
	else
	{
		EmissiveLightBulb->SetScalarParameterValue(TEXT("Intensity"), 1 - LightPositionTimeline.GetPlaybackPosition());
		BeamEndLight->SetIntensity((1 - LightPositionTimeline.GetPlaybackPosition()) * SimpleBeamLightConfig.MaxBeamEndLightIntensity);
		Spotlight->SetIntensity((1 - LightPositionTimeline.GetPlaybackPosition()) * SimpleBeamLightConfig.MaxSpotlightIntensity);
	}
}

void ASimpleBeamLight::UpdateBeamEndLightLocation()
{
	LineTrace(SpotlightHead->GetForwardVector() * FVector(Constants::TraceDistance));
}
