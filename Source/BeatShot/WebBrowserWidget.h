// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WebBrowser.h"
#include "Delegates/DelegateCombinations.h"
#include "WebBrowserWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnURLLoaded, bool, bLoadedSuccessfully);

/**
 * 
 */
UCLASS()
class BEATSHOT_API UWebBrowserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UWebBrowser* WebInterface;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void LoadCustomGameModesURL(const FString& Username);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void LoadDefaultGameModesURL(const FString& Username);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void LoadProfileURL(const FString& Username);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void IsLoggedIntoWebsite(bool& bIsLoggedIn);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void LoadPatchNotesURL();

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnURLLoaded OnURLLoaded;
};
