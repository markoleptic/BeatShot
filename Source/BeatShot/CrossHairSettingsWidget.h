// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrossHairSettingsWidget.generated.h"

class USlider;
class UEditableTextBox;
class UButton;

/**
 * 
 */
UCLASS()
class BEATSHOT_API UCrossHairSettingsWidget : public UUserWidget
{
	GENERATED_BODY()
	
	protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* ColorAValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* ColorRValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* ColorGValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* ColorBValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* HexValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* InnerOffsetValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* LineLengthValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* LineWidthValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* OutlineOpacityValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* OutlineWidthValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	USlider* ColorASlider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	USlider* ColorRSlider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	USlider* ColorGSlider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	USlider* ColorBSlider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	USlider* InnerOffsetSlider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	USlider* LineLengthSlider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	USlider* LineWidthSlider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	USlider* OutlineOpacitySlider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	USlider* OutlineWidthSlider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* ResetToDefaultButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* RevertCrosshairButton;
};
