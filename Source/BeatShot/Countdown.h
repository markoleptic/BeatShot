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
	UFUNCTION(BlueprintCallable, Category = "Countdown")
	void StartCountDownTimer();
	UFUNCTION(BlueprintCallable, Category = "Countdown")
	void StartGameMode() const;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Countdown")
	FTimerHandle CountDownTimer;
	UPROPERTY(BlueprintReadOnly)
	float PlayerDelay;
	UPROPERTY(EditDefaultsOnly, Category = "Materials")
	UMaterialInstanceDynamic* MID_Countdown;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UImage* CountdownImage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* Counter;

private:
	const int32 CountdownTimerLength = 3;
	bool StartAAManagerOnce = true;
};
