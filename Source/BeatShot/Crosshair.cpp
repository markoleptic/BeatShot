// Fill out your copyright notice in the Description page of Project Settings.


#include "Crosshair.h"

#include "DefaultGameInstance.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/OverlaySlot.h"
#include "Components/ScaleBox.h"
#include "Kismet/GameplayStatics.h"


void UCrosshair::InitializeCrossHair(const FPlayerSettings& PlayerSettings)
{
	SetLineWidth(PlayerSettings.LineWidth);
	SetLineLength(PlayerSettings.LineLength);
	SetOutlineWidth(PlayerSettings.OutlineWidth);
	SetOutlineOpacity(PlayerSettings.OutlineOpacity);
	SetImageColor(PlayerSettings.CrosshairColor);
	SetInnerOffset(PlayerSettings.InnerOffset);
}

void UCrosshair::NativeConstruct()
{
	Super::NativeConstruct();
	UDefaultGameInstance* GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GI->OnPlayerSettingsChange.AddDynamic(this, &UCrosshair::OnPlayerSettingsChange);
	InitializeCrossHair(GI->LoadPlayerSettings());
}

void UCrosshair::SetLineWidth(const int32 NewWidthValue)
{
	const float SizeHorizontalX = Cast<UCanvasPanelSlot>(LeftContainer->Slot)->GetSize().X;
	const float SizeVerticalY = Cast<UCanvasPanelSlot>(TopContainer->Slot)->GetSize().Y;

	Cast<UCanvasPanelSlot>(TopContainer->Slot)->SetSize(FVector2d(NewWidthValue, SizeVerticalY));
	Cast<UCanvasPanelSlot>(BottomContainer->Slot)->SetSize(FVector2d(NewWidthValue, SizeVerticalY));

	Cast<UCanvasPanelSlot>(LeftContainer->Slot)->SetSize(FVector2d(SizeHorizontalX, NewWidthValue));
	Cast<UCanvasPanelSlot>(RightContainer->Slot)->SetSize(FVector2d(SizeHorizontalX, NewWidthValue));
}

void UCrosshair::SetLineLength(const float NewLengthValue)
{
	const float SizeVerticalX = Cast<UCanvasPanelSlot>(TopContainer->Slot)->GetSize().X;
	const float SizeHorizontalY = Cast<UCanvasPanelSlot>(LeftContainer->Slot)->GetSize().Y;

	Cast<UCanvasPanelSlot>(TopContainer->Slot)->SetSize(FVector2d(SizeVerticalX, NewLengthValue));
	Cast<UCanvasPanelSlot>(BottomContainer->Slot)->SetSize(FVector2d(SizeVerticalX, NewLengthValue));

	Cast<UCanvasPanelSlot>(LeftContainer->Slot)->SetSize(FVector2d(NewLengthValue, SizeHorizontalY));
	Cast<UCanvasPanelSlot>(RightContainer->Slot)->SetSize(FVector2d(NewLengthValue, SizeHorizontalY));
}

void UCrosshair::SetOutlineWidth(const int32 NewWidthValue)
{
	Cast<UOverlaySlot>(TopScaleBox->Slot)->SetPadding(FMargin(NewWidthValue, NewWidthValue, NewWidthValue,
	                                                          NewWidthValue));
	Cast<UOverlaySlot>(BottomScaleBox->Slot)->SetPadding(FMargin(NewWidthValue, NewWidthValue, NewWidthValue,
	                                                             NewWidthValue));
	Cast<UOverlaySlot>(LeftScaleBox->Slot)->SetPadding(FMargin(NewWidthValue, NewWidthValue, NewWidthValue,
	                                                           NewWidthValue));
	Cast<UOverlaySlot>(RightScaleBox->Slot)->SetPadding(FMargin(NewWidthValue, NewWidthValue, NewWidthValue,
	                                                            NewWidthValue));
}

void UCrosshair::SetOutlineOpacity(const float NewOpacityValue)
{
	TopOutlineImage->SetOpacity(NewOpacityValue);
	BottomOutlineImage->SetOpacity(NewOpacityValue);
	LeftOutlineImage->SetOpacity(NewOpacityValue);
	RightOutlineImage->SetOpacity(NewOpacityValue);
}

void UCrosshair::SetImageColor(const FLinearColor NewColor)
{
	TopImage->SetColorAndOpacity(NewColor);
	BottomImage->SetColorAndOpacity(NewColor);
	LeftImage->SetColorAndOpacity(NewColor);
	RightImage->SetColorAndOpacity(NewColor);
}

void UCrosshair::SetInnerOffset(const int32 NewOffsetValue)
{
	Cast<UCanvasPanelSlot>(TopContainer->Slot)->SetPosition(FVector2d(0, -NewOffsetValue));
	Cast<UCanvasPanelSlot>(BottomContainer->Slot)->SetPosition(FVector2d(0, NewOffsetValue));
	Cast<UCanvasPanelSlot>(LeftContainer->Slot)->SetPosition(FVector2d(-NewOffsetValue, 0));
	Cast<UCanvasPanelSlot>(RightContainer->Slot)->SetPosition(FVector2d(NewOffsetValue, 0));
}

void UCrosshair::OnPlayerSettingsChange(const FPlayerSettings& PlayerSettings)
{
	InitializeCrossHair(PlayerSettings);
}
