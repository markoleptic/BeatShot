// Fill out your copyright notice in the Description page of Project Settings.


#include "BeamLight.h"

#include "NiagaraComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/PointLightComponent.h"
//#include "NiagaraSystem.h"
//#include "NiagaraFunctionLibrary.h"
//#include "NiagaraComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABeamLight::ABeamLight()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	Spots_SpotBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spots_SpotBase"));
	if (BaseMesh)
	{
		Spots_SpotBase->SetStaticMesh(BaseMesh);
	}
	RootComponent = Spots_SpotBase;
	Spots_SpotLimb = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spots_SpotLimb"));
	if (LimbMesh)
	{
		Spots_SpotLimb->SetStaticMesh(LimbMesh);
	}
	Spots_SpotLimb->SetupAttachment(Spots_SpotBase);
	Spots_SpotLimb->SetRelativeLocation(FVector(0, 0, -18));
	Spots_SpotHead = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spots_SpotHead"));
	if (HeadMesh)
	{
		Spots_SpotHead->SetStaticMesh(HeadMesh);
	}
	Spots_SpotHead->SetupAttachment(Spots_SpotLimb);
	Spots_SpotHead->SetRelativeLocation(FVector(0, 0, -41));
	SpotLight = CreateDefaultSubobject<USpotLightComponent>("SpotLight");
	SpotLight->SetupAttachment(Spots_SpotHead);
	NiagaraCone = CreateDefaultSubobject<UNiagaraComponent>("NiagaraCone");
	NiagaraCone->SetupAttachment(Spots_SpotHead);
	LightShaftIllumination = CreateDefaultSubobject<UPointLightComponent>("LightShaftIllumination");
	LightShaftIllumination->SetupAttachment(Spots_SpotHead);
	FakeGISpotlight = CreateDefaultSubobject<USpotLightComponent>("FakeGISpotlight");
	FakeGISpotlight->SetupAttachment(Spots_SpotHead);
	
	LightAttenuation = 3000;
	LightIntensity = 1;
	LightIntensityMax = 2;
	ConeOuter = 2;
	ConeInner = 2;
	LightColor = FLinearColor::White;
	LightMultiplier = FLinearColor(3, 3, 3, 1);
	ConeLengthRatio = 0.5;
	OcclusionMaskDistance = 180;
	LightEmitterLength = 300;
	CustomAngle = 50;
	bCustomAngle = true;

	UpdateNiagaraConeProps();
}

void ABeamLight::InitBeamTarget(ABeamTarget* NewBeamTarget)
{
	BeamTarget = NewBeamTarget;
}

void ABeamLight::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABeamLight, LightIntensity);
}

// Called when the game starts or when spawned
void ABeamLight::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABeamLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABeamLight::UpdateNiagaraConeProps()
{
	NiagaraCone->SetFloatParameter(TEXT("User.Length"), LightAttenuation * ConeLengthRatio);
	NiagaraCone->SetColorParameter(TEXT("User.Color"), LightMultiplier * LightColor);
	if (AngleCurve)
	{
		
		const float ClampedValue = UKismetMathLibrary::MapRangeClamped(ConeOuter, 0, 45, 0, 1);
		const float ValueFromCurve = AngleCurve->GetFloatValue(ClampedValue);
		const float AdjustedValue = ValueFromCurve * ConeOuter * (LightAttenuation / 666);
		const float SelectedFloat = UKismetMathLibrary::SelectFloat(CustomAngle, AdjustedValue, bCustomAngle);
		NiagaraCone->SetFloatParameter(TEXT("User.Angle"), SelectedFloat);
	}
	const float LightIntensityClamped = UKismetMathLibrary::MapRangeClamped(LightIntensity, 0, LightIntensityMax, 0, 1.5);
	const float MatAngleClamped = UKismetMathLibrary::MapRangeClamped(ConeOuter, 2, 15, 0, 1);
	const float BeamAlphaClamped = UKismetMathLibrary::MapRangeClamped(LightIntensity, 0, LightIntensityMax, 0, 1);
	const FVector MatMaskEnd = SpotLight->GetComponentLocation() - UKismetMathLibrary::NegateVector(SpotLight->GetForwardVector()) * OcclusionMaskDistance;
	
	NiagaraCone->SetFloatParameter(TEXT("User.LightIntensity"), LightIntensityClamped);
	NiagaraCone->SetFloatParameter(TEXT("User.MaterialAngle"), MatAngleClamped);
	NiagaraCone->SetFloatParameter(TEXT("User.BeamAlpha"), BeamAlphaClamped);
	NiagaraCone->SetNiagaraVariableVec3(TEXT("User.ConeDir"), UKismetMathLibrary::NegateVector(SpotLight->GetForwardVector()));
	NiagaraCone->SetNiagaraVariableVec3(TEXT("User.ConeLoc"), SpotLight->GetComponentLocation());
	NiagaraCone->SetNiagaraVariableVec3(TEXT("User.MaterialMaskEnd") , MatMaskEnd);
	NiagaraCone->SetFloatParameter(TEXT("User.MaterialMaskWidth"), OcclusionMaskDistance / 1.5);
	NiagaraCone->SetFloatParameter(TEXT("User.LightsLength"), LightEmitterLength - 15);
}
