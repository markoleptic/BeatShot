// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultPlayerController.h"
#include "GameFramework/Pawn.h"

void ADefaultPlayerController::setPlayerEnabledState(bool bPlayerEnabled)
{
	if (bPlayerEnabled) 
	{
		GetPawn()->EnableInput(this);
	}
	else
	{
		GetPawn()->DisableInput(this);
	}
}
