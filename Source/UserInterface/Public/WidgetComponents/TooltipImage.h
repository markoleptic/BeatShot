// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TooltipImage.generated.h"

class UButton;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTooltipHovered, UTooltipImage*, TooltipImage);

UCLASS()
class USERINTERFACE_API UTooltipImage : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void OnTooltipImageHoveredCallback();
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UButton* Button;
	
	/** The tooltip text to display for this instance of TooltipImage */
	FText TooltipText;
	
	/** Called when Button is hovered over */
	FOnTooltipHovered OnTooltipHovered;
};
