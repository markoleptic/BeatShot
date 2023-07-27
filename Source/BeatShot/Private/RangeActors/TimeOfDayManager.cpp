// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "RangeActors/TimeOfDayManager.h"
#include "Components/StaticMeshComponent.h"
#include "RangeActors/Moon.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/LightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/TimelineComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/RectLight.h"
#include "Engine/SkyLight.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "GlobalConstants.h"

using namespace Constants;

ATimeOfDayManager::ATimeOfDayManager()
{
	PrimaryActorTick.bCanEverTick = true;
	LastLerpRotation = 0;
	NighttimeLeftRoofLocation = DaytimeLeftRoofLocation + FVector(DayToNightRoofXTravelDistance, 0, DayToNightRoofZTravelDistance);
	NighttimeRightRoofLocation = DaytimeRightRoofLocation + FVector(DayToNightRoofXTravelDistance, 0, DayToNightRoofZTravelDistance);
}

void ATimeOfDayManager::PreInitializeComponents()
{
	OnTransitionTimelineTick.BindUFunction(this, FName("TransitionTimeOfDay"));
	OnTimelineCompleted.BindUFunction(this, FName("OnTimelineCompletedCallback"));
	TransitionTimeline.AddInterpFloat(PositionCurve, OnTransitionTimelineTick);
	TransitionTimeline.SetTimelineFinishedFunc(OnTimelineCompleted);

	if (SkySphere_Soft)
	{
		SkySphereMaterial = Cast<UMaterialInstanceDynamic>(Cast<UStaticMeshComponent>(SkySphere_Soft.Get()->GetComponentByClass(UStaticMeshComponent::StaticClass()))->GetMaterial(0));
	}
	
	Super::PreInitializeComponents();
}

void ATimeOfDayManager::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ATimeOfDayManager::BeginPlay()
{
	Super::BeginPlay();
}

void ATimeOfDayManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (TransitionTimeline.IsPlaying())
	{
		TransitionTimeline.TickTimeline(DeltaTime);
	}
}

void ATimeOfDayManager::BeginTransitionToNight()
{
	if (GetTimeOfDay() == ETimeOfDay::DayToNight || GetTimeOfDay() == ETimeOfDay::NightToDay)
	{
		return;
	}
	LastLerpRotation = 0;
	TimeOfDay = ETimeOfDay::DayToNight;
	TransitionTimeline.SetPlayRate(1.f / DayNightCycleSpeed);
	TransitionTimeline.PlayFromStart();
}

void ATimeOfDayManager::BeginTransitionToDay()
{
	if (GetTimeOfDay() == ETimeOfDay::DayToNight || GetTimeOfDay() == ETimeOfDay::NightToDay)
	{
		return;
	}
	LastLerpRotation = 0;
	TimeOfDay = ETimeOfDay::NightToDay;
	TransitionTimeline.SetPlayRate(1.f / DayNightCycleSpeed);
	TransitionTimeline.PlayFromStart();
}

void ATimeOfDayManager::UpdateTimeOfDay()
{
	if (SkySphere_Soft && !SkySphereMaterial)
	{
		SkySphereMaterial = Cast<UMaterialInstanceDynamic>(Cast<UStaticMeshComponent>(SkySphere_Soft.Get()->GetComponentByClass(UStaticMeshComponent::StaticClass()))->GetMaterial(0));
	}
	SetTimeOfDay(TimeOfDay_Editor);
	SkySphereMaterial = nullptr;
}

void ATimeOfDayManager::BeginTransitionToNight_Editor()
{
	if (SkySphere_Soft && !SkySphereMaterial)
	{
		SkySphereMaterial = Cast<UMaterialInstanceDynamic>(Cast<UStaticMeshComponent>(SkySphere_Soft.Get()->GetComponentByClass(UStaticMeshComponent::StaticClass()))->GetMaterial(0));
	}
	BeginTransitionToNight();
}

void ATimeOfDayManager::BeginTransitionToDay_Editor()
{
	if (SkySphere_Soft && !SkySphereMaterial)
	{
		SkySphereMaterial = Cast<UMaterialInstanceDynamic>(Cast<UStaticMeshComponent>(SkySphere_Soft.Get()->GetComponentByClass(UStaticMeshComponent::StaticClass()))->GetMaterial(0));
	}
	BeginTransitionToDay();
}

void ATimeOfDayManager::SetTimeOfDay(const ETimeOfDay InTimeOfDay)
{
	TimeOfDay = InTimeOfDay;
	
	if (!Skylight_Soft || !Moon_Soft || !Daylight_Soft || !SkySphereMaterial)
	{
		return;
	}
	
	if (InTimeOfDay == ETimeOfDay::Night)
	{
		SkySphereMaterial->SetScalarParameterValue("NightAlpha", 1);

		// Need to add world rotation instead of setting so things don't get weird
		if (FMath::IsNearlyEqual(static_cast<float>(Daylight_Soft->GetLightComponent()->GetComponentRotation().GetNormalized().Roll), 0.f, 0.1f))
		{
			Daylight_Soft->GetLightComponent()->AddWorldRotation(FRotator(0, 0, 180));
		}
		
		Moon_Soft->MoonMaterialInstance->SetScalarParameterValue("Opacity", 1);
		Moon_Soft->MoonGlowMaterialInstance->SetScalarParameterValue("Opacity", 1);
		Moon_Soft->MoonLight->SetIntensity(MaxMoonlightIntensity);
		LeftWindowCover->GetStaticMeshComponent()->SetRelativeLocation(NighttimeLeftRoofLocation);
		RightWindowCover->GetStaticMeshComponent()->SetRelativeLocation(NighttimeRightRoofLocation);
		Skylight_Soft->GetLightComponent()->SetIntensity(NightSkylightIntensity);
	}
	else if (InTimeOfDay == ETimeOfDay::Day)
	{
		SkySphereMaterial->SetScalarParameterValue("NightAlpha", 0);

		// Need to add world rotation instead of setting so things don't get weird
		if (FMath::IsNearlyEqual(static_cast<float>(Daylight_Soft->GetLightComponent()->GetComponentRotation().GetNormalized().Roll), 180.f, 0.1f))
		{
			Daylight_Soft->GetLightComponent()->AddWorldRotation(FRotator(0, 0, 180));
		}
		
		Moon_Soft->MoonMaterialInstance->SetScalarParameterValue("Opacity", 0);
		Moon_Soft->MoonGlowMaterialInstance->SetScalarParameterValue("Opacity", 0);
		Moon_Soft->MoonLight->SetIntensity(0);
		LeftWindowCover->GetStaticMeshComponent()->SetRelativeLocation(DaytimeLeftRoofLocation);
		RightWindowCover->GetStaticMeshComponent()->SetRelativeLocation(DaytimeRightRoofLocation);
		Skylight_Soft->GetLightComponent()->SetIntensity(DaySkylightIntensity);
	}

	RefreshSkySphereMaterial();
}

void ATimeOfDayManager::OnTimelineCompletedCallback()
{
	if (TimeOfDay == ETimeOfDay::DayToNight)
	{
		TimeOfDay = ETimeOfDay::Night;
		return;
	}
	TimeOfDay = ETimeOfDay::Day;
}

void ATimeOfDayManager::TransitionTimeOfDay(const float Value)
{
	float PositionAlpha;
	float DaylightIntensity;
	float MoonlightIntensity;
	float SkylightIntensity;
	float NightAlpha;
	const float PlaybackPosition = TransitionTimeline.GetPlaybackPosition();
	
	if (TimeOfDay == ETimeOfDay::DayToNight)
	{
		PositionAlpha = UKismetMathLibrary::Lerp(0, 1, Value);
		DaylightIntensity = UKismetMathLibrary::Lerp(0, MaxDaylightIntensity, DaylightCurve->GetFloatValue(Value));
		MoonlightIntensity = UKismetMathLibrary::Lerp(0, MaxMoonlightIntensity, MoonlightCurve->GetFloatValue(Value));
		SkylightIntensity = UKismetMathLibrary::Lerp(DaySkylightIntensity, NightSkylightIntensity, SkylightCurve->GetFloatValue(Value));
		NightAlpha = UKismetMathLibrary::Lerp(0, 1, SkyMaterialCurve->GetFloatValue(PlaybackPosition));
	}
	else
	{
		PositionAlpha = UKismetMathLibrary::Lerp(1, 0, Value);
		DaylightIntensity = UKismetMathLibrary::Lerp(MaxDaylightIntensity, 0, DaylightCurve->GetFloatValue(Value));
		MoonlightIntensity = UKismetMathLibrary::Lerp(MaxMoonlightIntensity, 0, MoonlightCurve->GetFloatValue(Value));
		SkylightIntensity = UKismetMathLibrary::Lerp(NightSkylightIntensity, DaySkylightIntensity, SkylightCurve->GetFloatValue(Value));
		NightAlpha = UKismetMathLibrary::Lerp(1, 0, SkyMaterialCurve->GetFloatValue(PlaybackPosition));
	}
	
	if (PositionAlpha <= 0.2f)
	{
		const float CurrentRoofZTravelDistance = UKismetMathLibrary::Lerp(0, DayToNightRoofZTravelDistance, PositionAlpha / 0.2f);
		LeftWindowCover->GetStaticMeshComponent()->SetRelativeLocation(DaytimeLeftRoofLocation + FVector(0, 0, CurrentRoofZTravelDistance));
		RightWindowCover->GetStaticMeshComponent()->SetRelativeLocation(DaytimeRightRoofLocation + FVector(0, 0, CurrentRoofZTravelDistance));
	}
	else
	{
		const float CurrentRoofXTravelDistance = UKismetMathLibrary::Lerp(0, DayToNightRoofXTravelDistance, (PositionAlpha - 0.2f) / (0.8f));
		LeftWindowCover->GetStaticMeshComponent()->SetRelativeLocation(DaytimeLeftRoofLocation + FVector(CurrentRoofXTravelDistance, 0, DayToNightRoofZTravelDistance));
		RightWindowCover->GetStaticMeshComponent()->SetRelativeLocation(DaytimeRightRoofLocation + FVector(CurrentRoofXTravelDistance, 0, DayToNightRoofZTravelDistance));
	}

	const float CurrentLerpRotation = UKismetMathLibrary::Lerp(0, 180, Value);
	Daylight_Soft->GetLightComponent()->AddWorldRotation(FRotator(0, 0, CurrentLerpRotation - LastLerpRotation));
	LastLerpRotation = CurrentLerpRotation;

	Daylight_Soft->GetLightComponent()->SetIntensity(DaylightIntensity);
	Moon_Soft->MoonLight->SetIntensity(MoonlightIntensity);
	Moon_Soft->MoonMaterialInstance->SetScalarParameterValue("Opacity", PositionAlpha);
	Moon_Soft->MoonGlowMaterialInstance->SetScalarParameterValue("Opacity", PositionAlpha);
	Skylight_Soft->GetLightComponent()->SetIntensity(SkylightIntensity);
	SkySphereMaterial->SetScalarParameterValue("NightAlpha", NightAlpha);

	RefreshSkySphereMaterial();
}

