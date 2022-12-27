// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeButton.h"
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