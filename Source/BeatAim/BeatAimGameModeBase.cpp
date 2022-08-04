// Copyright Epic Games, Inc. All Rights Reserved.

#include "BeatAimGameModeBase.h"
#include "SphereTarget.h"

void ABeatAimGameModeBase::ActorDied(AActor* DeadActor)
{
	if (ASphereTarget* DestroyedTarget = Cast<ASphereTarget>(DeadActor))
	{
		DestroyedTarget->HandleDestruction();
	}
}
