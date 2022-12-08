// Fill out your copyright notice in the Description page of Project Settings.


#include "Countdown.h"

#include "AudioAnalyzerManager.h"
#include "DefaultGameMode.h"
#include "DefaultPlayerController.h"
#include "Kismet/GameplayStatics.h"

void UCountdown::NativeConstruct()
{
	Super::NativeConstruct();
	DefaultPlayerController = Cast<ADefaultPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	DefaultGameMode = Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	PlayerDelay = DefaultGameMode->GameModeActorBase->GameModeActorStruct.PlayerDelay;
}

void UCountdown::NativeDestruct()
{
	Super::NativeDestruct();
	GetWorld()->GetTimerManager().ClearTimer(CountDownTimer);
	CountDownTimer.Invalidate();
}

void UCountdown::StartCountDownTimer()
{
	GetWorld()->GetTimerManager().SetTimer(CountDownTimer, this, &UCountdown::StartGameMode, CountdownTimerLength, false);
}

void UCountdown::StartGameMode() const
{
	UE_LOG(LogTemp, Display, TEXT("StartGameMode firing"));
	DefaultGameMode->GameModeActorBase->StartGameMode();
	DefaultPlayerController->ShowCrosshair();
	DefaultPlayerController->ShowPlayerHUD();
	DefaultPlayerController->HideCountdown();
	if (!DefaultGameMode->AATracker->IsPlaying())
	{
		DefaultGameMode->StartAAManagerPlayback();
	}
}
