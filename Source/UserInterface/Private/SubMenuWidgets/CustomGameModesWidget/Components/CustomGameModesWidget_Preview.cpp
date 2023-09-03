// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_Preview.h"
#include "Components/OverlaySlot.h"
#include "Components/TextBlock.h"

void UCustomGameModesWidget_Preview::InitComponent(FBSConfig* InConfigPtr, TObjectPtr<UCustomGameModesWidgetComponent> InNext)
{
	Super::InitComponent(InConfigPtr, InNext);
}

UTargetWidget* UCustomGameModesWidget_Preview::ConstructTargetWidget()
{
	UTargetWidget* TargetWidget = CreateWidget<UTargetWidget>(this, TargetWidgetClass);
	UOverlaySlot* OverlaySlot = Overlay->AddChildToOverlay(TargetWidget);
	OverlaySlot->SetHorizontalAlignment(HAlign_Center);
	OverlaySlot->SetVerticalAlignment(VAlign_Center);
	return TargetWidget;
}

void UCustomGameModesWidget_Preview::ToggleGameModePreview(const bool bEnable)
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
