// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Target/TargetPreview.h"


// Sets default values
ATargetPreview::ATargetPreview()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATargetPreview::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATargetPreview::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

