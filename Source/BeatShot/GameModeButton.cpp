// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeButton.h"

void UGameModeButton::NativeConstruct()
{
	Super::NativeConstruct();
}

void UGameModeButton::OnButtonClickedCallback()
{
	OnGameModeButtonClicked.Broadcast(this);
}