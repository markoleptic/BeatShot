// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Countdown.generated.h"

class AGameModeActorBase;
class ADefaultGameMode;
class ADefaultPlayerController;
/**
 * 
 */
UCLASS()
class BEATSHOT_API UCountdown : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

public:

	UFUNCTION(BlueprintCallable, Category = "Countdown")
		void StartCountDownTimer();
	UFUNCTION(BlueprintCallable, Category = "Countdown")
		void StartGameMode();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Countdown")
		FTimerHandle CountDownTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
		ADefaultPlayerController* DefaultPlayerController;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
		ADefaultGameMode* DefaultGameMode;
	UPROPERTY(BlueprintReadOnly)
		float PlayerDelay;

private:
	
	const int32 CountdownTimerLength = 3;
};
