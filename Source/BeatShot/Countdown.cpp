// Fill out your copyright notice in the Description page of Project Settings.


#include "Countdown.h"
#include "DefaultGameMode.h"
#include "DefaultPlayerController.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"

void UCountdown::NativeConstruct()
{
	DefaultPlayerController = Cast<ADefaultPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	DefaultGameMode = Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	GameModeActorBase = DefaultGameMode->GameModeActorBase;
	CountdownSwitcher->SetActiveWidgetIndex(0);
	PlayAnimationForward(FadeFromBlack);
}
