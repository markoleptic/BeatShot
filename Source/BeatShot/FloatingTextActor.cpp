// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingTextActor.h"

AFloatingTextActor::AFloatingTextActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AFloatingTextActor::BeginPlay()
{
	Super::BeginPlay();
	AnchorLocation = GetActorLocation();
}

void AFloatingTextActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

