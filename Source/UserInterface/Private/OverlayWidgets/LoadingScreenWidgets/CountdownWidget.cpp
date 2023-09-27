// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "OverlayWidgets/LoadingScreenWidgets/CountdownWidget.h"
#include "GlobalConstants.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetMathLibrary.h"

void UCountdownWidget::NativeConstruct()
{
	Super::NativeConstruct();
	/* Use Color Changing Material, this is required in order to change color using C++ */
	MID_Countdown = Image_Countdown->GetDynamicMaterial();
}

void UCountdownWidget::NativeDestruct()
{
	Super::NativeDestruct();
	GetWorld()->GetTimerManager().ClearTimer(CountDownTimer);
	CountDownTimer.Invalidate();
}

void UCountdownWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!GetWorld()->GetTimerManager().IsTimerActive(CountDownTimer) || GetWorld()->GetTimerManager().GetTimerRemaining(CountDownTimer) < 0)
	{
		return;
	}
	const float CurrentTime = GetWorld()->GetTimerManager().GetTimerRemaining(CountDownTimer);
	TextBlock_Counter->SetText(FText::AsNumber(ceil(CurrentTime)));
	MID_Countdown->SetScalarParameterValue(FName("Progress"), UKismetMathLibrary::Fraction(CurrentTime));

	if (PlayerDelay <= 0.01 || CurrentTime - PlayerDelay > 0 || CurrentTime == -1)
	{
		return;
	}
	/** If reached this point, CurrentTime is equal to PlayerDelay, and AAManager playback can be started */
	if (!bHasCalledStartAAManagerPlayback)
	{
		if (!StartAAManagerPlayback.ExecuteIfBound())
		{
			UE_LOG(LogTemp, Display, TEXT("StartAAManagerPlayback not bound."));
		}
		bHasCalledStartAAManagerPlayback = true;
	}
}

void UCountdownWidget::StartCountDownTimer()
{
	GetWorld()->GetTimerManager().SetTimer(CountDownTimer, this, &UCountdownWidget::StartGameMode, Constants::CountdownTimerLength, false);
}

void UCountdownWidget::StartGameMode() const
{
	if (!OnCountdownCompleted.ExecuteIfBound())
	{
		UE_LOG(LogTemp, Display, TEXT("OnCountdownCompleted not bound."));
	}
	if (!bHasCalledStartAAManagerPlayback)
	{
		if (!StartAAManagerPlayback.ExecuteIfBound())
		{
			UE_LOG(LogTemp, Display, TEXT("StartAAManagerPlayback not bound."));
		}
	}
}
