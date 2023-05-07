// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BandThresholdWidget.generated.h"

DECLARE_DELEGATE_ThreeParams(FOnThresholdValueCommitted, const UBandThresholdWidget* BandThreshold, const int32 Index, const float NewValue);

class UBorder;
class USlider;
class UEditableTextBox;
class UTextBlock;

/** Widget representing a Band Threshold designed to be used with a linked list. Contains pointer to the next band threshold widget */
UCLASS()
class USERINTERFACE_API UBandThresholdWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:
	/** Pointer to the next band threshold widget inside a collection of them */
	UPROPERTY()
	UBandThresholdWidget* Next = nullptr;

	/** Executed when a new threshold value has been committed */
	FOnThresholdValueCommitted OnThresholdValueCommitted;

	/** Sets the threshold value and the channel index */
	void SetDefaultValue(const float Value, const int32 ChannelIndex);

	/** The index of this widget inside a collection of them */
	int32 Index;

protected:
	UFUNCTION()
	void OnValueChanged_Threshold(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnSliderChanged_Threshold(const float NewValue);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	USlider* Slider_Threshold;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* Value_Threshold;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TextBlock_Channel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UBorder* Border_Left;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UBorder* Border_Right;
};
