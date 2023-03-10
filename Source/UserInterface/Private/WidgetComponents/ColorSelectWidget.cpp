// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/ColorSelectWidget.h"

#include "Components/Border.h"
#include "Components/EditableTextBox.h"
#include "Components/Image.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetStringLibrary.h"

void UColorSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ColorAValue->OnTextCommitted.AddDynamic(this, &UColorSelectWidget::OnColorAValueChange);
	ColorRValue->OnTextCommitted.AddDynamic(this, &UColorSelectWidget::OnColorRValueChange);
	ColorGValue->OnTextCommitted.AddDynamic(this, &UColorSelectWidget::OnColorGValueChange);
	ColorBValue->OnTextCommitted.AddDynamic(this, &UColorSelectWidget::OnColorBValueChange);
	HexValue->OnTextCommitted.AddDynamic(this, &UColorSelectWidget::OnHexValueChange);

	ColorASlider->OnValueChanged.AddDynamic(this, &UColorSelectWidget::OnColorASliderChange);
	ColorRSlider->OnValueChanged.AddDynamic(this, &UColorSelectWidget::OnColorRSliderChange);
	ColorGSlider->OnValueChanged.AddDynamic(this, &UColorSelectWidget::OnColorGSliderChange);
	ColorBSlider->OnValueChanged.AddDynamic(this, &UColorSelectWidget::OnColorBSliderChange);
}

void UColorSelectWidget::InitializeColor(const FLinearColor& NewColor)
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

void UColorSelectWidget::SetBorderColors(const bool bStartLeftLight, const bool bShowAlpha)
{
	FSlateBrush LightBrush = FSlateBrush();
	LightBrush.TintColor = FLinearColor(0, 0, 0, 0.1);
	FSlateBrush DarkBrush = FSlateBrush();
	DarkBrush.TintColor = FLinearColor(0, 0, 0, 0.2);

	if (bStartLeftLight)
	{
		LeftBorder->SetBrush(LightBrush);
		RBorder->SetBrush(DarkBrush);
		GBorder->SetBrush(DarkBrush);
		BBorder->SetBrush(DarkBrush);
		ColorPreviewBorder->SetBrush(LightBrush);
		if (bShowAlpha)
		{
			AlphaBorder->SetVisibility(ESlateVisibility::Visible);
			AlphaBorder->SetBrush(DarkBrush);
			HexBorder->SetBrush(DarkBrush);
		}
		else
		{
			AlphaBorder->SetVisibility(ESlateVisibility::Collapsed);
			HexBorder->SetBrush(DarkBrush);
		}
		return;
	}
	LeftBorder->SetBrush(DarkBrush);
	RBorder->SetBrush(LightBrush);
	GBorder->SetBrush(LightBrush);
	BBorder->SetBrush(LightBrush);
	ColorPreviewBorder->SetBrush(DarkBrush);
	if (bShowAlpha)
	{
		AlphaBorder->SetVisibility(ESlateVisibility::Visible);
		AlphaBorder->SetBrush(LightBrush);
		HexBorder->SetBrush(LightBrush);
	}
	else
	{
		AlphaBorder->SetVisibility(ESlateVisibility::Collapsed);
		HexBorder->SetBrush(LightBrush);
	}
}

void UColorSelectWidget::SetColorText(const FText& Key)
{
	ColorText->SetText(Key);
}

void UColorSelectWidget::OnColorAValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float SnappedValue = OnEditableTextBoxChanged(NewValue, ColorAValue, ColorASlider, 1, 0, 100);
	Color.A = FMath::GridSnap(SnappedValue / 100, 0.01);
	ColorPreview->SetColorAndOpacity(Color);
	HexValue->SetText(FText::FromString(Color.ToFColor(false).ToHex()));
	if (!OnColorChanged.ExecuteIfBound(Color))
	{
		UE_LOG(LogTemp, Display, TEXT("OnColorChanged not bound."));
	}
}

void UColorSelectWidget::OnColorRValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float SnappedValue = OnEditableTextBoxChanged(NewValue, ColorRValue, ColorRSlider, 1, 0, 255);
	Color.R = SnappedValue / 255;
	ColorPreview->SetColorAndOpacity(Color);
	HexValue->SetText(FText::FromString(Color.ToFColor(false).ToHex()));
	if (!OnColorChanged.ExecuteIfBound(Color))
	{
		UE_LOG(LogTemp, Display, TEXT("OnColorChanged not bound."));
	}
}

void UColorSelectWidget::OnColorGValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float SnappedValue = OnEditableTextBoxChanged(NewValue, ColorGValue, ColorGSlider, 1, 0, 255);
	Color.G = SnappedValue / 255;
	ColorPreview->SetColorAndOpacity(Color);
	HexValue->SetText(FText::FromString(Color.ToFColor(false).ToHex()));
	if (!OnColorChanged.ExecuteIfBound(Color))
	{
		UE_LOG(LogTemp, Display, TEXT("OnColorChanged not bound."));
	}
}

void UColorSelectWidget::OnColorBValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float SnappedValue = OnEditableTextBoxChanged(NewValue, ColorBValue, ColorBSlider, 1, 0, 255);
	Color.B = SnappedValue / 255;
	ColorPreview->SetColorAndOpacity(Color);
	HexValue->SetText(FText::FromString(Color.ToFColor(false).ToHex()));
	if (!OnColorChanged.ExecuteIfBound(Color))
	{
		UE_LOG(LogTemp, Display, TEXT("OnColorChanged not bound."));
	}
}

void UColorSelectWidget::OnHexValueChange(const FText& NewValue, ETextCommit::Type CommitType)
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

void UColorSelectWidget::OnColorASliderChange(const float NewValue)
{
	const float SnappedValue = OnSliderChanged(NewValue, ColorAValue, 1);
	Color.A = SnappedValue / 100;
	ColorPreview->SetColorAndOpacity(Color);
	HexValue->SetText(FText::FromString(Color.ToFColor(false).ToHex()));
	if (!OnColorChanged.ExecuteIfBound(Color))
	{
		UE_LOG(LogTemp, Display, TEXT("OnColorChanged not bound."));
	}
}

void UColorSelectWidget::OnColorRSliderChange(const float NewValue)
{
	const float SnappedValue = OnSliderChanged(NewValue, ColorRValue, 1);
	Color.R = SnappedValue / 255;
	ColorPreview->SetColorAndOpacity(Color);
	HexValue->SetText(FText::FromString(Color.ToFColor(false).ToHex()));
	if (!OnColorChanged.ExecuteIfBound(Color))
	{
		UE_LOG(LogTemp, Display, TEXT("OnColorChanged not bound."));
	}
}

void UColorSelectWidget::OnColorGSliderChange(const float NewValue)
{
	const float SnappedValue = OnSliderChanged(NewValue, ColorGValue, 1);
	Color.G = SnappedValue / 255;
	ColorPreview->SetColorAndOpacity(Color);
	HexValue->SetText(FText::FromString(Color.ToFColor(false).ToHex()));
	if (!OnColorChanged.ExecuteIfBound(Color))
	{
		UE_LOG(LogTemp, Display, TEXT("OnColorChanged not bound."));
	}
}

void UColorSelectWidget::OnColorBSliderChange(const float NewValue)
{
	const float SnappedValue = OnSliderChanged(NewValue, ColorBValue, 1);
	Color.B = SnappedValue / 255;
	ColorPreview->SetColorAndOpacity(Color);
	HexValue->SetText(FText::FromString(Color.ToFColor(false).ToHex()));
	if (!OnColorChanged.ExecuteIfBound(Color))
	{
		UE_LOG(LogTemp, Display, TEXT("OnColorChanged not bound."));
	}
}

float UColorSelectWidget::OnEditableTextBoxChanged(const FText& NewTextValue, UEditableTextBox* TextBoxToChange, USlider* SliderToChange, const float GridSnapSize, const float Min, const float Max)
{
	const FString StringTextValue = UKismetStringLibrary::Replace(NewTextValue.ToString(), ",", "");
	const float ClampedValue = FMath::Clamp(FCString::Atof(*StringTextValue), Min, Max);
	const float SnappedValue = FMath::GridSnap(ClampedValue, GridSnapSize);
	TextBoxToChange->SetText(FText::AsNumber(SnappedValue));
	SliderToChange->SetValue(SnappedValue);
	return SnappedValue;
}

float UColorSelectWidget::OnSliderChanged(const float NewValue, UEditableTextBox* TextBoxToChange, const float GridSnapSize)
{
	const float SnappedValue = FMath::GridSnap(NewValue, GridSnapSize);
	TextBoxToChange->SetText(FText::AsNumber(SnappedValue));
	return SnappedValue;
}
