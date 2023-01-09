// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveGamePlayerSettings.h"
#include "Blueprint/UserWidget.h"
#include "CrossHairSettingsWidget.generated.h"

class UCrossHairWidget;
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

	virtual void NativeConstruct() override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UCrossHairWidget* CrossHairWidget;
	
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
	void OnInnerOffsetValueChange(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnLineLengthValueChange(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnLineWidthValueChange(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnOutlineOpacityValueChange(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnOutlineWidthValueChange(const FText& NewValue, ETextCommit::Type CommitType);

	UFUNCTION()
	void OnColorASliderChange(const float NewValue);
	UFUNCTION()
	void OnColorRSliderChange(const float NewValue);
	UFUNCTION()
	void OnColorGSliderChange(const float NewValue);
	UFUNCTION()
	void OnColorBSliderChange(const float NewValue);
	UFUNCTION()
	void OnInnerOffsetSliderChange(const float NewValue);
	UFUNCTION()
	void OnLineLengthSliderChange(const float NewValue);
	UFUNCTION()
	void OnLineWidthSliderChange(const float NewValue);
	UFUNCTION()
	void OnOutlineOpacitySliderChange(const float NewValue);
	UFUNCTION()
	void OnOutlineWidthSliderChange(const float NewValue);

	UFUNCTION()
	void OnResetToDefaultButtonClicked();
	UFUNCTION()
	void OnRevertCrossHairButtonClicked();
	UFUNCTION()
	void OnSaveCrossHairButtonClicked();

	/** Fills out all CrossHair Settings given PlayerSettings */
	void SetCrossHairOptions(const FPlayerSettings& CrossHairSettings);
	/** The PlayerSettings that were initially loaded */
	FPlayerSettings InitialCrossHairSettings;
	/** The PlayerSettings that are changed when interacting with the CrossHairSettingsWidget */
	FPlayerSettings NewCrossHairSettings;
};
