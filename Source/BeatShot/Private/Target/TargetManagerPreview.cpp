// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Target/TargetManagerPreview.h"


// Sets default values
ATargetManagerPreview::ATargetManagerPreview()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATargetManagerPreview::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATargetManagerPreview::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

