// Fill out your copyright notice in the Description page of Project Settings.


#include "SphereTarget.h"
#include "DefaultGameInstance.h"
#include "GameModeActorBase.h"
#include "DefaultHealthComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "TargetSpawner.h"
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
	HealthComp = CreateDefaultSubobject<UDefaultHealthComponent>("Health Component");
	InitialLifeSpan = 1.5f;
}

void ASphereTarget::BeginPlay()
{
	Super::BeginPlay();
	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	GI->RegisterSphereTarget(this);

	// Use Color Changing Material, this is required in order to change color using C++
	Material = BaseMesh->GetMaterial(0);
	MID_TargetColorChanger = UMaterialInstanceDynamic::Create(Material, this);
	BaseMesh->SetMaterial(0, MID_TargetColorChanger);

	if (GI->GameModeActorStruct.IsBeatGridMode)
	{
		SetLifeSpan(0);
		SetMaxHealth(1000000);
		SetCanBeDamaged(false);
		MID_TargetColorChanger->SetVectorParameterValue(TEXT("StartColor"), BeatGridPurple);
	}
	else if (GI->GameModeActorStruct.IsBeatTrackMode)
	{
		SetLifeSpan(0);
		SetMaxHealth(1000000);
		MID_TargetColorChanger->SetVectorParameterValue(TEXT("StartColor"), FLinearColor::Red);
	}
	else
	{
		SetLifeSpan(GI->GameModeActorStruct.TargetMaxLifeSpan);
		GetWorldTimerManager().SetTimer(TimeSinceSpawn, GI->GameModeActorStruct.TargetMaxLifeSpan, false);
	}
}

void ASphereTarget::LifeSpanExpired()
{
	const FVector TopOfSphereLocation = { GetActorLocation().X,
		GetActorLocation().Y,
		GetActorLocation().Z + 
		BaseSphereRadius * GetActorScale3D().Z };
	OnLifeSpanExpired.Broadcast(true, TopOfSphereLocation);
	Super::LifeSpanExpired();
}

void ASphereTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASphereTarget::HandleDestruction()
{
	// Get the time that the sphere was alive for
	float TimeAlive;
	if (GetWorldTimerManager().GetTimerElapsed(TimeSinceSpawn) > 0) {
		TimeAlive = GetWorldTimerManager().GetTimerElapsed(TimeSinceSpawn);
	}
	else if (GetLifeSpan() > 0)
	{
		TimeAlive = GI->GameModeActorStruct.TargetMaxLifeSpan - GetLifeSpan();
	}
	// if TimeSinceSpawn or LifeSpan expired
	else
	{
		TimeAlive = -1;
	}
	// Destroy target and don't show explosion if timer expired
	if (TimeAlive < 0)
	{
		Destroy();
		return;
	}
	const FVector ExplosionLocation = BaseMesh->GetComponentLocation();
	const float SphereRadius = BaseSphereRadius * GetActorScale3D().X;
	const FLinearColor ColorWhenDestroyed = MID_TargetColorChanger->K2_GetVectorParameterValue(TEXT("StartColor"));
	// Beat Track shouldn't reach this
	if (GI->GameModeActorStruct.IsBeatTrackMode == true)
	{
		return;
	}
	const FVector TopOfSphereLocation = { GetActorLocation().X,
		GetActorLocation().Y,
		GetActorLocation().Z +
		BaseSphereRadius * GetActorScale3D().Z };
	OnLifeSpanExpired.Broadcast(false, TopOfSphereLocation);
	GI->GameModeActorBaseRef->UpdatePlayerScores(TimeAlive);
	GetWorldTimerManager().ClearTimer(TimeSinceSpawn);

	// Beat Grid specific behavior
	if (GI->GameModeActorStruct.IsBeatGridMode == true)
	{
		SetCanBeDamaged(false);
		RemoveAndReappear();
		GI->TargetSpawnerRef->SetShouldSpawn(true);
	}
	else
	{
		Destroy();
	}

	// Play Explosion effect
	if (NS_Standard_Explosion)
	{
		UNiagaraComponent* ExplosionComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			NS_Standard_Explosion,
			ExplosionLocation);
		ExplosionComp->SetNiagaraVariableFloat(FString("SphereRadius"), SphereRadius);
		ExplosionComp->SetColorParameter(FName("SphereColor"), ColorWhenDestroyed);
	}
}

void ASphereTarget::StartBeatGridTimer(float Lifespan)
{
	GetWorldTimerManager().SetTimer(TimeSinceSpawn, this, &ASphereTarget::OnBeatGridTimerTimeOut, Lifespan, false);
	SetCanBeDamaged(true);
	PlayColorGradient();
}

void ASphereTarget::OnBeatGridTimerTimeOut()
{
	SetCanBeDamaged(false);
	GetWorldTimerManager().ClearTimer(TimeSinceSpawn);
	if (GI->GameModeActorStruct.IsSingleBeatMode == true)
	{
		GI->TargetSpawnerRef->SetShouldSpawn(true);
	}
	const FVector TopOfSphereLocation = { GetActorLocation().X,
		GetActorLocation().Y,
		GetActorLocation().Z + 
		BaseSphereRadius * GetActorScale3D().Z };
	OnLifeSpanExpired.Broadcast(true, TopOfSphereLocation);
}

void ASphereTarget::SetMaxHealth(float NewMaxHealth)
{
	HealthComp->SetMaxHealth(NewMaxHealth);
}





