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
	const float SizeHorizontalX = Cast<UCanvasPanelSlot>(Container_Left->Slot)->GetSize().X;
	const float SizeVerticalY = Cast<UCanvasPanelSlot>(Container_Top->Slot)->GetSize().Y;

	Cast<UCanvasPanelSlot>(Container_Top->Slot)->SetSize(FVector2d(NewWidthValue, SizeVerticalY));
	Cast<UCanvasPanelSlot>(Container_Bottom->Slot)->SetSize(FVector2d(NewWidthValue, SizeVerticalY));

	Cast<UCanvasPanelSlot>(Container_Left->Slot)->SetSize(FVector2d(SizeHorizontalX, NewWidthValue));
	Cast<UCanvasPanelSlot>(Container_Right->Slot)->SetSize(FVector2d(SizeHorizontalX, NewWidthValue));
}

void UCrossHairWidget::SetLineLength(const float NewLengthValue)
{
	const float SizeVerticalX = Cast<UCanvasPanelSlot>(Container_Top->Slot)->GetSize().X;
	const float SizeHorizontalY = Cast<UCanvasPanelSlot>(Container_Left->Slot)->GetSize().Y;

	Cast<UCanvasPanelSlot>(Container_Top->Slot)->SetSize(FVector2d(SizeVerticalX, NewLengthValue));
	Cast<UCanvasPanelSlot>(Container_Bottom->Slot)->SetSize(FVector2d(SizeVerticalX, NewLengthValue));

	Cast<UCanvasPanelSlot>(Container_Left->Slot)->SetSize(FVector2d(NewLengthValue, SizeHorizontalY));
	Cast<UCanvasPanelSlot>(Container_Right->Slot)->SetSize(FVector2d(NewLengthValue, SizeHorizontalY));
}

void UCrossHairWidget::SetOutlineWidth(const int32 NewWidthValue)
{
	Cast<UOverlaySlot>(ScaleBox_Top->Slot)->SetPadding(FMargin(NewWidthValue, NewWidthValue, NewWidthValue, NewWidthValue));
	Cast<UOverlaySlot>(ScaleBox_Bottom->Slot)->SetPadding(FMargin(NewWidthValue, NewWidthValue, NewWidthValue, NewWidthValue));
	Cast<UOverlaySlot>(ScaleBox_Left->Slot)->SetPadding(FMargin(NewWidthValue, NewWidthValue, NewWidthValue, NewWidthValue));
	Cast<UOverlaySlot>(ScaleBox_Right->Slot)->SetPadding(FMargin(NewWidthValue, NewWidthValue, NewWidthValue, NewWidthValue));
}

void UCrossHairWidget::SetOutlineOpacity(const float NewOpacityValue)
{
	Image_TopOutline->SetOpacity(NewOpacityValue);
	Image_BottomOutline->SetOpacity(NewOpacityValue);
	Image_LeftOutline->SetOpacity(NewOpacityValue);
	Image_RightOutline->SetOpacity(NewOpacityValue);
}

void UCrossHairWidget::SetImageColor(const FLinearColor NewColor)
{
	Image_Top->SetColorAndOpacity(NewColor);
	Image_Bottom->SetColorAndOpacity(NewColor);
	Image_Left->SetColorAndOpacity(NewColor);
	Image_Right->SetColorAndOpacity(NewColor);
}

void UCrossHairWidget::SetInnerOffset(const int32 NewOffsetValue)
{
	Cast<UCanvasPanelSlot>(Container_Top->Slot)->SetPosition(FVector2d(0, -NewOffsetValue));
	Cast<UCanvasPanelSlot>(Container_Bottom->Slot)->SetPosition(FVector2d(0, NewOffsetValue));
	Cast<UCanvasPanelSlot>(Container_Left->Slot)->SetPosition(FVector2d(-NewOffsetValue, 0));
	Cast<UCanvasPanelSlot>(Container_Right->Slot)->SetPosition(FVector2d(NewOffsetValue, 0));
}

void UCrossHairWidget::OnPlayerSettingsChanged_CrossHair(const FPlayerSettings_CrossHair& CrossHairSettings)
{
	InitializeCrossHair(CrossHairSettings);
}
