// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSButton.h"
#include "GlobalEnums.h"
#include "GameModeButton.generated.h"

/** Button used for difficulty selection in GameModesWidget */
UCLASS()
class USERINTERFACE_API UGameModeButton : public UBSButton
{
	GENERATED_BODY()

public:
	virtual UGameModeButton* GetNext() const override { return Cast<UGameModeButton>(Next); }
	
	/** Sets the GameModeDifficulty this button represents */
	void SetDefaults(EGameModeDifficulty GameModeDifficulty, UGameModeButton* NextButton);

	/** Sets the BaseGameMode this button represents */
	void SetDefaults(EBaseGameMode InDefaultMode, UGameModeButton* NextButton);

	/** Returns the GameModeDifficulty this button represents */
	EGameModeDifficulty GetDifficulty() const { return Difficulty; }
	
	/** Returns the BaseGameMode this button represents */
	EBaseGameMode GetDefaultMode() const { return DefaultMode; }

protected:
	/** The GameModeDifficulty associated with this button */
	EGameModeDifficulty Difficulty = EGameModeDifficulty::None;

	/** The BaseGameMode associated with this button */
	EBaseGameMode DefaultMode = EBaseGameMode::None;
};
