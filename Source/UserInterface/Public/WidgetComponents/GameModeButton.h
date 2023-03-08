// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveGameCustomGameMode.h"
#include "GameModeButton.generated.h"

class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameModeButtonClicked, const UGameModeButton*, GameModeButton);

/** Simple UButton wrapper that stores info about a GameMode, and a pointer to the next game mode button */
UCLASS()
class USERINTERFACE_API UGameModeButton : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void OnButtonClicked();

	void SetDefaults(EGameModeDifficulty GameModeDifficulty, EGameModeActorName GameModeActorName, UGameModeButton* NextButton);
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UButton* Button;
	
	UPROPERTY()
	EGameModeDifficulty Difficulty;
	
	UPROPERTY()
	EGameModeActorName GameModeName;
	
	UPROPERTY()
	UGameModeButton* Next;

	FOnGameModeButtonClicked OnGameModeButtonClicked;
};
