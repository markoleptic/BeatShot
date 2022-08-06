// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DefaultPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BEATAIM_API ADefaultPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void setPlayerEnabledState(bool bPlayerEnabled);
};
