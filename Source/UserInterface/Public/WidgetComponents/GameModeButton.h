// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveGameCustomGameMode.h"
#include "GameModeButton.generated.h"

class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameModeButtonClicked, const UGameModeButton*, GameModeButton);

UCLASS()
class USERINTERFACE_API UGameModeButton : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void OnButtonClickedCallback();
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UButton* Button;
	UPROPERTY()
	int32 Quality;
	UPROPERTY()
	EGameModeDifficulty Difficulty;
	UPROPERTY()
	EGameModeActorName GameModeName;
	UPROPERTY()
	UGameModeButton* Next;

	FOnGameModeButtonClicked OnGameModeButtonClicked;
};
