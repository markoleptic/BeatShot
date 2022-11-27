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

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	bool bShowFPSCounter = false;
	
	int32 CounterUpdateInterval;
	
	float SumOfTicks;

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Player Stats")
		UTextBlock* FPSCounter;
};
