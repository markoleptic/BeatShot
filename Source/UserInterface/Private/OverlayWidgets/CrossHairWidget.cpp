// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "OverlayWidgets/CrossHairWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/OverlaySlot.h"
#include "Components/ScaleBox.h"

void UCrossHairWidget::InitializeCrossHair(const FPlayerSettings_CrossHair& CrossHairSettings)
{
	SetLineWidth(CrossHairSettings.LineWidth);
	SetLineLength(CrossHairSettings.LineLength);
	SetOutlineWidth(CrossHairSettings.OutlineWidth);
	SetOutlineOpacity(CrossHairSettings.OutlineOpacity);
	SetImageColor(CrossHairSettings.CrossHairColor);
	SetInnerOffset(CrossHairSettings.InnerOffset);
}

void UCrossHairWidget::NativeConstruct()
{
	Super::NativeConstruct();
	InitializeCrossHair(LoadPlayerSettings().CrossHair);
}

void UCrossHairWidget::SetLineWidth(const int32 NewWidthValue)
{
	const float SizeHorizontalX = Cast<UCanvasPanelSlot>(LeftContainer->Slot)->GetSize().X;
	const float SizeVerticalY = Cast<UCanvasPanelSlot>(TopContainer->Slot)->GetSize().Y;

	Cast<UCanvasPanelSlot>(TopContainer->Slot)->SetSize(FVector2d(NewWidthValue, SizeVerticalY));
	Cast<UCanvasPanelSlot>(BottomContainer->Slot)->SetSize(FVector2d(NewWidthValue, SizeVerticalY));

	Cast<UCanvasPanelSlot>(LeftContainer->Slot)->SetSize(FVector2d(SizeHorizontalX, NewWidthValue));
	Cast<UCanvasPanelSlot>(RightContainer->Slot)->SetSize(FVector2d(SizeHorizontalX, NewWidthValue));
}

void UCrossHairWidget::SetLineLength(const float NewLengthValue)
{
	const float SizeVerticalX = Cast<UCanvasPanelSlot>(TopContainer->Slot)->GetSize().X;
	const float SizeHorizontalY = Cast<UCanvasPanelSlot>(LeftContainer->Slot)->GetSize().Y;

	Cast<UCanvasPanelSlot>(TopContainer->Slot)->SetSize(FVector2d(SizeVerticalX, NewLengthValue));
	Cast<UCanvasPanelSlot>(BottomContainer->Slot)->SetSize(FVector2d(SizeVerticalX, NewLengthValue));

	Cast<UCanvasPanelSlot>(LeftContainer->Slot)->SetSize(FVector2d(NewLengthValue, SizeHorizontalY));
	Cast<UCanvasPanelSlot>(RightContainer->Slot)->SetSize(FVector2d(NewLengthValue, SizeHorizontalY));
}

void UCrossHairWidget::SetOutlineWidth(const int32 NewWidthValue)
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

void UCrossHairWidget::SetOutlineOpacity(const float NewOpacityValue)
{
	TopOutlineImage->SetOpacity(NewOpacityValue);
	BottomOutlineImage->SetOpacity(NewOpacityValue);
	LeftOutlineImage->SetOpacity(NewOpacityValue);
	RightOutlineImage->SetOpacity(NewOpacityValue);
}

void UCrossHairWidget::SetImageColor(const FLinearColor NewColor)
{
	TopImage->SetColorAndOpacity(NewColor);
	BottomImage->SetColorAndOpacity(NewColor);
	LeftImage->SetColorAndOpacity(NewColor);
	RightImage->SetColorAndOpacity(NewColor);
}

void UCrossHairWidget::SetInnerOffset(const int32 NewOffsetValue)
{
	Cast<UCanvasPanelSlot>(TopContainer->Slot)->SetPosition(FVector2d(0, -NewOffsetValue));
	Cast<UCanvasPanelSlot>(BottomContainer->Slot)->SetPosition(FVector2d(0, NewOffsetValue));
	Cast<UCanvasPanelSlot>(LeftContainer->Slot)->SetPosition(FVector2d(-NewOffsetValue, 0));
	Cast<UCanvasPanelSlot>(RightContainer->Slot)->SetPosition(FVector2d(NewOffsetValue, 0));
}

void UCrossHairWidget::OnPlayerSettingsChange(const FPlayerSettings& PlayerSettings)
{
	InitializeCrossHair(PlayerSettings.CrossHair);
}
