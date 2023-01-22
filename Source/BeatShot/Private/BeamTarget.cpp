// Fill out your copyright notice in the Description page of Project Settings.


#include "BeamTarget.h"
#include "BeamLight.h"

// Sets default values
ABeamTarget::ABeamTarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABeamTarget::BeginPlay()
{
	Super::BeginPlay();
	UpdateBeamLightRefs(nullptr);
}

// Called every frame
void ABeamTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABeamTarget::UpdateBeamLightRefs(ABeamLight* BeamLightRef)
{
	if (IsValid(BeamLightRef))
	{
		BeamLightRefs.Emplace(BeamLightRef);
	}
	for (ABeamLight* BeamLight : BeamLightRefs)
	{
		if (IsValid(BeamLight))
		{
			BeamLight->UpdateRotation();
		}
		else
		{
			BeamLightRefs.Remove(BeamLight);
		}
	}
}

