// Fill out your copyright notice in the Description page of Project Settings.


#include "SphereTarget.h"
#include "DefaultGameInstance.h"
#include "GameModeActorBase.h"
#include "DefaultHealthComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
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
	Guid = FGuid::NewGuid();
}

void ASphereTarget::BeginPlay()
{
	Super::BeginPlay();
	const UDefaultGameInstance* GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	GameModeActorStruct = GI->GameModeActorStruct;
	const float WhiteToGreenMultiplier = 1 / GameModeActorStruct.PlayerDelay;
	const float GreenToRedMultiplier = 1 / (GameModeActorStruct.TargetMaxLifeSpan - GameModeActorStruct.PlayerDelay);

	/* Use Color Changing Material, this is required in order to change color using C++ */
	Material = BaseMesh->GetMaterial(0);
	MID_TargetColorChanger = UMaterialInstanceDynamic::Create(Material, this);
	BaseMesh->SetMaterial(0, MID_TargetColorChanger);

	FOnTimelineLinearColor OnWhiteToGreenTimeline;
	OnWhiteToGreenTimeline.BindUFunction(this, FName("SetSphereColor"));
	WhiteToGreenTimeline.AddInterpLinearColor(WhiteToGreenCurve, OnWhiteToGreenTimeline);

	FOnTimelineEvent OnWhiteToGreenTimelineFinished;
	OnWhiteToGreenTimelineFinished.BindUFunction(this, FName("PlayGreenToRedTimeline"));
	WhiteToGreenTimeline.SetTimelineFinishedFunc(OnWhiteToGreenTimelineFinished);

	FOnTimelineLinearColor OnGreenToRedCurveTimeline;
	OnGreenToRedCurveTimeline.BindUFunction(this, FName("SetSphereColor"));
	GreenToRedTimeline.AddInterpLinearColor(GreenToRedCurve, OnGreenToRedCurveTimeline);

	FOnTimelineLinearColor OnFadeAndReappearTimeline;
	OnFadeAndReappearTimeline.BindUFunction(this, FName("SetSphereColor"));
	FadeAndReappearTimeline.AddInterpLinearColor(FadeAndReappearCurve, OnFadeAndReappearTimeline);

	WhiteToGreenTimeline.SetPlayRate(WhiteToGreenMultiplier);
	GreenToRedTimeline.SetPlayRate(GreenToRedMultiplier);
	FadeAndReappearTimeline.SetPlayRate(WhiteToGreenMultiplier);

	if (GameModeActorStruct.IsBeatGridMode)
	{
		SetLifeSpan(0);
		SetMaxHealth(1000000);
		SetCanBeDamaged(false);
		MID_TargetColorChanger->SetVectorParameterValue(TEXT("StartColor"), BeatGridPurple);
	}
	else if (GameModeActorStruct.IsBeatTrackMode)
	{
		SetLifeSpan(0);
		SetMaxHealth(1000000);
		MID_TargetColorChanger->SetVectorParameterValue(TEXT("StartColor"), FLinearColor::Red);
		HealthComp->ShouldUpdateTotalPossibleDamage = true;
	}
	else
	{
		SetLifeSpan(GameModeActorStruct.TargetMaxLifeSpan);
		PlayWhiteToGreenTimeline();
		GetWorldTimerManager().SetTimer(TimeSinceSpawn, GameModeActorStruct.TargetMaxLifeSpan, false);
	}
}

void ASphereTarget::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	switch (TimelineSwitch)
	{
	case 0:
		{
			WhiteToGreenTimeline.TickTimeline(DeltaSeconds);
			break;
		}
	case 1:
		{
			GreenToRedTimeline.TickTimeline(DeltaSeconds);
			break;
		}
	case 2:
		{
			FadeAndReappearTimeline.TickTimeline(DeltaSeconds);
			break;
		}
	default: break;
	}
}

void ASphereTarget::StartBeatGridTimer(const float Lifespan)
{
	GetWorldTimerManager().SetTimer(TimeSinceSpawn, this, &ASphereTarget::OnBeatGridTimerTimeOut, Lifespan, false);
	SetCanBeDamaged(true);
	PlayWhiteToGreenTimeline();
}

void ASphereTarget::PlayWhiteToGreenTimeline()
{
	if (FadeAndReappearTimeline.IsPlaying())
	{
		FadeAndReappearTimeline.Stop();
	}
	if (GreenToRedTimeline.IsPlaying())
	{
		GreenToRedTimeline.Stop();
	}
	TimelineSwitch = 0;
	WhiteToGreenTimeline.PlayFromStart();
}

void ASphereTarget::PlayGreenToRedTimeline()
{
	if (WhiteToGreenTimeline.IsPlaying())
	{
		WhiteToGreenTimeline.Stop();
	}
	if (FadeAndReappearTimeline.IsPlaying())
	{
		FadeAndReappearTimeline.Stop();
	}
	TimelineSwitch = 1;
	GreenToRedTimeline.PlayFromStart();
}

void ASphereTarget::PlayFadeAndReappearTimeline()
{
	if (WhiteToGreenTimeline.IsPlaying())
	{
		WhiteToGreenTimeline.Stop();
	}
	if (GreenToRedTimeline.IsPlaying())
	{
		GreenToRedTimeline.Stop();
	}
	TimelineSwitch = 2;
	FadeAndReappearTimeline.Play();
}

void ASphereTarget::SetSphereColor(const FLinearColor Output)
{
	MID_TargetColorChanger->SetVectorParameterValue(TEXT("StartColor"), Output);
}

void ASphereTarget::LifeSpanExpired()
{
	OnLifeSpanExpired.Broadcast(true, -1, this);
	Super::LifeSpanExpired();
}

void ASphereTarget::HandleDestruction()
{
	/* Beat Track shouldn't reach this */
	if (GameModeActorStruct.IsBeatTrackMode == true)
	{
		return;
	}

	/* Get the time that the sphere was alive for */
	const float TimeAlive = GetWorldTimerManager().GetTimerElapsed(TimeSinceSpawn);
	if (TimeAlive < 0 || TimeAlive >= GameModeActorStruct.TargetMaxLifeSpan)
	{
		Destroy();
		return;
	}

	/* Broadcast that the target has been destroyed by player */
	OnLifeSpanExpired.Broadcast(false, TimeAlive, this);
	GetWorldTimerManager().ClearTimer(TimeSinceSpawn);
	PlayExplosionEffect(BaseMesh->GetComponentLocation(), BaseSphereRadius * GetActorScale3D().X,
	                    MID_TargetColorChanger->K2_GetVectorParameterValue(TEXT("StartColor")));

	/* If BeatGrid mode, don't destroy target, make it not damageable, and play RemoveAndReappear blueprint event */
	if (GameModeActorStruct.IsBeatGridMode == true)
	{
		SetCanBeDamaged(false);
		PlayFadeAndReappearTimeline();
	}
	else
	{
		Destroy();
	}
}

void ASphereTarget::OnBeatGridTimerTimeOut()
{
	SetCanBeDamaged(false);
	MID_TargetColorChanger->SetVectorParameterValue(TEXT("StartColor"), BeatGridPurple);
	GetWorldTimerManager().ClearTimer(TimeSinceSpawn);
	OnLifeSpanExpired.Broadcast(true, -1, this);
}

void ASphereTarget::PlayExplosionEffect(const FVector ExplosionLocation, const float SphereRadius,
                                        const FLinearColor ColorWhenDestroyed) const
{
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

void ASphereTarget::SetMaxHealth(const float NewMaxHealth) const
{
	HealthComp->SetMaxHealth(NewMaxHealth);
}
