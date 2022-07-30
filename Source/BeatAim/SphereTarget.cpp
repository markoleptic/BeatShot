// Fill out your copyright notice in the Description page of Project Settings.


#include "SphereTarget.h"
#include "TargetSpawner.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"

ASphereTarget::ASphereTarget()
{
	PrimaryActorTick.bCanEverTick = true;
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>("Capsule Collider");
	RootComponent = CapsuleComp;
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("Base Mesh");
	BaseMesh->SetupAttachment(CapsuleComp);
}

void ASphereTarget::BeginPlay()
{
	Super::BeginPlay();
}

void ASphereTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASphereTarget::HandleDestruction()
{
	Destroy();
}





