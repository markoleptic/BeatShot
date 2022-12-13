// Fill out your copyright notice in the Description page of Project Settings.


#include "WebBrowserOverlay.h"

#include "Components/TextBlock.h"
#include "Components/Overlay.h"

void UWebBrowserOverlay::NativeConstruct()
{
	Super::NativeConstruct();
	BindToAnimationFinished(FadeOutOverlay, FadeOutDelegate);
	FadeOutDelegate.BindUFunction(BrowserOverlay, FName("RemoveFromParent"));
	OverlayText->SetText(FText::FromStringTable("WebBrowserOverlayStrings", "BaseGameMode"));
}

void UWebBrowserOverlay::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UWebBrowserOverlay::SetOverlayText(const FString& Key)
{
	
	OverlayText->SetText(FText::FromStringTable("WebBrowserOverlayStrings", Key));
}

void UWebBrowserOverlay::FadeOut()
{
	PlayAnimationForward(FadeOutOverlay);
}
