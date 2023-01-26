// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleBeamLight.h"
#include "NiagaraComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/PointLightComponent.h"
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
	SpotlightLimb->SetRelativeLocation(FVector(0, 0, -18));
	
	SpotlightHead = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpotlightHead"));
	SpotlightHead->SetupAttachment(SpotlightLimb);
	SpotlightHead->SetRelativeLocation(FVector(0, 0, -41));
	SpotlightHead->SetRelativeRotation(FRotator(-90, 0, 0));

	Spotlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Spotlight"));
	Spotlight->SetupAttachment(SpotlightHead);
	Spotlight->SetRelativeLocation(FVector(22, 0, 0));

	BeamEndLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("BeamEndLight"));
	BeamEndLight->SetupAttachment(SpotlightHead);
	BeamEndLight->SetAutoActivate(false);

	SimpleBeamComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SimpleBeam"));
	SimpleBeamComponent->SetupAttachment(SpotlightHead);
	SimpleBeamComponent->OnSystemFinished.AddDynamic(this, &ASimpleBeamLight::OnNiagaraBeamFinished);
	SimpleBeamComponent->SetColorParameter(TEXT("User.BeamColor"), LightColor);
	SimpleBeamComponent->SetAutoActivate(false);
}

void ASimpleBeamLight::BeginPlay()
{
	Super::BeginPlay();

	LineTrace();
	
	EmissiveLightBulbMaterial = SpotlightHead->GetMaterial(1);
	EmissiveLightBulb = UMaterialInstanceDynamic::Create(EmissiveLightBulbMaterial, this);
	SpotlightHead->SetMaterial(1, EmissiveLightBulb);

	Spotlight->SetInnerConeAngle(InnerConeAngle);
	Spotlight->SetOuterConeAngle(OuterConeAngle);
	
	SetLightColor(FLinearColor(LightColor.R, LightColor.G, LightColor.B, 0));
	SetBeamEndLightColor(FLinearColor(LightColor.R, LightColor.G, LightColor.B, 0));

	ColorTimelineDelegate.BindUFunction(this, FName("SetLightColor"));
	ColorTimeline.AddInterpLinearColor(LightColorCurve, ColorTimelineDelegate);

	SimpleBeamComponent->SetColorParameter(TEXT("User.BeamColor"), LightColor);
}

void ASimpleBeamLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (ColorTimeline.IsPlaying())
	{
		ColorTimeline.TickTimeline(DeltaTime);
	}
}

void ASimpleBeamLight::UpdateNiagaraBeam(const float Alpha)
{
	if (SimpleBeamComponent)
	{
		SimpleBeamComponent->Activate();
		const float ClampedAlpha = UKismetMathLibrary::MapRangeClamped(Alpha, 0, 1, 0, MaxBeamWidth);
		SimpleBeamComponent->SetFloatParameter(TEXT("User.BeamWidth"), ClampedAlpha);
		ColorTimeline.PlayFromStart();
	}
}

void ASimpleBeamLight::OnNiagaraBeamFinished(UNiagaraComponent* NiagaraComponent)
{
	if (ColorTimeline.IsPlaying())
	{
		ColorTimeline.Stop();
	}
	SetLightColor(FLinearColor(LightColor.R, LightColor.G, LightColor.B, 0));
	NiagaraComponent->Deactivate();
}

void ASimpleBeamLight::SetLightColor(const FLinearColor Color)
{
	SetEmissiveBulbColor(Color);
	SetSpotlightColor(Color);
	if (BeamEndLight->IsActive())
	{
		SetBeamEndLightColor(Color);
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

void ASimpleBeamLight::LineTrace()
{
	const FVector StartLoc = SpotlightHead->GetComponentLocation();
	const FVector EndLoc = SpotlightHead->GetForwardVector() * FVector(TraceDistance);
	//DrawDebugLine(GetWorld(), StartLoc, EndLoc, FColor::Red, false, 10.f);
	if (FHitResult Hit; GetWorld()->LineTraceSingleByChannel(Hit, StartLoc, EndLoc, ECC_Camera,
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
			BeamEndLight->Activate();
			Spotlight->SetAttenuationRadius(Hit.Distance + 500);
		}
	}
	else
	{
		BeamEndLight->Deactivate();
	}
}
