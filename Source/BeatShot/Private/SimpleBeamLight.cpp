// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleBeamLight.h"
#include "NiagaraComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Components/TimelineComponent.h"
#include "Kismet/KismetMathLibrary.h"

ASimpleBeamLight::ASimpleBeamLight()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpotlightBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpotlightBase"));
	RootComponent = SpotlightBase;

	SpotlightLimb = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpotlightLimb"));
	SpotlightLimb->SetupAttachment(SpotlightBase);
	SpotlightLimb->SetRelativeLocation(DefaultSpotlightLimbOffset);
	
	SpotlightHead = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpotlightHead"));
	SpotlightHead->SetupAttachment(SpotlightLimb);
	SpotlightHead->SetRelativeLocation(DefaultSpotlightHeadOffset);
	SpotlightHead->SetRelativeRotation(DefaultSpotlightHeadRotation);

	Spotlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Spotlight"));
	Spotlight->SetupAttachment(SpotlightHead);
	Spotlight->SetRelativeLocation(DefaultSpotlightOffset);

	LightPositionComponent = CreateDefaultSubobject<USceneComponent>("LightPosition");
	LightPositionComponent->SetupAttachment(SpotlightBase);
	
	BeamEndLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("BeamEndLight"));
	BeamEndLight->SetupAttachment(LightPositionComponent);
	BeamEndLight->SetAutoActivate(false);

	SimpleBeamComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SimpleBeam"));
	SimpleBeamComponent->SetupAttachment(SpotlightHead);
	SimpleBeamComponent->OnSystemFinished.AddDynamic(this, &ASimpleBeamLight::OnNiagaraBeamFinished);
	SimpleBeamComponent->SetAutoActivate(false);
}

void ASimpleBeamLight::BeginPlay()
{
	Super::BeginPlay();

	LineTrace(SpotlightHead->GetForwardVector() * FVector(TraceDistance));
	
	EmissiveLightBulbMaterial = SpotlightHead->GetMaterial(1);
	EmissiveLightBulb = UMaterialInstanceDynamic::Create(EmissiveLightBulbMaterial, this);
	SpotlightHead->SetMaterial(1, EmissiveLightBulb);
	
	Spotlight->SetInnerConeAngle(InnerConeAngle);
	Spotlight->SetOuterConeAngle(OuterConeAngle);
	
	ColorTimelineDelegate.BindUFunction(this, FName("SetLightColor"));
	ColorTimeline.AddInterpLinearColor(LightColorCurve, ColorTimelineDelegate);
	TimelineVectorDelegate.BindUFunction(this, FName("UpdateBeamEndLightPosition"));
	LightPositionTimeline.AddInterpVector(LightMovementCurve, TimelineVectorDelegate);
}

void ASimpleBeamLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (ColorTimeline.IsPlaying())
	{
		ColorTimeline.TickTimeline(DeltaTime);
	}
	if (LightPositionTimeline.IsPlaying() || LightPositionTimeline.IsReversing())
	{
		LightPositionTimeline.TickTimeline(DeltaTime);
	}
}

void ASimpleBeamLight::UpdateNiagaraBeam(const float Alpha)
{
	if (ColorTimeline.IsPlaying() || LightPositionTimeline.IsPlaying() || LightPositionTimeline.IsReversing())
	{
		return;
	}
	if (SimpleBeamComponent)
	{
		SimpleBeamComponent->Activate();
		const float ClampedAlpha = UKismetMathLibrary::MapRangeClamped(Alpha, 0, 1, 0, MaxBeamWidth);
		SimpleBeamComponent->SetFloatParameter(TEXT("User.BeamWidth"), ClampedAlpha);
		ColorTimeline.PlayFromStart();
		if (Index % 2 == 0)
		{
			LightPositionTimeline.ReverseFromEnd();
			return;
		}
		LightPositionTimeline.PlayFromStart();
	}
}

void ASimpleBeamLight::SetColor(const FLinearColor Color)
{
	LightColor = Color;
	Spotlight->SetLightColor(FLinearColor(LightColor.R, LightColor.G, LightColor.B, 0));
	BeamEndLight->SetLightColor(FLinearColor(LightColor.R, LightColor.G, LightColor.B, 0));
	EmissiveLightBulb->SetVectorParameterValue(TEXT("Color"), LightColor);
	SimpleBeamComponent->SetColorParameter(TEXT("User.BeamColor"), LightColor);
}

void ASimpleBeamLight::OnNiagaraBeamFinished(UNiagaraComponent* NiagaraComponent)
{
	if (ColorTimeline.IsPlaying())
	{
		ColorTimeline.Stop();
	}
	if (LightPositionTimeline.IsPlaying())
	{
		LightPositionTimeline.Stop();
	}
	LightPositionComponent->SetRelativeLocation(FVector(0));
	SpotlightLimb->SetRelativeRotation(FRotator(0));
	SpotlightHead->SetRelativeRotation(DefaultSpotlightHeadRotation);
	SetLightColor(FLinearColor(LightColor.R, LightColor.G, LightColor.B, 0));
	NiagaraComponent->Deactivate();
}

void ASimpleBeamLight::SetLightColor(const FLinearColor Color)
{
	SetEmissiveBulbColor(FLinearColor(LightColor.R, LightColor.G, LightColor.B, Color.A));
	SetSpotlightColor(FLinearColor(LightColor.R, LightColor.G, LightColor.B, Color.A));
	if (BeamEndLight->IsActive())
	{
		SetBeamEndLightColor(FLinearColor(LightColor.R, LightColor.G, LightColor.B, Color.A));
	}
}

void ASimpleBeamLight::SetEmissiveBulbColor(const FLinearColor Color)
{
	if (EmissiveLightBulb)
	{
		EmissiveLightBulb->SetVectorParameterValue(TEXT("Color"), Color);
	}
}

void ASimpleBeamLight::SetSpotlightColor(const FLinearColor Color)
{
	if (Spotlight)
	{
		Spotlight->SetIntensity(Color.A * MaxSpotlightIntensity);
	}
}

void ASimpleBeamLight::SetBeamEndLightColor(const FLinearColor Color)
{
	if (BeamEndLight)
	{
		BeamEndLight->SetIntensity(Color.A * MaxBeamEndLightIntensity);
	}
}

void ASimpleBeamLight::LineTrace(const FVector EndLocation)
{
	const FVector StartLoc = SpotlightHead->GetComponentLocation();
	//DrawDebugLine(GetWorld(), StartLoc, EndLocation, FColor::Red, false, 0.5f);
	if (FHitResult Hit; GetWorld()->LineTraceSingleByChannel(Hit, StartLoc, EndLocation, ECC_Camera,
	                                                         FCollisionQueryParams::DefaultQueryParam))
	{
		const FVector HitLoc = Hit.Location;
		const FVector HitNormal = Hit.Normal;
		if (Hit.bBlockingHit)
		{
			BeamEndLight->SetWorldLocation(HitLoc + SpotlightHead->GetComponentRotation().Vector() * BeamEndLightOffset);
			FVector ForwardVector = SpotlightHead->GetForwardVector() * FVector(TraceDistance);
			double DotProduct = HitNormal.Dot(SpotlightHead->GetForwardVector() * FVector(TraceDistance));
			FRotator BeamEndRotation = (ForwardVector - DotProduct * 2 * HitNormal).Rotation();
			BeamEndLight->SetWorldRotation(BeamEndRotation);
			Spotlight->SetAttenuationRadius(Hit.Distance + 2000);
			SimpleBeamComponent->SetFloatParameter(TEXT("User.BeamLength"), Hit.Distance + 500);
			if (!BeamEndLight->IsActive())
			{
				BeamEndLight->Activate();
			}
		}
	}
	else
	{
		BeamEndLight->Deactivate();
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
	LineTrace(SpotlightHead->GetForwardVector() * FVector(TraceDistance));
}
