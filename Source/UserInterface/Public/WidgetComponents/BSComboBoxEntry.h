﻿// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BSComboBoxEntry.generated.h"

class UBorder;
class UTextBlock;
class UTooltipImage;

/** Simple class used for ComboBox entries */
UCLASS()
class USERINTERFACE_API UBSComboBoxEntry : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor NotSelectedColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor SelectedColor;

	/** The image to the left of the text, that will display a tooltip when hovered */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* TooltipImage;

	/** The main text of the entry */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_Entry;

	/** The background of the entry */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* Background;
	
public:
	/** Sets the main text of the entry */
	void SetEntryText(const FText& InText) const;

	/** Returns the TooltipImage */
	UTooltipImage* GetTooltipImage() const { return TooltipImage; }

	/** Returns the string form of the main entry text */
	FString GetEntryTextAsString() const;

	/** Shows or hides the TooltipImage */
	void SetTooltipImageVisibility(const bool bIsVisible) const;

	/** Sets the Brush tint for the Border */
	void SetBackgroundBrushTint(const FLinearColor& Color) const;
};