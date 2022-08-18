// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DefaultCharacter.h"
#include "WideSpreadMultiBeat.h"
#include "GameFramework/GameModeBase.h"
#include "BeatAimGameModeBase.generated.h"

UCLASS()
class BEATAIM_API ABeatAimGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:

	// Reference Game Instance
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	class UDefaultGameInstance* GI;

	UFUNCTION(BlueprintCallable, Category = "Game Properties")
	void SetGameModeSelected(bool IsSelected);

	UFUNCTION(BlueprintCallable, Category = "Game Properties")
	bool IsGameModeSelected();

	// Blueprint event used to stop BPAAPlayer and BPAATracker during pause game
	UFUNCTION(BlueprintImplementableEvent)
	void StopAAPlayerAndTracker();

private:

	bool GameModeSelected;
};
