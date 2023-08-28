// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_Preview.h"

#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"

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

void UCustomGameModesWidget_Preview::NativeConstruct()
{
	Super::NativeConstruct();
}

bool UCustomGameModesWidget_Preview::UpdateAllOptionsValid()
{
	return true;
}

void UCustomGameModesWidget_Preview::UpdateOptionsFromConfig()
{
	Super::UpdateOptionsFromConfig();
	if (FloorDistance)
	{
		FloorDistance->SetHeightOverride(BSConfig->TargetConfig.FloorDistance);
	}
}
