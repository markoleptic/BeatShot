// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CountdownWidget.generated.h"

DECLARE_DELEGATE(FOnCountdownCompleted);

DECLARE_DELEGATE(FStartAAManagerPlayback);

class UImage;
class UMaterialInstanceDynamic;
class UTextBlock;

UCLASS()
class USERINTERFACE_API UCountdownWidget : public UUserWidget
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

	/** Called when CountDownTimer has finished, which then calls StartGameModeMainMenu in DefaultGameMode */
	UFUNCTION()
	void StartGameMode() const;

	/** The delay to set between the AAManagers, initialized in PlayerController when added to viewport */
	float PlayerDelay;

	/** Executes when the countdown timer has completed. DefaultGameMode listens and calls StartGameModeMainMenu */
	FOnCountdownCompleted OnCountdownCompleted;

	/** Executes when there is PlayerDelay time left in the countdown timer. DefaultGameMode listens and calls
	 *  StartAAManagerPlayback() */
	FStartAAManagerPlayback StartAAManagerPlayback;

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
	/** Whether or not NativeTick or StartGameModeMainMenu has called StartAAManagerPlayback from DefaultGameMode */
	bool bHasCalledStartAAManagerPlayback = false;
};
