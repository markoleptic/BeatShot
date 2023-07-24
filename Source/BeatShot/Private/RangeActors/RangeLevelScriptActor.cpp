// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "RangeActors/RangeLevelScriptActor.h"
#include "BSGameInstance.h"
#include "BSGameMode.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "RangeActors/TimeOfDayManager.h"

using namespace Constants;

ARangeLevelScriptActor::ARangeLevelScriptActor()
{
}

void ARangeLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (!HasAuthority())
	{
		return;
	}

	UBSGameInstance* GI = Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GI->GetPublicGameSettingsChangedDelegate().AddUniqueDynamic(this, &ARangeLevelScriptActor::OnPlayerSettingsChanged_Game);

	ABSGameMode* GameMode = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	GameMode->OnStreakThresholdPassed.BindUObject(this, &ARangeLevelScriptActor::OnStreakThresholdPassed);
	
	if (LoadPlayerSettings().User.bNightModeUnlocked && LoadPlayerSettings().Game.bNightModeSelected)
	{
		TimeOfDayManager->SetTimeOfDay(ETimeOfDay::Night);
	}
	else
	{
		TimeOfDayManager->SetTimeOfDay(ETimeOfDay::Day);
	}
}

void ARangeLevelScriptActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ARangeLevelScriptActor::OnStreakThresholdPassed()
{
	if (TimeOfDayManager->GetTimeOfDay() == ETimeOfDay::DayToNight || TimeOfDayManager->GetTimeOfDay() == ETimeOfDay::NightToDay)
	{
		return;
	}
	if (TimeOfDayManager->GetTimeOfDay() == ETimeOfDay::Day)
	{
		TimeOfDayManager->BeginTransitionToNight();
	}
}

void ARangeLevelScriptActor::OnPlayerSettingsChanged_Game(const FPlayerSettings_Game& GameSettings)
{
	if (!LoadPlayerSettings().User.bNightModeUnlocked)
	{
		return;
	}
	if (GameSettings.bNightModeSelected)
	{
		if (TimeOfDayManager->GetTimeOfDay() == ETimeOfDay::Day)
		{
			TimeOfDayManager->BeginTransitionToNight();
		}
	}
	else
	{
		if (TimeOfDayManager->GetTimeOfDay() == ETimeOfDay::Night)
		{
			TimeOfDayManager->BeginTransitionToDay();
		}
	}
}
