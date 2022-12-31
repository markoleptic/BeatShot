// Fill out your copyright notice in the Description page of Project Settings.


#include "Countdown.h"

#include "DefaultGameInstance.h"
#include "DefaultGameMode.h"
#include "DefaultPlayerController.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void UCountdown::NativeConstruct()
{
	Super::NativeConstruct();
	/* Use Color Changing Material, this is required in order to change color using C++ */
	MID_Countdown = CountdownImage->GetDynamicMaterial();
	PlayerDelay = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->GameModeActorStruct.
		PlayerDelay;
}

void UCountdown::NativeDestruct()
{
	Super::NativeDestruct();
	GetWorld()->GetTimerManager().ClearTimer(CountDownTimer);
	CountDownTimer.Invalidate();
}

void UCountdown::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!GetWorld()->GetTimerManager().IsTimerActive(CountDownTimer) || GetWorld()->GetTimerManager().
		GetTimerRemaining(CountDownTimer) < 0)
	{
		return;
	}
	const float CurrentTime = GetWorld()->GetTimerManager().GetTimerRemaining(CountDownTimer);
	Counter->SetText(FText::AsNumber(ceil(CurrentTime)));
	MID_Countdown->SetScalarParameterValue(FName("Progress"), UKismetMathLibrary::Fraction(CurrentTime));

	if (PlayerDelay < 0.05 || CurrentTime - PlayerDelay > 0 || CurrentTime == -1)
	{
		return;
	}
	/** If reached this point, CurrentTime is equal to PlayerDelay, and AAManager playback can be started */
	if (!bHasCalledStartAAManagerPlayback)
	{
		UE_LOG(LogTemp, Display, TEXT("StartAAManagerOnce from Countdown"));
		Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->StartAAManagerPlayback();
		bHasCalledStartAAManagerPlayback = true;
	}
}

void UCountdown::StartCountDownTimer()
{
	GetWorld()->GetTimerManager().SetTimer(CountDownTimer, this, &UCountdown::StartGameMode, CountdownTimerLength,
	                                       false);
}

void UCountdown::StartGameMode() const
{
	ADefaultGameMode* DefaultGameMode = Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	DefaultGameMode->StartGameMode();
	if (!bHasCalledStartAAManagerPlayback)
	{
		DefaultGameMode->StartAAManagerPlayback();
	}
}
