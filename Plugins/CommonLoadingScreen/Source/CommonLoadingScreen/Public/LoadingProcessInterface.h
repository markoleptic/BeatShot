// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LoadingProcessInterface.generated.h"

/** Called when the LoadingScreenManager is ready to hide the loading screen */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnReadyToHideLoadingScreenDelegate, float);

/** Called when the loading screen visibility changes */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLoadingScreenVisibilityChangedDelegate, bool);

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
	virtual bool ShouldShowLoadingScreen(FString& OutReason) const{ return false; }

	/** Called by LoadingScreenManager if the LoadingScreenWidget implements this interface */
	virtual void BindToLoadingScreenDelegates(FOnLoadingScreenVisibilityChangedDelegate& OnLoadingScreenVisibilityChanged, FOnReadyToHideLoadingScreenDelegate& OnReadyToHideLoadingScreen) {}

	/** Check to see if this implemented interface has bound to Loading Screen Manager's delegates */
	virtual bool HasBoundToLoadingScreenDelegates() const { return bHasBoundToDelegates; }

	/** Set if this implemented interface has bound to Loading Screen Manager's delegates. Only called by LoadingScreenManager */
	virtual void SetHasBoundToLoadingScreenDelegates(const bool bHasBound) { bHasBoundToDelegates = bHasBound; }
	
protected:
	/** Whether or not this implemented interface has bound to Loading Screen Manager's delegates */
	bool bHasBoundToDelegates = false;
};
