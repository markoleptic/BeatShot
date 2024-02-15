// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/GameModesWidgets/Components/CGMWC_Preview.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBoxSlot.h"


void UCGMWC_Preview::SetBoxBounds_Current(const FVector2d& InBounds, const float VerticalOffset) const
{
	BoxBounds_Current->SetBoxBounds(InBounds);
	BoxBounds_Current->SetBoxBoundsPosition(VerticalOffset);
}

void UCGMWC_Preview::SetBoxBounds_Max(const FVector2d& InBounds, const float VerticalOffset) const
{
	BoxBounds_Max->SetBoxBounds(InBounds);
	BoxBounds_Max->SetBoxBoundsPosition(VerticalOffset);
}

void UCGMWC_Preview::SetBoxBounds_Min(const FVector2d& InBounds, const float VerticalOffset) const
{
	BoxBounds_Min->SetBoxBounds(InBounds);
	BoxBounds_Min->SetBoxBoundsPosition(VerticalOffset);
}

void UCGMWC_Preview::SetBoxBoundsVisibility_Max(const ESlateVisibility& InVisibility) const
{
	BoxBounds_Max->SetVisibility(InVisibility);
}

void UCGMWC_Preview::SetBoxBoundsVisibility_Min(const ESlateVisibility& InVisibility) const
{
	BoxBounds_Min->SetVisibility(InVisibility);
}

void UCGMWC_Preview::SetStaticBoundsHeight(const float InHeight) const
{
	StaticBounds->SetHeightOverride(InHeight);
}

void UCGMWC_Preview::SetFloorDistanceHeight(const float InHeight) const
{
	FloorDistance->SetHeightOverride(InHeight);
}

void UCGMWC_Preview::SetText_FloorDistance(const FText& InText) const
{
	TextBlock_FloorDistance->SetText(InText);
}

void UCGMWC_Preview::SetOverlayPadding(const FMargin InMargin) const
{
	if (UVerticalBoxSlot* BoxSlot = Cast<UVerticalBoxSlot>(Overlay->Slot))
	{
		TopSpacer->SetSize(FVector2d(0, InMargin.Top));
		FMargin Before = BoxSlot->GetPadding();
		Before.Left = InMargin.Left;
		Before.Right = InMargin.Right;
		BoxSlot->SetPadding(Before);
	}
	if (UVerticalBoxSlot* BoxSlot = Cast<UVerticalBoxSlot>(Overlay_Bottom->Slot))
	{
		FMargin Before = BoxSlot->GetPadding();
		Before.Left = InMargin.Left;
		Before.Right = InMargin.Right;
		BoxSlot->SetPadding(Before);
	}
}

float UCGMWC_Preview::GetSpacerOffset() const
{
	if (TopSpacer)
	{
		return TopSpacer->GetSize().Y;
	}
	return 0.f;
}

UTargetWidget* UCGMWC_Preview::ConstructTargetWidget()
{
	UTargetWidget* TargetWidget = CreateWidget<UTargetWidget>(this, TargetWidgetClass);
	UOverlaySlot* OverlaySlot = Overlay->AddChildToOverlay(TargetWidget);
	OverlaySlot->SetHorizontalAlignment(HAlign_Center);
	OverlaySlot->SetVerticalAlignment(VAlign_Center);
	return TargetWidget;
}

void UCGMWC_Preview::ToggleGameModePreview(const bool bEnable) const
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
