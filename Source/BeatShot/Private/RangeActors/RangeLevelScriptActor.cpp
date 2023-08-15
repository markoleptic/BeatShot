// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "RangeActors/RangeLevelScriptActor.h"
#include "Components/StaticMeshComponent.h"

ARangeLevelScriptActor::ARangeLevelScriptActor()
{
}

void ARangeLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();
	/*if (!HasAuthority())
	{
		return;
	}*/
}

void ARangeLevelScriptActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
