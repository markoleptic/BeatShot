// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCounterWidget.h"

#include "Components/TextBlock.h"

void UFPSCounterWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
		// average the last 40 ticks
		if (CounterUpdateInterval < 40)
		{
			SumOfTicks += InDeltaTime;
			CounterUpdateInterval++;
		}
		else
		{
			// update frame rate
			FPSCounter->SetText(FText::AsNumber(round(CounterUpdateInterval / SumOfTicks)));
			SumOfTicks = 0;
			CounterUpdateInterval = 0;
		}
}
