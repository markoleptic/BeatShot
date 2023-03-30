#include "OverlayWidgets/RLAgentWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Overlay.h"
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
	for (const float Value : QTable)
	{
		if (Value < MinValue)
		{
			MinValue = Value;
		}
		if (Value > MaxValue)
		{
			MaxValue = Value;
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
		Points[i].Image->SetColorAndOpacity(InterpColor(QTable[i]));
	}
}

void URLAgentWidget::InitQTable(const int32 Rows, const int32 Columns)
{
	for (int j = 0; j <Columns; j++)
	{
		for (int i = 0; i < Rows; i++)
		{
			UOverlay* Overlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass());
			UImage* Image = WidgetTree->ConstructWidget<UImage>(DefaultImage);
			UTextBlock* Text = WidgetTree->ConstructWidget<UTextBlock>(DefaultText);
			Overlay->AddChildToOverlay(Image);
			Overlay->AddChildToOverlay(Text);
			GridPanel->AddChildToUniformGrid(Overlay, i, j);
			Points.Add(FAgentPoint(Image, Text));
		}
	}
}

void URLAgentWidget::InitQTable(const int32 Rows, const int32 Columns, const TArray<float>& QTable)
{
	for (const float Value : QTable)
	{
		if (Value < MinValue)
		{
			MinValue = Value;
		}
		if (Value > MaxValue)
		{
			MaxValue = Value;
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
			GridPanel->AddChildToUniformGrid(Overlay, i, j);
			Points.Add(FAgentPoint(Image, Text));
		}
	}
	UpdatePanel(QTable);
}

FLinearColor URLAgentWidget::InterpColor(const float Value) const
{
	return UKismetMathLibrary::LinearColorLerp(FLinearColor(FColor::Red), FLinearColor(FColor::Green),
		UKismetMathLibrary::MapRangeClamped(Value, MinValue, MaxValue, 0, 1));
}
