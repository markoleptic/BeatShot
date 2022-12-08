// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPSCounterWidget.generated.h"

class UTextBlock;
struct FPlayerSettings;
/**
 * 
 */
UCLASS()
class BEATSHOT_API UFPSCounterWidget : public UUserWidget
{
	GENERATED_BODY()

	/* Called every frame */
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/* Whether or not to show the FPS counter */
	bool bShowFPSCounter = false;

	/* How often to poll for FPS updates */
	int32 CounterUpdateInterval;

	/* The sum of ticks that have occurred since the last interval */
	float SumOfTicks;

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Player Stats")
		UTextBlock* FPSCounter;
};
