// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/BoxBoundsWidget.h"
#include "Components/SizeBox.h"

void UBoxBoundsWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UBoxBoundsWidget::SetBoxBounds(const FVector2d& InBounds) const
{
	BoxBounds->SetWidthOverride(InBounds.X);
	BoxBounds->SetHeightOverride(InBounds.Y);
}
