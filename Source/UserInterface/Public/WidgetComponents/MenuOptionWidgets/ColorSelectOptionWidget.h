﻿// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MenuOptionWidget.h"
#include "ColorSelectOptionWidget.generated.h"

DECLARE_DELEGATE_OneParam(FOnColorChanged, const FLinearColor& NewColor);

class UTextBlock;
class UBorder;
class USavedTextWidget;
class USlider;
class UImage;
class UEditableTextBox;
class UButton;

UCLASS()
class USERINTERFACE_API UColorSelectOptionWidget : public UMenuOptionWidget
{
	GENERATED_BODY()

	protected:
	virtual void NativeConstruct() override;

public:
	void InitializeColor(const FLinearColor& NewColor);

	FOnColorChanged OnColorChanged;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* ColorAValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* ColorRValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* ColorGValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* ColorBValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* HexValue;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* ColorASlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* ColorRSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* ColorGSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* ColorBSlider;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* ColorPreview;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* AlphaBorder;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* HexBorder;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* RBorder;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* GBorder;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* BBorder;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* ColorPreviewBorder;

private:
	UFUNCTION()
	void OnColorAValueChange(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnColorRValueChange(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnColorGValueChange(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnColorBValueChange(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnHexValueChange(const FText& NewValue, ETextCommit::Type CommitType);

	UFUNCTION()
	void OnColorASliderChange(const float NewValue);
	UFUNCTION()
	void OnColorRSliderChange(const float NewValue);
	UFUNCTION()
	void OnColorGSliderChange(const float NewValue);
	UFUNCTION()
	void OnColorBSliderChange(const float NewValue);

	FLinearColor Color;
};