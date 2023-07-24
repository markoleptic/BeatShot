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
	
	if (SkySphere_Soft)
	{
		SkySphereMaterial = Cast<UMaterialInstanceDynamic>(Cast<UStaticMeshComponent>(SkySphere_Soft.Get()->GetComponentByClass(UStaticMeshComponent::StaticClass()))->GetMaterial(0));
	}

	NighttimeLeftRoofLocation = DaytimeLeftRoofLocation + FVector(DayToNightRoofXTravelDistance, 0, DayToNightRoofZTravelDistance);
	NighttimeRightRoofLocation = DaytimeRightRoofLocation + FVector(DayToNightRoofXTravelDistance, 0, DayToNightRoofZTravelDistance);
}

void ATimeOfDayManager::BeginPlay()
{
	Super::BeginPlay();
	
	OnTimelineVector.BindUFunction(this, FName("TransitionTimeOfDay"));
	OnTransitionMaterialTick.BindUFunction(this, FName("TransitionSkySphereMaterial"));
	OnTimelineCompleted.BindUFunction(this, FName("OnTimelineCompletedCallback"));

	TransitionTimeline.AddInterpVector(LightCurve, OnTimelineVector);
	TransitionTimeline.AddInterpFloat(SkyMaterialCurve, OnTransitionMaterialTick);
	TransitionTimeline.SetTimelineFinishedFunc(OnTimelineCompleted);
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
	LastLerpRotation = 0;
	TimeOfDay = ETimeOfDay::DayToNight;
	TransitionTimeline.SetPlayRate(1.f / DayNightCycleSpeed);
	TransitionTimeline.PlayFromStart();
}

void ATimeOfDayManager::BeginTransitionToDay()
{
	LastLerpRotation = 0;
	TimeOfDay = ETimeOfDay::NightToDay;
	TransitionTimeline.SetPlayRate(1.f / DayNightCycleSpeed);
	TransitionTimeline.PlayFromStart();
}

void ATimeOfDayManager::ToggleTimeOfDay()
{
	if (TimeOfDay_Editor == ETimeOfDay::Night)
	{
		TimeOfDay_Editor = ETimeOfDay::Day;
		SetTimeOfDay(ETimeOfDay::Day);
	}
	else if (TimeOfDay_Editor == ETimeOfDay::Day)
	{
		TimeOfDay_Editor = ETimeOfDay::Night;
		SetTimeOfDay(ETimeOfDay::Night);
	}
}

void ATimeOfDayManager::SetTimeOfDay(const ETimeOfDay InTimeOfDay)
{
	TimeOfDay = InTimeOfDay;
	
	if (!SkySphereMaterial)
	{
		SkySphereMaterial = Cast<UMaterialInstanceDynamic>(Cast<UStaticMeshComponent>(SkySphere_Soft->GetComponentByClass(UStaticMeshComponent::StaticClass()))->GetMaterial(0));
	}
	
	if (!Skylight_Soft || !Moon_Soft || !Daylight_Soft)
	{
		return;
	}
	if (InTimeOfDay == ETimeOfDay::Night)
	{
		SkySphereMaterial->SetScalarParameterValue("NightAlpha", 1);
		Daylight_Soft->GetLightComponent()->AddWorldRotation(FRotator(0, 0, 180));
		Moon_Soft->MoonMaterialInstance->SetScalarParameterValue("Opacity", 1);
		Moon_Soft->MoonGlowMaterialInstance->SetScalarParameterValue("Opacity", 1);
		Moon_Soft->MoonLight->SetIntensity(MaxMoonlightIntensity);
		LeftWindowCover->GetStaticMeshComponent()->SetRelativeLocation(NighttimeLeftRoofLocation);
		RightWindowCover->GetStaticMeshComponent()->SetRelativeLocation(NighttimeRightRoofLocation);
		UE_LOG(LogTemp, Display, TEXT("NighttimeLeftRoofLocation %s"), *NighttimeLeftRoofLocation.ToString());
		Skylight_Soft->GetLightComponent()->SetIntensity(NightSkylightIntensity);
	}
	else if (InTimeOfDay == ETimeOfDay::Day)
	{
		SkySphereMaterial->SetScalarParameterValue("NightAlpha", 0);
		Daylight_Soft->GetLightComponent()->AddWorldRotation(FRotator(0, 0, 180));
		Moon_Soft->MoonMaterialInstance->SetScalarParameterValue("Opacity", 0);
		Moon_Soft->MoonGlowMaterialInstance->SetScalarParameterValue("Opacity", 0);
		Moon_Soft->MoonLight->SetIntensity(0);
		LeftWindowCover->GetStaticMeshComponent()->SetRelativeLocation(DaytimeLeftRoofLocation);
		RightWindowCover->GetStaticMeshComponent()->SetRelativeLocation(DaytimeRightRoofLocation);
		UE_LOG(LogTemp, Display, TEXT("DaytimeLeftRoofLocation %s"), *DaytimeLeftRoofLocation.ToString());
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

void ATimeOfDayManager::TransitionTimeOfDay(const FVector Vector)
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
	
	if (Value <= 0.2f)
	{
		const float CurrentRoofZTravelDistance = UKismetMathLibrary::Lerp(0, DayToNightRoofZTravelDistance, Value / 0.2f);
		LeftWindowCover->GetStaticMeshComponent()->SetRelativeLocation(DaytimeLeftRoofLocation + FVector(0, 0, CurrentRoofZTravelDistance));
		RightWindowCover->GetStaticMeshComponent()->SetRelativeLocation(DaytimeRightRoofLocation + FVector(0, 0, CurrentRoofZTravelDistance));
	}
	else
	{
		const float CurrentRoofXTravelDistance = UKismetMathLibrary::Lerp(0, DayToNightRoofXTravelDistance, (Value - 0.2f) / (0.8f));
		LeftWindowCover->GetStaticMeshComponent()->SetRelativeLocation(DaytimeLeftRoofLocation + FVector(CurrentRoofXTravelDistance, 0, DayToNightRoofZTravelDistance));
		RightWindowCover->GetStaticMeshComponent()->SetRelativeLocation(DaytimeRightRoofLocation + FVector(CurrentRoofXTravelDistance, 0, DayToNightRoofZTravelDistance));
	}

	const float CurrentLerpRotation = UKismetMathLibrary::Lerp(0, 180, Vector.X);
	Daylight_Soft->GetLightComponent()->AddWorldRotation(FRotator(0, 0, CurrentLerpRotation - LastLerpRotation));
	LastLerpRotation = CurrentLerpRotation;

	Daylight_Soft->GetLightComponent()->SetIntensity(DaylightValue);
	Moon_Soft->MoonLight->SetIntensity(MoonlightValue);
	Moon_Soft->MoonMaterialInstance->SetScalarParameterValue("Opacity", MoonValue);
	Moon_Soft->MoonGlowMaterialInstance->SetScalarParameterValue("Opacity", MoonValue);
	Skylight_Soft->GetLightComponent()->SetIntensity(SkylightValue);

	RefreshSkySphereMaterial();
}

void ATimeOfDayManager::TransitionSkySphereMaterial(float Alpha)
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

