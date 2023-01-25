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

	Spots_SpotBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spots_SpotBase"));
	RootComponent = Spots_SpotBase;
	
	Spots_SpotLimb = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spots_SpotLimb"));
	Spots_SpotLimb->SetupAttachment(Spots_SpotBase);
	Spots_SpotLimb->SetRelativeLocation(FVector(0, 0, -18));
	
	Spots_SpotHead = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spots_SpotHead"));
	Spots_SpotHead->SetupAttachment(Spots_SpotLimb);
	Spots_SpotHead->SetRelativeLocation(FVector(0, 0, -41));
	
	BeamStartSpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("BeamStartSpotLight"));
	BeamStartSpotLight->SetupAttachment(Spots_SpotHead);
	BeamStartSpotLight->SetRelativeLocation(FVector(22, 0, 0));
	
	BeamEndSpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("BeamEndSpotLight"));
	BeamEndSpotLight->SetupAttachment(Spots_SpotHead);
	
	BeamIllumination = CreateDefaultSubobject<UPointLightComponent>(TEXT("BeamIllumination"));
	BeamIllumination->SetupAttachment(Spots_SpotHead);
	
	SimpleBeamComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SimpleBeam"));
	SimpleBeamComponent->SetupAttachment(Spots_SpotHead);
	SimpleBeamComponent->OnSystemFinished.AddDynamic(this, &ASimpleBeamLight::OnNiagaraBeamFinished);
	SimpleBeamComponent->SetColorParameter(TEXT("User.BeamColor"), LightColor);
	SimpleBeamComponent->SetAutoActivate(false);
}

void ASimpleBeamLight::BeginPlay()
{
	Super::BeginPlay();
	LineTrace();

	BeamStartSpotLight->SetIntensity(0);
	BeamStartSpotLight->SetLightColor(LightColor);

	BeamEndSpotLight->SetIntensity(0);
	BeamEndSpotLight->SetLightColor(LightColor);

	EmissiveLightBulbMaterial = Spots_SpotHead->GetMaterial(1);
	EmissiveLightBulb = UMaterialInstanceDynamic::Create(EmissiveLightBulbMaterial, this);
	Spots_SpotHead->SetMaterial(1, EmissiveLightBulb);
	EmissiveLightBulb->SetVectorParameterValue(TEXT("Color"), LightColor);
	EmissiveLightBulb->SetScalarParameterValue(TEXT("Intensity"), 0);
	
	FOnTimelineLinearColor OnEmissiveLightTimeline;
	OnEmissiveLightTimeline.BindUFunction(this, FName("SetEmissiveLightColor"));
	EmissiveBulbTimeline.AddInterpLinearColor(EmissiveLightCurve, OnEmissiveLightTimeline);
	
	SimpleBeamComponent->SetColorParameter(TEXT("User.BeamColor"), LightColor);
}

void ASimpleBeamLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (EmissiveBulbTimeline.IsPlaying())
	{
		EmissiveBulbTimeline.TickTimeline(DeltaTime);
	}
}

void ASimpleBeamLight::UpdateNiagaraBeam(const float Value)
{
	if (SimpleBeamComponent)
	{
		SimpleBeamComponent->Activate();
		const float ClampedValue = UKismetMathLibrary::MapRangeClamped(Value, 0, 1, 0, 50);
		SimpleBeamComponent->SetFloatParameter(TEXT("User.BeamWidth"), ClampedValue);
		EmissiveBulbTimeline.PlayFromStart();
	}
}

void ASimpleBeamLight::OnNiagaraBeamFinished(UNiagaraComponent* NiagaraComponent)
{
	NiagaraComponent->Deactivate();
	if (EmissiveBulbTimeline.IsPlaying())
	{
		EmissiveBulbTimeline.Stop();
		SetEmissiveLightColor(FLinearColor(LightColor.R, LightColor.G, LightColor.B, 0));
		BeamStartSpotLight->SetIntensity(0);
		BeamEndSpotLight->SetIntensity(0);
	}
}

void ASimpleBeamLight::SetEmissiveLightColor(const FLinearColor Value)
{
	if (EmissiveLightBulb)
	{
		EmissiveLightBulb->SetScalarParameterValue(TEXT("Intensity"), Value.A * 8);
		BeamStartSpotLight->SetIntensity(Value.A * 16000000);
		BeamEndSpotLight->SetIntensity(Value.A * 80000);
	}
}

void ASimpleBeamLight::LineTrace()
{
	const FVector StartLoc = Spots_SpotHead->GetComponentLocation();
	const FVector EndLoc = Spots_SpotHead->GetForwardVector() * FVector(
		TraceDistance, TraceDistance, TraceDistance);
	DrawDebugLine(GetWorld(), StartLoc, EndLoc, FColor::Red, false, 10.f);
	if (FHitResult Hit; GetWorld()->LineTraceSingleByChannel(Hit, StartLoc, EndLoc, ECC_Camera,
															 FCollisionQueryParams::DefaultQueryParam))
	{
		const FVector HitLoc = Hit.Location;
		const FVector HitNormal = Hit.Normal;
		if (Hit.bBlockingHit)
		{
			BeamEndSpotLight->SetWorldLocation(HitLoc + Spots_SpotHead->GetComponentRotation().Vector() * 5);
			FRotator BeamEndRotation = (Spots_SpotHead->GetForwardVector() * FVector(
		TraceDistance, TraceDistance, TraceDistance) - HitNormal.Dot(Spots_SpotHead->GetForwardVector() * FVector(
		TraceDistance, TraceDistance, TraceDistance)) * 2 * HitNormal).Rotation();
			
			BeamEndSpotLight->SetWorldRotation(BeamEndRotation);
		}
	}
}
