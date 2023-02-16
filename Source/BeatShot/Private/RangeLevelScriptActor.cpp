// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "RangeLevelScriptActor.h"
#include "DefaultGameInstance.h"
#include "DefaultGameMode.h"
#include "Components/StaticMeshComponent.h"
#include "Moon.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/LightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/VolumetricCloudComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/RectLight.h"
#include "Engine/SkyLight.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ARangeLevelScriptActor::ARangeLevelScriptActor()
{
	LastLerpRotation = 0;
}

void ARangeLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();
	Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->OnTargetDestroyed.AddUniqueDynamic(
		this, &ARangeLevelScriptActor::OnTargetDestroyed);
	Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->OnPlayerSettingsChange.AddUniqueDynamic(
		this, &ARangeLevelScriptActor::OnPlayerSettingsChanged);
	OnTimelineCompleted.BindUFunction(this, FName("OnTimelineCompletedCallback"));
	OnTransitionTick.BindUFunction(this, FName("TransitionTimeOfDay"));
	OnTransitionMaterialTick.BindUFunction(this, FName("TransitionSkySphereMaterial"));
	OnTransitionSkylightTick.BindUFunction(this, FName("TransitionSkylightIntensity"));
	TransitionTimeline.SetTimelineFinishedFunc(OnTimelineCompleted);
	TransitionTimeline.AddInterpFloat(MovementCurve, OnTransitionTick);
	TransitionTimeline.AddInterpFloat(SkyMaterialCurve, OnTransitionMaterialTick);
	
	SkylightIntensityTimeline.AddInterpFloat(SkylightIntensityCurve, OnTransitionSkylightTick);
	SkylightIntensityReverseTimeline.AddInterpFloat(SkylightIntensityCurveReverse, OnTransitionSkylightTick);

	SkySphere = SkySphere_Soft.Get();
	Moon = Moon_Soft.Get();
	Daylight = Daylight_Soft.Get();
	Skylight = Skylight_Soft.Get();
	SkySphereMaterial = Cast<UMaterialInstanceDynamic>(
		Cast<UStaticMeshComponent>(SkySphere->
			GetComponentByClass(UStaticMeshComponent::StaticClass()))->GetMaterial(0));
	InitialLeftWindowCoverLoc = LeftWindowCover->GetStaticMeshComponent()->GetRelativeLocation();
	InitialRightWindowCoverLoc = RightWindowCover->GetStaticMeshComponent()->GetRelativeLocation();
	if (LoadPlayerSettings().User.bNightModeUnlocked && LoadPlayerSettings().Game.bNightModeSelected)
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
		SkylightIntensityTimeline.TickTimeline(DeltaSeconds);
		SkylightIntensityReverseTimeline.TickTimeline(DeltaSeconds);
	}
}

void ARangeLevelScriptActor::OnTargetDestroyed(const float TimeAlive, const int32 NewStreak, const FVector Position)
{
	if (TimeOfDay == ETimeOfDay::DayToNight || TimeOfDay == ETimeOfDay::NightToDay)
	{
		return;
	}
	if (NewStreak > StreakThreshold)
	{
		if (FPlayerSettings Settings = LoadPlayerSettings(); !Settings.User.bNightModeUnlocked)
		{
			Settings.User.bNightModeUnlocked = true;
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
	TimeOfDay = ETimeOfDay::DayToNight;
	TransitionTimeline.SetPlayRate(1.f / CycleSpeed);
	TransitionTimeline.PlayFromStart();
	SkylightIntensityTimeline.SetPlayRate(1.f / CycleSpeed);
	SkylightIntensityTimeline.PlayFromStart();
}

void ARangeLevelScriptActor::BeginTransitionToDay()
{
	LastLerpRotation = 0;
	TimeOfDay = ETimeOfDay::NightToDay;
	TransitionTimeline.SetPlayRate(1.f / CycleSpeed);
	TransitionTimeline.PlayFromStart();
	SkylightIntensityReverseTimeline.SetPlayRate(1.f / CycleSpeed);
	SkylightIntensityReverseTimeline.PlayFromStart();
}

void ARangeLevelScriptActor::SetTimeOfDayToNight()
{
	TimeOfDay = ETimeOfDay::Night;
	SkySphereMaterial->SetScalarParameterValue("NightAlpha", 1);
	Daylight->GetLightComponent()->AddWorldRotation(FRotator(0, 0, 180));
	Moon->MoonMaterialInstance->SetScalarParameterValue("Opacity", 1);
	Moon->MoonGlowMaterialInstance->SetScalarParameterValue("Opacity", 1);
	Moon->MoonLight->SetIntensity(0.3);
	LeftWindowCover->GetStaticMeshComponent()->SetRelativeLocation(InitialLeftWindowCoverLoc + FVector(WindowCoverOffset, 0,0));
	RightWindowCover->GetStaticMeshComponent()->SetRelativeLocation(InitialRightWindowCoverLoc + FVector(WindowCoverOffset,0,0));
	Skylight->GetLightComponent()->SetIntensity(50);
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
	const float CurrentWindowOffset = UKismetMathLibrary::Lerp(0, WindowCoverOffset, Value);
	LeftWindowCover->GetStaticMeshComponent()->SetRelativeLocation(InitialLeftWindowCoverLoc + FVector(CurrentWindowOffset,0,0));
	RightWindowCover->GetStaticMeshComponent()->SetRelativeLocation(InitialRightWindowCoverLoc + FVector(CurrentWindowOffset,0,0));
	
	Moon->MoonMaterialInstance->SetScalarParameterValue("Opacity", Value);
	Moon->MoonGlowMaterialInstance->SetScalarParameterValue("Opacity", Value);
	
	const float CurrentLerpRotation = UKismetMathLibrary::Lerp(0, 180, Alpha);
	Daylight->GetLightComponent()->AddWorldRotation(FRotator(0, 0, CurrentLerpRotation - LastLerpRotation));
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

void ARangeLevelScriptActor::TransitionSkylightIntensity(float Alpha)
{
	Skylight->GetLightComponent()->SetIntensity(UKismetMathLibrary::Lerp(1, 50, Alpha));
	Moon->MoonLight->SetIntensity(UKismetMathLibrary::Lerp(0, 0.3, Alpha));
}

void ARangeLevelScriptActor::OnPlayerSettingsChanged(const FPlayerSettings& PlayerSettings)
{
	if (!PlayerSettings.User.bNightModeUnlocked)
	{
		return;
	}
	if (PlayerSettings.Game.bNightModeSelected)
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
