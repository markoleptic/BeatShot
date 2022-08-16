// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainMenuHUD.generated.h"

/**
 * 
 */
class UMainMenuWidget;
UCLASS()
class BEATAIM_API AMainMenuHUD : public AHUD
{
	GENERATED_BODY()

public:
	// Make BlueprintCallable for testing
	UFUNCTION(BlueprintCallable)
		void ShowMainMenu();
	UFUNCTION(BlueprintCallable)
		void HideMainMenu();

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMainMenuWidget> MainMenuClass;

	// Keep a pointer to be able to hide it
	UPROPERTY()
	UMainMenuWidget* MainMenu;
};
