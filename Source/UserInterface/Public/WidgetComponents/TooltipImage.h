// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TooltipImage.generated.h"

class UButton;
class UTooltipImage;

/** Contains data for the tooltip of a widget */
USTRUCT(BlueprintType)
struct FTooltipData
{
	GENERATED_BODY()

	UPROPERTY()
	FText TooltipText;
	
	UPROPERTY()
	FString TooltipString;
	
	UPROPERTY()
	bool bAllowTextWrap;
	
	FTooltipData()
	{
		TooltipText = FText();
		TooltipString = "";
		bAllowTextWrap = false;
	}

	FTooltipData(const FText& InTooltipText, const bool InbAllowTextWrap)
	{
		TooltipText = InTooltipText;
		TooltipString = InTooltipText.ToString();
		bAllowTextWrap = InbAllowTextWrap;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTooltipHovered, UTooltipImage*, TooltipImage, const FTooltipData&, TooltipData);

/** The image to draw on a TooltipWidget */
UCLASS()
class USERINTERFACE_API UTooltipImage : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void OnTooltipImageHoveredCallback();

	/** Sets the TooltipData that is accessed when the tooltip is hovered over */
	void SetupTooltipImage(const FText& InText, const bool bAllowTextWrap = false);

	/** Info about what this Tooltip image should display */
	UPROPERTY()
	FTooltipData TooltipData;

	/** Called when Button is hovered over */
	FOnTooltipHovered OnTooltipHovered;

protected:
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UButton* Button;
};
