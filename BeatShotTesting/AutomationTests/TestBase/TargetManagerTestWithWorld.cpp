// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "TargetManagerTestWithWorld.h"
#include "Target/TargetManager.h"

bool FTargetManagerTestWithWorld::Init()
{
	World = UWorld::CreateWorld(EWorldType::Game, false);
	GameInstance = NewObject<UGameInstance>();
	GameInstance->AddToRoot();
	World->SetGameInstance(GameInstance);
	
	FWorldContext &WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
	WorldContext.SetCurrentWorld(World);
	
	GameInstance->Init();
	World->InitializeActorsForPlay(FURL());
	World->BeginPlay();
	InitialFrameCounter = GFrameCounter;
	
	return Super::Init();
}

bool FTargetManagerTestWithWorld::InitTargetManager()
{
	UObject* TargetManagerObj = StaticLoadObject(UObject::StaticClass(), nullptr,
		TargetManagerTestHelpers::TargetManagerAssetPath);
	if (!TargetManagerObj)
	{
		AddError("Failed to load Target Manager");
		return false;
	}
		
	const UBlueprint* GeneratedBP = Cast<UBlueprint>(TargetManagerObj);
	
	const FActorSpawnParameters SpawnInfo = FActorSpawnParameters();
	ATargetManager* Actor = World->SpawnActor<ATargetManager>(GeneratedBP->GeneratedClass,
		DefaultTargetManagerLocation, FRotator::ZeroRotator, SpawnInfo);
	Actor->AddToRoot();
	Actor->DispatchBeginPlay();
	TargetManager = MakeShareable(Actor);

	if (!TargetManager.IsValid())
	{
		AddError("Failed to spawn Target Manager");
		return false;
	}
	return true;
}
