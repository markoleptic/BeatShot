// Fill out your copyright notice in the Description page of Project Settings.


#include "RangeLevelScriptActor.h"

#include "DefaultGameMode.h"
#include "Components/StaticMeshComponent.h"
#include "Moon.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/LightComponent.h"
#include "Components/SphereComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/VolumetricCloudComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/RectLight.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ARangeLevelScriptActor::ARangeLevelScriptActor()
{
	LastLerpRotation = 0;
	bIsTransitioning = false;
	bIsDaytime = true;
}

void ARangeLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();
	Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->OnStreakUpdate.AddDynamic(this, &ARangeLevelScriptActor::OnStreakUpdated);
	OnTimelineCompleted.BindUFunction(this, FName("OnTimelineCompletedCallback"));
	TransitionTimeline.SetTimelineFinishedFunc(OnTimelineCompleted);
	OnTransitionTick.BindUFunction(this, FName("TransitionDayState"));
	TransitionTimeline.AddInterpFloat(MovementCurve, OnTransitionTick);
	OnTransitionMaterialTick.BindUFunction(this, FName("TransitionDayStateMaterial"));
	TransitionTimeline.AddInterpFloat(SkyMaterialCurve, OnTransitionMaterialTick);
	SkySphereMaterial = Cast<UMaterialInstanceDynamic>(Cast<UStaticMeshComponent>(SkySphere->GetComponentByClass(UStaticMeshComponent::StaticClass()))->GetMaterial(0));
}

void ARangeLevelScriptActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UE_LOG(LogTemp, Display, TEXT("Tick %f"), DeltaSeconds);
	if (TransitionTimeline.IsPlaying())
	{
		TransitionTimeline.TickTimeline(DeltaSeconds);
	}
}

void ARangeLevelScriptActor::OnStreakUpdated(const int32 NewStreak, const FVector Position)
{
	if (bIsTransitioning)
	{
		return;
	}
	if (NewStreak > StreakThreshold)
	{
		if (bIsDaytime)
		{
			BeginNightTransition();
		}
		else
		{
			BeginDayTransition();
		}
	}
}

void ARangeLevelScriptActor::OnTimelineCompletedCallback()
{
	bIsTransitioning = false;
	if (Moon->SphereComponent->GetRelativeRotation().Equals(Moon->DaytimeMoonRotation, 1.f))
	{
		bIsDaytime = true;
	}
	else
	{
		bIsDaytime = false;
	}
	BeginDayTransition();
}

void ARangeLevelScriptActor::BeginNightTransition()
{
	UE_LOG(LogTemp, Display, TEXT("Beginning NightTransition"));
	LastLerpRotation = 0;
	VolumetricCloud->Destroy();
	bIsTransitioning = true;
	TransitionTimeline.SetPlayRate(1.f/CycleSpeed);
	TransitionTimeline.PlayFromStart();
}

void ARangeLevelScriptActor::BeginDayTransition()
{
	UE_LOG(LogTemp, Display, TEXT("Beginning DayTransition"));
	LastLerpRotation = 0;
	VolumetricCloud = GetWorld()->SpawnActor<AVolumetricCloud>(AVolumetricCloud::StaticClass());
	bIsTransitioning = true;
	TransitionTimeline.SetPlayRate(1.f/CycleSpeed);
	TransitionTimeline.PlayFromStart();
}

void ARangeLevelScriptActor::TransitionDayState(float Alpha)
{
	float Value;
	if (bIsDaytime)
	{
		Value = UKismetMathLibrary::Lerp(0, 1, Alpha);
	}
	else
	{
		Value = UKismetMathLibrary::Lerp(1, 0, Alpha);
	}

	Moon->MoonLight->SetRelativeRotation(UKismetMathLibrary::FindLookAtRotation(Moon->MoonLight->GetComponentLocation(), FVector::Zero()));
	TargetSpawnerLight->GetLightComponent()->SetIntensity(Value);
	Moon->MoonMaterialInstance->SetScalarParameterValue("Opacity", Value);
	
	const float CurrentLerpRotation = UKismetMathLibrary::Lerp(0, 180, Alpha);
	
	Moon->SphereComponent->AddLocalRotation(FRotator(0, 0, -(CurrentLerpRotation - LastLerpRotation)));
	Daylight->GetLightComponent()->AddLocalRotation(FRotator(0, CurrentLerpRotation - LastLerpRotation, 0));
	
	LastLerpRotation = CurrentLerpRotation;
	RefreshSkySphereMaterial();
	UE_LOG(LogTemp, Display, TEXT("Daylight Rotation: %s"), *Daylight->GetLightComponent()->GetRelativeRotation().ToString());
}

void ARangeLevelScriptActor::TransitionDayStateMaterial(float Alpha)
{
	float Value;
	if (bIsDaytime)
	{
		Value = UKismetMathLibrary::Lerp(0, 1, Alpha);
	}
	else
	{
		Value = UKismetMathLibrary::Lerp(1, 0, Alpha);
	}
	SkySphereMaterial->SetScalarParameterValue("NightAlpha", Value);
}
