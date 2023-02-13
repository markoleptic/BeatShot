// Fill out your copyright notice in the Description page of Project Settings.


#include "SphereTarget.h"
#include "DefaultGameInstance.h"
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
#include "Kismet/KismetMathLibrary.h"

ASphereTarget::ASphereTarget()
{
	PrimaryActorTick.bCanEverTick = true;
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>("Capsule Collider");
	RootComponent = CapsuleComp;
	
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("Base Mesh");
	BaseMesh->SetupAttachment(CapsuleComp);
	
	OutlineMesh = CreateDefaultSubobject<UStaticMeshComponent>("Outline Mesh");
	OutlineMesh->SetupAttachment(BaseMesh);
	
	HealthComp = CreateDefaultSubobject<UDefaultHealthComponent>("Health Component");
	
	InitialLifeSpan = 1.5f;
	Guid = FGuid::NewGuid();
}

void ASphereTarget::SetSphereScale(const FVector NewScale)
{
	TargetScale = NewScale.X;
	BaseMesh->SetRelativeScale3D(NewScale * BaseToOutlineRatio);
	OutlineMesh->SetRelativeScale3D(FVector(1 / BaseToOutlineRatio));
}

void ASphereTarget::BeginPlay()
{
	Super::BeginPlay();

	PlayerSettings = LoadPlayerSettings();

	UE_LOG(LogTemp, Display, TEXT("PeakColor: %s"), *PlayerSettings.PeakTargetColor.ToString());
	UE_LOG(LogTemp, Display, TEXT("FadeColor: %s"), *PlayerSettings.FadeTargetColor.ToString());

	/* Use Color Changing Material, this is required in order to change color using C++ */
	Material = BaseMesh->GetMaterial(0);
	MID_TargetColorChanger = UMaterialInstanceDynamic::Create(Material, this);
	BaseMesh->SetMaterial(0, MID_TargetColorChanger);

	/* Use Color Changing Material, this is required in order to change color using C++ */
	OutlineMaterial = OutlineMesh->GetMaterial(0);
	MID_TargetOutline = UMaterialInstanceDynamic::Create(OutlineMaterial, this);
	OutlineMesh->SetMaterial(0, MID_TargetOutline);

	/* Set Outline Color */
	if (PlayerSettings.bUseSeparateOutlineColor)
	{
		SetOutlineColor(PlayerSettings.TargetOutlineColor);
	}
	
	GameModeActorStruct = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this))->GameModeActorStruct;
	const float WhiteToPeakMultiplier = 1 / GameModeActorStruct.PlayerDelay;
	const float PeakToFadeMultiplier = 1 / (GameModeActorStruct.TargetMaxLifeSpan - GameModeActorStruct.PlayerDelay);
	
	/* White to Peak Target Color */
	FOnTimelineFloat OnWhiteToPeak;
	OnWhiteToPeak.BindUFunction(this, FName("WhiteToPeak"));
	WhiteToPeakTimeline.AddInterpFloat(WhiteToPeakCurve, OnWhiteToPeak);

	/* Play PeakToFade when WhiteToPeak is finished */
	FOnTimelineEvent OnWhiteToPeakFinished;
	OnWhiteToPeakFinished.BindUFunction(this, FName("PlayPeakToFadeTimeline"));
	WhiteToPeakTimeline.SetTimelineFinishedFunc(OnWhiteToPeakFinished);
	
	/* Peak Color to Fade Color */
	FOnTimelineFloat OnPeakToFade;
	OnPeakToFade.BindUFunction(this, FName("PeakToFade"));
	PeakToFadeTimeline.AddInterpFloat(PeakToFadeCurve, OnPeakToFade);

	/* Fade the target from transparent to BeatGridInactiveColor */
	FOnTimelineFloat OnFadeAndReappear;
	OnFadeAndReappear.BindUFunction(this, FName("FadeAndReappear"));
	FadeAndReappearTimeline.AddInterpFloat(FadeAndReappearCurve, OnFadeAndReappear);

	WhiteToPeakTimeline.SetPlayRate(WhiteToPeakMultiplier);
	PeakToFadeTimeline.SetPlayRate(PeakToFadeMultiplier);
	FadeAndReappearTimeline.SetPlayRate(WhiteToPeakMultiplier);

	if (GameModeActorStruct.IsBeatGridMode)
	{
		SetLifeSpan(0);
		SetMaxHealth(1000000);
		SetCanBeDamaged(false);
		SetColorToBeatGridColor();
		FOnTimelineEvent OnPeakToFadeFinished;
		OnPeakToFadeFinished.BindUFunction(this, FName("SetColorToBeatGridColor"));
		PeakToFadeTimeline.SetTimelineFinishedFunc(OnPeakToFadeFinished);
	}
	else if (GameModeActorStruct.IsBeatTrackMode)
	{
		SetLifeSpan(0);
		SetMaxHealth(1000000);
		SetSphereColor(PlayerSettings.FadeTargetColor);
		SetOutlineColor(PlayerSettings.FadeTargetColor);
		HealthComp->ShouldUpdateTotalPossibleDamage = true;
	}
	else
	{
		SetLifeSpan(GameModeActorStruct.TargetMaxLifeSpan);
		PlayWhiteToPeakTimeline();
		GetWorldTimerManager().SetTimer(TimeSinceSpawn, GameModeActorStruct.TargetMaxLifeSpan, false);
	}
}

void ASphereTarget::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	WhiteToPeakTimeline.TickTimeline(DeltaSeconds);
	PeakToFadeTimeline.TickTimeline(DeltaSeconds);
	FadeAndReappearTimeline.TickTimeline(DeltaSeconds);
}

void ASphereTarget::StartBeatGridTimer(const float Lifespan)
{
	GetWorldTimerManager().SetTimer(TimeSinceSpawn, this, &ASphereTarget::OnBeatGridTimerTimeOut, Lifespan, false);
	SetCanBeDamaged(true);
	PlayWhiteToPeakTimeline();
}

void ASphereTarget::PlayWhiteToPeakTimeline()
{
	if (FadeAndReappearTimeline.IsPlaying())
	{
		FadeAndReappearTimeline.Stop();
	}
	if (PeakToFadeTimeline.IsPlaying())
	{
		PeakToFadeTimeline.Stop();
	}
	WhiteToPeakTimeline.PlayFromStart();
}

void ASphereTarget::PlayPeakToFadeTimeline()
{
	if (WhiteToPeakTimeline.IsPlaying())
	{
		WhiteToPeakTimeline.Stop();
	}
	if (FadeAndReappearTimeline.IsPlaying())
	{
		FadeAndReappearTimeline.Stop();
	}
	PeakToFadeTimeline.PlayFromStart();
}

void ASphereTarget::PlayFadeAndReappearTimeline()
{
	if (WhiteToPeakTimeline.IsPlaying())
	{
		WhiteToPeakTimeline.Stop();
	}
	if (PeakToFadeTimeline.IsPlaying())
	{
		PeakToFadeTimeline.Stop();
	}
	FadeAndReappearTimeline.PlayFromStart();
}

void ASphereTarget::SetColorToBeatGridColor()
{
	SetSphereColor(PlayerSettings.BeatGridInactiveTargetColor);
	SetOutlineColor(PlayerSettings.BeatGridInactiveTargetColor);
}

void ASphereTarget::WhiteToPeak(const float Alpha)
{
	const FLinearColor Color = UKismetMathLibrary::LinearColorLerp(FLinearColor::White, PlayerSettings.PeakTargetColor, Alpha);
	SetSphereColor(Color);
	if (!PlayerSettings.bUseSeparateOutlineColor)
	{
		SetOutlineColor(Color);
	}
}

void ASphereTarget::PeakToFade(const float Alpha)
{
	const FLinearColor Color = UKismetMathLibrary::LinearColorLerp(PlayerSettings.PeakTargetColor, PlayerSettings.FadeTargetColor, Alpha);
	SetSphereColor(Color);
	if (!PlayerSettings.bUseSeparateOutlineColor)
	{
		SetOutlineColor(Color);
	}
}

void ASphereTarget::FadeAndReappear(const float Alpha)
{
	const FLinearColor Color = UKismetMathLibrary::LinearColorLerp(FLinearColor::Transparent, PlayerSettings.BeatGridInactiveTargetColor, Alpha);
	SetSphereColor(Color);
	SetOutlineColor(Color);
}

void ASphereTarget::SetSphereColor(const FLinearColor Output)
{
	MID_TargetColorChanger->SetVectorParameterValue(TEXT("Color"), Output);
}

void ASphereTarget::SetOutlineColor(const FLinearColor Output)
{
	MID_TargetOutline->SetVectorParameterValue(TEXT("Color"), Output);
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
	PlayExplosionEffect(BaseMesh->GetComponentLocation(), BaseSphereRadius * TargetScale,
	                    MID_TargetColorChanger->K2_GetVectorParameterValue(TEXT("Color")));
	
	/* If BeatGrid mode, don't destroy target, make it not damageable, and play RemoveAndReappear*/
	if (GameModeActorStruct.IsBeatGridMode)
	{
		SetCanBeDamaged(false);
		PeakToFadeTimeline.Stop();
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
