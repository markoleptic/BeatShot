// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Equipment/BSKnife.h"
#include "PhysicsEngine/RadialForceComponent.h"


ABSKnife::ABSKnife()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bAlwaysRelevant = true;
	bReplicates = true;

	Knife = CreateDefaultSubobject<UStaticMeshComponent>("Knife");
	SetRootComponent(Knife);

	//RadialForce = CreateDefaultSubobject<URadialForceComponent>("Radial Force Component");
	//RadialForce->SetupAttachment(Knife);
	//RadialForce->Radius = 250;
	//RadialForce->Falloff = ERadialImpulseFalloff::RIF_Linear;
	//RadialForce->ImpulseStrength = 500;
	//RadialForce->bImpulseVelChange = true;
	//RadialForce->bIgnoreOwningActor = true;
}

void ABSKnife::BeginPlay()
{
	Super::BeginPlay();
	//Knife->IgnoreActorWhenMoving(GetInstigator(), false);
}

void ABSKnife::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//RadialForce->ImpulseStrength = Knife->GetComponentVelocity().Length() / 5.f;
}

