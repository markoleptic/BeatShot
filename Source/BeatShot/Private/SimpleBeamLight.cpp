// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleBeamLight.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Components/TimelineComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
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
	//SimpleBeamComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SimpleBeamComp"));
	//SimpleBeamComp->SetupAttachment(Spots_SpotHead);
	//SimpleBeamComp->OnSystemFinished.AddDynamic(this, &ASimpleBeamLight::OnNiagaraBeamFinished);
	//SimpleBeamComp->SetAgeUpdateMode(ENiagaraAgeUpdateMode::DesiredAge);
}

// Called when the game starts or when spawned
void ASimpleBeamLight::BeginPlay()
{
	Super::BeginPlay();
	EmissiveLightBulbMaterial = Spots_SpotHead->GetMaterial(1);
	EmissiveLightBulb = UMaterialInstanceDynamic::Create(EmissiveLightBulbMaterial, this);
	Spots_SpotHead->SetMaterial(1, EmissiveLightBulb);
	EmissiveLightBulb->SetVectorParameterValue(TEXT("Color"), FLinearColor(LightColor).Desaturate(0));
	FOnTimelineLinearColor OnEmissiveLightTimeline;
	OnEmissiveLightTimeline.BindUFunction(this, FName("SetEmissiveLightColor"));
	EmissiveBulbTimeline.AddInterpLinearColor(EmissiveLightCurve, OnEmissiveLightTimeline);
}

// Called every frame
void ASimpleBeamLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (EmissiveBulbTimeline.IsPlaying())
	{
		EmissiveBulbTimeline.TickTimeline(DeltaTime);
	}
}

void ASimpleBeamLight::InitializeBeam()
{
	SimpleBeamComp = UNiagaraFunctionLibrary::SpawnSystemAttached(SimpleBeam, Spots_SpotHead, TEXT("BeamAttachPoint"),
	                                                              FVector::ZeroVector, FRotator::ZeroRotator,
	                                                              EAttachLocation::SnapToTarget, false);
	SimpleBeamComp->SetColorParameter(TEXT("User.BeamColor"), LightColor);
	SimpleBeamComp->OnSystemFinished.AddDynamic(this, &ASimpleBeamLight::OnNiagaraBeamFinished);
}

void ASimpleBeamLight::UpdateNiagaraBeam(float Value)
{
	if (SimpleBeamComp)
	{
		SimpleBeamComp->Activate();
		const float ClampedValue = UKismetMathLibrary::MapRangeClamped(Value , 0, 1, 0, 50);
		SimpleBeamComp->SetFloatParameter(TEXT("User.BeamWidth"), ClampedValue);
		if (EmissiveBulbTimeline.IsPlaying())
		{
			EmissiveBulbTimeline.Stop();
		}
		EmissiveBulbTimeline.PlayFromStart();
	}
	//SimpleBeamComp->SetDesiredAge(Value);
	/*SimpleBeamComp->SetFloatParameter(TEXT("User.Alpha"), Value);
	SimpleBeamComp->SetColorParameter(TEXT("User.BeamColor"), LightColor);*/
}

void ASimpleBeamLight::OnNiagaraBeamFinished(UNiagaraComponent* NiagaraComponent)
{
	NiagaraComponent->Deactivate();
	UE_LOG(LogTemp, Display, TEXT("Niagara System Finished"));
}

void ASimpleBeamLight::SetEmissiveLightColor(const FLinearColor Value)
{
	if (EmissiveLightBulb)
	{
		EmissiveLightBulb->SetScalarParameterValue(TEXT("Intensity"), Value.A * 8);
		EmissiveLightBulb->SetVectorParameterValue(TEXT("Color"), FLinearColor(LightColor) * Value);
	}
}
