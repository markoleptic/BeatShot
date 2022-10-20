// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/Widget.h"
#include "CoreMinimal.h"
#include "WebBrowser.h"
#include "BeatShotWebBrowser.generated.h"

/**
 * 
 */
UCLASS()
class BEATSHOT_API UBeatShotWebBrowser : public UWebBrowser
{
	GENERATED_BODY()

	virtual void SynchronizeProperties() override;

	UFUNCTION(BlueprintCallable, Category = "Web Browser")
	void SetCookie();
};
