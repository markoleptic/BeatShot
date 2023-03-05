// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/GameModeButton.h"
#include "Components/Button.h"

void UGameModeButton::NativeConstruct()
{
	Super::NativeConstruct();
	Button->OnClicked.AddUniqueDynamic(this, &UGameModeButton::OnButtonClickedCallback);
}

void UGameModeButton::OnButtonClickedCallback()
{
	OnGameModeButtonClicked.Broadcast(this);
}