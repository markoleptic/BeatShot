// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveGamePlayerSettings.h"
#include "Blueprint/UserWidget.h"
#include "CrossHairSettingsWidget.generated.h"

class UCrosshair;
class USlider;
class UImage;
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

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCrosshair* CrossHairWidget;
	
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
	UEditableTextBox* InnerOffsetValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* LineLengthValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* LineWidthValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* OutlineOpacityValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* OutlineWidthValue;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* ColorASlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* ColorRSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* ColorGSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* ColorBSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* InnerOffsetSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* LineLengthSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* LineWidthSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* OutlineOpacitySlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* OutlineWidthSlider;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* ColorPreview;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* ResetToDefaultButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* RevertCrossHairButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* SaveCrossHairButton;

	FPlayerSettings InitialCrossHairSettings;
	FPlayerSettings NewCrossHairSettings;
};
