// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPSCounterWidget.generated.h"

class UTextBlock;

/** Widget used to show FPS counter */
UCLASS()
class USERINTERFACE_API UFPSCounterWidget : public UUserWidget
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
	UTextBlock* TextBlock_FPSCounter;
};
