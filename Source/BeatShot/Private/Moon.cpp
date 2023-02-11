// Fill out your copyright notice in the Description page of Project Settings.


#include "Moon.h"

#include "Components/SphereComponent.h"
#include "Components/DirectionalLightComponent.h"

// Sets default values
AMoon::AMoon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SphereComponent	= CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = SphereComponent;
	SphereComponent->SetMobility(EComponentMobility::Movable);
	SphereComponent->SetSphereRadius(SphereCompRadius);

	MoonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MoonMesh"));
	MoonMesh->SetupAttachment(SphereComponent);
	MoonMesh->SetRelativeLocation(MoonMeshOffset);
	MoonMesh->SetWorldScale3D(MoonMeshScale);
	MoonMesh->SetMobility(EComponentMobility::Movable);

	MoonGlowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MoonGlowMesh"));
	MoonGlowMesh->SetupAttachment(MoonMesh);
	MoonGlowMesh->SetWorldScale3D(MoonGlowMeshScale);
	MoonGlowMesh->SetMobility(EComponentMobility::Movable);

	MoonLight = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("MoonLight"));
	MoonLight->SetupAttachment(MoonMesh);
	MoonLight->SetWorldScale3D(MoonLightScale);
	MoonLight->SetMobility(EComponentMobility::Movable);
}

// Called when the game starts or when spawned
void AMoon::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMoon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

