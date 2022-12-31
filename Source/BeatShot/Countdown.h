// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Countdown.generated.h"

class UImage;
class UMaterialInstanceDynamic;
class UTextBlock;
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
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	/** Called from Blueprint when user clicks on screen */
	UFUNCTION(BlueprintCallable)
	void StartCountDownTimer();

	/** Called when CountDownTimer has finished, which then calls StartGameMode in DefaultGameMode */
	UFUNCTION()
	void StartGameMode() const;

protected:
	UPROPERTY(EditDefaultsOnly)
	UMaterialInstanceDynamic* MID_Countdown;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UImage* CountdownImage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* Counter;

private:
	UPROPERTY()
	FTimerHandle CountDownTimer;
	/** The delay to set between the AAManagers */
	float PlayerDelay;
	/** The length of the countdown timer */
	const int32 CountdownTimerLength = 3;
	/** Whether or not NativeTick or StartGameMode has called StartAAManagerPlayback from DefaultGameMode */
	bool bHasCalledStartAAManagerPlayback = false;
};
