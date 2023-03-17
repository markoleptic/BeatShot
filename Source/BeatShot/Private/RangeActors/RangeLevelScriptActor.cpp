// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "RangeActors/RangeLevelScriptActor.h"
#include "BSGameInstance.h"
#include "BSGameMode.h"
#include "Components/StaticMeshComponent.h"
#include "RangeActors/Moon.h"
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
	
	if (!HasAuthority())
	{
		return;
	}
	
	Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->OnTargetDestroyed.AddUFunction(this, "OnTargetDestroyed");
	Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->OnPlayerSettingsChange.AddDynamic(this, &ARangeLevelScriptActor::OnPlayerSettingsChanged);

	OnTimelineVector.BindUFunction(this, FName("TransitionTimeOfDay"));
	OnTransitionMaterialTick.BindUFunction(this, FName("TransitionSkySphereMaterial"));
	OnTimelineCompleted.BindUFunction(this, FName("OnTimelineCompletedCallback"));

	TransitionTimeline.AddInterpVector(LightCurve, OnTimelineVector);
	TransitionTimeline.AddInterpFloat(SkyMaterialCurve, OnTransitionMaterialTick);
	TransitionTimeline.SetTimelineFinishedFunc(OnTimelineCompleted);

	SkySphere = SkySphere_Soft.Get();
	Moon = Moon_Soft.Get();
	Moonlight = Moon->MoonLight;
	Daylight = Daylight_Soft.Get();
	Skylight = Skylight_Soft.Get();
	SkySphereMaterial = Cast<UMaterialInstanceDynamic>(Cast<UStaticMeshComponent>(SkySphere->GetComponentByClass(UStaticMeshComponent::StaticClass()))->GetMaterial(0));
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
	}
}

void ARangeLevelScriptActor::OnTargetDestroyed(const float TimeAlive, const int32 NewStreak, const FVector Position)
{
	if (TimeOfDay == ETimeOfDay::DayToNight || TimeOfDay == ETimeOfDay::NightToDay || NewStreak < StreakThreshold)
	{
		return;
	}

	if (FPlayerSettings Settings = LoadPlayerSettings(); !Settings.User.bNightModeUnlocked)
	{
		Settings.User.bNightModeUnlocked = true;
		SavePlayerSettings(Settings);
		Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->OnPlayerSettingsChange.Broadcast(Settings);
		if (TimeOfDay == ETimeOfDay::Day)
		{
			BeginTransitionToNight();
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
}

void ARangeLevelScriptActor::BeginTransitionToDay()
{
	LastLerpRotation = 0;
	TimeOfDay = ETimeOfDay::NightToDay;
	TransitionTimeline.SetPlayRate(1.f / CycleSpeed);
	TransitionTimeline.PlayFromStart();
}

void ARangeLevelScriptActor::SetTimeOfDayToNight()
{
	TimeOfDay = ETimeOfDay::Night;
	SkySphereMaterial->SetScalarParameterValue("NightAlpha", 1);
	Daylight->GetLightComponent()->AddWorldRotation(FRotator(0, 0, 180));
	Moon->MoonMaterialInstance->SetScalarParameterValue("Opacity", 1);
	Moon->MoonGlowMaterialInstance->SetScalarParameterValue("Opacity", 1);
	Moon->MoonLight->SetIntensity(MaxMoonlightIntensity);
	LeftWindowCover->GetStaticMeshComponent()->SetRelativeLocation(InitialLeftWindowCoverLoc + FVector(WindowCoverOffset, 0, 0));
	RightWindowCover->GetStaticMeshComponent()->SetRelativeLocation(InitialRightWindowCoverLoc + FVector(WindowCoverOffset, 0, 0));
	Skylight->GetLightComponent()->SetIntensity(NightSkylightIntensity);
	RefreshSkySphereMaterial();
}

void ARangeLevelScriptActor::TransitionTimeOfDay(const FVector Vector)
{
	float Value;
	float DaylightValue;
	float MoonlightValue;
	float SkylightValue;
	float MoonValue;

	if (TimeOfDay == ETimeOfDay::DayToNight)
	{
		Value = UKismetMathLibrary::Lerp(0, 1, Vector.X);
		DaylightValue = UKismetMathLibrary::Lerp(0, MaxDaylightIntensity, 1 - Vector.Z);
		MoonlightValue = UKismetMathLibrary::Lerp(0, MaxMoonlightIntensity, Vector.Z);
		MoonValue = UKismetMathLibrary::Lerp(0, 1, Vector.Z);
		SkylightValue = UKismetMathLibrary::Lerp(DaySkylightIntensity, NightSkylightIntensity, Vector.Z);
	}
	else
	{
		Value = UKismetMathLibrary::Lerp(1, 0, Vector.X);
		DaylightValue = UKismetMathLibrary::Lerp(MaxDaylightIntensity, 0, 1 - Vector.Z);
		MoonlightValue = UKismetMathLibrary::Lerp(MaxMoonlightIntensity, 0, Vector.Z);
		MoonValue = UKismetMathLibrary::Lerp(1, 0, Vector.Z);
		SkylightValue = UKismetMathLibrary::Lerp(DaySkylightIntensity, NightSkylightIntensity, Vector.Y);
	}
	const float CurrentWindowOffset = UKismetMathLibrary::Lerp(0, WindowCoverOffset, Value);
	LeftWindowCover->GetStaticMeshComponent()->SetRelativeLocation(InitialLeftWindowCoverLoc + FVector(CurrentWindowOffset, 0, 0));
	RightWindowCover->GetStaticMeshComponent()->SetRelativeLocation(InitialRightWindowCoverLoc + FVector(CurrentWindowOffset, 0, 0));

	const float CurrentLerpRotation = UKismetMathLibrary::Lerp(0, 180, Vector.X);
	Daylight->GetLightComponent()->AddWorldRotation(FRotator(0, 0, CurrentLerpRotation - LastLerpRotation));
	LastLerpRotation = CurrentLerpRotation;

	Daylight->GetLightComponent()->SetIntensity(DaylightValue);
	Moonlight->SetIntensity(MoonlightValue);
	Moon->MoonMaterialInstance->SetScalarParameterValue("Opacity", MoonValue);
	Moon->MoonGlowMaterialInstance->SetScalarParameterValue("Opacity", MoonValue);
	Skylight->GetLightComponent()->SetIntensity(SkylightValue);

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
