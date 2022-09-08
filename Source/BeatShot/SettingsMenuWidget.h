// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class BEATSHOT_API USettingsMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Make BlueprintCallable for testing
	UFUNCTION(BlueprintCallable)
		void ShowSettingsMenu();
	UFUNCTION(BlueprintCallable)
		void HideSettingsMenu();

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USettingsMenuWidget> SettingsMenuClass;

	// Keep a pointer to be able to hide it
	UPROPERTY()
	USettingsMenuWidget* SettingsMenu;
};
