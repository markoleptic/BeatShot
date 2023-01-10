// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TooltipImage.generated.h"

class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTooltipImageHovered, UTooltipImage*, TooltipImage, const FText&, TooltipText);

UCLASS()
class BEATSHOT_API UTooltipImage : public UUserWidget
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
	FOnTooltipImageHovered OnTooltipImageHovered;
};