// Fill out your copyright notice in the Description page of Project Settings.


#include "SphereTarget.h"
#include "DefaultGameInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

ASphereTarget::ASphereTarget()
{
	PrimaryActorTick.bCanEverTick = true;
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>("Capsule Collider");
	RootComponent = CapsuleComp;
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("Base Mesh");
	BaseMesh->SetupAttachment(CapsuleComp);
	InitialLifeSpan = MaxLifeSpan;

	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
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





