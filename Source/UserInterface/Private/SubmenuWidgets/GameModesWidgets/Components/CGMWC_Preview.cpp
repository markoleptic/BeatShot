// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/GameModesWidgets/Components/CGMWC_Preview.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"


void UCGMWC_Preview::SetBoxBounds_Current(const FVector2d& InBounds, const float VerticalOffset)
{
	BoxBounds_Current->SetBoxBounds(InBounds);
	BoxBounds_Current->SetBoxBoundsPosition(VerticalOffset);
}

void UCGMWC_Preview::SetBoxBounds_Max(const FVector2d& InBounds, const float VerticalOffset)
{
	BoxBounds_Max->SetBoxBounds(InBounds);
	BoxBounds_Max->SetBoxBoundsPosition(VerticalOffset);
}

void UCGMWC_Preview::SetBoxBounds_Min(const FVector2d& InBounds, const float VerticalOffset)
{
	BoxBounds_Min->SetBoxBounds(InBounds);
	BoxBounds_Min->SetBoxBoundsPosition(VerticalOffset);
}

void UCGMWC_Preview::SetBoxBoundsVisibility_Max(const ESlateVisibility& InVisibility)
{
	BoxBounds_Max->SetVisibility(InVisibility);
}

void UCGMWC_Preview::SetBoxBoundsVisibility_Min(const ESlateVisibility& InVisibility)
{
	BoxBounds_Min->SetVisibility(InVisibility);
}

void UCGMWC_Preview::SetStaticBoundsHeight(const float InHeight)
{
	StaticBounds->SetHeightOverride(InHeight);
}

void UCGMWC_Preview::SetFloorDistanceHeight(const float InHeight)
{
	FloorDistance->SetHeightOverride(InHeight);
}

void UCGMWC_Preview::SetText_FloorDistance(const FText& InText)
{
	TextBlock_FloorDistance->SetText(InText);
}

UTargetWidget* UCGMWC_Preview::ConstructTargetWidget()
{
	UTargetWidget* TargetWidget = CreateWidget<UTargetWidget>(this, TargetWidgetClass);
	UOverlaySlot* OverlaySlot = Overlay->AddChildToOverlay(TargetWidget);
	OverlaySlot->SetHorizontalAlignment(HAlign_Center);
	OverlaySlot->SetVerticalAlignment(VAlign_Center);
	return TargetWidget;
}

void UCGMWC_Preview::ToggleGameModePreview(const bool bEnable)
{
	if (bEnable)
	{
		if (BoxBounds_Current)
		{
			BoxBounds_Current->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		if (BoxBounds_Max)
		{
			BoxBounds_Max->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		if (BoxBounds_Min)
		{
			BoxBounds_Min->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		if (TextBlock_GameModePreviewAvailability)
		{
			TextBlock_GameModePreviewAvailability->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	else
	{
		if (BoxBounds_Current)
		{
			BoxBounds_Current->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (BoxBounds_Max)
		{
			BoxBounds_Max->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (BoxBounds_Min)
		{
			BoxBounds_Min->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (TextBlock_GameModePreviewAvailability)
		{
			TextBlock_GameModePreviewAvailability->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
}
