// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WebBrowserOverlay.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UWebBrowserOverlay* WebBrowserOverlay;
};
