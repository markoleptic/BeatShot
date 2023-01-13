// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveGameCustomGameMode.h"
#include "Engine/GameInstance.h"
#include "DefaultGameInstance.generated.h"

class AGameModeBase;
class ASphereTarget;
class AGameModeActorBase;
class ADefaultPlayerController;

UCLASS()
class BEATSHOT_API UDefaultGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	/** Executed from GameModesWidget when a game mode has been selected. PlayerController binds
	 *  the OnGameModeSelected delegate to this function when the parent widget is constructed */
	UFUNCTION()
	void InitializeGameModeActor(const FGameModeActorStruct& GameModeActor);
	/** Executed from QuitMenuWidget when a quit state has been reached. PlayerController binds
	 *  the OnQuit delegate to this function when the parent widget is constructed. Calls
	 *  QuitToMainMenu, QuitToDesktop, or StartGameMode depending on parameters */
	UFUNCTION()
	void OnQuit(const bool bShouldSaveScores, const bool bShouldRestart, const bool bGoToMainMenu);
	UFUNCTION()
	void QuitToMainMenu(const bool bShouldSaveScores) const;
	UFUNCTION()
	void QuitToDesktop(const bool bShouldSaveScores) const;
	UFUNCTION()
	void StartGameMode(const bool bIsRestarting, const bool bShouldSaveScores) const;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameModeActorStruct GameModeActorStruct;
};


