// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/MenuOptionWidgets/ColorSelectOptionWidget.h"
#include "BSWidgetInterface.h"
#include "Components/EditableTextBox.h"
#include "Components/Image.h"
#include "Components/Slider.h"

void UColorSelectOptionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ColorAValue->OnTextCommitted.AddDynamic(this, &UColorSelectOptionWidget::OnColorAValueChange);
	ColorRValue->OnTextCommitted.AddDynamic(this, &UColorSelectOptionWidget::OnColorRValueChange);
	ColorGValue->OnTextCommitted.AddDynamic(this, &UColorSelectOptionWidget::OnColorGValueChange);
	ColorBValue->OnTextCommitted.AddDynamic(this, &UColorSelectOptionWidget::OnColorBValueChange);
	HexValue->OnTextCommitted.AddDynamic(this, &UColorSelectOptionWidget::OnHexValueChange);

	ColorASlider->OnValueChanged.AddDynamic(this, &UColorSelectOptionWidget::OnColorASliderChange);
	ColorRSlider->OnValueChanged.AddDynamic(this, &UColorSelectOptionWidget::OnColorRSliderChange);
	ColorGSlider->OnValueChanged.AddDynamic(this, &UColorSelectOptionWidget::OnColorGSliderChange);
	ColorBSlider->OnValueChanged.AddDynamic(this, &UColorSelectOptionWidget::OnColorBSliderChange);
}

void UColorSelectOptionWidget::InitializeColor(const FLinearColor& NewColor)
{
	Color = NewColor;

	ColorAValue->SetText(FText::AsNumber(roundf(Color.A * 100)));
	ColorRValue->SetText(FText::AsNumber(roundf(Color.R * 255)));
	ColorGValue->SetText(FText::AsNumber(roundf(Color.G * 255)));
	ColorBValue->SetText(FText::AsNumber(roundf(Color.B * 255)));
	HexValue->SetText(FText::FromString(Color.ToFColor(false).ToHex()));

	ColorASlider->SetValue(roundf(Color.A * 100));
	ColorRSlider->SetValue(roundf(Color.R * 255));
	ColorGSlider->SetValue(roundf(Color.G * 255));
	ColorBSlider->SetValue(roundf(Color.B * 255));

	ColorPreview->SetColorAndOpacity(Color);
}

void UColorSelectOptionWidget::OnColorAValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float SnappedValue = IBSWidgetInterface::OnEditableTextBoxChanged(NewValue, ColorAValue, ColorASlider, 1, 0, 100);
	Color.A = FMath::GridSnap(SnappedValue / 100, 0.01);
	ColorPreview->SetColorAndOpacity(Color);
	HexValue->SetText(FText::FromString(Color.ToFColor(false).ToHex()));
	if (!OnColorChanged.ExecuteIfBound(Color))
	{
		UE_LOG(LogTemp, Display, TEXT("OnColorChanged not bound."));
	}
}

void UColorSelectOptionWidget::OnColorRValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float SnappedValue = IBSWidgetInterface::OnEditableTextBoxChanged(NewValue, ColorRValue, ColorRSlider, 1, 0, 255);
	Color.R = SnappedValue / 255;
	ColorPreview->SetColorAndOpacity(Color);
	HexValue->SetText(FText::FromString(Color.ToFColor(false).ToHex()));
	if (!OnColorChanged.ExecuteIfBound(Color))
	{
		UE_LOG(LogTemp, Display, TEXT("OnColorChanged not bound."));
	}
}

void UColorSelectOptionWidget::OnColorGValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float SnappedValue = IBSWidgetInterface::OnEditableTextBoxChanged(NewValue, ColorGValue, ColorGSlider, 1, 0, 255);
	Color.G = SnappedValue / 255;
	ColorPreview->SetColorAndOpacity(Color);
	HexValue->SetText(FText::FromString(Color.ToFColor(false).ToHex()));
	if (!OnColorChanged.ExecuteIfBound(Color))
	{
		UE_LOG(LogTemp, Display, TEXT("OnColorChanged not bound."));
	}
}

void UColorSelectOptionWidget::OnColorBValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float SnappedValue = IBSWidgetInterface::OnEditableTextBoxChanged(NewValue, ColorBValue, ColorBSlider, 1, 0, 255);
	Color.B = SnappedValue / 255;
	ColorPreview->SetColorAndOpacity(Color);
	HexValue->SetText(FText::FromString(Color.ToFColor(false).ToHex()));
	if (!OnColorChanged.ExecuteIfBound(Color))
	{
		UE_LOG(LogTemp, Display, TEXT("OnColorChanged not bound."));
	}
}

void UColorSelectOptionWidget::OnHexValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const FString String = FColor::FromHex(NewValue.ToString()).ToString();
	Color.InitFromString(String);
	Color.R = Color.R / 255;
	Color.G = Color.G / 255;
	Color.B = Color.B / 255;
	Color.A = Color.A / 255;
	ColorAValue->SetText(FText::AsNumber(roundf(Color.A * 100)));
	ColorRValue->SetText(FText::AsNumber(Color.R * 255));
	ColorGValue->SetText(FText::AsNumber(Color.G * 255));
	ColorBValue->SetText(FText::AsNumber(Color.B * 255));
	ColorASlider->SetValue(roundf(Color.A * 100));
	ColorRSlider->SetValue(Color.R * 255);
	ColorGSlider->SetValue(Color.G * 255);
	ColorBSlider->SetValue(Color.B * 255);
	ColorPreview->SetColorAndOpacity(Color);
	if (!OnColorChanged.ExecuteIfBound(Color))
	{
		UE_LOG(LogTemp, Display, TEXT("OnColorChanged not bound."));
	}
}

void UColorSelectOptionWidget::OnColorASliderChange(const float NewValue)
{
	const float SnappedValue = IBSWidgetInterface::OnSliderChanged(NewValue, ColorAValue, 1);
	Color.A = SnappedValue / 100;
	ColorPreview->SetColorAndOpacity(Color);
	HexValue->SetText(FText::FromString(Color.ToFColor(false).ToHex()));
	if (!OnColorChanged.ExecuteIfBound(Color))
	{
		UE_LOG(LogTemp, Display, TEXT("OnColorChanged not bound."));
	}
}

void UColorSelectOptionWidget::OnColorRSliderChange(const float NewValue)
{
	const float SnappedValue = IBSWidgetInterface::OnSliderChanged(NewValue, ColorRValue, 1);
	Color.R = SnappedValue / 255;
	ColorPreview->SetColorAndOpacity(Color);
	HexValue->SetText(FText::FromString(Color.ToFColor(false).ToHex()));
	if (!OnColorChanged.ExecuteIfBound(Color))
	{
		UE_LOG(LogTemp, Display, TEXT("OnColorChanged not bound."));
	}
}

void UColorSelectOptionWidget::OnColorGSliderChange(const float NewValue)
{
	const float SnappedValue = IBSWidgetInterface::OnSliderChanged(NewValue, ColorGValue, 1);
	Color.G = SnappedValue / 255;
	ColorPreview->SetColorAndOpacity(Color);
	HexValue->SetText(FText::FromString(Color.ToFColor(false).ToHex()));
	if (!OnColorChanged.ExecuteIfBound(Color))
	{
		UE_LOG(LogTemp, Display, TEXT("OnColorChanged not bound."));
	}
}

void UColorSelectOptionWidget::OnColorBSliderChange(const float NewValue)
{
	const float SnappedValue = IBSWidgetInterface::OnSliderChanged(NewValue, ColorBValue, 1);
	Color.B = SnappedValue / 255;
	ColorPreview->SetColorAndOpacity(Color);
	HexValue->SetText(FText::FromString(Color.ToFColor(false).ToHex()));
	if (!OnColorChanged.ExecuteIfBound(Color))
	{
		UE_LOG(LogTemp, Display, TEXT("OnColorChanged not bound."));
	}
}