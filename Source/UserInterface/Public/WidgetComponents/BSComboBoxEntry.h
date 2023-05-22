// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BSComboBoxEntry.generated.h"

class UTextBlock;
class UTooltipImage;

/** Simple class used for ComboBox entries */
UCLASS()
class USERINTERFACE_API UBSComboBoxEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetText(const FText& InText);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* TooltipImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock;

	void ToggleTooltipImageVisibility(const bool bIsVisible);
};
