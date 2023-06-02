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

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor NotSelectedColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor SelectedColor;

public:
	void SetText(const FText& InText) const;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* TooltipImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* Background;

	UFUNCTION(BlueprintPure, BlueprintCallable)
	bool GetIsSelectedOption() const { return bIsSelectedOption; }

	void ToggleTooltipImageVisibility(const bool bIsVisible) const;

	/** Sets the Brush tint for the Border */
	void SetBackgroundBrushTint(const FLinearColor& Color) const;

	void SetIsSelectedOption(const bool bIsSelected) const;

	FString GetTextBlockAsString() const;

	mutable bool bIsSelectedOption;
};
