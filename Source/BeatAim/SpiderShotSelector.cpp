// Fill out your copyright notice in the Description page of Project Settings.


#include "SpiderShotSelector.h"
#include "Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ASpiderShotSelector::ASpiderShotSelector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>("Capsule Collider");
	RootComponent = CapsuleComp;
	TargetMesh = CreateDefaultSubobject<UStaticMeshComponent>("Target Mesh");
	TargetMesh->SetupAttachment(CapsuleComp);
}

// Called when the game starts or when spawned
void ASpiderShotSelector::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
//void ASpiderShotSelector::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

