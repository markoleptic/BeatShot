// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PostGameMenuWidget.generated.h"

class UWebBrowserWidget;
/**
 * 
 */
UCLASS()
class BEATSHOT_API UPostGameMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	bool bSavedScores;

	UPROPERTY(BlueprintReadOnly)
	bool bShowScoresWebBrowser;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UWebBrowserWidget* WebBrowserWidget;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ScoresOverlayTextSwitcher(const int32 IndexID);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ScoringButtonClicked();
};
