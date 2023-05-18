// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/GameModeButton.h"
#include "Components/Button.h"

void UGameModeButton::NativeConstruct()
{
	Super::NativeConstruct();
	Button->OnClicked.AddUniqueDynamic(this, &UGameModeButton::OnButtonClicked);
}

void UGameModeButton::OnButtonClicked()
{
	OnGameModeButtonClicked.Broadcast(this);
}

void UGameModeButton::SetDefaults(EGameModeDifficulty GameModeDifficulty, EBaseGameMode InDefaultMode, UGameModeButton* NextButton)
{
	Difficulty = GameModeDifficulty;
	DefaultMode = InDefaultMode;
	Next = NextButton;
}
