// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Equipment/BSEquipmentInstance.h"

#include "Character/BSCharacter.h"
#include "Equipment/BSEquipmentDefinition.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"

UBSEquipmentInstance::UBSEquipmentInstance()
{
}

UWorld* UBSEquipmentInstance::GetWorld() const
{
	if (const APawn* OwningPawn = GetPawn())
	{
		return OwningPawn->GetWorld();
	}
	return nullptr;
}

void UBSEquipmentInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Instigator);
	DOREPLIFETIME(ThisClass, SpawnedActors);
}

APawn* UBSEquipmentInstance::GetPawn() const
{
	return Cast<APawn>(GetOuter());
}

APawn* UBSEquipmentInstance::GetTypedPawn(TSubclassOf<APawn> PawnType) const
{
	APawn* Result = nullptr;
	if (UClass* ActualPawnType = PawnType)
	{
		if (GetOuter()->IsA(ActualPawnType))
		{
			Result = Cast<APawn>(GetOuter());
		}
	}
	return Result;
}

AActor* UBSEquipmentInstance::GetFirstSpawnedActor() const
{
	if (!SpawnedActors.IsEmpty())
	{
		return SpawnedActors[0];
	}
	return nullptr;
}

void UBSEquipmentInstance::SpawnEquipmentActors(const TArray<FBSEquipmentActorToSpawn>& ActorsToSpawn)
{
	if (APawn* OwningPawn = GetPawn())
	{
		USceneComponent* AttachTarget = OwningPawn->GetRootComponent();
		if (ABSCharacter* Char = Cast<ABSCharacter>(OwningPawn))
		{
			AttachTarget = Char->GetHandsMesh();
		}

		for (const FBSEquipmentActorToSpawn& SpawnInfo : ActorsToSpawn)
		{
			AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnInfo.ActorToSpawn, FTransform::Identity,
				OwningPawn);
			NewActor->FinishSpawning(FTransform::Identity, /*bIsDefaultTransform=*/ true);
			NewActor->SetActorRelativeTransform(SpawnInfo.AttachTransform);
			NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform,
				SpawnInfo.AttachSocket);
			SpawnedActors.Add(NewActor);
		}
	}
}

void UBSEquipmentInstance::DestroyEquipmentActors()
{
	for (AActor* Actor : SpawnedActors)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}
}

void UBSEquipmentInstance::OnEquipped()
{
	K2_OnEquipped();
}

void UBSEquipmentInstance::OnUnequipped()
{
	K2_OnUnequipped();
}

void UBSEquipmentInstance::OnRep_Instigator()
{
}
