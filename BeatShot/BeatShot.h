// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define ACTOR_ROLE_FSTRING *(FindObject<UEnum>(nullptr, TEXT("/Script/Engine.ENetRole"), true)->GetNameStringByValue(GetLocalRole()))
#define GET_ACTOR_ROLE_FSTRING(Actor) *(FindObject<UEnum>(nullptr, TEXT("/Script/Engine.ENetRole"), true)->GetNameStringByValue(Actor->GetLocalRole()))

class FBeatShot : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual bool IsGameModule() const override { return true; }
};
