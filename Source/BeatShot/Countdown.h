// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Countdown.generated.h"

class UWidgetSwitcher;
class AGameModeActorBase;
class ADefaultGameMode;
class ADefaultPlayerController;
/**
 * 
 */
UCLASS()
class BEATSHOT_API UCountdown : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
		ADefaultPlayerController* DefaultPlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
		ADefaultGameMode* DefaultGameMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
		AGameModeActorBase* GameModeActorBase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient, meta = (BindWidgetAnim), Category = "Animations")
		UWidgetAnimation* FadeFromBlack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UWidgetSwitcher* CountdownSwitcher;
};
