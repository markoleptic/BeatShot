// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Crosshair.generated.h"
class UEditableTextBox;
class USlider;
/**
 * 
 */
UCLASS()
class BEATSHOT_API UCrosshair : public UUserWidget
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "Color")
	static FLinearColor HexToColor(FString HexString);

	UFUNCTION(BlueprintCallable, Category = "Color")
	static FString ColorToHex(FLinearColor Color);
	
};
