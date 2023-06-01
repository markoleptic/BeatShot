// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

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

public:
	void SetText(const FText& InText) const;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* TooltipImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* Background;

	void ToggleTooltipImageVisibility(const bool bIsVisible) const;

	/** Sets the Brush tint for the Border */
	void SetBackgroundBrushTint(const FLinearColor& Color) const;
};
