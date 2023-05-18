// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GlobalEnums.h"
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

	/** Sets the default button information about the game mode it represents */
	void SetDefaults(EGameModeDifficulty GameModeDifficulty, EBaseGameMode InDefaultMode, UGameModeButton* NextButton);

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UButton* Button;

	/** The difficulty associated with this button */
	UPROPERTY()
	EGameModeDifficulty Difficulty;

	/** The game mode associated with this button */
	UPROPERTY()
	EBaseGameMode DefaultMode;

	/** Pointer to the next GameModeButton in a collection of them */
	UPROPERTY()
	UGameModeButton* Next;

	/** Broadcast when this GameModeButton is clicked */
	FOnGameModeButtonClicked OnGameModeButtonClicked;
};
