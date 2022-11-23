// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Crosshair.generated.h"

/**
 * 
 */
UCLASS()
class BEATSHOT_API UCrosshair : public UUserWidget
{
	GENERATED_BODY()

		UFUNCTION(BlueprintCallable, Category = "Color")
		FLinearColor HexToColor(FString HexString);

	UFUNCTION(BlueprintCallable, Category = "Color")
		FString ColorToHex(FLinearColor Color);
	
};
