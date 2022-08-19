// Fill out your copyright notice in the Description page of Project Settings.


#include "SphereTarget.h"
#include "DefaultGameInstance.h"
#include "GameModeActorBase.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

ASphereTarget::ASphereTarget()
{
	PrimaryActorTick.bCanEverTick = true;
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>("Capsule Collider");
	RootComponent = CapsuleComp;
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("Base Mesh");
	BaseMesh->SetupAttachment(CapsuleComp);
	InitialLifeSpan = MaxLifeSpan;
	//DynamicTargetColorMaterial = UMaterialInstanceDynamic::Create(BaseMesh->GetMaterial(0), nullptr);
}

void ASphereTarget::BeginPlay()
{
	Super::BeginPlay();
	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	GetWorldTimerManager().SetTimer(TimeSinceSpawn, MaxLifeSpan, false);
}

void ASphereTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//float blend = 0.5f + FMath::Cos(GetWorld()->TimeSeconds) / 2;
	//DynamicTargetColorMaterial->SetScalarParameterValue(TEXT("Blend"), blend);
}

void ASphereTarget::HandleDestruction()
{
	float TimeAlive = GetWorldTimerManager().GetTimerElapsed(TimeSinceSpawn);
	if (TimeAlive > 0.f && GI->GameModeActorBaseRef)
	{
		GI->GameModeActorBaseRef->UpdateScore(TimeAlive);
		GetWorldTimerManager().ClearTimer(TimeSinceSpawn);
		Destroy();
	}
}





