// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SphereTarget.h"
#include "BSGameInstance.h"
#include "BSHealthComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
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
	
	HealthComp = CreateDefaultSubobject<UBSHealthComponent>("Health Component");
	
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

	PlayerSettings = LoadPlayerSettings().Game;

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
	
	GameModeActorStruct = Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(this))->GameModeActorStruct;
	const float WhiteToPeakMultiplier = 1 / GameModeActorStruct.PlayerDelay;
	const float PeakToFadeMultiplier = 1 / (GameModeActorStruct.TargetMaxLifeSpan - GameModeActorStruct.PlayerDelay);
	
	/* White to Peak Target Color */
	FOnTimelineFloat OnStartToPeak;
	OnStartToPeak.BindUFunction(this, FName("StartToPeak"));
	StartToPeakTimeline.AddInterpFloat(StartToPeakCurve, OnStartToPeak);

	/* Play PeakToEnd when StartToPeak is finished */
	FOnTimelineEvent OnWhiteToPeakFinished;
	OnWhiteToPeakFinished.BindUFunction(this, FName("PlayPeakToEndTimeline"));
	StartToPeakTimeline.SetTimelineFinishedFunc(OnWhiteToPeakFinished);
	
	/* Peak Color to Fade Color */
	FOnTimelineFloat OnPeakToFade;
	OnPeakToFade.BindUFunction(this, FName("PeakToEnd"));
	PeakToEndTimeline.AddInterpFloat(PeakToEndCurve, OnPeakToFade);

	/* Fade the target from transparent to BeatGridInactiveColor */
	FOnTimelineFloat OnFadeAndReappear;
	OnFadeAndReappear.BindUFunction(this, FName("FadeAndReappear"));
	FOnTimelineEvent OnFadeAndReappearFinished;
	OnFadeAndReappearFinished.BindUFunction(this, FName("ShowTargetOutline"));
	FadeAndReappearTimeline.AddInterpFloat(FadeAndReappearCurve, OnFadeAndReappear);
	FadeAndReappearTimeline.SetTimelineFinishedFunc(OnFadeAndReappearFinished);

	StartToPeakTimeline.SetPlayRate(WhiteToPeakMultiplier);
	PeakToEndTimeline.SetPlayRate(PeakToFadeMultiplier);
	FadeAndReappearTimeline.SetPlayRate(WhiteToPeakMultiplier);

	if (GameModeActorStruct.IsBeatGridMode)
	{
		SetLifeSpan(0);
		SetMaxHealth(1000000);
		SetCanBeDamaged(false);
		SetColorToBeatGridColor();
		SetSphereColor(PlayerSettings.BeatGridInactiveTargetColor);
		SetOutlineColor(PlayerSettings.BeatGridInactiveTargetColor);
		FOnTimelineEvent OnPeakToFadeFinished;
		OnPeakToFadeFinished.BindUFunction(this, FName("SetColorToBeatGridColor"));
		PeakToEndTimeline.SetTimelineFinishedFunc(OnPeakToFadeFinished);
	}
	else if (GameModeActorStruct.IsBeatTrackMode)
	{
		bIsBeatTrackTarget = true;
		SetLifeSpan(0);
		SetMaxHealth(1000000);
		SetSphereColor(PlayerSettings.EndTargetColor);
		SetOutlineColor(PlayerSettings.EndTargetColor);
		HealthComp->ShouldUpdateTotalPossibleDamage = true;
	}
	else
	{
		SetLifeSpan(GameModeActorStruct.TargetMaxLifeSpan);
		PlayStartToPeakTimeline();
		GetWorldTimerManager().SetTimer(TimeSinceSpawn, GameModeActorStruct.TargetMaxLifeSpan, false);
	}
}

void ASphereTarget::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	StartToPeakTimeline.TickTimeline(DeltaSeconds);
	PeakToEndTimeline.TickTimeline(DeltaSeconds);
	FadeAndReappearTimeline.TickTimeline(DeltaSeconds);
}

void ASphereTarget::StartBeatGridTimer(const float Lifespan)
{
	GetWorldTimerManager().SetTimer(TimeSinceSpawn, this, &ASphereTarget::OnBeatGridTimerTimeOut, Lifespan, false);
	SetCanBeDamaged(true);
	PlayStartToPeakTimeline();
}

void ASphereTarget::PlayStartToPeakTimeline()
{
	if (FadeAndReappearTimeline.IsPlaying())
	{
		FadeAndReappearTimeline.Stop();
	}
	if (PeakToEndTimeline.IsPlaying())
	{
		PeakToEndTimeline.Stop();
	}
	StartToPeakTimeline.PlayFromStart();
}

void ASphereTarget::PlayPeakToEndTimeline()
{
	if (StartToPeakTimeline.IsPlaying())
	{
		StartToPeakTimeline.Stop();
	}
	if (FadeAndReappearTimeline.IsPlaying())
	{
		FadeAndReappearTimeline.Stop();
	}
	PeakToEndTimeline.PlayFromStart();
}

void ASphereTarget::PlayFadeAndReappearTimeline()
{
	if (StartToPeakTimeline.IsPlaying())
	{
		StartToPeakTimeline.Stop();
	}
	if (PeakToEndTimeline.IsPlaying())
	{
		PeakToEndTimeline.Stop();
	}
	MID_TargetOutline->SetScalarParameterValue(TEXT("ShowOutline"), 0.f);
	FadeAndReappearTimeline.PlayFromStart();
}

void ASphereTarget::SetColorToBeatGridColor()
{
	SetSphereColor(PlayerSettings.BeatGridInactiveTargetColor);
	SetOutlineColor(PlayerSettings.BeatGridInactiveTargetColor);
}

void ASphereTarget::StartToPeak(const float Alpha)
{
	const FLinearColor Color = UKismetMathLibrary::LinearColorLerp(PlayerSettings.StartTargetColor, PlayerSettings.PeakTargetColor, Alpha);
	SetSphereColor(Color);
	if (!PlayerSettings.bUseSeparateOutlineColor)
	{
		SetOutlineColor(Color);
	}
}

void ASphereTarget::PeakToEnd(const float Alpha)
{
	const FLinearColor Color = UKismetMathLibrary::LinearColorLerp(PlayerSettings.PeakTargetColor, PlayerSettings.EndTargetColor, Alpha);
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
		PeakToEndTimeline.Stop();
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

void ASphereTarget::ShowTargetOutline()
{
	MID_TargetOutline->SetScalarParameterValue(TEXT("ShowOutline"), 1.f);
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
