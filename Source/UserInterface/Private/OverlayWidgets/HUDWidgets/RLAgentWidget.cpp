// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "OverlayWidgets/HUDWidgets/RLAgentWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/UniformGridPanel.h"
#include "Kismet/KismetMathLibrary.h"

void URLAgentWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void URLAgentWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void URLAgentWidget::UpdatePanel(const TArray<float>& QTable)
{
	const int32 Middle = QTable.Num() / 2;
	for (int i = 0; i < QTable.Num(); i++)
	{
		if (i == Middle)
		{
			continue;
		}
		if (QTable[i] < MinValue)
		{
			MinValue = QTable[i];
		}
		if (QTable[i] > MaxValue)
		{
			MaxValue = QTable[i];
		}
	}
	FNumberFormattingOptions FormattingOptions;
	FormattingOptions.MaximumFractionalDigits = 2;
	FormattingOptions.MinimumFractionalDigits = 2;
	FormattingOptions.MinimumIntegralDigits = 2;
	FormattingOptions.MaximumIntegralDigits = 2;
	for (int i = 0; i < QTable.Num(); i++)
	{
		const float Value = roundf(QTable[i] * 100.f) / 100.f;
		Points[i].Text->SetText(FText::AsNumber(Value, &FormattingOptions));
		if (i == Middle)
		{
			Points[i].Image->SetColorAndOpacity(InterpColor((MinValue + MaxValue) / 2));
			continue;
		}
		Points[i].Image->SetColorAndOpacity(InterpColor(QTable[i]));
	}
}

void URLAgentWidget::UpdatePanel2(const TArray<float>& QTable)
{
	for (const float Value : QTable)
	{
		if (Value < MinValue2)
		{
			MinValue2 = Value;
		}
		if (Value > MaxValue2)
		{
			MaxValue2 = Value;
		}
	}
	FNumberFormattingOptions FormattingOptions;
	FormattingOptions.MaximumFractionalDigits = 2;
	FormattingOptions.MinimumFractionalDigits = 2;
	FormattingOptions.MinimumIntegralDigits = 2;
	FormattingOptions.MaximumIntegralDigits = 2;
	for (int i = 0; i < QTable.Num(); i++)
	{
		const float Value = roundf(QTable[i] * 100.f) / 100.f;
		Points2[i].Text->SetText(FText::AsNumber(Value, &FormattingOptions));
		Points2[i].Image->SetColorAndOpacity(InterpColor(QTable[i]));
	}
}

void URLAgentWidget::InitQTable(const int32 Rows, const int32 Columns, const TArray<float>& QTable)
{
	const int32 Middle = QTable.Num() / 2;
	for (int i = 0; i < QTable.Num(); i++)
	{
		if (i == Middle)
		{
			continue;
		}
		if (QTable[i] < MinValue)
		{
			MinValue = QTable[i];
		}
		if (QTable[i] > MaxValue)
		{
			MaxValue = QTable[i];
		}
	}
	for (int j = 0; j < Columns; j++)
	{
		for (int i = 0; i < Rows; i++)
		{
			UOverlay* Overlay =  WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass());
			UImage* Image =  WidgetTree->ConstructWidget<UImage>(DefaultImage);
			UTextBlock* Text =  WidgetTree->ConstructWidget<UTextBlock>(DefaultText);
			Text->SetColorAndOpacity(FSlateColor(FLinearColor(FColor::Black)));
			Overlay->AddChildToOverlay(Image);
			UOverlaySlot* TextSlot = Overlay->AddChildToOverlay(Text);
			TextSlot->SetVerticalAlignment(VAlign_Center);
			TextSlot->SetHorizontalAlignment(HAlign_Center);
			GridPanel->AddChildToUniformGrid(Overlay, i, j);
			Points.Add(FAgentPoint(Image, Text));
		}
	}
	UpdatePanel(QTable);
}

void URLAgentWidget::InitQTable2(const int32 Rows, const int32 Columns, const TArray<float>& QTable)
{
	for (const float Value : QTable)
	{
		if (Value < MinValue2)
		{
			MinValue2 = Value;
		}
		if (Value > MaxValue2)
		{
			MaxValue2 = Value;
		}
	}
	for (int j = 0; j < Columns; j++)
	{
		for (int i = 0; i < Rows; i++)
		{
			UOverlay* Overlay =  WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass());
			UImage* Image =  WidgetTree->ConstructWidget<UImage>(DefaultImage);
			UTextBlock* Text =  WidgetTree->ConstructWidget<UTextBlock>(DefaultText);
			Text->SetColorAndOpacity(FSlateColor(FLinearColor(FColor::Black)));
			Overlay->AddChildToOverlay(Image);
			Overlay->AddChildToOverlay(Text);
			GridPanel2->AddChildToUniformGrid(Overlay, i, j);
			Points2.Add(FAgentPoint(Image, Text));
		}
	}
	UpdatePanel2(QTable);
}

FLinearColor URLAgentWidget::InterpColor(const float Value) const
{
	return UKismetMathLibrary::LinearColorLerp(FLinearColor(FColor::Red), FLinearColor(FColor::Green),
		UKismetMathLibrary::MapRangeClamped(Value, MinValue, MaxValue, 0, 1));
}
