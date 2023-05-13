// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "LoadingScreenWidget.h"
#include "LoadingProcessTask.h"

bool ULoadingScreenWidget::ShouldShowLoadingScreen(FString& OutReason) const
{
	OutReason.Append(Reason);
	return false;
	//return bShowLoadingScreen;
}

void ULoadingScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void ULoadingScreenWidget::OnFadeOutFinished()
{
	UE_LOG(LogTemp, Display, TEXT("FadeOutFinished LoadingScreenWidget"));
	bShowLoadingScreen = false;
}

void ULoadingScreenWidget::CreateLoadingScreenTask(FOnLoadingScreenShown& OnLoadingScreenShown, FOnReadyToHideLoadingScreen& OnReadyToHideLoadingScreen)
{
	//OnLoadingScreenShown.AddUniqueDynamic(this, &ULoadingScreenWidget::FadeIn);
	//OnReadyToHideLoadingScreen.AddUniqueDynamic(this, &ULoadingScreenWidget::FadeOut);
	//ULoadingProcessTask::CreateLoadingScreenProcessTask(this, Reason);
	//FadeOutDelegate.BindDynamic(this, &ULoadingScreenWidget::OnFadeOutFinished);
	//BindToAnimationFinished(FadeOutAnim, FadeOutDelegate);
}

void ULoadingScreenWidget::FadeIn()
{
	UE_LOG(LogTemp, Display, TEXT("Fading IN LoadingScreenWidget"));
	PlayAnimationForward(FadeInAnim);
}

void ULoadingScreenWidget::FadeOut()
{
	if (!bHasReceivedOnReadyToHideLoadingScreenBroadcast)
	{
		bShowLoadingScreen = true;
		
		UE_LOG(LogTemp, Display, TEXT("Fading OUT LoadingScreenWidget"));

		if (IsAnimationPlaying(FadeInAnim))
		{
			StopAnimation(FadeInAnim);
		}

		PlayAnimationForward(FadeOutAnim);
	}
	
	bHasReceivedOnReadyToHideLoadingScreenBroadcast = true;
}
