// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "OverlayWidgets/LoadingScreenWidgets/LoadingScreenWidget.h"
#include "LoadingProcessTask.h"

bool ULoadingScreenWidget::ShouldShowLoadingScreen(FString& OutReason) const
{
	OutReason.Append(Reason);
	return bShowLoadingScreen;
}

void ULoadingScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void ULoadingScreenWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void ULoadingScreenWidget::BindToLoadingScreenDelegates(FOnLoadingScreenVisibilityChangedDelegate& OnLoadingScreenShown,
	FOnReadyToHideLoadingScreenDelegate& OnReadyToHideLoadingScreen)
{
	OnReadyToHideLoadingScreen.AddUObject(this, &ULoadingScreenWidget::FadeOut);
}

void ULoadingScreenWidget::FadeOut(const float LoadingScreenWidgetFadeOutTime)
{
	bShowLoadingScreen = true;
	if (IsAnimationPlaying(FadeInAnim))
	{
		StopAnimation(FadeInAnim);
	}
	PlayAnimationForward(FadeOutAnim, 1 / LoadingScreenWidgetFadeOutTime);
}
