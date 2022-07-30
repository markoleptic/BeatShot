// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class BEATAIM_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetTargetBar(float TargetsHit);
	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UProgressBar* TargetBar;

	UPROPERTY(EditAnywhere, Category = "Widgets|Text")
		FText TargetsHit;
	UFUNCTION(BlueprintPure, Category = "Widgets|Text")
		FText GetTargetsHit() const;
	UFUNCTION(BlueprintCallable, Category = "Widgets|Text")
		void SetTargetsHit(const FText& NewTargetsHit);
};

