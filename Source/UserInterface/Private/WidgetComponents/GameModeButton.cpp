// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/GameModeButton.h"

void UGameModeButton::SetDefaults(const EGameModeDifficulty GameModeDifficulty, UGameModeButton* NextButton)
{
	Difficulty = GameModeDifficulty;
	Next = NextButton;
	SetHasSetDefaults(true);
}

void UGameModeButton::SetDefaults(const EBaseGameMode InDefaultMode, UGameModeButton* NextButton)
{
	DefaultMode = InDefaultMode;
	Next = NextButton;
	SetHasSetDefaults(true);
}