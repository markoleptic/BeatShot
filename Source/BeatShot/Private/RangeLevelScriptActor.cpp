// Fill out your copyright notice in the Description page of Project Settings.


#include "RangeLevelScriptActor.h"
#include "DefaultGameInstance.h"
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
}

void ARangeLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();
	Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->OnStreakUpdate.AddUniqueDynamic(
		this, &ARangeLevelScriptActor::OnStreakUpdated);
	Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->OnPlayerSettingsChange.AddUniqueDynamic(
		this, &ARangeLevelScriptActor::OnPlayerSettingsChanged);
	OnTimelineCompleted.BindUFunction(this, FName("OnTimelineCompletedCallback"));
	OnTransitionTick.BindUFunction(this, FName("TransitionTimeOfDay"));
	OnTransitionMaterialTick.BindUFunction(this, FName("TransitionSkySphereMaterial"));
	TransitionTimeline.SetTimelineFinishedFunc(OnTimelineCompleted);
	TransitionTimeline.AddInterpFloat(MovementCurve, OnTransitionTick);
	TransitionTimeline.AddInterpFloat(SkyMaterialCurve, OnTransitionMaterialTick);
	SkySphereMaterial = Cast<UMaterialInstanceDynamic>(
		Cast<UStaticMeshComponent>(
			SkySphere->GetComponentByClass(UStaticMeshComponent::StaticClass()))->GetMaterial(0));

	if (LoadPlayerSettings().bNightModeUnlocked && LoadPlayerSettings().bNightModeSelected)
	{
		SetTimeOfDayToNight();
	}
}

void ARangeLevelScriptActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (TransitionTimeline.IsPlaying())
	{
		TransitionTimeline.TickTimeline(DeltaSeconds);
	}
}

void ARangeLevelScriptActor::OnStreakUpdated(const int32 NewStreak, const FVector Position)
{
	if (TimeOfDay == ETimeOfDay::DayToNight || TimeOfDay == ETimeOfDay::NightToDay)
	{
		return;
	}
	if (NewStreak > StreakThreshold)
	{
		if (FPlayerSettings Settings = LoadPlayerSettings(); !Settings.bNightModeUnlocked)
		{
			Settings.bNightModeUnlocked = true;
			SavePlayerSettings(Settings);
			Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->OnPlayerSettingsChange.
				Broadcast(Settings);
			if (TimeOfDay == ETimeOfDay::Day)
			{
				BeginTransitionToNight();
			}
		}
	}
}

void ARangeLevelScriptActor::OnTimelineCompletedCallback()
{
	if (TimeOfDay == ETimeOfDay::DayToNight)
	{
		TimeOfDay = ETimeOfDay::Night;
		return;
	}
	TimeOfDay = ETimeOfDay::Day;
}

void ARangeLevelScriptActor::BeginTransitionToNight()
{
	LastLerpRotation = 0;
	VolumetricCloud->Destroy();
	TimeOfDay = ETimeOfDay::DayToNight;
	TransitionTimeline.SetPlayRate(1.f / CycleSpeed);
	TransitionTimeline.PlayFromStart();
}

void ARangeLevelScriptActor::BeginTransitionToDay()
{
	LastLerpRotation = 0;
	VolumetricCloud = GetWorld()->SpawnActor<AVolumetricCloud>(AVolumetricCloud::StaticClass());
	TimeOfDay = ETimeOfDay::NightToDay;
	TransitionTimeline.SetPlayRate(1.f / CycleSpeed);
	TransitionTimeline.PlayFromStart();
}

void ARangeLevelScriptActor::SetTimeOfDayToNight()
{
	VolumetricCloud->Destroy();
	TimeOfDay = ETimeOfDay::Night;
	SkySphereMaterial->SetScalarParameterValue("NightAlpha", 1);
	Moon->SphereComponent->AddLocalRotation(FRotator(0, 0, 180));
	Daylight->GetLightComponent()->AddLocalRotation(FRotator(0, 180, 0));
	TargetSpawnerLight->GetLightComponent()->SetIntensity(1);
	Moon->MoonMaterialInstance->SetScalarParameterValue("Opacity", 1);
	Moon->MoonLight->SetRelativeRotation(
		UKismetMathLibrary::FindLookAtRotation(Moon->MoonLight->GetComponentLocation(), FVector::Zero()));
	RefreshSkySphereMaterial();
}

void ARangeLevelScriptActor::TransitionTimeOfDay(float Alpha)
{
	float Value;
	if (TimeOfDay == ETimeOfDay::DayToNight)
	{
		Value = UKismetMathLibrary::Lerp(0, 1, Alpha);
	}
	else
	{
		Value = UKismetMathLibrary::Lerp(1, 0, Alpha);
	}

	Moon->MoonLight->SetRelativeRotation(
		UKismetMathLibrary::FindLookAtRotation(Moon->MoonLight->GetComponentLocation(), FVector::Zero()));
	TargetSpawnerLight->GetLightComponent()->SetIntensity(Value);
	Moon->MoonMaterialInstance->SetScalarParameterValue("Opacity", Value);

	const float CurrentLerpRotation = UKismetMathLibrary::Lerp(0, 180, Alpha);
	Moon->SphereComponent->AddLocalRotation(FRotator(0, 0, -(CurrentLerpRotation - LastLerpRotation)));
	Daylight->GetLightComponent()->AddLocalRotation(FRotator(0, CurrentLerpRotation - LastLerpRotation, 0));
	LastLerpRotation = CurrentLerpRotation;
	RefreshSkySphereMaterial();
}

void ARangeLevelScriptActor::TransitionSkySphereMaterial(float Alpha)
{
	float Value;
	if (TimeOfDay == ETimeOfDay::DayToNight)
	{
		Value = UKismetMathLibrary::Lerp(0, 1, Alpha);
	}
	else
	{
		Value = UKismetMathLibrary::Lerp(1, 0, Alpha);
	}
	SkySphereMaterial->SetScalarParameterValue("NightAlpha", Value);
}

void ARangeLevelScriptActor::OnPlayerSettingsChanged(const FPlayerSettings& PlayerSettings)
{
	if (!PlayerSettings.bNightModeUnlocked)
	{
		return;
	}
	if (PlayerSettings.bNightModeSelected)
	{
		if (TimeOfDay == ETimeOfDay::Day)
		{
			BeginTransitionToNight();
		}
	}
	else
	{
		if (TimeOfDay == ETimeOfDay::Night)
		{
			BeginTransitionToDay();
		}
	}
}
