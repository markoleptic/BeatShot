// Fill out your copyright notice in the Description page of Project Settings.


#include "SphereTarget.h"
#include "DefaultGameInstance.h"
#include "GameModeActorBase.h"
#include "HealthComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

ASphereTarget::ASphereTarget()
{
	PrimaryActorTick.bCanEverTick = true;
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>("Capsule Collider");
	RootComponent = CapsuleComp;
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("Base Mesh");
	BaseMesh->SetupAttachment(CapsuleComp);
	HealthComp = CreateDefaultSubobject<UHealthComponent>("Health Component");
	InitialLifeSpan = 1.5f;
}

void ASphereTarget::BeginPlay()
{
	Super::BeginPlay();
	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	SetLifeSpan(GI->GameModeActorStruct.TargetMaxLifeSpan);
	GetWorldTimerManager().SetTimer(TimeSinceSpawn, GI->GameModeActorStruct.TargetMaxLifeSpan, false);

	// Use Color Changing Material
	Material = BaseMesh->GetMaterial(0);
	MID_TargetColorChanger = UMaterialInstanceDynamic::Create(Material, this);
	BaseMesh->SetMaterial(0, MID_TargetColorChanger);
	if (GI->GameModeActorStruct.IsBeatTrackMode == true)
	{
		MID_TargetColorChanger->SetVectorParameterByIndex(0, FLinearColor::Red);
	}
}

void ASphereTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASphereTarget::HandleDestruction()
{

	float TimeAlive = GetWorldTimerManager().GetTimerElapsed(TimeSinceSpawn);
	FVector ExplosionLocation = BaseMesh->GetComponentLocation();
	FVector SphereScale = BaseMesh->GetComponentScale();
	FLinearColor ColorWhenDestroyed = MID_TargetColorChanger->K2_GetVectorParameterValue(TEXT("StartColor"));
	if (TimeAlive > 0.f && GI->GameModeActorBaseRef && (GI->GameModeActorStruct.IsBeatTrackMode == false))
	{
		GI->GameModeActorBaseRef->UpdatePlayerScores(TimeAlive);
		GetWorldTimerManager().ClearTimer(TimeSinceSpawn);
		Destroy();
	}
	else
	{
		Destroy();
	}
	if (NS_Standard_Explosion)
	{
		UNiagaraComponent* ExplosionComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS_Standard_Explosion, ExplosionLocation);
		ExplosionComp->SetNiagaraVariableFloat(FString("SphereRadius"), SphereScale.X);
		ExplosionComp->SetColorParameter(FName("SphereColor"), ColorWhenDestroyed);
	}
}

void ASphereTarget::SetMaxHealth(float NewMaxHealth)
{
	HealthComp->SetMaxHealth(NewMaxHealth);
}





