// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LoadingProcessInterface.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadingScreenShown);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReadyToHideLoadingScreen);

/** Interface for things that might cause loading to happen which requires a loading screen to be displayed */
UINTERFACE(BlueprintType)
class COMMONLOADINGSCREEN_API ULoadingProcessInterface : public UInterface
{
	GENERATED_BODY()
};

class COMMONLOADINGSCREEN_API ILoadingProcessInterface
{
	GENERATED_BODY()

public:
	/** Checks to see if this object implements the interface, and if so asks whether or not we should
	 *  be currently showing a loading screen. Defined inside LoadingScreenManager */
	static bool ShouldShowLoadingScreen(UObject* TestObject, FString& OutReason);

	/** Function that classes should override if they wish to interact with the loading screen. Called
	 *  on tick inside LoadingScreenManager */
	virtual bool ShouldShowLoadingScreen(FString& OutReason) const { return false; }

	/** Called by LoadingScreenManager if the LoadingScreenWidget implements this interface */
	virtual void CreateLoadingScreenTask(FOnLoadingScreenShown& OnLoadingScreenShown, FOnReadyToHideLoadingScreen& OnReadyToHideLoadingScreen) {}
};
